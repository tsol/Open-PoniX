/**
 * FreeRDP: A Remote Desktop Protocol client.
 * USB Redirector (www.incentivespro.com) Virtual Channel Support
 *
 * Copyright 2012-2014 SimplyCore LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <freerdp/types.h>
#include <freerdp/constants.h>
#include <freerdp/utils/memory.h>
#include <freerdp/utils/svc_plugin.h>

#include "usbrdr_main.h"


static void
usbrdr_send(usbrdrPlugin* usbrdr, const void *data, size_t len)
{
	STREAM* s;

	s = stream_new(len);
	stream_write(s, data, len);
	svc_plugin_send((rdpSvcPlugin*)usbrdr, s);
}

/* find connection context by connection id */
static USBRDR_CONNECTION_CONTEXT *
usbrdr_connection_find(usbrdrPlugin* usbrdr, sint64 id_connection)
{
	USBRDR_CONNECTION_CONTEXT *ptr;

	pthread_mutex_lock(&usbrdr->connection_list_lock);

	ptr = usbrdr->connection_list;

	while (ptr)
	{
		if (ptr->socket == id_connection) break;
		ptr = ptr->next;
	}

	pthread_mutex_unlock(&usbrdr->connection_list_lock);

	return ptr;
}

/* remove connection context by connection id */
static void 
usbrdr_connection_remove(usbrdrPlugin* usbrdr, USBRDR_CONNECTION_CONTEXT *ctx)
{
	USBRDR_CONNECTION_CONTEXT *ptr, *prevptr;

	pthread_mutex_lock(&usbrdr->connection_list_lock);

	prevptr = NULL;
	ptr = usbrdr->connection_list;

	while (ptr)
	{
		if (ptr == ctx) break;
		prevptr = ptr;
		ptr = ptr->next;
	}

	if (ptr)
	{
		if(prevptr) prevptr->next = ptr->next; else usbrdr->connection_list = ptr->next;
	}

	pthread_mutex_unlock(&usbrdr->connection_list_lock);
}

/* add connection context to the list */
static void 
usbrdr_connection_add(usbrdrPlugin* usbrdr, USBRDR_CONNECTION_CONTEXT *ptr)
{
	pthread_mutex_lock(&usbrdr->connection_list_lock);
	ptr->next = usbrdr->connection_list;
	usbrdr->connection_list = ptr;
	pthread_mutex_unlock(&usbrdr->connection_list_lock);
}

/* close all socket connections, terminate all threads */
static void 
usbrdr_process_rdp_cleanup(usbrdrPlugin* usbrdr)
{
	while (usbrdr->connection_list)
	{
		USBRDR_CONNECTION_CONTEXT *context = usbrdr->connection_list;

		usbrdr->connection_list = usbrdr->connection_list->next;

		DEBUG_USBRDR("USBRDR: cleaning up connection %08X...\n", context->socket);

		context->disconnecting = true;
		context->stop = true;
		shutdown(context->socket, SHUT_RD);
		pthread_join(*context->thread, NULL);

		DEBUG_USBRDR("USBRDR: connection %08X cleaned up\n", context->socket);

		xfree(context->thread);
		xfree(context);	
	}
}


