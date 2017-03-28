/* SANE - Scanner Access Now Easy.

   Copyright (C) 2008 by Louis Lagendijk

   This file is part of the SANE package.

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
#include  "../include/sane/config.h"
#include  "../include/sane/sane.h"

/*
 * Standard types etc
 */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <unistd.h>
#include <stdio.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/timeb.h>
#endif

/* 
 * networking stuff
 */
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif
#ifdef HAVE_SYS_SELSECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#include <errno.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include "pixma_bjnp_private.h"
#include "pixma_bjnp.h"
#include "pixma_rename.h"
#include "pixma.h"
#include "pixma_common.h"


/* static data */
static device_t device[BJNP_NO_DEVICES];

int first_free_device = 0;

/*
 * Private functions
 */

static int
parse_IEEE1284_to_model (char *scanner_id, char *model)
{
/*
 * parses the  IEEE1284  ID of the scanner to retrieve make and model
 * of the scanner
 * Returns: 0 = not found
 *          1 = found, model is set
 */

  char s[BJNP_IEEE1284_MAX];
  char *tok;

  strcpy (s, scanner_id);
  model[0] = '\0';

  tok = strtok (s, ";");
  while (tok != NULL)
    {
      /* DES contains make and model */

      if (strncmp (tok, "DES:", 4) == 0)
	{
	  strcpy (model, tok + 4);
	  return 1;
	}
      tok = strtok (NULL, ";");
    }
  return 0;
}

static int
charTo2byte (char d[], char s[], int len)
{
  /*
   * copy ASCII string to 2 byte unicode string
   * len is length of destination buffer
   * Returns: number of characters copied
   */

  int done = 0;
  int copied = 0;
  int i;

  len = len / 2;
  for (i = 0; i < len; i++)
    {
      d[2 * i] = '\0';
      if (s[i] == '\0')
	{
	  done = 1;
	}
      if (done == 0)
	{
	  d[2 * i + 1] = s[i];
	  copied++;
	}
      else
	d[2 * i + 1] = '\0';
    }
  return copied;
}

static char *
getusername (void)
{
  static char noname[] = "sane_pixma";
  struct passwd *pwdent;

#ifdef HAVE_PWD_H
  if (((pwdent = getpwuid (geteuid ())) != NULL) && (pwdent->pw_name != NULL))
    return pwdent->pw_name;
#endif
  return noname;
}


static char *
truncate_hostname (char *hostname, char *short_hostname)
{
  char *dot;

  /* determine a short hostname (max HOSTNAME_SHORT_MAX chars */

  strncpy (short_hostname, hostname, SHORT_HOSTNAME_MAX);
  short_hostname[SHORT_HOSTNAME_MAX - 1] = '\0';

  if (strlen (hostname) > SHORT_HOSTNAME_MAX)
    {
      /* this is a hostname, not an ip-address, so remove domain part of the name */

      if ((dot = strchr (short_hostname, '.')) != NULL)
	*dot = '\0';
    }
  return short_hostname;
}

static int
bjnp_open_tcp (int devno)
{
  int sock;
  int val;

  if ((sock = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
      PDBG (pixma_dbg (LOG_CRIT, "bjnp_open_tcp: Can not create socket: %s\n",
		       strerror (errno)));
      return -1;
    }

  val = 1;
  setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof (val));

#if 0
  val = 1;
  setsockopt (sock, SOL_SOCKET, SO_REUSEPORT, &val, sizeof (val));

  val = 1;
#endif

  /*
   * Using TCP_NODELAY improves responsiveness, especially on systems
   * with a slow loopback interface...
   */

  val = 1;
  setsockopt (sock, IPPROTO_TCP, TCP_NODELAY, &val, sizeof (val));

/*
 * Close this socket when starting another process...
 */

  fcntl (sock, F_SETFD, FD_CLOEXEC);

  if (connect
      (sock, (struct sockaddr *) &device[devno].addr,
       sizeof (device[devno].addr)) != 0)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT, "bjnp_open_tcp: Can not connect to scanner: %s\n",
	     strerror (errno)));
      return -1;
    }
  device[devno].fd = sock;
  return 0;
}

static int
split_uri (const char *devname, char *method, char *hostname, int *port,
	   char *args)
{
  char copy[1024];
  char *start;
  char next;
  int i;

  strcpy (copy, devname);
  start = copy;

/*
 * retrieve method
 */

  i = 0;
  while ((start[i] != '\0') && (start[i] != ':'))
    {
      i++;
    }

  if (((strncmp (start + i, "://", 3) != 0)) || (i > 255))
    {
      PDBG (pixma_dbg (LOG_NOTICE, "Can not find method in %s (offset %d)\n",
		       devname, i));
      return -1;
    }

  start[i] = '\0';
  strcpy (method, start);
  start = start + i + 3;

/*
 * retrieve hostname
 */

  i = 0;
  while ((start[i] != '\0') && (start[i] != '/') && (start[i] != ':'))
    {
      i++;
    }


  if ((strlen (start) == 0) || (i > 255))
    {
      PDBG (pixma_dbg (LOG_NOTICE, "Can not find hostname in %s\n", devname));
      return -1;
    }

  next = start[i];
  strncpy (hostname, start, i);
  hostname[i] = '\0';
  start = start + i + 1;

/*
 * retrieve port number
 */

  if (next != ':')
    *port = 0;
  else
    {
      i = 0;
      while ((start[i] != '\0') && (start[i] != '/'))
	{
	  if ((start[i] < '0') || (start[i] > '9') || (i > 5))
	    {
	      PDBG (pixma_dbg (LOG_NOTICE, "Can not find port number in %s\n",
			       devname));
	      return -1;
	    }

	  i++;
	}
      next = start[i];
      start[i] = '\0';
      sscanf (start, "%d", port);
      start = start + i + 1;
    }

/*
 * Retrieve arguments
 */

  if (next == '/')
    strcpy (args, start);
  else
    strcpy (args, "");
  return 0;
}



static void
set_cmd (int devno, struct BJNP_command *cmd, char cmd_code, int payload_len)
{
  /*
   * Set command buffer with command code, session_id and lenght of payload
   * Returns: sequence number of command
   */
  strncpy (cmd->BJNP_id, BJNP_STRING, sizeof (cmd->BJNP_id));
  cmd->dev_type = BJNP_CMD_SCAN;
  cmd->cmd_code = cmd_code;
  cmd->unknown1 = htons (0);
  if (devno == -1)
    {
      /* device not opened, use 0 for serial and session) */
      cmd->seq_no = htons (0);
      cmd->session_id = htons (0);
    }
  else
    {
      cmd->seq_no = htons (++(device[devno].serial));
      cmd->session_id = htons (device[devno].session_id);
    }
  cmd->payload_len = htonl (payload_len);
  device[devno].last_cmd = cmd_code;
}



