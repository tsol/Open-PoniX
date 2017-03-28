/* ply-upstart-monitor.c - Upstart D-Bus monitor
 *
 * Copyright (C) 2010, 2011 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by: Colin Watson <cjwatson@ubuntu.com>
 */
#include "config.h"
#include "ply-upstart-monitor.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/eventfd.h>

#include <dbus/dbus.h>

#include "ply-logger.h"
#include "ply-event-loop.h"
#include "ply-hashtable.h"
#include "ply-list.h"
#include "ply-utils.h"

typedef struct
{
  ply_upstart_monitor_t *monitor;
  DBusTimeout           *timeout;
} ply_upstart_monitor_timeout_t;

struct _ply_upstart_monitor
{
  DBusConnection                              *connection;
  char                                        *owner;
  ply_event_loop_t                            *loop;
  ply_hashtable_t                             *jobs;
  ply_hashtable_t                             *all_instances;
  ply_upstart_monitor_state_changed_handler_t  state_changed_handler;
  void                                        *state_changed_data;
  ply_upstart_monitor_failed_handler_t         failed_handler;
  void                                        *failed_data;
  int                                          dispatch_fd;
};

typedef struct
{
  ply_upstart_monitor_t                *monitor;
  ply_upstart_monitor_job_properties_t  properties;
  ply_hashtable_t                      *instances;
  ply_list_t                           *pending_calls;
} ply_upstart_monitor_job_t;

typedef struct
{
  ply_upstart_monitor_job_t                 *job;
  ply_upstart_monitor_instance_properties_t  properties;
  ply_list_t                                *pending_calls;
  uint32_t                                   state_changed : 1;
  uint32_t                                   call_failed : 1;
} ply_upstart_monitor_instance_t;

#define UPSTART_SERVICE                 "com.ubuntu.Upstart"
#define UPSTART_PATH                    "/com/ubuntu/Upstart"
#define UPSTART_INTERFACE_0_6           "com.ubuntu.Upstart0_6"
#define UPSTART_INTERFACE_0_6_JOB       "com.ubuntu.Upstart0_6.Job"
#define UPSTART_INTERFACE_0_6_INSTANCE  "com.ubuntu.Upstart0_6.Instance"

/* Remove an entry from a hashtable, free the key, and return the data. */
static void *
hashtable_remove_and_free_key (ply_hashtable_t *hashtable,
                               const void      *key)
{
  void *reply_key, *reply_data;

  if (!ply_hashtable_lookup_full (hashtable, (void *) key,
                                  &reply_key, &reply_data))
    return NULL;
  ply_hashtable_remove (hashtable, (void *) key);
  free (reply_key);

  return reply_data;
}

/* We assume, in general, that Upstart responds to D-Bus messages in a
 * single thread, and that it processes messages on a given connection in
 * the order in which they were sent.  Taken together, these assumptions
 * imply a kind of coherence: a Properties.GetAll reply received after a
 * StateChanged signal must have been computed entirely after the state
 * change.  Thus, if this function returns false (properties have not been
 * fetched yet), it should be safe to record an event as call until such
 * time as the properties of the instance are known.
 */
static bool
instance_is_initialized (ply_upstart_monitor_instance_t *instance)
{
  /* Note that the job may not have a description. */
  if (instance->job->properties.name &&
      instance->properties.name && instance->properties.goal &&
      instance->properties.state)
    return true;
  else
    return false;
}

static void
on_get_all_instance_properties_finished (DBusPendingCall                *call,
                                         ply_upstart_monitor_instance_t *instance)
{
  DBusMessage *reply;
  DBusMessageIter iter, array_iter, dict_iter, variant_iter;
  const char *key, *name, *goal, *state;
  ply_upstart_monitor_t *monitor;

  assert (call != NULL);
  assert (instance != NULL);

  reply = dbus_pending_call_steal_reply (call);
  if (reply == NULL)
    return;
  if (dbus_message_get_type (reply) != DBUS_MESSAGE_TYPE_METHOD_RETURN)
    goto out;

  dbus_message_iter_init (reply, &iter);
  if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_ARRAY)
    goto out;
  dbus_message_iter_recurse (&iter, &array_iter);

  while (dbus_message_iter_get_arg_type (&array_iter) == DBUS_TYPE_DICT_ENTRY)
    {
      dbus_message_iter_recurse (&array_iter, &dict_iter);

      if (dbus_message_iter_get_arg_type (&dict_iter) != DBUS_TYPE_STRING)
        goto next_item;

      dbus_message_iter_get_basic (&dict_iter, &key);
      if (key == NULL)
        goto next_item;

      dbus_message_iter_next (&dict_iter);
      if (dbus_message_iter_get_arg_type (&dict_iter) != DBUS_TYPE_VARIANT)
        goto next_item;
      dbus_message_iter_recurse (&dict_iter, &variant_iter);
      if (dbus_message_iter_get_arg_type (&variant_iter) != DBUS_TYPE_STRING)
        goto next_item;

      if (strcmp (key, "name") == 0)
        {
          dbus_message_iter_get_basic (&variant_iter, &name);
          if (name != NULL)
            {
              ply_trace ("%s: name = '%s'",
                         instance->job->properties.name, name);
              instance->properties.name = strdup (name);
            }
        }
      else if (strcmp (key, "goal") == 0)
        {
          dbus_message_iter_get_basic (&variant_iter, &goal);
          if (goal != NULL)
            {
              ply_trace ("%s: goal = '%s'",
                         instance->job->properties.name, goal);
              instance->properties.goal = strdup (goal);
            }
        }
      else if (strcmp (key, "state") == 0)
        {
          dbus_message_iter_get_basic (&variant_iter, &state);
          if (state != NULL)
            {
              ply_trace ("%s: state = '%s'",
                         instance->job->properties.name, state);
              instance->properties.state = strdup (state);
            }
        }

next_item:
      dbus_message_iter_next (&array_iter);
    }