static void *
socket_handler_function(void *data)
{
	USBRDR_CONNECTION_CONTEXT *context = (USBRDR_CONNECTION_CONTEXT*)data;
	usbrdrPlugin* usbrdr = context->usbrdr;
	void *buf;

	buf = xmalloc(USBRDR_RECV_BUFFER_SIZE);
	if(buf == NULL)
	{
		DEBUG_WARN("USBRDR: THREAD %08X no memory for recv buffer\n", context->socket);
		return NULL;
	}

	while (!context->stop)
	{
		int nrecv;
		USBRDR_RDP_HEADER header;

		nrecv = recv(context->socket, buf, USBRDR_RECV_BUFFER_SIZE,0);

		if (nrecv <= 0) 
		{
			DEBUG_USBRDR("USBRDR: THREAD %08X recv error\n", context->socket);
			break;
		}

		header.id_connection = context->socket;
		header.packet_size = nrecv;

		pthread_mutex_lock(&usbrdr->channel_send_lock);
		usbrdr_send(usbrdr, &header, sizeof(header));
		usbrdr_send(usbrdr, buf, nrecv);
		pthread_mutex_unlock(&usbrdr->channel_send_lock);
	}

	xfree(buf);

	if (!context->disconnecting)
	{
		USBRDR_RDP_HEADER header;

		DEBUG_USBRDR("USBRDR: THREAD %08X sending disconnect for\n", context->socket);

		header.id_connection = context->socket;
		header.packet_size = -1;

		pthread_mutex_lock(&usbrdr->channel_send_lock);
		usbrdr_send(usbrdr, &header, sizeof(header));
		pthread_mutex_unlock(&usbrdr->channel_send_lock);
	}

	shutdown(context->socket, SHUT_WR);
	close(context->socket);

	DEBUG_USBRDR("USBRDR: THREAD %08X terminating\n", context->socket);
	return NULL;
}

static void 
usbrdr_process_rdp_connect(usbrdrPlugin* usbrdr)
{
	int			s = -1;
	pthread_t	*socket_handler	= NULL;
	USBRDR_CONNECTION_CONTEXT *context = NULL;

	do /* while(0) */
	{
		USBRDR_RDP_HEADER reply_header;
	    struct sockaddr_in sin;
		int opt = 1;

		reply_header.id_connection = 0;
		reply_header.packet_size = -1;

		context = xmalloc(sizeof(*context));

		if (context == NULL)
		{
			DEBUG_WARN("USBRDR: no memory for connection context\n");
			break;
		}

		socket_handler = xmalloc(sizeof(*socket_handler));

		if (socket_handler == NULL)
		{
			DEBUG_WARN("USBRDR: no memory for thread\n");
			break;
		}

		s = socket(PF_INET, SOCK_STREAM, 0);

		if (s < 0) 
		{
			DEBUG_WARN("USBRDR: cannot create socket\n");
			break;
		}
		
		if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0)
		{
			DEBUG_WARN("USBRDR: cannot enable TCP_NODELAY option\n");
		}

		sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		sin.sin_port = htons(USBRDR_TCPPORT);
		sin.sin_family = AF_INET;

		if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) != 0) 
		{
			DEBUG_WARN("USBRDR: cannot connect to local server");
			break;
		}

		context->socket = s;
		context->thread = socket_handler;
		context->stop = false;
		context->disconnecting = false;
		context->usbrdr = usbrdr;

		pthread_mutex_lock(&usbrdr->channel_send_lock);
	
		if (pthread_create(socket_handler, NULL, (void *(*)(void *))socket_handler_function, context))
		{
			pthread_mutex_unlock(&usbrdr->channel_send_lock);
			DEBUG_WARN("USBRDR: cannot create thread\n");
			break;
		}

		reply_header.id_connection = s;
		reply_header.packet_size = 0;

		usbrdr_send(usbrdr, &reply_header, sizeof(reply_header));

		usbrdr_connection_add(usbrdr, context);

		pthread_mutex_unlock(&usbrdr->channel_send_lock);

		s = -1;
		socket_handler = NULL;
		context = NULL;

		DEBUG_USBRDR("USBRDR: new connection successful, connectionid is %08X\n", (uint32)reply_header.id_connection);
	} while(0);

	/* cleanup on error */
	if (s > 0) 
	{
		shutdown(s, SHUT_RDWR);
		close(s);
	}
	if (socket_handler) xfree(socket_handler);
	if (context) xfree(context);
}