static int
udp_command (const int dev_no, char *command, int cmd_len, char *response,
	     int resp_len)
{
  /*
   * Send UDP command and retrieve response
   * Returns: length of response or -1 in case of error
   */

  int sockfd;
  int numbytes = 0;
  fd_set fdset;
  struct timeval timeout;
  int result;
  int try, attempt;

  PDBG (pixma_dbg (LOG_DEBUG, "udp_command: Sending UDP command to %s:%d\n",
		   inet_ntoa (device[dev_no].addr.sin_addr),
		   ntohs (device[dev_no].addr.sin_port)));

  if ((sockfd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT, "udp_command: can not open socket - %s\n",
	     strerror (errno)));
      return -1;
    }

  if (connect
      (sockfd, (struct sockaddr *) &device[dev_no].addr,
       (socklen_t) sizeof (struct sockaddr_in)) != 0)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT, "udp_command: connect failed- %s\n",
	     strerror (errno)));
      return -1;
    }

  for (try = 0; try < 3; try++)
    {
      if ((numbytes = send (sockfd, command, cmd_len, 0)) != cmd_len)
	{
	  PDBG (pixma_dbg
		(LOG_CRIT, "udp_command: Sent only 0x%x = %d bytes of packet",
		 numbytes, numbytes));
	  continue;
	}

      attempt = 0;
      do
	{
	  /* wait for data to be received, ignore signals being received */
	  FD_ZERO (&fdset);
	  FD_SET (sockfd, &fdset);
	  timeout.tv_sec = device[dev_no].bjnp_timeout_sec;
	  timeout.tv_usec = device[dev_no].bjnp_timeout_msec;
	}
      while (((result =
	       select (sockfd + 1, &fdset, NULL, NULL, &timeout)) <= 0)
	     && (errno == EINTR) && (attempt++ < MAX_SELECT_ATTEMPTS));

      if (result <= 0)
	{
	  PDBG (pixma_dbg
		(LOG_CRIT, "udp_command: No data received (select): %s\n",
		 result == 0 ? "timed out" : strerror (errno)));
	  continue;
	}

      if ((numbytes = recv (sockfd, response, resp_len, 0)) == -1)
	{
	  PDBG (pixma_dbg
		(LOG_CRIT, "udp_command: no data received (recv): %s",
		 strerror (errno)));
	  continue;
	}
      close (sockfd);
      return numbytes;
    }

  /* no response even after retry */

  return -1;
}

static int
get_scanner_id (const int dev_no, char *model)
{
  /*
   * get scanner identity
   * Sets model (make and model)
   * Return 0 on success, -1 in case of errors
   */

  struct BJNP_command cmd;
  struct IDENTITY *id;
  char scanner_id[BJNP_IEEE1284_MAX];
  int resp_len;
  int id_len;
  char resp_buf[BJNP_RESP_MAX];

  /* set defaults */

  strcpy (model, "Unidentified scanner");

  set_cmd (dev_no, &cmd, CMD_UDP_GET_ID, 0);

  PDBG (pixma_dbg (LOG_DEBUG2, "Get scanner identity\n"));
  PDBG (pixma_hexdump (LOG_DEBUG2, (char *) &cmd,
		       sizeof (struct BJNP_command)));

  resp_len =
    udp_command (dev_no, (char *) &cmd, sizeof (struct BJNP_command),
		 resp_buf, BJNP_RESP_MAX);

  if (resp_len <= 0)
    return -1;

  PDBG (pixma_dbg (LOG_DEBUG2, "scanner identity:\n"));
  PDBG (pixma_hexdump (LOG_DEBUG2, resp_buf, resp_len));

  id = (struct IDENTITY *) resp_buf;

  /* truncate string to be safe */
  id->id[BJNP_IEEE1284_MAX - 1] = '\0';
  id_len = ntohs (id->id_len) - sizeof (id->id_len);

  strcpy (scanner_id, id->id);

  PDBG (pixma_dbg (LOG_INFO, "Scanner identity string = %s\n", scanner_id));

  /* get make&model from IEEE1284 id  */

  if (model != NULL)
    {
      parse_IEEE1284_to_model (scanner_id, model);
      PDBG (pixma_dbg (LOG_INFO, "Scanner model = %s\n", model));
    }
  return 0;
}

static void
u8tohex (uint8_t x, char *str)
{
  static const char hdigit[16] =
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd',
    'e', 'f'
  };
  str[0] = hdigit[(x >> 4) & 0xf];
  str[1] = hdigit[x & 0xf];
  str[2] = '\0';
}


static void
parse_scanner_address (char *resp_buf, char *address, char *serial)
{
  /*
   * Parse identify command responses to ip-address
   * and hostname
   */

  struct in_addr ip_addr;
  struct hostent *myhost;
  char ip_address[16];
  int res;
  int i, j;
  uint8_t byte;


  struct DISCOVER_RESPONSE *init_resp;

  init_resp = (struct DISCOVER_RESPONSE *) resp_buf;
  sprintf (ip_address, "%u.%u.%u.%u",
	   init_resp->ip_addr[0],
	   init_resp->ip_addr[1],
	   init_resp->ip_addr[2], init_resp->ip_addr[3]);

  PDBG (pixma_dbg
	(LOG_INFO, "Found scanner at ip address: %s\n", ip_address));

  /* do reverse name lookup, if hostname can not be fouund return ip-address */

  res = inet_aton (ip_address, &ip_addr);
  myhost = gethostbyaddr ((void *) &ip_addr, sizeof (ip_addr), AF_INET);

  if (myhost == NULL)
    strcpy (address, ip_address);
  else
    strcpy (address, myhost->h_name);

  /* construct serial, first 3 bytes contain vendor ID, skip them */

  j = 0;
  for (i = 3; i < 6; i++)
    {
      byte = init_resp->mac_addr[i];
      u8tohex (byte, (serial + j));
      j = j + 2;
    }
  serial[j] = '\0';

}

