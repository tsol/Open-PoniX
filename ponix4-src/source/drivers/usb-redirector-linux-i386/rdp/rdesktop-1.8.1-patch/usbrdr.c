/*  -*- c-basic-offset: 8 -*-
+   rdesktop: A Remote Desktop Protocol client.
+   Support for the USB channel
+
+   This program is free software; you can redistribute it and/or modify
+   it under the terms of the GNU General Public License as published by
+   the Free Software Foundation; either version 2 of the License, or
+   (at your option) any later version.
+
+   This program is distributed in the hope that it will be useful,
+   but WITHOUT ANY WARRANTY; without even the implied warranty of
+   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+   GNU General Public License for more details.
+
+   You should have received a copy of the GNU General Public License
+   along with this program; if not, write to the Free Software
+   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
+*/

#include "rdesktop.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#ifdef WITH_DEBUG_USBRDR
#define DEBUG_USBRDR(args) printf args;
#else
#define DEBUG_USBRDR(args)
#endif

// for RDP Edition
#define USBRDR_RDP_VCHANNEL_ENABLE	0xDEEEEEEA // set to 0 to disable this virtual channel
#define USBRDR_VCHANNEL_NAME_RDP	"USBRDR"
// for TS Edition
#define USBRDR_TS_VCHANNEL_ENABLE	0xDEEEEEEB // set to 0 to disable this virtual channel
#define USBRDR_VCHANNEL_NAME_TS		"USBTS0"
// General parameters
#define USBRDR_TCPPORT				32032
#define USBRDR_RECV_BUFFER_SIZE		100000

#pragma pack (push, 1)

typedef struct _rdp_header {
	__int64_t id_connection;
	__int32_t packet_size;
} RDP_HEADER, *PRDP_HEADER;

#pragma pack (pop)

struct _USBRDR_VCHANNEL;

typedef struct _USBRDR_CONNECTION_CONTEXT
{
	struct _USBRDR_CONNECTION_CONTEXT *next;
	struct _USBRDR_VCHANNEL *vchannel;
	int socket;
	pthread_t *thread;
	int stop;
	int disconnecting;
} USBRDR_CONNECTION_CONTEXT;

typedef struct _USBRDR_VCHANNEL
{
	char *name;
	VCHANNEL *channel;
	pthread_mutex_t send_lock;
	RDP_HEADER cur_header;
	USBRDR_CONNECTION_CONTEXT *cur_context;
	int state;
	USBRDR_CONNECTION_CONTEXT *connection_list;
	pthread_mutex_t connection_list_lock;
} USBRDR_VCHANNEL;

static pthread_mutex_t **usbrdr_mutex = NULL;

#if USBRDR_RDP_VCHANNEL_ENABLE
static USBRDR_VCHANNEL vchannel_rdp;
#endif

#if USBRDR_TS_VCHANNEL_ENABLE
static USBRDR_VCHANNEL vchannel_ts;
#endif

static void usbrdr_send(USBRDR_VCHANNEL *vchannel, const void *output, size_t len);

/* find connection context by connection id */
static USBRDR_CONNECTION_CONTEXT *
usbrdr_connection_find(USBRDR_VCHANNEL *vchannel, __int64_t id_connection)
{
	USBRDR_CONNECTION_CONTEXT *ptr;

	pthread_mutex_lock(&vchannel->connection_list_lock);

	ptr = vchannel->connection_list;

	while (ptr)
	{
		if (ptr->socket == id_connection) break;
		ptr = ptr->next;
	}

	pthread_mutex_unlock(&vchannel->connection_list_lock);

	return ptr;
}

/* remove connection context by connection id */
static void 
usbrdr_connection_remove(USBRDR_VCHANNEL *vchannel, USBRDR_CONNECTION_CONTEXT *ctx)
{
	USBRDR_CONNECTION_CONTEXT *ptr, *prevptr;

	pthread_mutex_lock(&vchannel->connection_list_lock);

	prevptr = NULL;
	ptr = vchannel->connection_list;

	while (ptr)
	{
		if (ptr == ctx) break;
		prevptr = ptr;
		ptr = ptr->next;
	}

	if (ptr)
	{
		if (prevptr) prevptr->next = ptr->next; else vchannel->connection_list = ptr->next;
	}

	pthread_mutex_unlock(&vchannel->connection_list_lock);
}