static void 
usbrdr_process_rdp_disconnect(usbrdrPlugin* usbrdr)
{
	USBRDR_CONNECTION_CONTEXT *connection_context = usbrdr_connection_find(usbrdr, usbrdr->rdp_header.id_connection);	

	if (!connection_context) return;

	DEBUG_USBRDR("USBRDR: disconnecting %08X...\n", (uint32)usbrdr->rdp_header.id_connection);

	connection_context->disconnecting = true;
	connection_context->stop = true;
	shutdown(connection_context->socket, SHUT_RD);
	pthread_join(*connection_context->thread, NULL);

	usbrdr_connection_remove(usbrdr, connection_context);

	xfree(connection_context->thread);
	xfree(connection_context);
	DEBUG_USBRDR("USBRDR: connection %08X is now disconnected\n", (uint32)usbrdr->rdp_header.id_connection);
}

static int 
usbrdr_process_rdp_data_header(usbrdrPlugin* usbrdr)
{
	usbrdr->connection_context = usbrdr_connection_find(usbrdr, usbrdr->rdp_header.id_connection);	
	return 1;
}

static int 
usbrdr_process_rdp_data_chunk(usbrdrPlugin* usbrdr, void *data, size_t length)
{
	size_t tosend = (usbrdr->rdp_header.packet_size > length) ? length : usbrdr->rdp_header.packet_size;

	usbrdr->rdp_header.packet_size -= tosend;

	if (usbrdr->connection_context)
	{
		if( send(usbrdr->connection_context->socket, data, tosend, 0) <= 0 )
		{
			usbrdr->connection_context->stop = true;
			shutdown(usbrdr->connection_context->socket, SHUT_RD);
			pthread_join(*usbrdr->connection_context->thread, NULL);
			usbrdr_connection_remove(usbrdr, usbrdr->connection_context);
			xfree(usbrdr->connection_context->thread);
			xfree(usbrdr->connection_context);
			usbrdr->connection_context = NULL;
		}
	}
	return (usbrdr->rdp_header.packet_size > 0);
}


/* called by FreeRDP when remote connection is established */
static void usbrdr_process_connect(rdpSvcPlugin* plugin)
{
	usbrdrPlugin* usbrdr = (usbrdrPlugin*)plugin;

	DEBUG_USBRDR("usbrdr_process_connect");

	usbrdr->rdp_state = 0;
	usbrdr->connection_list = NULL;
	usbrdr->connection_context = NULL;
	memset(&usbrdr->rdp_header, 0, sizeof(&usbrdr->rdp_header));

	if (pthread_mutex_init(&usbrdr->channel_send_lock, NULL) != 0)
	{
		DEBUG_WARN("USBRDR: can not initialize channel access mutex\n");
		goto error;
	}

	if (pthread_mutex_init(&usbrdr->connection_list_lock, NULL) != 0)
	{
		DEBUG_WARN("USBRDR: can not initialize connection list access mutex\n");
		goto error;
	}

	return;

error:
	usbrdr->rdp_state = -1;
	return;
}

/* called by FreeRDP when data arrives to virtual channel */
static void usbrdr_process_receive(rdpSvcPlugin* plugin, STREAM* s)
{
	usbrdrPlugin* usbrdr = (usbrdrPlugin*)plugin;

	DEBUG_USBRDR("usbrdr_process_receive");

	switch(usbrdr->rdp_state)
	{
		case 0: /* state: waiting for header */

			if( stream_get_left(s) == sizeof(USBRDR_RDP_HEADER) )
			{
				memcpy(&usbrdr->rdp_header, stream_get_tail(s), sizeof(USBRDR_RDP_HEADER));

				if( usbrdr->rdp_header.id_connection == 0 && usbrdr->rdp_header.packet_size == 0 )
				{
					DEBUG_USBRDR("USBRDR: connect request\n");
					usbrdr_process_rdp_connect(usbrdr);

				} else if( usbrdr->rdp_header.id_connection != 0 && usbrdr->rdp_header.packet_size == -1 )
				{
					DEBUG_USBRDR("USBRDR: disconnect request for %08X\n", 
						(uint32)usbrdr->rdp_header.id_connection);
					usbrdr_process_rdp_disconnect(usbrdr);

				} else if( usbrdr->rdp_header.id_connection == 0 && usbrdr->rdp_header.packet_size == -1 )
				{
					DEBUG_USBRDR("USBRDR: cleanup request\n");
					usbrdr_process_rdp_cleanup(usbrdr);

				} else
				{
					DEBUG_USBRDR("USBRDR: data header\n");
					if (usbrdr_process_rdp_data_header(usbrdr))
						usbrdr->rdp_state = 1; /* change state to: wait for data */
				} 
			} else
			{
				DEBUG_WARN("USBRDR: error, wrong header size\n");
			}
			break;

		case 1: /* state: waiting for data */
			DEBUG_USBRDR("USBRDR: data chunk %d bytes\n", usbrdr->rdp_header.packet_size);
			if (!usbrdr_process_rdp_data_chunk(usbrdr, stream_get_tail(s), stream_get_left(s)))
				usbrdr->rdp_state = 0; /* change state to: wait for header */
			break;

		case -1: /* state: initialization failed in usbrdr_process_connect */
			break;

		default: /* omg! something is wrong! */
			DEBUG_WARN("USBRDR: invalid rdp connection state");
			break;			
	}

	stream_free(s);
}