static int
bjnp_send_broadcast (struct in_addr local_addr, int local_port,
		     struct in_addr broadcast_addr, struct BJNP_command cmd,
		     int size)
{
  /*
   * send command to interface and return open socket
   */

  struct sockaddr_in sendaddr;
  int sockfd;
  int broadcast = 1;
  int numbytes;

  if ((sockfd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
      PDBG (pixma_dbg
	    (LOG_INFO, "bjnp_send_broadcast: can not open socket - %s",
	     strerror (errno)));
      return -1;
    }

  /* Set broadcast flag on socket */

  if (setsockopt
      (sockfd, SOL_SOCKET, SO_BROADCAST, (const char *) &broadcast,
       sizeof (broadcast)) != 0)
    {
      PDBG (pixma_dbg
	    (LOG_INFO,
	     "bjnp_send_broadcast: setting socket options failed - %s",
	     strerror (errno)));
      close (sockfd);
      return -1;
    };

  /* Bind to local address, use BJNP port */

  sendaddr.sin_family = AF_INET;
  sendaddr.sin_port = htons (local_port);
  sendaddr.sin_addr = local_addr;
  memset (sendaddr.sin_zero, '\0', sizeof sendaddr.sin_zero);

  if (bind
      (sockfd, (struct sockaddr *) &sendaddr,
       (socklen_t) sizeof (sendaddr)) != 0)
    {
      PDBG (pixma_dbg
	    (LOG_INFO,
	     "bjnp_send_broadcast: bind socket to local address failed - %s\n",
	     strerror (errno)));
      close (sockfd);
      return -1;
    }

  /* set address to send packet to */

  sendaddr.sin_family = AF_INET;
  sendaddr.sin_port = htons (BJNP_PORT_SCAN);

  /* usebroadcast address of interface */
  sendaddr.sin_addr = broadcast_addr;
  memset (sendaddr.sin_zero, '\0', sizeof sendaddr.sin_zero);


  if ((numbytes = sendto (sockfd, &cmd, size, 0,
			  (struct sockaddr *) &sendaddr,
			  sizeof (sendaddr))) != size)
    {
      PDBG (pixma_dbg (LOG_INFO,
		       "bjnp_send_broadcast: Sent only %x = %d bytes of packet, error = %s\n",
		       numbytes, numbytes, strerror (errno)));
      /* not allowed, skip this interface */

      close (sockfd);
      return -1;
    }
  return sockfd;
}

static void
bjnp_finish_job (int devno)
{
/* 
 * Signal end of scanjob to scanner
 */

  char resp_buf[BJNP_RESP_MAX];
  int resp_len;
  struct BJNP_command cmd;

  set_cmd (devno, &cmd, CMD_UDP_CLOSE, 0);

  PDBG (pixma_dbg (LOG_DEBUG2, "Finish scanjob\n"));
  PDBG (pixma_hexdump
	(LOG_DEBUG2, (char *) &cmd, sizeof (struct BJNP_command)));
  resp_len =
    udp_command (devno, (char *) &cmd, sizeof (struct BJNP_command), resp_buf,
		 BJNP_RESP_MAX);

  if (resp_len != sizeof (struct BJNP_command))
    {
      PDBG (pixma_dbg
	    (LOG_CRIT,
	     "Received %d characters on close scanjob command, expected %d\n",
	     resp_len, (int) sizeof (struct BJNP_command)));
      return;
    }
  PDBG (pixma_dbg (LOG_DEBUG2, "Finish scanjob response\n"));
  PDBG (pixma_hexdump (LOG_DEBUG2, resp_buf, resp_len));

}

#ifdef PIXMA_BJNP_STATUS
static int
bjnp_get_intr (int devno, char type, char *hostname, char *user,
	       SANE_Byte * buffer, int len)
{
/* 
 * get interrupt status from device  
 * this appear to be the equivalent of the status read by sanei_usb_read_int
 * returns number of bytes received or -1
 */

  char resp_buf[BJNP_RESP_MAX];
  int resp_len;
  struct INTR_STATUS_REQ request;
  struct INTR_STATUS_RESP *resp;
  char details[64];
  time_t t;
  int plen;			/* payload lenght */

  if (len > (int) (sizeof (resp->status)))
    {
      PDBG (pixma_dbg
	    (LOG_CRIT, "requested status too big: %d, have %d\n", (int) len,
	     (int) (sizeof (resp->status))));
      return -1;
    }

  /* Cacluate payload length. Type 0 and 1 packets do not contain the date. 
     We format it all but do not send the date part */

  plen = (type < 2) ?
    sizeof (request) - sizeof (struct BJNP_command) -
    sizeof (request.ascii_date) : sizeof (request) -
    sizeof (struct BJNP_command);

  /* fill in the command header */

  set_cmd (devno, (struct BJNP_command *) &request, CMD_UDP_SCAN_INFO, plen);

  /* create payload */

  request.type = htons (type);
  request.unknown_2 = htonl (0);
  if (type < 2)
    {
      request.dialogue = htons (0);
    }
  else
    {
      request.dialogue = htons (device[devno].dialogue);
    }
  request.unknown_4 = htons (0x14);
  sprintf (details, "%s  %s", user, hostname);
  charTo2byte (request.user_details, details, sizeof (request.user_details));

  memset (request.unknown_5, 0, sizeof (request.unknown_5));
  request.date_len = (type < 2) ? 0 : sizeof (request.ascii_date);

  /* set time field */

  t = time (NULL);
  strftime (request.ascii_date, sizeof (request.ascii_date) + 2,
	    "%Y%m%d%H%M%S", localtime (&t));

  /* length (plen) determines how many bytes get sent, this skips
     date for type 0 and 1 */

  PDBG (pixma_dbg (LOG_DEBUG2, "Interrupt status request\n"));
  PDBG (pixma_hexdump (LOG_DEBUG2, (char *) &request,
		       sizeof (struct BJNP_command) + plen));

  resp_len = udp_command (devno, (char *) &request,
			  sizeof (struct BJNP_command) + plen, resp_buf,
			  BJNP_RESP_MAX);
  if (resp_len > 0)
    {
      PDBG (pixma_dbg (LOG_DEBUG2, "Interrupt status response:\n"));
      PDBG (pixma_hexdump (LOG_DEBUG2, resp_buf, resp_len));
    }
  if (resp_len != sizeof (struct INTR_STATUS_RESP))
    {
      PDBG (pixma_dbg
	    (LOG_CRIT,
	     "bjnp_get_intr: could not read interrupt status (expected %d bytes, got %d!\n",
	     (int) sizeof (struct INTR_STATUS_RESP), (int) resp_len));
      return -1;
    }
  resp = (struct INTR_STATUS_RESP *) resp_buf;
  memcpy (buffer, resp->status, len);
  return len;
}
#endif

static int
bjnp_send_job_details (int devno, char *hostname, char *user, char *title)
{
/* 
 * send details of scanjob to scanner
 * Returns: addrlist set to address details of used scanner
 */

  char cmd_buf[BJNP_CMD_MAX];
  char resp_buf[BJNP_RESP_MAX];
  int resp_len;
  struct JOB_DETAILS *job;
  struct BJNP_command *resp;

  /* send job details command */

  set_cmd (devno, (struct BJNP_command *) cmd_buf, CMD_UDP_JOB_DET,
	   sizeof (*job) - sizeof (struct BJNP_command));

  /* create payload */

  job = (struct JOB_DETAILS *) (cmd_buf);
  charTo2byte (job->unknown, "", sizeof (job->unknown));
  charTo2byte (job->hostname, hostname, sizeof (job->hostname));
  charTo2byte (job->username, user, sizeof (job->username));
  charTo2byte (job->jobtitle, title, sizeof (job->jobtitle));

  PDBG (pixma_dbg (LOG_DEBUG2, "Job details\n"));
  PDBG (pixma_hexdump (LOG_DEBUG2, cmd_buf,
		       (sizeof (struct BJNP_command) + sizeof (*job))));

  resp_len = udp_command (devno, cmd_buf,
			  sizeof (struct JOB_DETAILS), resp_buf,
			  BJNP_RESP_MAX);

  if (resp_len > 0)
    {
      PDBG (pixma_dbg (LOG_DEBUG2, "Job details response:\n"));
      PDBG (pixma_hexdump (LOG_DEBUG2, resp_buf, resp_len));
      resp = (struct BJNP_command *) resp_buf;
      device[devno].session_id = ntohs (resp->session_id);
    }
  return 0;
}

static int
bjnp_write (int devno, const SANE_Byte * buf, size_t count)
{
/*
 * This function writes scandata to the scanner. 
 * Returns: number of bytes written to the scanner
 */
  int sent_bytes;
  int terrno;
  struct SCAN_BUF bjnp_buf;

  if (device[devno].scanner_data_left)
    PDBG (pixma_dbg
	  (LOG_CRIT, "bjnp_write: ERROR scanner data left = 0x%lx = %ld\n",
	   (unsigned long) device[devno].scanner_data_left,
	   (unsigned long) device[devno].scanner_data_left));

  /* set BJNP command header */

  set_cmd (devno, (struct BJNP_command *) &bjnp_buf, CMD_TCP_SEND, count);
  memcpy (bjnp_buf.scan_data, buf, count);
  PDBG (pixma_dbg (LOG_DEBUG, "bjnp_write: sending 0x%lx = %ld bytes\n",
		   (unsigned long) count, (unsigned long) count);
	PDBG (pixma_hexdump (LOG_DEBUG2, (char *) &bjnp_buf,
			     sizeof (struct BJNP_command) + count)));

  if ((sent_bytes =
       send (device[devno].fd, &bjnp_buf,
	     sizeof (struct BJNP_command) + count, 0)) <
      (ssize_t) (sizeof (struct BJNP_command) + count))
    {
      /* return result from write */
      terrno = errno;
      PDBG (pixma_dbg (LOG_CRIT, "bjnp_write: Could not send data!\n"));
      errno = terrno;
      return sent_bytes;
    }
  /* correct nr of bytes sent for length of command */

  else if (sent_bytes != (int) (sizeof (struct BJNP_command) + count))
    {
      errno = EIO;
      return -1;
    }
  return count;
}

static int
bjnp_send_read_request (int devno)
{
/*
 * This function reads responses from the scanner.  
 * Returns: 0 on success, else -1
 *  
 */
  int sent_bytes;
  int terrno;
  struct BJNP_command bjnp_buf;

  if (device[devno].scanner_data_left)
    PDBG (pixma_dbg
	  (LOG_CRIT,
	   "bjnp_send_read_request: ERROR scanner data left = 0x%lx = %ld\n",
	   (unsigned long) device[devno].scanner_data_left,
	   (unsigned long) device[devno].scanner_data_left));

  /* set BJNP command header */

  set_cmd (devno, (struct BJNP_command *) &bjnp_buf, CMD_TCP_REQ, 0);

  PDBG (pixma_dbg (LOG_DEBUG, "bjnp_send_read_req sending command\n"));
  PDBG (pixma_hexdump (LOG_DEBUG2, (char *) &bjnp_buf,
		       sizeof (struct BJNP_command)));

  if ((sent_bytes =
       send (device[devno].fd, &bjnp_buf, sizeof (struct BJNP_command),
	     0)) < 0)
    {
      /* return result from write */
      terrno = errno;
      PDBG (pixma_dbg
	    (LOG_CRIT, "bjnp_send_read_request: Could not send data!\n"));
      errno = terrno;
      return -1;
    }
  return 0;
}

static SANE_Status
bjnp_recv_header (int devno)
{
/*
 * This function receives the response header to bjnp commands.
 * devno device number
 * Returns: 
 * SANE_STATUS_IO_ERROR when any IO error occurs
 * SANE_STATUS_GOOD in case no errors were encountered
 */
  struct BJNP_command resp_buf;
  fd_set input;
  struct timeval timeout;
  int recv_bytes;
  int terrno;
  int result;
  int fd;
  int attempt;

  PDBG (pixma_dbg
	(LOG_DEBUG, "bjnp_recv_header: receiving response header\n"));
  fd = device[devno].fd;

  if (device[devno].scanner_data_left)
    PDBG (pixma_dbg
	  (LOG_CRIT,
	   "bjnp_send_request: ERROR scanner data left = 0x%lx = %ld\n",
	   (unsigned long) device[devno].scanner_data_left,
	   (unsigned long) device[devno].scanner_data_left));

  attempt = 0;
  do
    {
      /* wait for data to be received, ignore signals being received */
      FD_ZERO (&input);
      FD_SET (fd, &input);

      timeout.tv_sec = device[devno].bjnp_timeout_sec;
      timeout.tv_usec = device[devno].bjnp_timeout_msec;
    }
  while (((result = select (fd + 1, &input, NULL, NULL, &timeout)) == -1) &&
	 (errno == EINTR) && (attempt++ < MAX_SELECT_ATTEMPTS));

  if (result < 0)
    {
      terrno = errno;
      PDBG (pixma_dbg (LOG_CRIT,
		       "bjnp_recv_header: could not read response header (select): %s!\n",
		       strerror (terrno)));
      errno = terrno;
      return SANE_STATUS_IO_ERROR;
    }
  else if (result == 0)
    {
      terrno = errno;
      PDBG (pixma_dbg (LOG_CRIT,
		       "bjnp_recv_header: could not read response header (select timed out): %s!\n",
		       strerror (terrno)));
      errno = terrno;
      return SANE_STATUS_IO_ERROR;
    }

  /* get response header */

  if ((recv_bytes =
       recv (fd, (char *) &resp_buf,
	     sizeof (struct BJNP_command),
	     0)) != sizeof (struct BJNP_command))
    {
      terrno = errno;
      PDBG (pixma_dbg (LOG_CRIT,
		       "bjnp_recv_header: (recv) could not read response header, received %d bytes!\n",
		       recv_bytes));
      PDBG (pixma_dbg
	    (LOG_CRIT, "bjnp_recv_header: (recv) error: %s!\n",
	     strerror (terrno)));
      errno = terrno;
      return SANE_STATUS_IO_ERROR;
    }

  if (resp_buf.cmd_code != device[devno].last_cmd)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT,
	     "bjnp_recv_header:ERROR, Received response has cmd code %d, expected %d\n",
	     resp_buf.cmd_code, device[devno].last_cmd));
      return SANE_STATUS_IO_ERROR;
    }

  if (ntohs (resp_buf.seq_no) != (uint16_t) device[devno].serial)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT,
	     "bjnp_recv_header:ERROR, Received response has serial %d, expected %d\n",
	     (int) ntohs (resp_buf.seq_no), (int) device[devno].serial));
      return SANE_STATUS_IO_ERROR;
    }

  /* got response header back, retrieve length of scanner data */


  device[devno].scanner_data_left = ntohl (resp_buf.payload_len);
  PDBG (pixma_dbg
	(LOG_DEBUG2, "TCP response header(scanner data = %ld bytes):\n",
	 (unsigned long) device[devno].scanner_data_left));
  PDBG (pixma_hexdump
	(LOG_DEBUG2, (char *) &resp_buf, sizeof (struct BJNP_command)));
  return SANE_STATUS_GOOD;
}

