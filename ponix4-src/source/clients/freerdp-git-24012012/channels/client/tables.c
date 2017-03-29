/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Static Entry Point Tables
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
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

#include "tables.h"



extern void parallel_DeviceServiceEntry();
extern void drive_DeviceServiceEntry();
extern void serial_DeviceServiceEntry();

extern void tsmf_DVCPluginEntry();
extern void audin_DVCPluginEntry();

extern void cliprdr_VirtualChannelEntry();
extern void rail_VirtualChannelEntry();
extern void drdynvc_VirtualChannelEntry();
extern void rdpsnd_VirtualChannelEntry();
extern void rdpdr_VirtualChannelEntry();


const STATIC_ENTRY CLIENT_DeviceServiceEntry_TABLE[] =
{

	{ "parallel", parallel_DeviceServiceEntry },
	{ "drive", drive_DeviceServiceEntry },
	{ "serial", serial_DeviceServiceEntry },
	{ "", NULL }
};
const STATIC_ENTRY CLIENT_DVCPluginEntry_TABLE[] =
{

	{ "tsmf", tsmf_DVCPluginEntry },
	{ "audin", audin_DVCPluginEntry },
	{ "", NULL }
};
const STATIC_ENTRY CLIENT_VirtualChannelEntry_TABLE[] =
{

	{ "cliprdr", cliprdr_VirtualChannelEntry },
	{ "rail", rail_VirtualChannelEntry },
	{ "drdynvc", drdynvc_VirtualChannelEntry },
	{ "rdpsnd", rdpsnd_VirtualChannelEntry },
	{ "rdpdr", rdpdr_VirtualChannelEntry },
	{ "", NULL }
};


const STATIC_ENTRY_TABLE CLIENT_STATIC_ENTRY_TABLES[] =
{
	{ "DeviceServiceEntry", CLIENT_DeviceServiceEntry_TABLE },
	{ "DVCPluginEntry", CLIENT_DVCPluginEntry_TABLE },
	{ "VirtualChannelEntry", CLIENT_VirtualChannelEntry_TABLE },
	{ "", NULL }
};


extern void alsa_freerdp_tsmf_client_audio_subsystem_entry();
extern void alsa_freerdp_audin_client_subsystem_entry();
extern void alsa_freerdp_rdpsnd_client_subsystem_entry();


const STATIC_SUBSYSTEM_ENTRY CLIENT_PARALLEL_SUBSYSTEM_TABLE[] =
{
	{ NULL, NULL, NULL }
};
const STATIC_SUBSYSTEM_ENTRY CLIENT_TSMF_SUBSYSTEM_TABLE[] =
{
	{ "alsa", "audio", alsa_freerdp_tsmf_client_audio_subsystem_entry },
	{ NULL, NULL, NULL }
};
const STATIC_SUBSYSTEM_ENTRY CLIENT_CLIPRDR_SUBSYSTEM_TABLE[] =
{
	{ NULL, NULL, NULL }
};
const STATIC_SUBSYSTEM_ENTRY CLIENT_RAIL_SUBSYSTEM_TABLE[] =
{
	{ NULL, NULL, NULL }
};
const STATIC_SUBSYSTEM_ENTRY CLIENT_DRIVE_SUBSYSTEM_TABLE[] =
{
	{ NULL, NULL, NULL }
};
const STATIC_SUBSYSTEM_ENTRY CLIENT_AUDIN_SUBSYSTEM_TABLE[] =
{
	{ "alsa", "", alsa_freerdp_audin_client_subsystem_entry },
	{ NULL, NULL, NULL }
};
const STATIC_SUBSYSTEM_ENTRY CLIENT_DRDYNVC_SUBSYSTEM_TABLE[] =
{
	{ NULL, NULL, NULL }
};
const STATIC_SUBSYSTEM_ENTRY CLIENT_RDPSND_SUBSYSTEM_TABLE[] =
{
	{ "alsa", "", alsa_freerdp_rdpsnd_client_subsystem_entry },
	{ NULL, NULL, NULL }
};
const STATIC_SUBSYSTEM_ENTRY CLIENT_RDPDR_SUBSYSTEM_TABLE[] =
{
	{ NULL, NULL, NULL }
};
const STATIC_SUBSYSTEM_ENTRY CLIENT_SERIAL_SUBSYSTEM_TABLE[] =
{
	{ NULL, NULL, NULL }
};

const STATIC_ADDIN_TABLE CLIENT_STATIC_ADDIN_TABLE[] =
{
	{ "parallel", parallel_DeviceServiceEntry, CLIENT_PARALLEL_SUBSYSTEM_TABLE },
	{ "tsmf", tsmf_DVCPluginEntry, CLIENT_TSMF_SUBSYSTEM_TABLE },
	{ "cliprdr", cliprdr_VirtualChannelEntry, CLIENT_CLIPRDR_SUBSYSTEM_TABLE },
	{ "rail", rail_VirtualChannelEntry, CLIENT_RAIL_SUBSYSTEM_TABLE },
	{ "drive", drive_DeviceServiceEntry, CLIENT_DRIVE_SUBSYSTEM_TABLE },
	{ "audin", audin_DVCPluginEntry, CLIENT_AUDIN_SUBSYSTEM_TABLE },
	{ "drdynvc", drdynvc_VirtualChannelEntry, CLIENT_DRDYNVC_SUBSYSTEM_TABLE },
	{ "rdpsnd", rdpsnd_VirtualChannelEntry, CLIENT_RDPSND_SUBSYSTEM_TABLE },
	{ "rdpdr", rdpdr_VirtualChannelEntry, CLIENT_RDPDR_SUBSYSTEM_TABLE },
	{ "serial", serial_DeviceServiceEntry, CLIENT_SERIAL_SUBSYSTEM_TABLE },
	{ NULL, NULL, NULL }
};