/* add connection context to the list */
static void 
usbrdr_connection_add(USBRDR_VCHANNEL *vchannel, USBRDR_CONNECTION_CONTEXT *ptr)
{
	pthread_mutex_lock(&vchannel->connection_list_lock);
	ptr->next = vchannel->connection_list;
	vchannel->connection_list = ptr;
	pthread_mutex_unlock(&vchannel->connection_list_lock);
}

static void *
socket_handler_function(void *data)
{
	USBRDR_CONNECTION_CONTEXT *context = (USBRDR_CONNECTION_CONTEXT*)data;
	void *buf;

	buf = xmalloc(USBRDR_RECV_BUFFER_SIZE);
	if(buf == NULL)
	{
		error("USBRDR: THREAD %08X no memory for recv buffer\n", context->socket);
		return NULL;
	}

	while (!context->stop)
	{
		int nrecv;
		RDP_HEADER header;

		nrecv = recv(context->socket, buf, USBRDR_RECV_BUFFER_SIZE,0);

		if (nrecv <= 0) 
		{
			DEBUG_USBRDR(("USBRDR: THREAD %08X recv error\n", context->socket));
			break;
		}

		header.id_connection = context->socket;
		header.packet_size = nrecv;

		pthread_mutex_lock(&context->vchannel->send_lock);
		usbrdr_send(context->vchannel, &header, sizeof(header));
		usbrdr_send(context->vchannel, buf, nrecv);
		pthread_mutex_unlock(&context->vchannel->send_lock);
	}

	xfree(buf);

	if (!context->disconnecting)
	{
		RDP_HEADER header;

		DEBUG_USBRDR(("USBRDR: THREAD %08X sending disconnect for\n", context->socket));

		header.id_connection = context->socket;
		header.packet_size = -1;

		pthread_mutex_lock(&context->vchannel->send_lock);
		usbrdr_send(context->vchannel, &header, sizeof(header));
		pthread_mutex_unlock(&context->vchannel->send_lock);
	}

	shutdown(context->socket, SHUT_WR);
	close(context->socket);

	DEBUG_USBRDR(("USBRDR: THREAD %08X terminating\n", context->socket));
	return NULL;
}


static void 
usbrdr_process_cleanup(USBRDR_VCHANNEL *vchannel)
{
	/* close all connections */

	while (vchannel->connection_list)
	{
		USBRDR_CONNECTION_CONTEXT *context = vchannel->connection_list;

		vchannel->connection_list = vchannel->connection_list->next;

		DEBUG_USBRDR(("USBRDR: cleaning up connection %08X...\n", context->socket));

		context->disconnecting = 1;
		context->stop = 1;
		shutdown(context->socket, SHUT_RD);
		pthread_join(*context->thread, NULL);

		DEBUG_USBRDR(("USBRDR: connection %08X cleaned up\n", context->socket));

		xfree(context->thread);
		xfree(context);	
	}
}

static void 
usbrdr_process_connect(USBRDR_VCHANNEL *vchannel)
{
	int							s = -1;
	pthread_t 					*socket_handler	= NULL;
	USBRDR_CONNECTION_CONTEXT	*context 		= NULL;


	do /*while(0)*/
	{
		RDP_HEADER reply_header;
	    struct sockaddr_in sin;
		int opt = 1;

		reply_header.id_connection = 0;
		reply_header.packet_size = -1;

		context = xmalloc(sizeof(*context));

		if (context == NULL)
		{
			error("USBRDR: no memory for connection context\n");
			break;
		}

		socket_handler = xmalloc(sizeof(*socket_handler));

		if (socket_handler == NULL)
		{
			error("USBRDR: no memory for thread\n");
			break;
		}

		s = socket(PF_INET, SOCK_STREAM, 0);

		if (s < 0) 
		{
			error("USBRDR: cannot create socket\n");
			break;
		}
		
		if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0)
		{
			warning("USBRDR: cannot enable TCP_NODELAY option\n");
		}

		sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		sin.sin_port = htons(USBRDR_TCPPORT);
		sin.sin_family = AF_INET;

		if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) != 0) 
		{
			error("USBRDR: cannot connect to local server");
			break;
		}

		context->next = NULL;
		context->vchannel = vchannel;
		context->socket = s;
		context->thread = socket_handler;
		context->stop = 0;
		context->disconnecting = 0;

		pthread_mutex_lock(&vchannel->send_lock);
	
		if (pthread_create(socket_handler, NULL, (void *(*)(void *))socket_handler_function, context))
		{
			pthread_mutex_unlock(&vchannel->send_lock);
			error("USBRDR: cannot create thread\n");
			break;
		}

		reply_header.id_connection = s;
		reply_header.packet_size = 0;

		usbrdr_send(vchannel, &reply_header, sizeof(reply_header));

		usbrdr_connection_add(vchannel, context);

		pthread_mutex_unlock(&vchannel->send_lock);

		s = -1;
		socket_handler = NULL;
		context = NULL;


		DEBUG_USBRDR(("USBRDR: new connection successful, connectionid is %08X\n", (__int32_t)reply_header.id_connection));
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
usbrdr_process_disconnect(USBRDR_VCHANNEL *vchannel)
{
	USBRDR_CONNECTION_CONTEXT *context = usbrdr_connection_find(vchannel, vchannel->cur_header.id_connection);

	if (!context) return;

	DEBUG_USBRDR(("USBRDR: disconnecting %08X...\n", (__int32_t)vchannel->cur_header.id_connection));

	context->disconnecting = 1;
	context->stop = 1;
	shutdown(context->socket, SHUT_RD);
	pthread_join(*context->thread, NULL);

	usbrdr_connection_remove(vchannel, context);

	xfree(context->thread);
	xfree(context);
	DEBUG_USBRDR(("USBRDR: connection %08X is now disconnected\n", (__int32_t)vchannel->cur_header.id_connection));
}