static SANE_Status
bjnp_recv_data (int devno, SANE_Byte * buffer, size_t * len)
{
/*
 * This function receives the responses to the write commands.
 * NOTE: len may not exceed SSIZE_MAX (as that is max for recv)
 * Returns: number of bytes of payload received from device
 */

  fd_set input;
  struct timeval timeout;
  ssize_t recv_bytes;
  int terrno;
  int result;
  int fd;
  int attempt;

  PDBG (pixma_dbg (LOG_DEBUG, "bjnp_recv_data: receiving response data\n"));
  fd = device[devno].fd;

  PDBG (pixma_dbg
	(LOG_DEBUG, "bjnp_recv_data: read response payload (%ld bytes max)\n",
	 (unsigned long) *len));

  attempt = 0;
  do
    {
      /* wait for data to be received, retry on a signal being received */

      FD_ZERO (&input);
      FD_SET (fd, &input);
      timeout.tv_sec = device[devno].bjnp_timeout_sec;
      timeout.tv_usec = device[devno].bjnp_timeout_msec;
    }
  while (((result = select (fd + 1, &input, NULL, NULL, &timeout)) == -1) &&
	 (errno == EINTR) && (attempt++ < MAX_SELECT_ATTEMPTS));

  if (result < 0)
    {
      terrno = errno;
      PDBG (pixma_dbg (LOG_CRIT,
		       "bjnp_recv_data: could not read response payload (select): %s!\n",
		       strerror (errno)));
      errno = terrno;
      *len = 0;
      return SANE_STATUS_IO_ERROR;
    }
  else if (result == 0)
    {
      terrno = errno;
      PDBG (pixma_dbg (LOG_CRIT,
		       "bjnp_recv_data: could not read response payload (select timed out): %s!\n",
		       strerror (terrno)));
      errno = terrno;
      *len = 0;
      return SANE_STATUS_IO_ERROR;
    }

  if ((recv_bytes = recv (fd, buffer, *len, 0)) < 0)
    {
      terrno = errno;
      PDBG (pixma_dbg (LOG_CRIT,
		       "bjnp_recv_data: could not read response payload (recv): %s!\n",
		       strerror (errno)));
      errno = terrno;
      *len = 0;
      return SANE_STATUS_IO_ERROR;
    }
  PDBG (pixma_dbg (LOG_DEBUG2, "Received TCP response payload (%ld bytes):\n",
		   (unsigned long) recv_bytes));
  PDBG (pixma_hexdump (LOG_DEBUG2, buffer, recv_bytes));

  device[devno].scanner_data_left =
    device[devno].scanner_data_left - recv_bytes;

  *len = recv_bytes;
  return SANE_STATUS_GOOD;
}