/* called by FreeRDP when there is an outstanding event */
static void usbrdr_process_event(rdpSvcPlugin* plugin, RDP_EVENT* event)
{
	switch (event->event_type)
	{
		default:
			DEBUG_WARN("unknown event type %d", event->event_type);
			break;
	}

	freerdp_event_free(event);
}

/* called by FreeRDP after disconnect */
static void usbrdr_process_terminate(rdpSvcPlugin* plugin)
{
	usbrdrPlugin* usbrdr = (usbrdrPlugin*)plugin;
	usbrdr_process_rdp_cleanup(usbrdr);
	xfree(plugin);
}

int VirtualChannelEntry(PCHANNEL_ENTRY_POINTS pEntryPoints)
{
#if USBRDR_RDP_VCHANNEL_ENABLE
	usbrdrPlugin* usbrdr_rdp;
#endif

#if USBRDR_TS_VCHANNEL_ENABLE
	usbrdrPlugin* usbrdr_ts;
#endif

#if USBRDR_RDP_VCHANNEL_ENABLE

	usbrdr_rdp = xnew(usbrdrPlugin);

	if (usbrdr_rdp)
	{
		usbrdr_rdp->name = USBRDR_VCHANNEL_NAME_RDP;

		usbrdr_rdp->plugin.channel_def.options = 
			CHANNEL_OPTION_INITIALIZED | 
			CHANNEL_OPTION_ENCRYPT_RDP;

		strcpy(usbrdr_rdp->plugin.channel_def.name, usbrdr_rdp->name);

		usbrdr_rdp->plugin.connect_callback = usbrdr_process_connect;
		usbrdr_rdp->plugin.receive_callback = usbrdr_process_receive;
		usbrdr_rdp->plugin.event_callback = usbrdr_process_event;
		usbrdr_rdp->plugin.terminate_callback = usbrdr_process_terminate;

		svc_plugin_init((rdpSvcPlugin*)usbrdr_rdp, pEntryPoints);
	}

#endif

#if USBRDR_TS_VCHANNEL_ENABLE

	usbrdr_ts = xnew(usbrdrPlugin);

	if (usbrdr_ts)
	{
		usbrdr_ts->name = USBRDR_VCHANNEL_NAME_TS;

		usbrdr_ts->plugin.channel_def.options = 
			CHANNEL_OPTION_INITIALIZED | 
			CHANNEL_OPTION_ENCRYPT_RDP;

		strcpy(usbrdr_ts->plugin.channel_def.name, usbrdr_ts->name);

		usbrdr_ts->plugin.connect_callback = usbrdr_process_connect;
		usbrdr_ts->plugin.receive_callback = usbrdr_process_receive;
		usbrdr_ts->plugin.event_callback = usbrdr_process_event;
		usbrdr_ts->plugin.terminate_callback = usbrdr_process_terminate;

		svc_plugin_init((rdpSvcPlugin*)usbrdr_ts, pEntryPoints);
	}

#endif

	return 1;
}
