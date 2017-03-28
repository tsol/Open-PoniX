#!/bin/bash

NAMESPACE="Ply"
AUTHOR="YOUR NAME <youremail@here.com>"
#-----------------------------------------------------------------------------
uppercase ()
{
  echo "${1}" | tr 'a-z' 'A-Z'
}

lowercase ()
{
  echo "${1}" | tr 'A-Z' 'a-z'
}

change_character ()
{
  echo "${1}" | tr "${2}" "${3}"
}

delete_character ()
{
  echo "${1}" | tr -d "${2}"
}

#-----------------------------------------------------------------------------
MACRO_NAMESPACE="$(change_character $(uppercase ${NAMESPACE}) '-' '_')"
METHOD_NAMESPACE="$(change_character $(lowercase ${NAMESPACE}) '-' '_')"

OBJECT_NAME="$(delete_character ${1} '-')"
MACRO_OBJECT_NAME="$(change_character $(uppercase ${1}) '-' '_')"
METHOD_OBJECT_NAME="$(change_character $(lowercase ${1}) '-' '_')"
FULL_OBJECT_NAME="$(delete_character ${NAMESPACE} '-')${OBJECT_NAME}"
SHORT_OBJECT_NAME="$(lowercase $(echo ${1} | awk -F- '{print $NF}'))"

HEADER_FILENAME="$(lowercase ${NAMESPACE}-${1}).h"
HEADER_GUARD="$(change_character $(change_character $(uppercase ${HEADER_FILENAME}) '-' '_') '.' '_')" 

SOURCE_FILENAME="$(lowercase ${NAMESPACE}-${1}).c"

MACRO_PREFIX="${MACRO_NAMESPACE}_${MACRO_OBJECT_NAME}"
METHOD_PREFIX="${METHOD_NAMESPACE}_${METHOD_OBJECT_NAME}"

ERROR_QUARK="${METHOD_NAMESPACE}-$(lowercase ${1})"
HUMAN_READABLE_NAME=$(change_character $(lowercase ${1}) '-' ' ')

#-----------------------------------------------------------------------------
cat <<  > ${HEADER_FILENAME}
/* vim: ts=4 sw=2 expandtab autoindent cindent
 * ${HEADER_FILENAME} - ${2}
 *
 * Copyright (C) $(date +%Y) ${AUTHOR}
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
 * Written By: ${AUTHOR}
 */
#ifndef ${HEADER_GUARD}
#define ${HEADER_GUARD}

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ${MACRO_NAMESPACE}_TYPE_${MACRO_OBJECT_NAME} (${METHOD_PREFIX}_get_type ())
#define ${MACRO_PREFIX}(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ${MACRO_NAMESPACE}_TYPE_${MACRO_OBJECT_NAME}, ${FULL_OBJECT_NAME}))
#define ${MACRO_PREFIX}_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ${MACRO_NAMESPACE}_TYPE_${MACRO_OBJECT_NAME}, ${FULL_OBJECT_NAME}Class))
#define ${MACRO_NAMESPACE}_IS_${MACRO_OBJECT_NAME}(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ${MACRO_NAMESPACE}_TYPE_${MACRO_OBJECT_NAME}))
#define ${MACRO_NAMESPACE}_IS_${MACRO_OBJECT_NAME}_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ${MACRO_NAMESPACE}_TYPE_${MACRO_OBJECT_NAME}))
#define ${MACRO_PREFIX}_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), ${MACRO_NAMESPACE}_TYPE_${MACRO_OBJECT_NAME}, ${FULL_OBJECT_NAME}Class))
#define ${MACRO_PREFIX}_ERROR (${METHOD_PREFIX}_error_quark ())

typedef struct _${FULL_OBJECT_NAME} ${FULL_OBJECT_NAME};
typedef struct _${FULL_OBJECT_NAME}Class ${FULL_OBJECT_NAME}Class;
typedef struct _${FULL_OBJECT_NAME}Private ${FULL_OBJECT_NAME}Private;
typedef enum _${FULL_OBJECT_NAME}Error ${FULL_OBJECT_NAME}Error;

struct _${FULL_OBJECT_NAME}
{
  GObject parent;

  /*< private >*/
  ${FULL_OBJECT_NAME}Private *priv;
};

struct _${FULL_OBJECT_NAME}Class
{
  GObjectClass parent_class;