static BJNP_Status
bjnp_allocate_device (SANE_String_Const devname, SANE_Int * dn,
		      char *res_hostname)
{
  char method[256];
  char hostname[256];
  int port;
  char args[256];
  struct hostent *result;
  struct in_addr *addr_list;
  int i;

  PDBG (pixma_dbg (LOG_DEBUG, "bjnp_allocate_device(%s)", devname));

  if (split_uri (devname, method, hostname, &port, args) != 0)
    {
      return BJNP_STATUS_INVAL;
    }

  if (strlen (args) != 0)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT,
	     "URI may not contain userid, password or aguments: %s\n",
	     devname));

      return BJNP_STATUS_INVAL;
    }
  if (strcmp (method, BJNP_METHOD) != 0)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT, "URI %s contains invalid method: %s\n", devname,
	     method));
      return BJNP_STATUS_INVAL;
    }

  result = gethostbyname (hostname);
  if ((result == NULL) || result->h_addrtype != AF_INET)
    {
      PDBG (pixma_dbg (LOG_CRIT, "Cannot resolve hostname: %s\n", hostname));
      return SANE_STATUS_INVAL;
    }
  if (port == 0)
    {
      port = BJNP_PORT_SCAN;
    }

  addr_list = (struct in_addr *) *result->h_addr_list;

  /* Check if a device number is already allocated */

  for (i = 0; i < first_free_device; i++)
    {
      /* check address, AF_INET is assumed */

      if ((device[i].addr.sin_port == htons (port)) &&
	  (device[i].addr.sin_addr.s_addr == addr_list[0].s_addr))
	{
	  *dn = i;
	  return BJNP_STATUS_ALREADY_ALLOCATED;
	}
    }

  /* return hostname if required */

  if (res_hostname != NULL)
    strcpy (res_hostname, hostname);

  /*
   * No existing device structure found, fill new device structure
   */

  if (first_free_device == BJNP_NO_DEVICES)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT,
	     "Too many devices, ran out of device structures, can not add %s\n",
	     devname));
      return BJNP_STATUS_INVAL;
    }
  *dn = first_free_device++;
  device[*dn].open = 1;
  device[*dn].active = 0;
#ifdef PIXMA_BJNP_STATUS
  device[*dn].polling = 0;
  device[*dn].dialogue = 0;
#endif
  device[*dn].fd = -1;
  device[*dn].addr.sin_family = AF_INET;
  device[*dn].addr.sin_port = htons (port);
  device[*dn].addr.sin_addr = addr_list[0];

  device[*dn].session_id = 0;
  device[*dn].serial = -1;
  device[*dn].bjnp_timeout_sec = 1;
  device[*dn].bjnp_timeout_msec = 0;
  device[*dn].scanner_data_left = 0;
  device[*dn].last_cmd = 0;

  /* we make a pessimistic guess on blocksize, will be corrected to max size
   * of received block when we read data  */

  device[*dn].blocksize = 1024;
  device[*dn].short_read = 0;

  return BJNP_STATUS_GOOD;
}

/*
 * Public functions
 */

/** Initialize sanei_bjnp.
 *
 * Call this before any other sanei_bjnp function.
 */
extern void
sanei_bjnp_init (void)
{
  first_free_device = 0;
}

/** 
 * Find devices that implement the bjnp protocol
 *
 * The function attach is called for every device which has been found.
 *
 * @param attach attach function
 *
 * @return SANE_STATUS_GOOD - on success (even if no scanner was found)
 */