out:
  dbus_message_unref (reply);

  if (instance_is_initialized (instance))
    {
      /* Process any call events. */
      monitor = instance->job->monitor;

      if (instance->state_changed && monitor->state_changed_handler)
        monitor->state_changed_handler (monitor->state_changed_data, NULL,
                                        &instance->job->properties,
                                        &instance->properties);
      instance->state_changed = false;

      if (instance->call_failed && monitor->failed_handler)
        monitor->failed_handler (monitor->failed_data,
                                 &instance->job->properties,
                                 &instance->properties,
                                 instance->properties.failed);
      instance->call_failed = false;
    }
}

static void
on_get_all_job_properties_finished (DBusPendingCall           *call,
                                    ply_upstart_monitor_job_t *job)
{
  DBusMessage *reply;
  DBusMessageIter iter, array_iter, dict_iter, variant_iter;
  const char *key, *name, *description;
  dbus_uint32_t task;

  assert (call != NULL);
  assert (job != NULL);

  reply = dbus_pending_call_steal_reply (call);
  if (reply == NULL)
    return;
  if (dbus_message_get_type (reply) != DBUS_MESSAGE_TYPE_METHOD_RETURN)
    goto out;

  dbus_message_iter_init (reply, &iter);
  if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_ARRAY)
    goto out;
  dbus_message_iter_recurse (&iter, &array_iter);

  while (dbus_message_iter_get_arg_type (&array_iter) == DBUS_TYPE_DICT_ENTRY)
    {
      dbus_message_iter_recurse (&array_iter, &dict_iter);

      if (dbus_message_iter_get_arg_type (&dict_iter) != DBUS_TYPE_STRING)
        goto next_item;

      dbus_message_iter_get_basic (&dict_iter, &key);
      if (key == NULL)
        goto next_item;

      dbus_message_iter_next (&dict_iter);
      if (dbus_message_iter_get_arg_type (&dict_iter) != DBUS_TYPE_VARIANT)
        goto next_item;
      dbus_message_iter_recurse (&dict_iter, &variant_iter);

      if (strcmp (key, "name") == 0)
        {
          if (dbus_message_iter_get_arg_type (&variant_iter) !=
              DBUS_TYPE_STRING)
            goto next_item;
          dbus_message_iter_get_basic (&variant_iter, &name);
          if (name != NULL)
            {
              ply_trace ("name = '%s'", name);
              job->properties.name = strdup (name);
            }
        }
      else if (strcmp (key, "description") == 0)
        {
          if (dbus_message_iter_get_arg_type (&variant_iter) !=
              DBUS_TYPE_STRING)
            goto next_item;
          dbus_message_iter_get_basic (&variant_iter, &description);
          if (description != NULL)
            {
              ply_trace ("description = '%s'", description);
              job->properties.description = strdup (description);
            }
        }
      else if (strcmp (key, "task") == 0)
        {
          if (dbus_message_iter_get_arg_type (&variant_iter) !=
              DBUS_TYPE_BOOLEAN)
            goto next_item;
          dbus_message_iter_get_basic (&variant_iter, &task);
          ply_trace ("task = %s", task ? "TRUE" : "FALSE");
          job->properties.is_task = task ? true : false;
        }

next_item:
      dbus_message_iter_next (&array_iter);
    }

out:
  dbus_message_unref (reply);
}

static void
remove_instance_internal (ply_upstart_monitor_job_t *job, const char *path)
{
  ply_upstart_monitor_instance_t *instance;
  ply_list_node_t *node;

  instance = hashtable_remove_and_free_key (job->instances, path);
  if (instance == NULL)
    return;
  hashtable_remove_and_free_key (job->monitor->all_instances, path);

  node = ply_list_get_first_node (instance->pending_calls);
  while (node != NULL)
    {
      DBusPendingCall *call;
      ply_list_node_t *next_node;

      call = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (instance->pending_calls, node);
      dbus_pending_call_cancel (call);
      dbus_pending_call_unref (call);
      node = next_node;
    }
  ply_list_free (instance->pending_calls);

  free (instance->properties.name);
  free (instance->properties.goal);
  free (instance->properties.state);
  free (instance);
}