  /* signals */
#if 0
  void (* foo) (${FULL_OBJECT_NAME} *${SHORT_OBJECT_NAME});
#endif
};

enum _${FULL_OBJECT_NAME}Error
{
  ${MACRO_PREFIX}_ERROR_GENERIC = 0,
};

#ifndef ${MACRO_NAMESPACE}_HIDE_FUNCTION_DECLARATIONS
GType ${METHOD_PREFIX}_get_type (void);
GQuark ${METHOD_PREFIX}_error_quark (void);

${FULL_OBJECT_NAME} *${METHOD_PREFIX}_new (void) G_GNUC_MALLOC;
#endif

G_END_DECLS
#endif /* ${HEADER_GUARD} */

#-----------------------------------------------------------------------------
cat <<  > ${SOURCE_FILENAME}
/* vim: ts=4 sw=2 expandtab autoindent cindent
 * ${SOURCE_FILENAME} - ${2}
 *
 * Copyright (C) $(date +%Y) ${AUTHOR}
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
 * Written by: ${AUTHOR}
 */
#include "config.h"
#include "${HEADER_FILENAME}"

#include <errno.h>
#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>

struct _${FULL_OBJECT_NAME}Private
{
  int bar;
};

static void ${METHOD_PREFIX}_finalize (GObject *object);
#if 0
static void ${METHOD_PREFIX}_class_install_signals (${FULL_OBJECT_NAME}Class *${SHORT_OBJECT_NAME}_class);
static void ${METHOD_PREFIX}_class_install_properties (${FULL_OBJECT_NAME}Class *${SHORT_OBJECT_NAME}_class);

static void ${METHOD_PREFIX}_set_property (GObject      *object,
                                           guint         prop_id,
                                           const GValue *value,
                                           GParamSpec   *pspec);
static void ${METHOD_PREFIX}_get_property (GObject    *object,
                                           guint       prop_id,
                                           GValue     *value,
                                           GParamSpec *pspec);

enum
{
  PROP_0 = 0,
  PROP_BAR
};

#define ${MACRO_PREFIX}_DEFAULT_BAR 1

enum
{
  FOO = 0,
  NUMBER_OF_SIGNALS
};

static guint ${METHOD_PREFIX}_signals[NUMBER_OF_SIGNALS];
#endif

G_DEFINE_TYPE (${FULL_OBJECT_NAME}, ${METHOD_PREFIX}, G_TYPE_OBJECT);

static void
${METHOD_PREFIX}_class_init (${FULL_OBJECT_NAME}Class *${SHORT_OBJECT_NAME}_class)
{
  GObjectClass *object_class;

  object_class = G_OBJECT_CLASS (${SHORT_OBJECT_NAME}_class);

  object_class->finalize = ${METHOD_PREFIX}_finalize;
  
#if 0
  ${METHOD_PREFIX}_class_install_properties (${SHORT_OBJECT_NAME}_class);
  ${METHOD_PREFIX}_class_install_signals (${SHORT_OBJECT_NAME}_class);
#endif

  g_type_class_add_private (${SHORT_OBJECT_NAME}_class, sizeof (${FULL_OBJECT_NAME}Private));
}