extern SANE_Status
sanei_bjnp_find_devices (const char **conf_devices,
			 SANE_Status (*attach_bjnp)
			 (SANE_String_Const devname,
			  SANE_String_Const makemodel,
			  SANE_String_Const serial,
			  const struct pixma_config_t *
			  const pixma_devices[]),
			 const struct pixma_config_t *const pixma_devices[])
{
  int numbytes = 0;
  struct BJNP_command cmd;
  char resp_buf[2048];
  int socket_fd[BJNP_SOCK_MAX];
  int no_sockets;
  int i;
  int attempt;
  int last_socketfd = 0;
  fd_set fdset;
  fd_set active_fdset;
  struct timeval timeout;
  char hostname[256];
  char short_hostname[SHORT_HOSTNAME_MAX];
  char makemodel[BJNP_IEEE1284_MAX];
  char uri[256];
  int dev_no;
  char serial[13];
#ifdef HAVE_IFADDRS_H
  struct ifaddrs *interfaces;
  struct ifaddrs *interface;
#else
  struct in_addr broadcast;
  struct in_addr local;
#endif

  PDBG (pixma_dbg (LOG_INFO, "sanei_bjnp_find_devices:\n"));
  first_free_device = 0;

  /* First add devices from config file */

  for (i = 0; conf_devices[i] != NULL; i++)
    {
      PDBG (pixma_dbg
	    (LOG_DEBUG, "Adding configured scanner: %s\n", conf_devices[i]));

      /* Allocate device structure for scanner and read its model */
      switch (bjnp_allocate_device (conf_devices[i], &dev_no, hostname))
	{
	case BJNP_STATUS_GOOD:
	  if (get_scanner_id (dev_no, makemodel) != 0)
	    {
	      PDBG (pixma_dbg
		    (LOG_CRIT,
		     "Cannot read scanner make & model: %s\n", uri));
	    }
	  else
	    {
	      /*
	       * inform caller of found scanner
	       */

	      truncate_hostname (hostname, short_hostname);
	      attach_bjnp (conf_devices[i], makemodel,
			   short_hostname, pixma_devices);
	    }
	  break;
	case BJNP_STATUS_ALREADY_ALLOCATED:
	  PDBG (pixma_dbg
		(LOG_NOTICE,
		 "Scanner at %s defined in configuration file, but it was added before, good!\n",
		 uri));
	  break;

	case BJNP_STATUS_INVAL:
	  PDBG (pixma_dbg
		(LOG_NOTICE,
		 "Scanner at %s defined in configuration file, but can not add it\n",
		 uri));
	  break;
	}
    }
  PDBG (pixma_dbg
	(LOG_DEBUG,
	 "Added all configured scanners, now do auto detection...\n"));

  /*
   * Send UDP broadcast to discover scanners and return the list of scanners found
   * Returns: number of scanners found
   */

  FD_ZERO (&fdset);
  set_cmd (-1, &cmd, CMD_UDP_DISCOVER, 0);

#ifdef HAVE_IFADDRS_H
  no_sockets = 0;
  getifaddrs (&interfaces);

  /* send MAX_SELECT_ATTEMPTS broadcasts on each suitable interface */
  for (attempt = 0; attempt < MAX_SELECT_ATTEMPTS; attempt++)
    {
      interface = interfaces;

      while ((no_sockets < BJNP_SOCK_MAX) && (interface != NULL))
	{
	  /* send broadcast packet to each suitable  interface */

	  if ((interface->ifa_addr == NULL)
	      || (interface->ifa_broadaddr == NULL)
	      || (interface->ifa_addr->sa_family != AF_INET)
	      || (((struct sockaddr_in *) interface->ifa_addr)->sin_addr.
		  s_addr == htonl (INADDR_LOOPBACK)))
	    {
	      /* not an IPv4 address */

	      PDBG (pixma_dbg
		    (LOG_DEBUG,
		     "%s is not a valid IPv4 interface, skipping...\n",
		     interface->ifa_name));

	    }
	  else
	    {
	      PDBG (pixma_dbg
		    (LOG_INFO, "%s is IPv4 capable, sending broadcast..\n",
		     interface->ifa_name));

	      if ((socket_fd[no_sockets] =
		   bjnp_send_broadcast (((struct sockaddr_in *)
					 interface->ifa_addr)->sin_addr,
					BJNP_PORT_BROADCAST_BASE + attempt,
					((struct sockaddr_in *)
					 interface->ifa_broadaddr)->sin_addr,
					cmd, sizeof (cmd))) != -1)
		{
		  if (socket_fd[no_sockets] > last_socketfd)
		    {
		      /* track highest used socket for use in select */

		      last_socketfd = socket_fd[no_sockets];
		    }
		  FD_SET (socket_fd[no_sockets], &fdset);
		  no_sockets++;
		}
	    }
	  interface = interface->ifa_next;
	}
      /* wait for some time between broadcast packets */
      usleep (100 * USLEEP_MS);
    }
  freeifaddrs (interfaces);
#else
  /* we have no easy way to find interfaces with their broadcast addresses, use global broadcast */

  no_sockets = 0;
  broadcast.s_addr = htonl (INADDR_BROADCAST);
  local.s_addr = htonl (INADDR_ANY);

  for (attempt = 0; attempt < MAX_SELECT_ATTEMPTS; attempt++)
    {
      if ((socket_fd[no_sockets] =
	   bjnp_send_broadcast (local, BJNP_PORT_BROADCAST_BASE + attempt,
				broadcast, cmd, sizeof (cmd))) != -1)
	{
	  if (socket_fd[no_sockets] > last_socketfd)
	    {
	      /* track highest used socket for use in select */

	      last_socketfd = socket_fd[no_sockets];
	    }
	  FD_SET (socket_fd[no_sockets], &fdset);
	  no_sockets++;
	}
      /* wait for some time between broadcast packets */
      usleep (100 * USLEEP_MS);
    }
#endif

  /* wait for up to 0.5 second for a UDP response */

  timeout.tv_sec = 0;
  timeout.tv_usec = 500 * USLEEP_MS;

  active_fdset = fdset;

  while (select (last_socketfd + 1, &active_fdset, NULL, NULL, &timeout) > 0)
    {
      PDBG (pixma_dbg (LOG_DEBUG, "Select returned, time left %d.%d....\n",
		       (int) timeout.tv_sec, (int) timeout.tv_usec));
      for (i = 0; i < no_sockets; i++)
	{
	  if (FD_ISSET (socket_fd[i], &active_fdset))
	    {
	      if ((numbytes =
		   recv (socket_fd[i], resp_buf, sizeof (resp_buf), 0)) == -1)
		{
		  PDBG (pixma_dbg
			(LOG_INFO, "bjnp_send_broadcasts: no data received"));
		  break;
		}
	      else
		{
		  PDBG (pixma_dbg (LOG_DEBUG2, "Discover response:\n"));
		  PDBG (pixma_hexdump (LOG_DEBUG2, &resp_buf, numbytes));

		  /* check if ip-address of scanner is returned */
		  if ((numbytes != sizeof (struct DISCOVER_RESPONSE))
		      || (strncmp ("BJNP", resp_buf, 4) != 0))
		    {
		      /* not a valid response, assume not a scanner  */
		      break;
		    }
		};

	      /* scanner found, get IP-address and hostname */
	      parse_scanner_address (resp_buf, hostname, serial);

	      /* construct URI */
	      sprintf (uri, "%s://%s:%d", BJNP_METHOD, hostname,
		       BJNP_PORT_SCAN);

	      /* Test scanner connection by attaching it and reading its IEEE1284 id */

	      switch (bjnp_allocate_device (uri, &dev_no, NULL))
		{
		case BJNP_STATUS_GOOD:
		  if (get_scanner_id (dev_no, makemodel) != 0)
		    {
		      PDBG (pixma_dbg
			    (LOG_CRIT,
			     "Scanner not added: Cannot read scanner make & model: %s\n",
			     uri));
		    }
		  else
		    {
		      /*
		       * inform caller of found scanner
		       */

		      truncate_hostname (hostname, short_hostname);
		      attach_bjnp (uri, makemodel, short_hostname,
				   pixma_devices);
		    }
		  break;

		case BJNP_STATUS_INVAL:
		  PDBG (pixma_dbg
			(LOG_NOTICE,
			 "Found scanner at %s, but can not open it\n", uri));
		  break;

		case BJNP_STATUS_ALREADY_ALLOCATED:
		  /* already allocated before, no action required  */
		  PDBG (pixma_dbg
			(LOG_NOTICE,
			 "Scanner at %s detected, but it was added before, good!\n",
			 uri));
		  break;
		}
	    }
	}
      active_fdset = fdset;
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;
    }
  PDBG (pixma_dbg (LOG_DEBUG, "scanner discovery finished...\n"));

  for (i = 0; i < no_sockets; i++)
    close (socket_fd[i]);

  return SANE_STATUS_GOOD;
}