static void
add_instance (ply_upstart_monitor_job_t *job,
              const char                *path)
{
  ply_upstart_monitor_instance_t *instance;
  DBusMessage *message;
  const char *interface = UPSTART_INTERFACE_0_6_INSTANCE;
  DBusPendingCall *call;

  ply_trace ("adding instance: %s", path);

  remove_instance_internal (job, path);

  instance = calloc (1, sizeof (ply_upstart_monitor_instance_t));
  instance->job = job;
  instance->properties.name = NULL;
  instance->properties.goal = NULL;
  instance->properties.state = NULL;
  instance->properties.failed = false;
  instance->pending_calls = ply_list_new ();
  instance->state_changed = false;
  instance->call_failed = false;

  /* Keep a hash of instances per job, to make InstanceRemoved handling
   * easy.
   */
  ply_hashtable_insert (job->instances, strdup (path), instance);
  /* Keep a separate hash of all instances, to make StateChanged handling
   * easy.
   */
  ply_hashtable_insert (job->monitor->all_instances, strdup (path), instance);

  /* Ask Upstart for the name, goal, and state properties. */
  ply_trace ("fetching properties of instance %s", path);
  message = dbus_message_new_method_call (UPSTART_SERVICE, path,
                                          DBUS_INTERFACE_PROPERTIES, "GetAll");
  dbus_message_append_args (message,
                            DBUS_TYPE_STRING, &interface,
                            DBUS_TYPE_INVALID);
  dbus_connection_send_with_reply (job->monitor->connection, message,
                                   &call, -1);
  dbus_message_unref (message);
  if (call != NULL)
    {
      dbus_pending_call_set_notify (call,
                                    (DBusPendingCallNotifyFunction)
                                    on_get_all_instance_properties_finished,
                                    instance, NULL);
      ply_list_append_data (instance->pending_calls, call);
    }
}

static void
remove_instance (ply_upstart_monitor_job_t *job,
                 const char                *path)
{
  ply_trace ("removing instance: %s", path);

  remove_instance_internal (job, path);
}

static void
on_get_all_instances_finished (DBusPendingCall           *call,
                               ply_upstart_monitor_job_t *job)
{
  DBusMessage *reply;
  DBusError error;
  char **instances;
  int n_instances, i;

  assert (call != NULL);
  assert (job != NULL);

  reply = dbus_pending_call_steal_reply (call);
  if (reply == NULL)
    return;
  if (dbus_message_get_type (reply) != DBUS_MESSAGE_TYPE_METHOD_RETURN)
    goto out;

  dbus_error_init (&error);
  dbus_message_get_args (reply, &error,
                         DBUS_TYPE_ARRAY, DBUS_TYPE_OBJECT_PATH,
                         &instances, &n_instances,
                         DBUS_TYPE_INVALID);
  if (dbus_error_is_set (&error))
    goto out;
  dbus_error_free (&error);

  for (i = 0; i < n_instances; ++i)
    add_instance (job, instances[i]);

  dbus_free_string_array (instances);

out:
  dbus_message_unref (reply);
}

static void
free_job_instance (void *key, void *data, void *user_data)
{
  const char *path = key;
  ply_upstart_monitor_instance_t *instance = data;
  ply_upstart_monitor_t *monitor = user_data;

  assert (monitor != NULL);

  if (instance == NULL)
    return;

  hashtable_remove_and_free_key (monitor->all_instances, path);
  free (instance->properties.name);
  free (instance->properties.goal);
  free (instance->properties.state);
  free (instance);
}

static void
remove_job_internal (ply_upstart_monitor_t *monitor, const char *path)
{
  ply_upstart_monitor_job_t *job;
  ply_list_node_t *node;

  job = hashtable_remove_and_free_key (monitor->jobs, path);
  if (job == NULL)
    return;

  node = ply_list_get_first_node (job->pending_calls);
  while (node != NULL)
    {
      DBusPendingCall *call;
      ply_list_node_t *next_node;

      call = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (job->pending_calls, node);
      dbus_pending_call_cancel (call);
      dbus_pending_call_unref (call);
      node = next_node;
    }
  ply_list_free (job->pending_calls);

  free (job->properties.name);
  free (job->properties.description);
  ply_hashtable_foreach (job->instances, free_job_instance, monitor);
  ply_hashtable_free (job->instances);
  free (job);
}

