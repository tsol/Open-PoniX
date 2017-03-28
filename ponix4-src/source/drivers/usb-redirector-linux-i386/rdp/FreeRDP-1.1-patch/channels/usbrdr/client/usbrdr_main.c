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

#ifndef _WIN32
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif
#include <winpr/synch.h>
#include <freerdp/utils/svc_plugin.h>
#include "usbrdr_main.h"


static void
usbrdr_send(usbrdrPlugin* usbrdr, const void *data, size_t len)
{
	wStream* s;

	s = Stream_New(NULL, len);
	Stream_Write(s, data, len);
	svc_plugin_send((rdpSvcPlugin*)usbrdr, s);
}


/* find connection context by connection id */
static USBRDR_CONNECTION_CONTEXT *
usbrdr_connection_find(usbrdrPlugin* usbrdr, LONG64 id_connection)
{
	USBRDR_CONNECTION_CONTEXT *ptr;

	WaitForSingleObject(usbrdr->hListMutex, INFINITE);

	ptr = usbrdr->connection_list;

	while (ptr)
	{
		if (ptr->socket == id_connection) break;
		ptr = ptr->next;
	}

	ReleaseMutex(usbrdr->hListMutex);

	return ptr;
}

/* remove connection context by connection id */
static void 
usbrdr_connection_remove(usbrdrPlugin* usbrdr, USBRDR_CONNECTION_CONTEXT *ctx)
{
	USBRDR_CONNECTION_CONTEXT *ptr, *prevptr;

	WaitForSingleObject(usbrdr->hListMutex, INFINITE);

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

	ReleaseMutex(usbrdr->hListMutex);
}

/* add connection context to the list */
static void 
usbrdr_connection_add(usbrdrPlugin* usbrdr, USBRDR_CONNECTION_CONTEXT *ptr)
{
	WaitForSingleObject(usbrdr->hListMutex, INFINITE);
	ptr->next = usbrdr->connection_list;
	usbrdr->connection_list = ptr;
	ReleaseMutex(usbrdr->hListMutex);
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

		context->bDisconnecting = TRUE;
		context->bStop = TRUE;
		shutdown(context->socket, SHUT_RD);
		WaitForSingleObject(context->hThread, INFINITE);
		CloseHandle(context->hThread);
		free(context);

		DEBUG_USBRDR("USBRDR: connection %08X cleaned up\n", context->socket);
	}
}

static void *
socket_handler_function(void *data)
{
	USBRDR_CONNECTION_CONTEXT *context = (USBRDR_CONNECTION_CONTEXT*)data;
	usbrdrPlugin* usbrdr = context->usbrdr;
	void *buf;

	buf = malloc(USBRDR_RECV_BUFFER_SIZE);
	if(buf == NULL)
	{
		DEBUG_WARN("USBRDR: THREAD %08X no memory for recv buffer\n", context->socket);
		return NULL;
	}

	while (!context->bStop)
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

		WaitForSingleObject(usbrdr->hSendMutex, INFINITE);
		usbrdr_send(usbrdr, &header, sizeof(header));
		usbrdr_send(usbrdr, buf, nrecv);
		ReleaseMutex(usbrdr->hSendMutex);
	}

	free(buf);

	if (!context->bDisconnecting)
	{
		USBRDR_RDP_HEADER header;

		DEBUG_USBRDR("USBRDR: THREAD %08X sending disconnect for\n", context->socket);

		header.id_connection = context->socket;
		header.packet_size = -1;

		WaitForSingleObject(usbrdr->hSendMutex, INFINITE);
		usbrdr_send(usbrdr, &header, sizeof(header));
		ReleaseMutex(usbrdr->hSendMutex);
	}

	shutdown(context->socket, SHUT_WR);
	closesocket(context->socket);

	DEBUG_USBRDR("USBRDR: THREAD %08X terminating\n", context->socket);
	return NULL;
}

