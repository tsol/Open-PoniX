/**
 * FreeRDP: A Remote Desktop Protocol client.
 * USB Redirector (www.incentivespro.com) Virtual Channel Support
 *
 * Copyright 2012 SimplyCore Ltd.
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

#ifndef __USBRDR_MAIN_H
#define __USBRDR_MAIN_H

#include <winpr/wtypes.h>
#include <winpr/windows.h>
#include <winpr/winsock.h>
#include <winpr/stream.h>
#include <freerdp/utils/debug.h>

#ifndef _WIN32
#define closesocket close
typedef int SOCKET;
#endif

// for RDP Edition
#define USBRDR_RDP_VCHANNEL_ENABLE	0xDEEEEEEA // set to 0 to disable this virtual channel
#define USBRDR_VCHANNEL_NAME_RDP	"USBRDR"
// for TS Edition
#define USBRDR_TS_VCHANNEL_ENABLE	0xDEEEEEEB // set to 0 to disable this virtual channel
#define USBRDR_VCHANNEL_NAME_TS		"USBTS0"
// General parameters
#define USBRDR_TCPPORT				32032
#define USBRDR_RECV_BUFFER_SIZE		(4096*24)

struct usbrdr_plugin;

#pragma pack (push, 1)

struct _USBRDR_RDP_HEADER {
	LONG64 id_connection;
	LONG32 packet_size;
};
typedef struct _USBRDR_RDP_HEADER USBRDR_RDP_HEADER;

#pragma pack (pop)

struct _USBRDR_CONNECTION_CONTEXT {
	struct _USBRDR_CONNECTION_CONTEXT *next;
	struct usbrdr_plugin *usbrdr;
	SOCKET socket;
	HANDLE hThread;
	BOOL bStop;
	BOOL bDisconnecting;
};
typedef struct _USBRDR_CONNECTION_CONTEXT USBRDR_CONNECTION_CONTEXT;

struct usbrdr_plugin
{
	rdpSvcPlugin plugin;
	char *name;
	int rdp_state;
	struct _USBRDR_RDP_HEADER rdp_header;
	HANDLE hSendMutex;
	HANDLE hListMutex;
	struct _USBRDR_CONNECTION_CONTEXT *connection_list;
	struct _USBRDR_CONNECTION_CONTEXT *connection_context;
};
typedef struct usbrdr_plugin usbrdrPlugin;

#ifdef WITH_DEBUG_USBRDR
#define DEBUG_USBRDR(fmt, ...) DEBUG_CLASS(USBRDR, fmt, ## __VA_ARGS__)
#else
#define DEBUG_USBRDR(fmt, ...) DEBUG_NULL(fmt, ## __VA_ARGS__)
#endif

#endif /* __USBRDR_MAIN_H */