/** Open a BJNP device.
 *
 * The device is opened by its name devname and the device number is
 * returned in dn on success.  
 *
 * Device names consist of an URI                                     
 * Where:
 * type = bjnp
 * hostname = resolvable name or IP-address
 * port = 8612 for a scanner
 * An example could look like this: bjnp://host.domain:8612
 *
 * @param devname name of the device to open
 * @param dn device number
 *
 * @return
 * - SANE_STATUS_GOOD - on success
 * - SANE_STATUS_ACCESS_DENIED - if the file couldn't be accessed due to
 *   permissions
 * - SANE_STATUS_INVAL - on every other error
 */

extern SANE_Status
sanei_bjnp_open (SANE_String_Const devname, SANE_Int * dn)
{
  char pid_str[64];
  char my_hostname[256];
  char *login;

  PDBG (pixma_dbg (LOG_INFO, "sanei_bjnp_open(%s, %d):\n", devname, *dn));

  if (bjnp_allocate_device (devname, dn, NULL) == BJNP_STATUS_INVAL)
    return SANE_STATUS_INVAL;

  login = getusername ();
  gethostname (my_hostname, 256);
  my_hostname[255] = '\0';
  sprintf (pid_str, "Process ID = %d", getpid ());

  bjnp_send_job_details (*dn, my_hostname, login, pid_str);

  if (bjnp_open_tcp (*dn) != 0)
    return SANE_STATUS_INVAL;

  return SANE_STATUS_GOOD;
}

/** Close a BJNP device.
 * 
 * @param dn device number
 */

void
sanei_bjnp_close (SANE_Int dn)
{
  PDBG (pixma_dbg (LOG_INFO, "sanei_bjnp_close(%d):\n", dn));
  if (device[dn].active)
    sanei_bjnp_deactivate (dn);

  device[dn].open = 0;
}

/** Activate BJNP device connection
 *
 * @param dn device number
 */

SANE_Status
sanei_bjnp_activate (SANE_Int dn)
{
  char hostname[256];
  char pid_str[64];

  PDBG (pixma_dbg (LOG_INFO, "sanei_bjnp_activate (%d)\n", dn));
  gethostname (hostname, 256);
  hostname[255] = '\0';
  sprintf (pid_str, "Process ID = %d", getpid ());

  bjnp_send_job_details (dn, hostname, getusername (), pid_str);

  if (bjnp_open_tcp (dn) != 0)
    return SANE_STATUS_INVAL;

  return SANE_STATUS_GOOD;
}

/** Deactivate BJNP device connection
 *
 * @paran dn device number
 */

SANE_Status
sanei_bjnp_deactivate (SANE_Int dn)
{
  PDBG (pixma_dbg (LOG_INFO, "sanei_bjnp_deactivate (%d)\n", dn));
  bjnp_finish_job (dn);
  close (device[dn].fd);
  device[dn].fd = -1;
#ifdef PIXMA_BJNP_STATUS
  device[dn].polling = 0;
#endif
  return SANE_STATUS_GOOD;
}

/** Set the timeout for bulk and interrupt reads.
 * 
 * @param timeout the new timeout in ms
 */
extern void
sanei_bjnp_set_timeout (SANE_Int devno, SANE_Int timeout)
{
  int my_timeout = timeout;

  /* timeout must be at least 10 second */

  if (my_timeout < 20000)
    my_timeout = 20000;
  PDBG (pixma_dbg (LOG_INFO, "bjnp_set_timeout(requested %d, set %d):\n",
		   timeout, my_timeout));

  device[devno].bjnp_timeout_sec = my_timeout / 1000;
  device[devno].bjnp_timeout_msec = my_timeout % 1000;
}

/** Initiate a bulk transfer read.
 *
 * Read up to size bytes from the device to buffer. After the read, size
 * contains the number of bytes actually read.
 *
 * @param dn device number
 * @param buffer buffer to store read data in
 * @param size size of the data
 *
 * @return 
 * - SANE_STATUS_GOOD - on succes
 * - SANE_STATUS_EOF - if zero bytes have been read
 * - SANE_STATUS_IO_ERROR - if an error occured during the read
 * - SANE_STATUS_INVAL - on every other error
 *
 */

extern SANE_Status
sanei_bjnp_read_bulk (SANE_Int dn, SANE_Byte * buffer, size_t * size)
{
  SANE_Status result;
  SANE_Status error;
  size_t recvd;
  size_t more;
  size_t left;

  PDBG (pixma_dbg
	(LOG_DEBUG, "bjnp_read_bulk(%d, bufferptr, 0x%lx = %ld)\n", dn,
	 (unsigned long) *size, (unsigned long) size));

  recvd = 0;
  left = *size;

  if ((device[dn].scanner_data_left == 0) && (device[dn].short_read != 0))
    {
      /* new read, but we have no data queued from scanner, last read was short, */
      /* so scanner needs first a high level read command. This is not an error */

      PDBG (pixma_dbg
	    (LOG_DEBUG,
	     "Scanner has no more data available, return immediately!\n"));
      *size = 0;
      return SANE_STATUS_EOF;
    }

  PDBG (pixma_dbg
	(LOG_DEBUG, "bjnp_read_bulk: 0x%lx = %ld bytes available at start, "
	 "Short block = %d blocksize = 0x%lx = %ld\n",
	 (unsigned long) device[dn].scanner_data_left,
	 (unsigned long) device[dn].scanner_data_left,
	 (int) device[dn].short_read,
	 (unsigned long) device[dn].blocksize, 
	 (unsigned long) device[dn].blocksize));

  while ((recvd < *size)
	 && (!device[dn].short_read || device[dn].scanner_data_left))
    {
      PDBG (pixma_dbg
	    (LOG_DEBUG,
	     "So far received 0x%lx bytes = %ld, need 0x%lx = %ld\n",
	     (unsigned long) recvd, (unsigned long) recvd, 
	     (unsigned long) *size, (unsigned long) *size));

      if (device[dn].scanner_data_left == 0)
	{
	  /*
	   * send new read request
	   * TODO: Better error checking below 
	   */

	  PDBG (pixma_dbg
		(LOG_DEBUG,
		 "No (more) scanner data available, requesting more\n"));

	  if ((error = bjnp_send_read_request (dn)) != SANE_STATUS_GOOD)
	    {
	      *size = recvd;
	      return SANE_STATUS_IO_ERROR;
	    }
	  if ((error = bjnp_recv_header (dn)) != SANE_STATUS_GOOD)
	    {
	      *size = recvd;
	      return SANE_STATUS_IO_ERROR;
	    }
	  PDBG (pixma_dbg
		(LOG_DEBUG, "Scanner reports 0x%lx = %ld bytes available\n",
		 (unsigned long) device[dn].scanner_data_left,
		 (unsigned long) device[dn].scanner_data_left));

	  /* correct blocksize if more data is sent by scanner than current blocksize assumption */

	  if (device[dn].scanner_data_left > device[dn].blocksize)
	    device[dn].blocksize = device[dn].scanner_data_left;

	  /* decide if we reached end of data to be sent by scanner */

	  device[dn].short_read =
	    (device[dn].scanner_data_left < device[dn].blocksize);
	}

      more = left;

      PDBG (pixma_dbg
	    (LOG_DEBUG,
	     "reading 0x%lx = %ld (of max 0x%lx = %ld) bytes more\n",
	     (unsigned long) device[dn].scanner_data_left, 
	     (unsigned long) device[dn].scanner_data_left,
	     (unsigned long) more, 
	     (unsigned long) more));
      result = bjnp_recv_data (dn, buffer, &more);
      if (result != SANE_STATUS_GOOD)
	{
	  *size = recvd;
	  return SANE_STATUS_IO_ERROR;
	}

      left = left - more;
      recvd = recvd + more;
      buffer = buffer + more;
    }
  *size = recvd;
  return SANE_STATUS_GOOD;
}

