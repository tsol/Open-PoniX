/* SANE - Scanner Access Now Easy.
 
   Copyright (C) 2008 by Louis Lagendijk

   This file is part of the SANE package.

   Data structures and definitions for
   bjnp backend for the Common UNIX Printing System (CUPS).

   These coded instructions, statements, and computer programs are the
   property of Louis Lagendijk and are protected by Federal copyright
   law.  Distribution and use rights are outlined in the file "LICENSE.txt"
   "LICENSE" which should have been included with this file.  If this
   file is missing or damaged, see the license at "http://www.cups.org/".

   This file is subject to the Apple OS-Developed Software exception.

   SANE is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   SANE is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with sane; see the file COPYING.  If not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   As a special exception, the authors of SANE give permission for
   additional uses of the libraries contained in this release of SANE.

   The exception is that, if you link a SANE library with other files
   to produce an executable, this does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License.  Your use of that executable is in no way restricted on
   account of linking the SANE library code into it.

   This exception does not, however, invalidate any other reasons why
   the executable file might be covered by the GNU General Public
   License.

   If you submit changes to SANE to the maintainers to be included in
   a subsequent release, you agree by submitting the changes that
   those changes may be distributed with this exception intact.

   If you write modifications of your own for SANE, it is your choice
   whether to permit this exception to apply to your modifications.
   If you do not wish that, delete this exception notice.
*/

/* 
 *  BJNP definitions 
 */

#define BJNP_PRINTBUF_MAX 1400	/* size of printbuffer */
#define BJNP_CMD_MAX 2048	/* size of BJNP response buffer */
#define BJNP_RESP_MAX 2048	/* size of BJNP response buffer */
#define BJNP_SOCK_MAX 256	/* maximum number of open sockets */
#define BJNP_MODEL_MAX 64	/* max allowed size for make&model */
#define BJNP_STATUS_MAX 256	/* max size for status string */
#define BJNP_IEEE1284_MAX 1024	/* max. allowed size of IEEE1284 id */
#define BJNP_NO_DEVICES 16	/* max number of open devices */
#define SCAN_BUF_MAX 65536	/* size of scanner data intermediate buffer */
#define MAX_SELECT_ATTEMPTS 5   /* max nr of retries on select (EINTR) */
#define USLEEP_MS 1000          /* sleep for 1 msec */

/* Do not yet use this, it does not work */
/* #define PIXMA_BJNP_STATUS 1 */

/* loglevel definitions */

#define LOG_CRIT 0
#define LOG_NOTICE 1
#define LOG_INFO 2
#define LOG_DEBUG 11
#define LOG_DEBUG2 12
#define LOG_DEBUG3 13

/*
 * BJNP protocol related definitions
 */

/* port numbers */
typedef enum bjnp_port_e
{
  BJNP_PORT_BROADCAST_BASE = 8610,
  BJNP_PORT_PRINT = 8611,
  BJNP_PORT_SCAN = 8612,
  BJNP_PORT_3 = 8613,
  BJNP_PORT_4 = 8614
} bjnp_port_t;

#define BJNP_METHOD "bjnp"
#define BJNP_STRING "BJNP"

/* commands */
typedef enum bjnp_cmd_e
{
  CMD_UDP_DISCOVER = 0x01,	/* discover if service type is listening at this port */
  CMD_UDP_JOB_DET = 0x10,	/* send print job owner details */
  CMD_UDP_CLOSE = 0x11,		/* request connection closure */
  CMD_UDP_GET_STATUS = 0x20,	/* get printer status  */
  CMD_TCP_REQ = 0x20,		/* read data from device */
  CMD_TCP_SEND = 0x21,		/* send data to device */
  CMD_UDP_GET_ID = 0x30,	/* get printer identity */
  CMD_UDP_SCAN_INFO = 0x32	/* Send some scan info?????? */
} bjnp_cmd_t;

/* command type */

typedef enum uint8_t
{
  BJNP_CMD_PRINT = 0x1,		/* printer command */
  BJNP_CMD_SCAN = 0x2,		/* scanner command */
  BJNP_RES_PRINT = 0x81,	/* printer response */
  BJNP_RES_SCAN = 0x82		/* scanner response */
} bjnp_cmd_type_t;