static int 
usbrdr_process_data_header(USBRDR_VCHANNEL *vchannel)
{
	vchannel->cur_context = usbrdr_connection_find(vchannel, vchannel->cur_header.id_connection);
	return 1;
}

static int 
usbrdr_process_data_chunk(USBRDR_VCHANNEL *vchannel, void *data, size_t length)
{
	size_t tosend = (vchannel->cur_header.packet_size > length) ? length : vchannel->cur_header.packet_size;

	vchannel->cur_header.packet_size -= tosend;

	if (vchannel->cur_context)
	{
		if( send(vchannel->cur_context->socket, data, tosend, 0) <= 0 )
		{
			vchannel->cur_context->stop = 1;
			shutdown(vchannel->cur_context->socket, SHUT_RD);
			pthread_join(*vchannel->cur_context->thread, NULL);
			usbrdr_connection_remove(vchannel, vchannel->cur_context);
			xfree(vchannel->cur_context->thread);
			xfree(vchannel->cur_context);
			vchannel->cur_context = NULL;
		}
	}
	return (vchannel->cur_header.packet_size > 0);
}

static void 
usbrdr_process_chunk(USBRDR_VCHANNEL *vchannel, void *data, size_t length)
{
	switch(vchannel->state)
	{
		case 0: // waiting for rdp_header

			if( length == sizeof(RDP_HEADER) )
			{
				memcpy(&vchannel->cur_header, data, sizeof(RDP_HEADER));

				if( vchannel->cur_header.id_connection == 0 && vchannel->cur_header.packet_size == 0 )
				{
					DEBUG_USBRDR(("USBRDR: connect request\n"));
					usbrdr_process_connect(vchannel);

				} else if( vchannel->cur_header.id_connection != 0 && vchannel->cur_header.packet_size == -1 )
				{
					DEBUG_USBRDR(("USBRDR: disconnect request for %08X\n", (__int32_t)vchannel->cur_header.id_connection));
					usbrdr_process_disconnect(vchannel);

				} else if( vchannel->cur_header.id_connection == 0 && vchannel->cur_header.packet_size == -1 )
				{
					DEBUG_USBRDR(("USBRDR: cleanup request\n"));
					usbrdr_process_cleanup(vchannel);

				} else
				{
					DEBUG_USBRDR(("USBRDR: data header\n"));
					if (usbrdr_process_data_header(vchannel)) vchannel->state = 1; // wait for data
				} 
			} else
			{
				error("USBRDR: error, wrong header size\n");
			}
			break;

		case 1: // waiting for data
			DEBUG_USBRDR(("USBRDR: data chunk %d bytes\n", vchannel->cur_header.packet_size));
			if( !usbrdr_process_data_chunk(vchannel, data, length) ) vchannel->state = 0; // wait for header
			break;

		default: // something wrong
			break;			
	}
}