static void
add_job (ply_upstart_monitor_t *monitor, const char *path)
{
  ply_upstart_monitor_job_t *job;
  DBusMessage *message;
  const char *interface = UPSTART_INTERFACE_0_6_JOB;
  DBusPendingCall *call;

  ply_trace ("adding job: %s", path);

  remove_job_internal (monitor, path);

  job = calloc (1, sizeof (ply_upstart_monitor_job_t));
  job->monitor = monitor;
  job->properties.name = NULL;
  job->properties.description = NULL;
  job->properties.is_task = false;
  job->instances = ply_hashtable_new (ply_hashtable_string_hash,
                                      ply_hashtable_string_compare);
  job->pending_calls = ply_list_new ();

  ply_hashtable_insert (monitor->jobs, strdup (path), job);

  /* Ask Upstart for the name and description properties. */
  ply_trace ("fetching properties of job %s", path);
  message = dbus_message_new_method_call (UPSTART_SERVICE, path,
                                          DBUS_INTERFACE_PROPERTIES, "GetAll");
  dbus_message_append_args (message,
                            DBUS_TYPE_STRING, &interface,
                            DBUS_TYPE_INVALID);
  dbus_connection_send_with_reply (monitor->connection, message, &call, -1);
  dbus_message_unref (message);
  if (call != NULL)
    {
      dbus_pending_call_set_notify (call,
                                    (DBusPendingCallNotifyFunction)
                                    on_get_all_job_properties_finished,
                                    job,
                                    NULL);
      ply_list_append_data (job->pending_calls, call);
    }

  /* Ask Upstart for a list of all instances of this job. */
  ply_trace ("calling GetAllInstances on job %s", path);
  message = dbus_message_new_method_call (UPSTART_SERVICE, path,
                                          UPSTART_INTERFACE_0_6_JOB,
                                          "GetAllInstances");
  dbus_connection_send_with_reply (monitor->connection, message, &call, -1);
  dbus_message_unref (message);
  if (call != NULL)
    {
      dbus_pending_call_set_notify (call,
                                    (DBusPendingCallNotifyFunction)
                                    on_get_all_instances_finished,
                                    job,
                                    NULL);
      ply_list_append_data (job->pending_calls, call);
    }
}

static void
remove_job (ply_upstart_monitor_t *monitor, const char *path)
{
  ply_trace ("removing job: %s", path);

  remove_job_internal (monitor, path);
}

static void
on_get_all_jobs_finished (DBusPendingCall       *call,
                          ply_upstart_monitor_t *monitor)
{
  DBusMessage *reply;
  DBusError error;
  char **jobs;
  int n_jobs, i;

  assert (call != NULL);
  assert (monitor != NULL);

  reply = dbus_pending_call_steal_reply (call);
  if (reply == NULL)
    return;
  if (dbus_message_get_type (reply) != DBUS_MESSAGE_TYPE_METHOD_RETURN)
    goto out;

  dbus_error_init (&error);
  dbus_message_get_args (reply, &error,
                         DBUS_TYPE_ARRAY, DBUS_TYPE_OBJECT_PATH,
                         &jobs, &n_jobs,
                         DBUS_TYPE_INVALID);
  if (dbus_error_is_set (&error))
    goto out;
  dbus_error_free (&error);

  for (i = 0; i < n_jobs; ++i)
    add_job (monitor, jobs[i]);

  dbus_free_string_array (jobs);

out:
  dbus_message_unref (reply);
}

static void
on_get_name_owner_finished (DBusPendingCall       *call,
                            ply_upstart_monitor_t *monitor)
{
  DBusMessage *reply, *message;
  DBusError error;
  const char *owner;

  assert (call != NULL);
  assert (monitor != NULL);

  reply = dbus_pending_call_steal_reply (call);
  if (reply == NULL)
    return;
  if (dbus_message_get_type (reply) != DBUS_MESSAGE_TYPE_METHOD_RETURN)
    goto out;

  dbus_error_init (&error);
  dbus_message_get_args (reply, &error,
                         DBUS_TYPE_STRING, &owner,
                         DBUS_TYPE_INVALID);
  if (dbus_error_is_set (&error))
    goto out;
  dbus_error_free (&error);

  ply_trace ("owner = '%s'", owner);

  free (monitor->owner);
  monitor->owner = strdup (owner);

  ply_trace ("calling GetAllJobs");
  message = dbus_message_new_method_call (UPSTART_SERVICE, UPSTART_PATH,
                                          UPSTART_INTERFACE_0_6,
                                          "GetAllJobs");
  dbus_connection_send_with_reply (monitor->connection, message, &call, -1);
  dbus_message_unref (message);
  if (call != NULL)
    dbus_pending_call_set_notify (call,
                                  (DBusPendingCallNotifyFunction)
                                  on_get_all_jobs_finished,
                                  monitor, NULL);

out:
  dbus_message_unref (reply);
}

static DBusHandlerResult
name_owner_changed_handler (DBusConnection        *connection,
                            DBusMessage           *message,
                            ply_upstart_monitor_t *monitor)
{
  DBusError error;
  const char *name, *old_owner, *new_owner;

  assert (connection != NULL);
  assert (message != NULL);
  assert (monitor != NULL);

  dbus_error_init (&error);
  if (dbus_message_get_args (message, &error,
                             DBUS_TYPE_STRING, &name,
                             DBUS_TYPE_STRING, &old_owner,
                             DBUS_TYPE_STRING, &new_owner,
                             DBUS_TYPE_INVALID) &&
      strcmp (name, UPSTART_SERVICE) == 0)
    {
      if (new_owner)
        ply_trace ("owner changed from '%s' to '%s'", old_owner, new_owner);
      else
        ply_trace ("owner left bus");
      free (monitor->owner);
      monitor->owner = new_owner ? strdup (new_owner) : NULL;
    }

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED; /* let other handlers try */
}