struct BJNP_command
{
  char BJNP_id[4];		/* string: BJNP */
  uint8_t dev_type;		/* 1 = printer, 2 = scanner */
  uint8_t cmd_code;		/* command code/response code */
  int16_t unknown1;		/* unknown, always 0? */
  int16_t seq_no;		/* sequence number */
  uint16_t session_id;		/* session id for printing */
  uint32_t payload_len;		/* length of command buffer */
} __attribute__ ((__packed__));

/* Layout of the init response buffer */

struct DISCOVER_RESPONSE
{
  struct BJNP_command response;	/* reponse header */
  char unknown1[6];		/* 00 01 08 00 06 04 */
  char mac_addr[6];		/* printers mac address */
  unsigned char ip_addr[4];	/* printers IP-address */
} __attribute__ ((__packed__));

/* layout of payload for the JOB_DETAILS command */

struct JOB_DETAILS
{
  struct BJNP_command cmd;	/* command header */
  char unknown[8];		/* don't know what these are for */
  char hostname[64];		/* hostname of sender */
  char username[64];		/* username */
  char jobtitle[256];		/* job title */
} __attribute__ ((__packed__));

/* layout of scan details, prety vague stuff */

struct INTR_STATUS_REQ
{
  struct BJNP_command cmd;	/* command header */
  uint16_t type;		/* 00 02 */
  uint32_t unknown_2;		/* 00 00 00 00 */
  uint16_t dialogue;		/* some kind of session number for */
  				/* the status requests */
  char user_details[66];	/* user <space> <space> hostname */
  uint16_t unknown_4;		/* 00 14 */
  char unknown_5[23];		/* only zeroes */
  char date_len;		/* length of date-field 0 for type 0 and 1 */
  				/* 0x16 for type 2 */
  char ascii_date[16];		/* YYYYMMDDHHMMSS */
} __attribute__ ((__packed__));

struct INTR_STATUS_RESP
{
  struct BJNP_command cmd;	/* command header */
  char unknown_1[16];		/* unknown stuff */
  char status[16];		/* interrupt status */
  char unknown_2[4];		/* 00 00 00 00 */
} __attribute__ ((__packed__));

/* Layout of ID and status responses */

struct IDENTITY
{
  struct BJNP_command cmd;
  uint16_t id_len;		/* length of identity */
  char id[BJNP_IEEE1284_MAX];	/* identity */
} __attribute__ ((__packed__));


/* response to TCP print command */

struct SCAN_BUF
{
  struct BJNP_command cmd;
  char scan_data[65536];
} __attribute__ ((__packed__));

typedef enum bjnp_paper_status_e
{
  BJNP_PAPER_UNKNOWN = -1,
  BJNP_PAPER_OK = 0,
  BJNP_PAPER_OUT = 1
} bjnp_paper_status_t;

typedef enum
{
  BJNP_STATUS_GOOD,
  BJNP_STATUS_INVAL,
  BJNP_STATUS_ALREADY_ALLOCATED
} BJNP_Status;

/*
 * Device information for opened devices
 */

typedef struct device_s
{
  int open;			/* connection to printer is opened */
  int active;			/* connection is active (has open tcp connection */
  int fd;			/* file descriptor */
  struct sockaddr_in addr;
  int session_id;		/* session id used in bjnp protocol for TCP packets */
  int16_t serial;		/* sequence number of command */
  int bjnp_timeout_sec;		/* timeout (seconds) for next command */
  int bjnp_timeout_msec;	/* timeout (msec) for next command */
  size_t scanner_data_left;	/* TCP data left from last read request */
  int last_cmd;			/* last command sent */
  size_t blocksize;		/* size of (TCP) blocks returned by the scanner */
  char short_read;		/* last TCP read command was shorter than blocksize */
#ifdef PIXMA_BJNP_STATUS
  char polling;			/* status polling ongoing */
  int dialogue;			/* polling dialogue-id */
#endif
} device_t;