static void 
usbrdr_process_rdp_connect(usbrdrPlugin* usbrdr)
{
	int			s = -1;
	USBRDR_CONNECTION_CONTEXT *context = NULL;

	do /* while(0) */
	{
		USBRDR_RDP_HEADER reply_header;
	    struct sockaddr_in sin;
		int opt = 1;

		reply_header.id_connection = 0;
		reply_header.packet_size = -1;

		context = malloc(sizeof(*context));

		if (context == NULL)
		{
			DEBUG_WARN("USBRDR: no memory for connection context\n");
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

		WaitForSingleObject(usbrdr->hSendMutex, INFINITE);

		context->socket = s;
		context->bStop = FALSE;
		context->bDisconnecting = FALSE;
		context->usbrdr = usbrdr;
		context->hThread = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE) socket_handler_function, context, CREATE_SUSPENDED, NULL);
	
		if (context->hThread == NULL)
		{
			ReleaseMutex(usbrdr->hSendMutex);
			DEBUG_WARN("USBRDR: cannot create thread\n");
			break;
		}

		ResumeThread(context->hThread);

		reply_header.id_connection = s;
		reply_header.packet_size = 0;

		usbrdr_send(usbrdr, &reply_header, sizeof(reply_header));

		usbrdr_connection_add(usbrdr, context);

		ReleaseMutex(usbrdr->hSendMutex);

		s = -1;
		context = NULL;

		DEBUG_USBRDR("USBRDR: new connection successful, connectionid is %08X\n", (LONG32)reply_header.id_connection);
	} while(0);

	/* cleanup on error */
	if (s > 0) 
	{
		shutdown(s, SHUT_RDWR);
		closesocket(s);
	}
	if (context) free(context);
}