/** Initiate a bulk transfer write.
 *
 * Write up to size bytes from buffer to the device. After the write size
 * contains the number of bytes actually written.
 *
 * @param dn device number
 * @param buffer buffer to write to device
 * @param size size of the data
 *
 * @return 
 * - SANE_STATUS_GOOD - on succes
 * - SANE_STATUS_IO_ERROR - if an error occured during the write
 * - SANE_STATUS_INVAL - on every other error
 */

extern SANE_Status
sanei_bjnp_write_bulk (SANE_Int dn, const SANE_Byte * buffer, size_t * size)
{
  ssize_t sent;
  size_t recvd;
  uint32_t buf;

  PDBG (pixma_dbg
	(LOG_INFO, "bjnp_write_bulk(%d, bufferptr, 0x%lx = %ld)\n", dn,
	 (unsigned long) *size, (unsigned long) *size));
  sent = bjnp_write (dn, buffer, *size);
  if (sent < 0)
    return SANE_STATUS_IO_ERROR;
  if (sent != (int) *size)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT, "Sent only %ld bytes to scanner, expected %ld!!\n",
	     (unsigned long) sent, (unsigned long) *size));
      return SANE_STATUS_IO_ERROR;
    }

  if (bjnp_recv_header (dn) != SANE_STATUS_GOOD)
    {
      PDBG (pixma_dbg (LOG_CRIT, "Could not read response to command!\n"));
      return SANE_STATUS_IO_ERROR;
    }

  if (device[dn].scanner_data_left != 4)
    {
      PDBG (pixma_dbg (LOG_CRIT,
		       "Scanner length of write confirmation = 0x%lx bytes = %ld, expected %d!!\n",
		       (unsigned long) device[dn].scanner_data_left,
		       (unsigned long) device[dn].scanner_data_left, 4));
      return SANE_STATUS_IO_ERROR;
    }
  recvd = 4;
  if ((bjnp_recv_data (dn, (unsigned char *) &buf, &recvd) !=
       SANE_STATUS_GOOD) || (recvd != 4))
    {
      PDBG (pixma_dbg (LOG_CRIT,
		       "Could not read length of data confirmed by device\n"));
      return SANE_STATUS_IO_ERROR;
    }
  recvd = ntohl (buf);
  if (recvd != *size)
    {
      PDBG (pixma_dbg
	    (LOG_CRIT, "Scanner confirmed %ld bytes, expected %ld!!\n",
	     (unsigned long) recvd, (unsigned long) *size));
      return SANE_STATUS_IO_ERROR;
    }

  /* we sent a new command, so reset end of block indication */

  device[dn].short_read = 0;

  return SANE_STATUS_GOOD;
}

/** Initiate a interrupt transfer read.
 *
 * Read up to size bytes from the interrupt endpoint from the device to
 * buffer. After the read, size contains the number of bytes actually read.
 *
 * @param dn device number
 * @param buffer buffer to store read data in
 * @param size size of the data
 *
 * @return 
 * - SANE_STATUS_GOOD - on succes
 * - SANE_STATUS_EOF - if zero bytes have been read
 * - SANE_STATUS_IO_ERROR - if an error occured during the read
 * - SANE_STATUS_INVAL - on every other error
 *
 */

extern SANE_Status
sanei_bjnp_read_int (SANE_Int dn, SANE_Byte * buffer, size_t * size)
{
#ifndef PIXMA_BJNP_STATUS
  PDBG (pixma_dbg
	(LOG_INFO, "bjnp_read_int(%d, bufferptr, 0x%lx = %ld):\n", dn,
	 (unsigned long) *size, (unsigned long) *size));
  memset (buffer, 0, *size);
  sleep (1);
  return SANE_STATUS_IO_ERROR;
#else
  char hostname[256];
  int result;
  int i;

  PDBG (pixma_dbg
	(LOG_INFO, "bjnp_read_int(%d, bufferptr, %lx = %ld):\n", dn,
	 (unsigned long) *size, (unsigned long) *size));

  gethostname (hostname, 32);
  hostname[32] = '\0';


  if (device[dn].polling == 0)
    {
      /* establish status/read_intr dialogue */

      device[dn].dialogue++;

      if ((bjnp_get_intr (dn, 0, hostname, getusername (), buffer,
			  *size) != (ssize_t) * size)
	  || sleep (4)
	  || (bjnp_get_intr (dn, 1, hostname, getusername (), buffer,
			     *size) != (ssize_t) * size))
	{
	  PDBG (pixma_dbg
		(LOG_NOTICE,
		 "Failed to setup read_intr dialogue with device!\n"));
	}
      device[dn].polling = 1;
    }

  result = bjnp_get_intr (dn, 2, hostname, getusername (), buffer, *size);
  if (result < (int) *size)
    {
      *size = (result > 0) ? result : 0;
      PDBG (pixma_dbg
	    (LOG_CRIT,
	     "Could not retrieve interrupt information from device!\n"));
      return SANE_STATUS_IO_ERROR;
    }
  for (i = 0; i < result; i++)
    {
      if (buffer[i] != '\0')
	{
	  PDBG (pixma_dbg (LOG_NOTICE, "Interrupt data received...\n"));
	  PDBG (pixma_hexdump (LOG_NOTICE, buffer, result));
	  return SANE_STATUS_GOOD;
	}
    }
  /* no interrupt received, we just sleep for the indicated mount of time
   * and leave it to the caller to retry */

  sleep (4);

  PDBG (pixma_dbg (LOG_NOTICE, "sanei_bjnp_read_int: no data received\n"));

  return SANE_STATUS_IO_ERROR;
#endif
}
