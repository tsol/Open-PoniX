/* ply-boot-protocol.h - shared language between boot status daemon and client
 *
 * Copyright (C) 2007 Red Hat, Inc.
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
 * Written By: Ray Strode <rstrode@redhat.com>
 */
#ifndef PLY_BOOT_PROTOCOL_H
#define PLY_BOOT_PROTOCOL_H

#define PLY_BOOT_PROTOCOL_TRIMMED_ABSTRACT_SOCKET_PATH "/org/freedesktop/plymouthd"
#define PLY_BOOT_PROTOCOL_OLD_ABSTRACT_SOCKET_PATH "/ply-boot-protocol"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_PING "P"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_UPDATE "U"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_CHANGE_MODE "C"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_UPDATE "u"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_INITIALIZED "S"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_DEACTIVATE "D"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_REACTIVATE "r"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUIT "Q"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_PASSWORD "*"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_CACHED_PASSWORD "c"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUESTION "W"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_MESSAGE "M"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_MESSAGE "m"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE "K"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE_REMOVE "L"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_PAUSE "A"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_UNPAUSE "a"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_SPLASH "$"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_SPLASH "H"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_NEWROOT "R"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_HAS_ACTIVE_VT "V"
#define PLY_BOOT_PROTOCOL_REQUEST_TYPE_ERROR "!"

#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK "\x6"
#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NAK "\x15"
#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ANSWER "\x2"
#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_MULTIPLE_ANSWERS "\t"
#define PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NO_ANSWER "\x5"

#endif /* PLY_BOOT_PROTOCOL_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