static void 
usbrdr_process_rdp_disconnect(usbrdrPlugin* usbrdr)
{
	USBRDR_CONNECTION_CONTEXT *connection_context = usbrdr_connection_find(usbrdr, usbrdr->rdp_header.id_connection);	

	if (!connection_context) return;

	DEBUG_USBRDR("USBRDR: disconnecting %08X...\n", (LONG32)usbrdr->rdp_header.id_connection);

	connection_context->bDisconnecting = TRUE;
	connection_context->bStop = TRUE;
	shutdown(connection_context->socket, SHUT_RD);
	WaitForSingleObject(connection_context->hThread, INFINITE);
	CloseHandle(connection_context->hThread);

	usbrdr_connection_remove(usbrdr, connection_context);

	free(connection_context);
	DEBUG_USBRDR("USBRDR: connection %08X is now disconnected\n", (LONG32)usbrdr->rdp_header.id_connection);
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
			usbrdr->connection_context->bStop = TRUE;
			shutdown(usbrdr->connection_context->socket, SHUT_RD);
			WaitForSingleObject(usbrdr->connection_context->hThread, INFINITE);
			CloseHandle(usbrdr->connection_context->hThread);
			usbrdr_connection_remove(usbrdr, usbrdr->connection_context);
			free(usbrdr->connection_context);
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

	usbrdr->hSendMutex = CreateMutex(NULL, FALSE, NULL);
    
	if (usbrdr->hSendMutex == NULL)
	{
		DEBUG_WARN("USBRDR: can not initialize channel access mutex\n");
		goto error;
	}

	usbrdr->hListMutex = CreateMutex(NULL, FALSE, NULL);

	if (usbrdr->hListMutex == NULL)
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
static void usbrdr_process_receive(rdpSvcPlugin* plugin, wStream* s)
{
	usbrdrPlugin* usbrdr = (usbrdrPlugin*)plugin;

	DEBUG_USBRDR("usbrdr_process_receive");

	switch(usbrdr->rdp_state)
	{
		case 0: /* state: waiting for header */

			if( Stream_GetRemainingLength(s) == sizeof(USBRDR_RDP_HEADER) )
			{
				memcpy(&usbrdr->rdp_header, Stream_Pointer(s), sizeof(USBRDR_RDP_HEADER));

				if( usbrdr->rdp_header.id_connection == 0 && usbrdr->rdp_header.packet_size == 0 )
				{
					DEBUG_USBRDR("USBRDR: connect request\n");
					usbrdr_process_rdp_connect(usbrdr);

				} else if( usbrdr->rdp_header.id_connection != 0 && usbrdr->rdp_header.packet_size == -1 )
				{
					DEBUG_USBRDR("USBRDR: disconnect request for %08X\n", 
						(LONG32)usbrdr->rdp_header.id_connection);
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
			if (!usbrdr_process_rdp_data_chunk(usbrdr, Stream_Pointer(s), Stream_GetRemainingLength(s)))
				usbrdr->rdp_state = 0; /* change state to: wait for header */
			break;

		case -1: /* state: initialization failed in usbrdr_process_connect */
			break;

		default: /* omg! something is wrong! */
			DEBUG_WARN("USBRDR: invalid rdp connection state");
			break;			
	}

	Stream_Free(s, TRUE);
}

/* called by FreeRDP when there is an outstanding event */
static void usbrdr_process_event(rdpSvcPlugin* plugin, wMessage* event)
{
	switch (GetMessageType(event->id))
	{
		default:
			DEBUG_WARN("USBRDR: unknown event type %d", GetMessageType(event->id));
			break;
	}

	freerdp_event_free(event);
}

/* called by FreeRDP after disconnect */
static void usbrdr_process_terminate(rdpSvcPlugin* plugin)
{
	usbrdrPlugin* usbrdr = (usbrdrPlugin*)plugin;
	usbrdr_process_rdp_cleanup(usbrdr);
	if (usbrdr->hListMutex)
		CloseHandle(usbrdr->hListMutex);
	if (usbrdr->hSendMutex)
		CloseHandle(usbrdr->hSendMutex);
	free(plugin);
}

#define VirtualChannelEntry	usbrdr_VirtualChannelEntry

int VirtualChannelEntry(PCHANNEL_ENTRY_POINTS pEntryPoints)
{
#if USBRDR_RDP_VCHANNEL_ENABLE
	usbrdrPlugin* usbrdr_rdp;
#endif

#if USBRDR_TS_VCHANNEL_ENABLE
	usbrdrPlugin* usbrdr_ts;
#endif

#if USBRDR_RDP_VCHANNEL_ENABLE

	usbrdr_rdp = (usbrdrPlugin*) malloc(sizeof(usbrdrPlugin));

	if (usbrdr_rdp)
	{
		ZeroMemory(usbrdr_rdp, sizeof(*usbrdr_rdp));

		usbrdr_rdp->name = USBRDR_VCHANNEL_NAME_RDP;

		usbrdr_rdp->plugin.channel_def.options =
				CHANNEL_OPTION_INITIALIZED |
				CHANNEL_OPTION_ENCRYPT_RDP;

		strcpy(usbrdr_rdp->plugin.channel_def.name, usbrdr_rdp->name);

		usbrdr_rdp->plugin.connect_callback = usbrdr_process_connect;
		usbrdr_rdp->plugin.receive_callback = usbrdr_process_receive;
		usbrdr_rdp->plugin.event_callback = usbrdr_process_event;
		usbrdr_rdp->plugin.terminate_callback = usbrdr_process_terminate;

		svc_plugin_init((rdpSvcPlugin*) usbrdr_rdp, pEntryPoints);
	}

#endif

#if USBRDR_TS_VCHANNEL_ENABLE

	usbrdr_ts = (usbrdrPlugin*) malloc(sizeof(usbrdrPlugin));

	if (usbrdr_ts)
	{
		ZeroMemory(usbrdr_ts, sizeof(*usbrdr_ts));

		usbrdr_ts->name = USBRDR_VCHANNEL_NAME_TS;

		usbrdr_ts->plugin.channel_def.options =
				CHANNEL_OPTION_INITIALIZED |
				CHANNEL_OPTION_ENCRYPT_RDP;

		strcpy(usbrdr_ts->plugin.channel_def.name, usbrdr_ts->name);

		usbrdr_ts->plugin.connect_callback = usbrdr_process_connect;
		usbrdr_ts->plugin.receive_callback = usbrdr_process_receive;
		usbrdr_ts->plugin.event_callback = usbrdr_process_event;
		usbrdr_ts->plugin.terminate_callback = usbrdr_process_terminate;

		svc_plugin_init((rdpSvcPlugin*) usbrdr_ts, pEntryPoints);
	}

#endif

	return 1;
}