static DBusHandlerResult
job_added_handler (DBusConnection        *connection,
                   DBusMessage           *message,
                   ply_upstart_monitor_t *monitor)
{
  DBusError error;
  const char *signal_path;

  ply_trace ("got JobAdded");
  dbus_error_init (&error);
  if (dbus_message_get_args (message, &error,
                             DBUS_TYPE_OBJECT_PATH, &signal_path,
                             DBUS_TYPE_INVALID))
    add_job (monitor, signal_path);
  dbus_error_free (&error);
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
job_removed_handler (DBusConnection        *connection,
                     DBusMessage           *message,
                     ply_upstart_monitor_t *monitor)
{
  DBusError error;
  const char *signal_path;

  ply_trace ("got JobRemoved");
  dbus_error_init (&error);
  if (dbus_message_get_args (message, &error,
                             DBUS_TYPE_OBJECT_PATH, &signal_path,
                             DBUS_TYPE_INVALID))
    remove_job (monitor, signal_path);
  dbus_error_free (&error);
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
instance_added_handler (DBusConnection *connection, DBusMessage *message,
                        ply_upstart_monitor_t *monitor, const char *path)
{
  DBusError error;
  const char *signal_path;
  ply_upstart_monitor_job_t *job;

  ply_trace ("got %s InstanceAdded", path);
  job = ply_hashtable_lookup (monitor->jobs, (void *) path);
  if (job != NULL)
    {
      dbus_error_init (&error);
      if (dbus_message_get_args (message, &error,
                                 DBUS_TYPE_OBJECT_PATH, &signal_path,
                                 DBUS_TYPE_INVALID))
        add_instance (job, signal_path);
      dbus_error_free (&error);
    }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
instance_removed_handler (DBusConnection *connection, DBusMessage *message,
                          ply_upstart_monitor_t *monitor, const char *path)
{
  DBusError error;
  const char *signal_path;
  ply_upstart_monitor_job_t *job;

  ply_trace ("got %s InstanceRemoved", path);
  job = ply_hashtable_lookup (monitor->jobs, (void *) path);
  if (job != NULL)
    {
      dbus_error_init (&error);
      if (dbus_message_get_args (message, &error,
                                 DBUS_TYPE_OBJECT_PATH, &signal_path,
                                 DBUS_TYPE_INVALID))
        remove_instance (job, signal_path);
      dbus_error_free (&error);
    }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
goal_changed_handler (DBusConnection *connection, DBusMessage *message,
                      ply_upstart_monitor_t *monitor, const char *path)
{
  DBusError error;
  const char *goal;
  ply_upstart_monitor_instance_t *instance;
  char *old_goal;

  ply_trace ("got %s GoalChanged", path);
  instance = ply_hashtable_lookup (monitor->all_instances, (void *) path);
  if (instance != NULL)
    {
      dbus_error_init (&error);
      if (dbus_message_get_args (message, &error,
                                 DBUS_TYPE_STRING, &goal,
                                 DBUS_TYPE_INVALID))
        {
          old_goal = instance->properties.goal;
          instance->properties.goal = strdup (goal);
          ply_trace ("goal changed from '%s' to '%s'", old_goal, goal);
          free (old_goal);
        }
      dbus_error_free (&error);
    }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
state_changed_handler (DBusConnection *connection, DBusMessage *message,
                       ply_upstart_monitor_t *monitor, const char *path)
{
  DBusError error;
  const char *state;
  ply_upstart_monitor_instance_t *instance;
  char *old_state;

  ply_trace ("got %s StateChanged", path);
  instance = ply_hashtable_lookup (monitor->all_instances, (void *) path);
  if (instance != NULL)
    {
      dbus_error_init (&error);
      if (dbus_message_get_args (message, &error,
                                 DBUS_TYPE_STRING, &state,
                                 DBUS_TYPE_INVALID))
        {
          old_state = instance->properties.state;
          instance->properties.state = strdup (state);
          ply_trace ("state changed from '%s' to '%s'", old_state, state);
          if (strcmp (state, "starting") == 0)
            {
              /* Clear any old failed information. */
              instance->properties.failed = 0;
              instance->call_failed = false;
            }
          if (instance_is_initialized (instance))
            {
              if (monitor->state_changed_handler)
                monitor->state_changed_handler (monitor->state_changed_data,
                                                old_state,
                                                &instance->job->properties,
                                                &instance->properties);
            }
          else
            instance->state_changed = true;
          free (old_state);
        }
      dbus_error_free (&error);
    }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
failed_handler (DBusConnection *connection, DBusMessage *message,
                ply_upstart_monitor_t *monitor, const char *path)
{
  DBusError error;
  ply_upstart_monitor_instance_t *instance;
  dbus_int32_t failed_status;

  ply_trace ("got %s Failed", path);
  instance = ply_hashtable_lookup (monitor->all_instances, (void *) path);
  if (instance != NULL)
    {
      dbus_error_init (&error);
      if (dbus_message_get_args (message, &error,
                                 DBUS_TYPE_INT32, &failed_status,
                                 DBUS_TYPE_INVALID))
        {
          instance->properties.failed = failed_status;
          if (instance_is_initialized (instance))
            {
              if (monitor->failed_handler)
                monitor->failed_handler (monitor->failed_data,
                                         &instance->job->properties,
                                         &instance->properties,
                                         (int) failed_status);
            }
          else
            instance->call_failed = true;
        }
      dbus_error_free (&error);
    }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
message_handler (DBusConnection *connection, DBusMessage *message, void *data)
{
  ply_upstart_monitor_t *monitor = data;
  const char *path;

  assert (connection != NULL);
  assert (message != NULL);
  assert (monitor != NULL);

  path = dbus_message_get_path (message);
  if (path == NULL)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (dbus_message_is_signal (message, DBUS_INTERFACE_DBUS,
                              "NameOwnerChanged") &&
      dbus_message_has_path (message, DBUS_PATH_DBUS) &&
      dbus_message_has_sender (message, DBUS_SERVICE_DBUS))
    return name_owner_changed_handler (connection, message, monitor);

  if (dbus_message_is_signal (message, UPSTART_INTERFACE_0_6,
                              "JobAdded"))
    return job_added_handler (connection, message, monitor);

  if (dbus_message_is_signal (message, UPSTART_INTERFACE_0_6,
                              "JobRemoved"))
    return job_removed_handler (connection, message, monitor);

  if (dbus_message_is_signal (message, UPSTART_INTERFACE_0_6_JOB,
                              "InstanceAdded"))
    return instance_added_handler (connection, message, monitor, path);

  if (dbus_message_is_signal (message, UPSTART_INTERFACE_0_6_JOB,
                              "InstanceRemoved"))
    return instance_removed_handler (connection, message, monitor, path);

  if (dbus_message_is_signal (message, UPSTART_INTERFACE_0_6_INSTANCE,
                              "GoalChanged"))
    return goal_changed_handler (connection, message, monitor, path);

  if (dbus_message_is_signal (message, UPSTART_INTERFACE_0_6_INSTANCE,
                              "StateChanged"))
    return state_changed_handler (connection, message, monitor, path);

  if (dbus_message_is_signal (message, UPSTART_INTERFACE_0_6_INSTANCE,
                              "Failed"))
    return failed_handler (connection, message, monitor, path);

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

ply_upstart_monitor_t *
ply_upstart_monitor_new (ply_event_loop_t *loop)
{
  DBusError error;
  DBusConnection *connection;
  ply_upstart_monitor_t *monitor;
  char *rule;
  DBusMessage *message;
  const char *monitor_service = UPSTART_SERVICE;
  DBusPendingCall *call;

  dbus_error_init (&error);

  /* Get a connection to the system bus and set it up to listen for messages
   * from Upstart.
   */
  connection = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
  if (connection == NULL)
    {
      ply_error ("unable to connect to system bus: %s", error.message);
      dbus_error_free (&error);
      return NULL;
    }
  dbus_error_free (&error);

  monitor = calloc (1, sizeof (ply_upstart_monitor_t));
  monitor->connection = connection;
  monitor->loop = NULL;
  monitor->jobs = ply_hashtable_new (ply_hashtable_string_hash,
                                     ply_hashtable_string_compare);
  monitor->all_instances = ply_hashtable_new (ply_hashtable_string_hash,
                                              ply_hashtable_string_compare);
  monitor->state_changed_handler = NULL;
  monitor->state_changed_data = NULL;
  monitor->failed_handler = NULL;
  monitor->failed_data = NULL;
  monitor->dispatch_fd = -1;

  if (!dbus_connection_add_filter (connection, message_handler, monitor, NULL))
    {
      ply_error ("unable to add filter to system bus connection");
      ply_upstart_monitor_free (monitor);
      return NULL;
    }

  asprintf (&rule, "type='%s',sender='%s',path='%s',"
                   "interface='%s',member='%s',arg0='%s'",
            "signal", DBUS_SERVICE_DBUS, DBUS_PATH_DBUS,
            DBUS_INTERFACE_DBUS, "NameOwnerChanged", UPSTART_SERVICE);
  dbus_bus_add_match (connection, rule, &error);
  free (rule);
  if (dbus_error_is_set (&error))
    {
      ply_error ("unable to add match rule to system bus connection: %s",
                 error.message);
      ply_upstart_monitor_free (monitor);
      dbus_error_free (&error);
      return NULL;
    }

  asprintf (&rule, "type='%s',sender='%s'", "signal", UPSTART_SERVICE);
  dbus_bus_add_match (connection, rule, &error);
  free (rule);
  if (dbus_error_is_set (&error))
    {
      ply_error ("unable to add match rule to system bus connection: %s",
                 error.message);
      ply_upstart_monitor_free (monitor);
      dbus_error_free (&error);
      return NULL;
    }

  /* Start the state machine going: find out the current owner of the
   * well-known Upstart name.
   * Ignore errors: the worst case is that we don't get any messages back
   * and our state machine does nothing.
   */
  ply_trace ("calling GetNameOwner");
  message = dbus_message_new_method_call (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS,
                                          DBUS_INTERFACE_DBUS, "GetNameOwner");
  dbus_message_append_args (message,
                            DBUS_TYPE_STRING, &monitor_service,
                            DBUS_TYPE_INVALID);
  dbus_connection_send_with_reply (connection, message, &call, -1);
  dbus_message_unref (message);
  if (call != NULL)
    dbus_pending_call_set_notify (call,
                                  (DBusPendingCallNotifyFunction)
                                  on_get_name_owner_finished,
                                  monitor,
                                  NULL);

  if (loop != NULL)
    ply_upstart_monitor_connect_to_event_loop (monitor, loop);

  return monitor;
}

void
ply_upstart_monitor_free (ply_upstart_monitor_t *monitor)
{
  if (monitor == NULL)
    return;

  ply_hashtable_free (monitor->all_instances);
  ply_hashtable_free (monitor->jobs);
  dbus_connection_unref (monitor->connection);
  if (monitor->dispatch_fd >= 0)
    close (monitor->dispatch_fd);
  free (monitor);
}

static void
read_watch_handler (void *data, int fd)
{
  DBusWatch *watch = data;

  assert (watch != NULL);

  dbus_watch_handle (watch, DBUS_WATCH_READABLE);
}

static void
write_watch_handler (void *data, int fd)
{
  DBusWatch *watch = data;

  assert (watch != NULL);

  dbus_watch_handle (watch, DBUS_WATCH_WRITABLE);
}

static dbus_bool_t
add_watch (DBusWatch *watch, void *data)
{
  ply_upstart_monitor_t *monitor = data;
  unsigned int flags;
  int fd;
  ply_event_loop_fd_status_t status;
  ply_fd_watch_t *read_watch_event = NULL, *write_watch_event = NULL;

  assert (monitor != NULL);
  assert (watch != NULL);

  if (!dbus_watch_get_enabled (watch))
    return TRUE;

  assert (dbus_watch_get_data (watch) == NULL);

  flags = dbus_watch_get_flags (watch);
  fd = dbus_watch_get_unix_fd (watch);

  if (flags & DBUS_WATCH_READABLE)
    {
      status = PLY_EVENT_LOOP_FD_STATUS_HAS_DATA;
      read_watch_event = ply_event_loop_watch_fd (monitor->loop, fd, status,
                                                  read_watch_handler, NULL,
                                                  watch);
      if (read_watch_event == NULL)
        return FALSE;
      dbus_watch_set_data (watch, read_watch_event, NULL);
    }

  if (flags & DBUS_WATCH_WRITABLE)
    {
      status = PLY_EVENT_LOOP_FD_STATUS_CAN_TAKE_DATA;
      write_watch_event = ply_event_loop_watch_fd (monitor->loop, fd, status,
                                                   write_watch_handler, NULL,
                                                   watch);
      if (write_watch_event == NULL)
        {
          if (read_watch_event != NULL)
            ply_event_loop_stop_watching_fd (monitor->loop, read_watch_event);
          return FALSE;
        }
      dbus_watch_set_data (watch, write_watch_event, NULL);
    }

  return TRUE;
}

static void
remove_watch (DBusWatch *watch, void *data)
{
  ply_upstart_monitor_t *monitor = data;
  ply_fd_watch_t *watch_event;

  assert (monitor != NULL);
  assert (watch != NULL);

  watch_event = dbus_watch_get_data (watch);
  if (watch_event == NULL)
    return;

  ply_event_loop_stop_watching_fd (monitor->loop, watch_event);

  dbus_watch_set_data (watch, NULL, NULL);
}

static void
toggled_watch (DBusWatch *watch, void *data)
{
  if (dbus_watch_get_enabled (watch))
    add_watch (watch, data);
  else
    remove_watch (watch, data);
}

static ply_upstart_monitor_timeout_t *
timeout_user_data_new (ply_upstart_monitor_t *monitor, DBusTimeout *timeout)
{
  ply_upstart_monitor_timeout_t *monitor_timeout;

  monitor_timeout = calloc (1, sizeof (ply_upstart_monitor_timeout_t));
  monitor_timeout->monitor = monitor;
  monitor_timeout->timeout = timeout;

  return monitor_timeout;
}

static void
timeout_user_data_free (void *data)
{
  ply_upstart_monitor_timeout_t *monitor_timeout = data;

  free (monitor_timeout);
}

static void
timeout_handler (void *data, ply_event_loop_t *loop)
{
  ply_upstart_monitor_timeout_t *monitor_timeout = data;

  assert (monitor_timeout != NULL);

  dbus_timeout_handle (monitor_timeout->timeout);
}

static dbus_bool_t
add_timeout (DBusTimeout *timeout, void *data)
{
  ply_upstart_monitor_t *monitor = data;
  int interval;
  ply_upstart_monitor_timeout_t *monitor_timeout;

  assert (monitor != NULL);
  assert (timeout != NULL);

  if (!dbus_timeout_get_enabled (timeout))
    return TRUE;

  interval = dbus_timeout_get_interval (timeout) * 1000;

  monitor_timeout = timeout_user_data_new (monitor, timeout);

  ply_event_loop_watch_for_timeout (monitor->loop, (double) interval,
                                    timeout_handler, monitor_timeout);

  dbus_timeout_set_data (timeout, monitor_timeout, timeout_user_data_free);

  return TRUE;
}

static void
remove_timeout (DBusTimeout *timeout, void *data)
{
  ply_upstart_monitor_t *monitor = data;
  ply_upstart_monitor_timeout_t *monitor_timeout;

  assert (monitor != NULL);
  assert (timeout != NULL);

  monitor_timeout = dbus_timeout_get_data (timeout);
  if (monitor_timeout == NULL)
    return;

  ply_event_loop_stop_watching_for_timeout (monitor->loop,
                                            timeout_handler, monitor_timeout);

  dbus_timeout_set_data (timeout, NULL, NULL);
}

static void
toggled_timeout (DBusTimeout *timeout, void *data)
{
  if (dbus_timeout_get_enabled (timeout))
    add_timeout (timeout, data);
  else
    remove_timeout (timeout, data);
}

static void
dispatch_status (DBusConnection *connection, DBusDispatchStatus new_status,
                 void *data)
{
  ply_upstart_monitor_t *monitor = data;
  uint64_t event_payload;

  assert (monitor != NULL);

  if (new_status != DBUS_DISPATCH_DATA_REMAINS)
    return;

  /* wake up event loop */
  event_payload = 1;
  ply_write (monitor->dispatch_fd, &event_payload, sizeof (event_payload));
}

static void
dispatch (void *data, int fd)
{
  ply_upstart_monitor_t *monitor = data;
  uint64_t event_payload;

  assert (monitor != NULL);

  /* reset eventfd to zero */
  ply_read (fd, &event_payload, sizeof (event_payload));

  while (dbus_connection_dispatch (monitor->connection) ==
         DBUS_DISPATCH_DATA_REMAINS)
    ;
}

bool
ply_upstart_monitor_connect_to_event_loop (ply_upstart_monitor_t    *monitor,
                                           ply_event_loop_t         *loop)
{
  ply_fd_watch_t *dispatch_event = NULL;
  uint64_t event_payload;

  assert (monitor != NULL);

  monitor->loop = loop;
  monitor->dispatch_fd = -1;

  if (!dbus_connection_set_watch_functions (monitor->connection,
                                            add_watch,
                                            remove_watch,
                                            toggled_watch,
                                            monitor, NULL))
    goto err;

  if (!dbus_connection_set_timeout_functions (monitor->connection,
                                              add_timeout,
                                              remove_timeout,
                                              toggled_timeout,
                                              monitor, NULL))
    goto err;

  monitor->dispatch_fd = eventfd (0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (monitor->dispatch_fd < 0)
    goto err;
  /* make sure we wake up to dispatch the first time through */
  event_payload = 1;
  ply_write (monitor->dispatch_fd, &event_payload, sizeof (event_payload));

  dispatch_event = ply_event_loop_watch_fd (monitor->loop,
                                            monitor->dispatch_fd,
                                            PLY_EVENT_LOOP_FD_STATUS_HAS_DATA,
                                            dispatch, NULL, monitor);
  if (dispatch_event == NULL)
    goto err;

  dbus_connection_set_dispatch_status_function (monitor->connection,
                                                dispatch_status,
                                                monitor, NULL);

  return true;

err:
  dbus_connection_set_watch_functions (monitor->connection,
                                       NULL, NULL, NULL, NULL, NULL);
  dbus_connection_set_timeout_functions (monitor->connection,
                                         NULL, NULL, NULL, NULL, NULL);
  dbus_connection_set_dispatch_status_function (monitor->connection,
                                                NULL, NULL, NULL);
  if (dispatch_event != NULL)
    ply_event_loop_stop_watching_fd (monitor->loop, dispatch_event);
  if (monitor->dispatch_fd >= 0)
    {
      close (monitor->dispatch_fd);
      monitor->dispatch_fd = -1;
    }
  monitor->loop = NULL;
  return false;
}

void
ply_upstart_monitor_add_state_changed_handler (ply_upstart_monitor_t                       *monitor,
                                               ply_upstart_monitor_state_changed_handler_t  handler,
                                               void                                        *user_data)
{
  monitor->state_changed_handler = handler;
  monitor->state_changed_data = user_data;
}

void
ply_upstart_monitor_add_failed_handler (ply_upstart_monitor_t                *monitor,
                                        ply_upstart_monitor_failed_handler_t  handler,
                                        void                                 *user_data)
{
  monitor->failed_handler = handler;
  monitor->failed_data = user_data;
}
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