#if 0
static void
${METHOD_PREFIX}_class_install_signals (${FULL_OBJECT_NAME}Class *${SHORT_OBJECT_NAME}_class)
{
  GObjectClass *object_class;

  object_class = G_OBJECT_CLASS (${SHORT_OBJECT_NAME}_class);

  ${METHOD_PREFIX}_signals[FOO] =
    g_signal_new ("foo", G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (${FULL_OBJECT_NAME}Class, foo),
                  NULL, NULL, g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
  ${SHORT_OBJECT_NAME}_class->foo = NULL;
}

static void
${METHOD_PREFIX}_class_install_properties (${FULL_OBJECT_NAME}Class *${SHORT_OBJECT_NAME}_class)
{
  GObjectClass *object_class;
  GParamSpec *param_spec;

  object_class = G_OBJECT_CLASS (${SHORT_OBJECT_NAME}_class);
  object_class->set_property = ${METHOD_PREFIX}_set_property;
  object_class->get_property = ${METHOD_PREFIX}_get_property;

  param_spec = g_param_spec_int ("bar", _("Bar"),
                               _("The amount of bar"),
                               0, G_MAXINT,
                               ${MACRO_PREFIX}_DEFAULT_BAR,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
  g_object_class_install_property (object_class, PROP_BAR, param_spec);
}
#endif

static void
${METHOD_PREFIX}_init (${FULL_OBJECT_NAME} *${SHORT_OBJECT_NAME})
{
  ${SHORT_OBJECT_NAME}->priv = G_TYPE_INSTANCE_GET_PRIVATE (${SHORT_OBJECT_NAME}, 
                                                            ${MACRO_NAMESPACE}_TYPE_${MACRO_OBJECT_NAME},
                                                            ${FULL_OBJECT_NAME}Private);

}

static void
${METHOD_PREFIX}_finalize (GObject *object)
{
  ${FULL_OBJECT_NAME} *${SHORT_OBJECT_NAME};
  GObjectClass *parent_class;
  
  ${SHORT_OBJECT_NAME} = ${MACRO_PREFIX} (object);

  parent_class = G_OBJECT_CLASS (${METHOD_PREFIX}_parent_class);



  if (parent_class->finalize != NULL)
    parent_class->finalize (object);
}

#if 0
static void 
${METHOD_PREFIX}_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  ${FULL_OBJECT_NAME} *${SHORT_OBJECT_NAME} = ${MACRO_PREFIX} (object);

  switch (prop_id)
    {
      case PROP_BAR:
        ${METHOD_PREFIX}_set_bar (${SHORT_OBJECT_NAME}, 
                                  g_value_get_int (value));
        break;
      
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void 
${METHOD_PREFIX}_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  ${FULL_OBJECT_NAME} *${SHORT_OBJECT_NAME} = ${MACRO_PREFIX} (object);

  switch (prop_id)
    {
      case PROP_BAR:
            g_value_set_int (value, 
                             ${METHOD_PREFIX}_get_bar (${SHORT_OBJECT_NAME}));
        break;
      
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}
#endif

GQuark
${METHOD_PREFIX}_error_quark (void)
{
  static GQuark error_quark = 0;

  if (error_quark == 0)
    error_quark = g_quark_from_static_string ("${ERROR_QUARK}");

  return error_quark;
}

${FULL_OBJECT_NAME} *
${METHOD_PREFIX}_new (void)
{
  ${FULL_OBJECT_NAME} *${SHORT_OBJECT_NAME};

  ${SHORT_OBJECT_NAME} = g_object_new (${MACRO_NAMESPACE}_TYPE_${MACRO_OBJECT_NAME}, NULL);

  return ${SHORT_OBJECT_NAME};
}

#if 0
void
${METHOD_PREFIX}_set_bar (${FULL_OBJECT_NAME} *${SHORT_OBJECT_NAME},
                          int                  bar)
{
  if (${SHORT_OBJECT_NAME}->priv->bar != bar)
    {
      ${SHORT_OBJECT_NAME}->priv->bar = bar;
      g_object_notify (G_OBJECT (${SHORT_OBJECT_NAME}), "bar");
    }
}

int
${METHOD_PREFIX}_get_bar (${FULL_OBJECT_NAME} *${SHORT_OBJECT_NAME})
{
  return ${SHORT_OBJECT_NAME}->priv->bar;
}
#endif

#ifdef ${MACRO_PREFIX}_ENABLE_TEST

#include <stdio.h>
#include <glib.h>

int
main (int    argc,
      char **argv)
{
  ${FULL_OBJECT_NAME} *${SHORT_OBJECT_NAME};
  int exit_code;

  g_log_set_always_fatal (G_LOG_LEVEL_ERROR 
                          | G_LOG_LEVEL_CRITICAL 
			  | G_LOG_LEVEL_WARNING);

  g_type_init ();

  ${SHORT_OBJECT_NAME} = ${METHOD_PREFIX}_new ();

  g_object_unref (${SHORT_OBJECT_NAME});

  exit_code = 0;

  return exit_code;
}
#endif /* ${MACRO_PREFIX}_ENABLE_TEST */


uncrustify -c $(dirname $0)/default.cfg ${HEADER_FILENAME}
mv ${HEADER_FILENAME}.uncrustify ${HEADER_FILENAME}
uncrustify -c $(dirname $0)/default.cfg ${SOURCE_FILENAME}
mv ${SOURCE_FILENAME}.uncrustify ${SOURCE_FILENAME}