/* Process new data from the virtual channel */
static void
usbrdr_process(USBRDR_VCHANNEL *vchannel, STREAM s)
{
	unsigned int len;

	len = s->end - s->p;

#if 0
	printf("USBRDR: %s recv %d\n", vchannel->name, len);
	hexdump(s->p, len);
#endif

	usbrdr_process_chunk(vchannel, s->p, len);
}

#if USBRDR_RDP_VCHANNEL_ENABLE
static void
usbrdr_process_rdp(STREAM s)
{
	usbrdr_process(&vchannel_rdp, s);
}
#endif

#if USBRDR_TS_VCHANNEL_ENABLE
static void
usbrdr_process_ts(STREAM s)
{
	usbrdr_process(&vchannel_ts, s);
}
#endif

/* Initialize this module: Register the usbrdr channel */
RD_BOOL
usbrdr_init(void)
{
#if USBRDR_RDP_VCHANNEL_ENABLE
	vchannel_rdp.name = USBRDR_VCHANNEL_NAME_RDP;
	vchannel_rdp.channel = NULL;
	vchannel_rdp.cur_context = NULL;
	vchannel_rdp.state = 0;
	vchannel_rdp.connection_list = NULL;

	memset(&vchannel_rdp.cur_header,0,sizeof(vchannel_rdp.cur_header));

	if (pthread_mutex_init(&vchannel_rdp.send_lock, NULL) != 0)
	{
		error("USBRDR: can not initialize channel access mutex\n");
		return False;
	}

	if (pthread_mutex_init(&vchannel_rdp.connection_list_lock, NULL) != 0)
	{
		error("USBRDR: can not initialize connection list access mutex\n");
		return False;
	}
	vchannel_rdp.channel = 
		channel_register(vchannel_rdp.name, CHANNEL_OPTION_INITIALIZED | CHANNEL_OPTION_ENCRYPT_RDP,
				 usbrdr_process_rdp);
#endif

#if USBRDR_TS_VCHANNEL_ENABLE
	vchannel_ts.name = USBRDR_VCHANNEL_NAME_TS;
	vchannel_ts.channel = NULL;
	vchannel_ts.cur_context = NULL;
	vchannel_ts.state = 0;
	vchannel_ts.connection_list = NULL;

	memset(&vchannel_ts.cur_header,0,sizeof(vchannel_ts.cur_header));

	if (pthread_mutex_init(&vchannel_ts.send_lock, NULL) != 0)
	{
		error("USBRDR: can not initialize channel access mutex\n");
		return False;
	}

	if (pthread_mutex_init(&vchannel_ts.connection_list_lock, NULL) != 0)
	{
		error("USBRDR: can not initialize connection list access mutex\n");
		return False;
	}

	vchannel_ts.channel = 
		channel_register(vchannel_ts.name, CHANNEL_OPTION_INITIALIZED | CHANNEL_OPTION_ENCRYPT_RDP,
				 usbrdr_process_ts);
#endif

	DEBUG_USBRDR(("USBRDR: init complete\n"));

	return True;
}

/* Send data to channel */
static void 
usbrdr_send(USBRDR_VCHANNEL *vchannel, const void *data, size_t len)
{
	STREAM s;

	usbrdr_lock(USBRDR_LOCK_RDPDR);

	s = channel_init(vchannel->channel, len);
	out_uint8p(s, data, len) s_mark_end(s);

#if 0
		printf("USBRDR: send %d\n", len);
		hexdump(s->channel_hdr+8, s->end - s->channel_hdr-8);
#endif

	channel_send(s, vchannel->channel);

	usbrdr_unlock(USBRDR_LOCK_RDPDR);
}

void
usbrdr_lock(int lock)
{
	if (!usbrdr_mutex)
	{
		int i;

		usbrdr_mutex =
			(pthread_mutex_t **) xmalloc(sizeof(pthread_mutex_t *) * USBRDR_LOCK_LAST);

		for (i = 0; i < USBRDR_LOCK_LAST; i++)
		{
			usbrdr_mutex[i] = NULL;
		}
	}

	if (!usbrdr_mutex[lock])
	{
		usbrdr_mutex[lock] = (pthread_mutex_t *) xmalloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(usbrdr_mutex[lock], NULL);
	}

	pthread_mutex_lock(usbrdr_mutex[lock]);
}

void
usbrdr_unlock(int lock)
{
	pthread_mutex_unlock(usbrdr_mutex[lock]);
}
