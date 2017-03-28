/* sane - Scanner Access Now Easy.
   Copyright (C) 2001 - 2005 Henning Meier-Geinitz
   Copyright (C) 2001 Frank Zago (sanei_usb_control_msg)
   Copyright (C) 2003 Rene Rebe (sanei_read_int,sanei_set_timeout)
   Copyright (C) 2005 Paul Smedley <paul@smedley.info> (OS/2 usbcalls)
   Copyright (C) 2008 m. allan noah (bus rescan support, sanei_usb_clear_halt)
   Copyright (C) 2009 Julien BLACHE <jb@jblache.org> (libusb-1.0)
   Copyright (C) 2011 Reinhold Kainhofer <reinhold@kainhofer.com> (sanei_usb_set_endpoint)
   This file is part of the SANE package.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.

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

   This file provides a generic USB interface.  */

#include "../include/sane/config.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#include <stdio.h>
#include <dirent.h>
#include <time.h>

#ifdef HAVE_RESMGR
#include <resmgr.h>
#endif

#ifdef HAVE_LIBUSB
#ifdef HAVE_LUSB0_USB_H
#include <lusb0_usb.h>
#else
#include <usb.h>
#endif
#endif /* HAVE_LIBUSB */

#ifdef HAVE_LIBUSB_1_0
#include <libusb.h>
#endif /* HAVE_LIBUSB_1_0 */

#ifdef HAVE_USBCALLS
#include <usb.h>
#include <os2.h>
#include <usbcalls.h>
#define MAX_RW 64000
static int usbcalls_timeout = 30 * 1000;	/* 30 seconds */
USBHANDLE dh;
PHEV pUsbIrqStartHev=NULL;

static
struct usb_descriptor_header *
GetNextDescriptor( struct usb_descriptor_header *currHead, UCHAR *lastBytePtr)
{
  UCHAR    *currBytePtr, *nextBytePtr;

  if (!currHead->bLength)
     return (NULL);
  currBytePtr=(UCHAR *)currHead;
  nextBytePtr=currBytePtr+currHead->bLength;
  if (nextBytePtr>=lastBytePtr)
     return (NULL);
  return ((struct usb_descriptor_header*)nextBytePtr);
}
#endif /* HAVE_USBCALLS */

#if (defined (__FreeBSD__) && (__FreeBSD_version < 800064))
#include <sys/param.h>
#include <dev/usb/usb.h>
#endif /* __FreeBSD__ */
#if defined (__DragonFly__)
#include <bus/usb/usb.h>
#endif

#define BACKEND_NAME	sanei_usb
#include "../include/sane/sane.h"
#include "../include/sane/sanei_debug.h"
#include "../include/sane/sanei_usb.h"
#include "../include/sane/sanei_config.h"

typedef enum
{
  sanei_usb_method_scanner_driver = 0,	/* kernel scanner driver 
					   (Linux, BSD) */
  sanei_usb_method_libusb,

  sanei_usb_method_usbcalls
}
sanei_usb_access_method_type;

typedef struct
{
  SANE_Bool open;
  sanei_usb_access_method_type method;
  int fd;
  SANE_String devname;
  SANE_Int vendor;
  SANE_Int product;
  SANE_Int bulk_in_ep;
  SANE_Int bulk_out_ep;
  SANE_Int iso_in_ep;
  SANE_Int iso_out_ep;
  SANE_Int int_in_ep;
  SANE_Int int_out_ep;
  SANE_Int control_in_ep;
  SANE_Int control_out_ep;
  SANE_Int interface_nr;
  SANE_Int missing;
#ifdef HAVE_LIBUSB
  usb_dev_handle *libusb_handle;
  struct usb_device *libusb_device;
#endif /* HAVE_LIBUSB */
#ifdef HAVE_LIBUSB_1_0
  libusb_device *lu_device;
  libusb_device_handle *lu_handle;
#endif /* HAVE_LIBUSB_1_0 */
}
device_list_type;

/** 
 * total number of devices that can be found at the same time */
#define MAX_DEVICES 100

/** 
 * per-device information, using the functions' parameters dn as index */
static device_list_type devices[MAX_DEVICES];

/**
 * total number of detected devices in devices array */
static int device_number=0;

#if defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0)
static int libusb_timeout = 30 * 1000;	/* 30 seconds */
#endif /* HAVE_LIBUSB */

#ifdef HAVE_LIBUSB_1_0
static libusb_context *sanei_usb_ctx;
#endif /* HAVE_LIBUSB_1_0 */

#if defined (__linux__)
/* From /usr/src/linux/driver/usb/scanner.h */
#define SCANNER_IOCTL_VENDOR _IOR('U', 0x20, int)
#define SCANNER_IOCTL_PRODUCT _IOR('U', 0x21, int)
#define SCANNER_IOCTL_CTRLMSG _IOWR('U', 0x22, devrequest)
/* Older (unofficial) IOCTL numbers for Linux < v2.4.13 */
#define SCANNER_IOCTL_VENDOR_OLD _IOR('u', 0xa0, int)
#define SCANNER_IOCTL_PRODUCT_OLD _IOR('u', 0xa1, int)

/* From /usr/src/linux/include/linux/usb.h */
typedef struct
{
  unsigned char requesttype;
  unsigned char request;
  unsigned short value;
  unsigned short index;
  unsigned short length;
}
devrequest;

/* From /usr/src/linux/driver/usb/scanner.h */
struct ctrlmsg_ioctl
{
  devrequest req;
  void *data;
}
cmsg;
#elif defined(__BEOS__)
#include <drivers/USB_scanner.h>
#include <kernel/OS.h>
#endif /* __linux__ */

/* Debug level from sanei_init_debug */
static SANE_Int debug_level;

static void
print_buffer (const SANE_Byte * buffer, SANE_Int size)
{
#define NUM_COLUMNS 16
#define PRINT_BUFFER_SIZE (4 + NUM_COLUMNS * (3 + 1) + 1 + 1)
  char line_str[PRINT_BUFFER_SIZE];
  char *pp;
  int column;
  int line;

  memset (line_str, 0, PRINT_BUFFER_SIZE);

  for (line = 0; line < ((size + NUM_COLUMNS - 1) / NUM_COLUMNS); line++)
    {
      pp = line_str;
      sprintf (pp, "%03X ", line * NUM_COLUMNS);
      pp += 4;
      for (column = 0; column < NUM_COLUMNS; column++)
	{
	  if ((line * NUM_COLUMNS + column) < size)
	    sprintf (pp, "%02X ", buffer[line * NUM_COLUMNS + column]);
	  else
	    sprintf (pp, "   ");
	  pp += 3;
	}
      for (column = 0; column < NUM_COLUMNS; column++)
	{
	  if ((line * NUM_COLUMNS + column) < size)
	    sprintf (pp, "%c",
		     (buffer[line * NUM_COLUMNS + column] < 127) &&
		     (buffer[line * NUM_COLUMNS + column] > 31) ?
		     buffer[line * NUM_COLUMNS + column] : '.');
	  else
	    sprintf (pp, " ");
	  pp += 1;
	}
      DBG (11, "%s\n", line_str);
    }
}

static void
kernel_get_vendor_product (int fd, const char *name, int *vendorID, int *productID)
{
#if defined (__linux__)
  /* read the vendor and product IDs via the IOCTLs */
  if (ioctl (fd, SCANNER_IOCTL_VENDOR, vendorID) == -1)
    {
      if (ioctl (fd, SCANNER_IOCTL_VENDOR_OLD, vendorID) == -1)
	DBG (3, "kernel_get_vendor_product: ioctl (vendor) "
	     "of device %s failed: %s\n", name, strerror (errno));
    }
  if (ioctl (fd, SCANNER_IOCTL_PRODUCT, productID) == -1)
    {
      if (ioctl (fd, SCANNER_IOCTL_PRODUCT_OLD, productID) == -1)
	DBG (3, "sanei_usb_get_vendor_product: ioctl (product) "
	     "of device %s failed: %s\n", name, strerror (errno));
    }
#elif defined(__BEOS__)
  {
    uint16 vendor, product;
    if (ioctl (fd, B_SCANNER_IOCTL_VENDOR, &vendor) != B_OK)
      DBG (3, "kernel_get_vendor_product: ioctl (vendor) "
	   "of device %d failed: %s\n", fd, strerror (errno));
    if (ioctl (fd, B_SCANNER_IOCTL_PRODUCT, &product) != B_OK)
      DBG (3, "sanei_usb_get_vendor_product: ioctl (product) "
	   "of device %d failed: %s\n", fd, strerror (errno));
    /* copy from 16 to 32 bit value */
    *vendorID = vendor;
    *productID = product;
  }
#elif (defined (__FreeBSD__) && __FreeBSD_version < 800064) || defined (__DragonFly__)
  {
    int controller;
    int ctrl_fd;
    char buf[40];
    int dev;
    
    for (controller = 0; ; controller++ )
      {
	snprintf (buf, sizeof (buf) - 1, "/dev/usb%d", controller);
	ctrl_fd = open (buf, O_RDWR);
	
	/* If we can not open the usb controller device, treat it
	   as the end of controller devices */
	if (ctrl_fd < 0)
	  break;
	
	/* Search for the scanner device on this bus */
	for (dev = 1; dev < USB_MAX_DEVICES; dev++)
	  {  
	    struct usb_device_info devInfo;
	    devInfo.udi_addr = dev;
	    
	    if (ioctl (ctrl_fd, USB_DEVICEINFO, &devInfo) == -1)
	      break; /* Treat this as the end of devices for this controller */
	    
	    snprintf (buf, sizeof (buf), "/dev/%s", devInfo.udi_devnames[0]);
	    if (strncmp (buf, name, sizeof (buf)) == 0)
	      {
		*vendorID = (int) devInfo.udi_vendorNo;
		*productID = (int) devInfo.udi_productNo;
		close (ctrl_fd);
		return;
	      }
	  }
	close (ctrl_fd);
      }
    DBG (3, "kernel_get_vendor_product: Could not retrieve "
	 "vendor/product ID from device %s\n", name);
  }
#endif /* defined (__linux__), defined(__BEOS__), ... */
  /* put more os-dependant stuff ... */
}

/**
 * store the given device in device list if it isn't already
 * in it
 * @param device device to store if new
 */
static void
store_device (device_list_type device)
{
  int i = 0;
  int pos = -1;

  /* if there are already some devices present, check against 
   * them and leave if an equal one is found */
  for (i = 0; i < device_number; i++)
    {
      if (devices[i].method == device.method
       && !strcmp (devices[i].devname, device.devname)
       && devices[i].vendor == device.vendor
       && devices[i].product == device.product)
	{
          /* 
          * Need to update the LibUSB device pointer, since it might
          * have changed after the latest USB scan.
          */
#ifdef HAVE_LIBUSB
          devices[i].libusb_device = device.libusb_device;
#endif
#ifdef HAVE_LIBUSB_1_0
          devices[i].lu_device = device.lu_device;
#endif

          devices[i].missing=0;
	  DBG (3, "store_device: not storing device %s\n", device.devname);
	  return;
	}
      if (devices[i].missing >= 2)
        pos = i;
    }

  if(pos > -1){
    DBG (3, "store_device: overwrite dn %d with %s\n", pos, device.devname);
  }
  else{
    if(device_number >= MAX_DEVICES){
      DBG (3, "store_device: no room for %s\n", device.devname);
      return;
    }
    pos = device_number;
    device_number++;
    DBG (3, "store_device: add dn %d with %s\n", pos, device.devname);
  }
  memcpy (&(devices[pos]), &device, sizeof (device));
  devices[pos].open = SANE_FALSE;
}

#ifdef HAVE_LIBUSB_1_0
static char *
sanei_libusb_strerror (int errcode)
{
  /* Error codes & descriptions from the libusb-1.0 documentation */

  switch (errcode)
    {
      case LIBUSB_SUCCESS:
	return "Success (no error)";

      case LIBUSB_ERROR_IO:
	return "Input/output error";

      case LIBUSB_ERROR_INVALID_PARAM:
	return "Invalid parameter";

      case LIBUSB_ERROR_ACCESS:
	return "Access denied (insufficient permissions)";

      case LIBUSB_ERROR_NO_DEVICE:
	return "No such device (it may have been disconnected)";

      case LIBUSB_ERROR_NOT_FOUND:
	return "Entity not found";

      case LIBUSB_ERROR_BUSY:
	return "Resource busy";

      case LIBUSB_ERROR_TIMEOUT:
	return "Operation timed out";

      case LIBUSB_ERROR_OVERFLOW:
	return "Overflow";

      case LIBUSB_ERROR_PIPE:
	return "Pipe error";

      case LIBUSB_ERROR_INTERRUPTED:
	return "System call interrupted (perhaps due to signal)";

      case LIBUSB_ERROR_NO_MEM:
	return "Insufficient memory";

      case LIBUSB_ERROR_NOT_SUPPORTED:
	return "Operation not supported or unimplemented on this platform";

      case LIBUSB_ERROR_OTHER:
	return "Other error";

      default:
	return "Unknown libusb-1.0 error code";
    }

  return "Unknown libusb-1.0 error code";
}
#endif /* HAVE_LIBUSB_1_0 */

void
sanei_usb_init (void)
{
  SANE_String *prefix;
  SANE_String prefixlist[] = {
#if defined(__linux__)
    "/dev/", "usbscanner",
    "/dev/usb/", "scanner",
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined (__OpenBSD__) || defined (__DragonFly__)
    "/dev/", "uscanner",
#elif defined(__BEOS__)
    "/dev/scanner/usb/", "",
#endif
    0, 0
  };
  SANE_Int vendor, product;
  SANE_Char devname[1024];
  int fd;
  int i;
  device_list_type device;
#ifdef HAVE_LIBUSB
  struct usb_bus *bus;
  struct usb_device *dev;
#endif /* HAVE_LIBUSB */
#ifdef HAVE_LIBUSB_1_0
  libusb_device **devlist;
  ssize_t ndev;

  libusb_device *dev;
  libusb_device_handle *hdl;
  struct libusb_device_descriptor desc;
  struct libusb_config_descriptor *config0;
  unsigned short vid, pid;
  unsigned char busno, address;
  int config;
  int interface;
  int ret;
#endif /* HAVE_LIBUSB_1_0 */

  DBG_INIT ();
#ifdef DBG_LEVEL
  debug_level = DBG_LEVEL;
#else
  debug_level = 0;
#endif

  /* if no device yet, clean up memory */
  if(device_number==0)
    memset (devices, 0, sizeof (devices));

  DBG (4, "sanei_usb_init: marking existing devices\n");
  for (i = 0; i < device_number; i++) {
    devices[i].missing++;
  }

  DBG (4, "sanei_usb_init: Looking for kernel scanner devices\n");
  /* Check for devices using the kernel scanner driver */

  for (prefix = prefixlist; *prefix; prefix += 2)
    {
      SANE_String dir_name = *prefix;
      SANE_String base_name = *(prefix + 1);
      struct stat stat_buf;
      DIR *dir;
      struct dirent *dir_entry;

      if (stat (dir_name, &stat_buf) < 0)
	{
	  DBG (5, "sanei_usb_init: can't stat %s: %s\n", dir_name,
	       strerror (errno));
	  continue;
	}
      if (!S_ISDIR (stat_buf.st_mode))
	{
	  DBG (5, "sanei_usb_init: %s is not a directory\n", dir_name);
	  continue;
	}
      if ((dir = opendir (dir_name)) == 0)
	{
	  DBG (5, "sanei_usb_init: cannot read directory %s: %s\n", dir_name,
	       strerror (errno));
	  continue;
	}

      while ((dir_entry = readdir (dir)) != 0)
	{
	  /* skip standard dir entries */
	  if (strcmp (dir_entry->d_name, ".") == 0 || strcmp (dir_entry->d_name, "..") == 0)
	  	continue;
	  		
	  if (strncmp (base_name, dir_entry->d_name, strlen (base_name)) == 0)
	    {
	      if (strlen (dir_name) + strlen (dir_entry->d_name) + 1 >
		  sizeof (devname))
		continue;
	      sprintf (devname, "%s%s", dir_name, dir_entry->d_name);
	      fd = -1;
#ifdef HAVE_RESMGR
	      fd = rsm_open_device (devname, O_RDWR);
#endif
	      if (fd == -1)
		fd = open (devname, O_RDWR);
	      if (fd < 0)
		{
		  DBG (5, "sanei_usb_init: couldn't open %s: %s\n", devname,
		       strerror (errno));
		  continue;
		}
	      vendor = -1;
	      product = -1;
	      kernel_get_vendor_product (fd, devname, &vendor, &product);
	      close (fd);
    	      memset (&device, 0, sizeof (device));
	      device.devname = strdup (devname);
	      if (!device.devname)
		{
		  closedir (dir);
		  return;
		}
	      device.vendor = vendor;
	      device.product = product;
	      device.method = sanei_usb_method_scanner_driver;
	      DBG (4,
		   "sanei_usb_init: found kernel scanner device (0x%04x/0x%04x) at %s\n",
		   vendor, product, devname);
	      store_device(device);
	    }
	}
      closedir (dir);
    }

  /* Check for devices using (old) libusb */
#ifdef HAVE_LIBUSB
  DBG (4, "sanei_usb_init: Looking for libusb devices\n");
  usb_init ();
#ifdef DBG_LEVEL
  if (DBG_LEVEL > 4)
    usb_set_debug (255);
#endif /* DBG_LEVEL */

  usb_find_busses ();
  usb_find_devices ();

  /* Check for the matching device */
  for (bus = usb_get_busses (); bus; bus = bus->next)
    {
      for (dev = bus->devices; dev; dev = dev->next)
	{
	  int interface;
	  SANE_Bool found = SANE_FALSE;

	  if (!dev->config)
	    {
	      DBG (1,
		   "sanei_usb_init: device 0x%04x/0x%04x is not configured\n",
		   dev->descriptor.idVendor, dev->descriptor.idProduct);
	      continue;
	    }
	  if (dev->descriptor.idVendor == 0 || dev->descriptor.idProduct == 0)
	    {
	      DBG (5,
		 "sanei_usb_init: device 0x%04x/0x%04x looks like a root hub\n",
		 dev->descriptor.idVendor, dev->descriptor.idProduct);
	      continue;
	    }

	  for (interface = 0;
	       interface < dev->config[0].bNumInterfaces && !found;
	       interface++)
	    {
	      switch (dev->descriptor.bDeviceClass)
		{
		case USB_CLASS_VENDOR_SPEC:
		  found = SANE_TRUE;
		  break;
		case USB_CLASS_PER_INTERFACE:
		  if (dev->config[0].interface[interface].num_altsetting == 0 || 
		      !dev->config[0].interface[interface].altsetting)
		    {
		      DBG (1, "sanei_usb_init: device 0x%04x/0x%04x doesn't "
			   "have an altsetting for interface %d\n",
			   dev->descriptor.idVendor, dev->descriptor.idProduct,
			   interface);
		      continue;
		    }
		  switch (dev->config[0].interface[interface].altsetting[0].
			  bInterfaceClass)
		    {
		    case USB_CLASS_VENDOR_SPEC:
		    case USB_CLASS_PER_INTERFACE:
		    case 6:	/* imaging? */
		    case 16:	/* data? */
		      found = SANE_TRUE;
		      break;
		    }
		  break;
		}
	      if (!found)
		DBG (5,
		     "sanei_usb_init: device 0x%04x/0x%04x, interface %d "
                     "doesn't look like a "
		     "scanner (%d/%d)\n", dev->descriptor.idVendor,
		     dev->descriptor.idProduct, interface,
		     dev->descriptor.bDeviceClass,
		     dev->config[0].interface[interface].altsetting != 0 
                       ? dev->config[0].interface[interface].altsetting[0].
		       bInterfaceClass : -1);
	    }
	  interface--;
	  if (!found)
	    {
	      DBG (5,
	       "sanei_usb_init: device 0x%04x/0x%04x: no suitable interfaces\n",
	        dev->descriptor.idVendor, dev->descriptor.idProduct);
	      continue;
	    }

    	  memset (&device, 0, sizeof (device));
	  device.libusb_device = dev;
	  snprintf (devname, sizeof (devname), "libusb:%s:%s",
		    dev->bus->dirname, dev->filename);
	  device.devname = strdup (devname);
	  if (!device.devname)
	    return;
	  device.vendor = dev->descriptor.idVendor;
	  device.product = dev->descriptor.idProduct;
	  device.method = sanei_usb_method_libusb;
	  device.interface_nr = interface;
	  DBG (4,
	       "sanei_usb_init: found libusb device (0x%04x/0x%04x) interface "
               "%d  at %s\n",
	       dev->descriptor.idVendor, dev->descriptor.idProduct, interface,
	       devname);
	  store_device(device);
	}
    }
#endif /* HAVE_LIBUSB */


  /* Check for devices using libusb-1.0 */
#ifdef HAVE_LIBUSB_1_0
  if (!sanei_usb_ctx)
    {
      DBG (4, "sanei_usb_init: initializing libusb-1.0\n");
      ret = libusb_init (&sanei_usb_ctx);
      if (ret < 0)
	{
	  DBG (1,
	       "sanei_usb_init: failed to initialize libusb-1.0, error %d\n",
	       ret);

	  goto failed_libusb_1_0;
	}
#ifdef DBG_LEVEL
      if (DBG_LEVEL > 4)
	libusb_set_debug (sanei_usb_ctx, 3);
#endif /* DBG_LEVEL */
    }

  DBG (4, "sanei_usb_init: Looking for libusb-1.0 devices\n");

  ndev = libusb_get_device_list (sanei_usb_ctx, &devlist);
  if (ndev < 0)
    {
      DBG (1,
	   "sanei_usb_init: failed to get libusb-1.0 device list, error %d\n",
	   (int) ndev);

      goto failed_libusb_1_0;
    }

  for (i = 0; i < ndev; i++)
    {
      SANE_Bool found = SANE_FALSE;

      dev = devlist[i];

      busno = libusb_get_bus_number (dev);
      address = libusb_get_device_address (dev);

      ret = libusb_get_device_descriptor (dev, &desc);
      if (ret < 0)
	{
	  DBG (1,
	       "sanei_usb_init: could not get device descriptor for device at %03d:%03d (err %d)\n",
	       busno, address, ret);
	  continue;
	}

      vid = desc.idVendor;
      pid = desc.idProduct;

      if ((vid == 0) || (pid == 0))
	{
	  DBG (5,
	       "sanei_usb_init: device 0x%04x/0x%04x at %03d:%03d looks like a root hub\n",
	       vid, pid, busno, address);
	  continue;
	}

      ret = libusb_open (dev, &hdl);
      if (ret < 0)
	{
	  DBG (1,
	       "sanei_usb_init: skipping device 0x%04x/0x%04x at %03d:%03d: cannot open: %s\n",
	       vid, pid, busno, address, sanei_libusb_strerror (ret));

	  continue;
	}

      ret = libusb_get_configuration (hdl, &config);

      libusb_close (hdl);

      if (ret < 0)
	{
	  DBG (1,
	       "sanei_usb_init: could not get configuration for device 0x%04x/0x%04x at %03d:%03d (err %d)\n",
	       vid, pid, busno, address, ret);
	  continue;
	}

      if (config == 0)
	{
	  DBG (1,
	       "sanei_usb_init: device 0x%04x/0x%04x at %03d:%03d is not configured\n",
	       vid, pid, busno, address);
	  continue;
	}

      ret = libusb_get_config_descriptor (dev, 0, &config0);
      if (ret < 0)
	{
	  DBG (1,
	       "sanei_usb_init: could not get config[0] descriptor for device 0x%04x/0x%04x at %03d:%03d (err %d)\n",
	       vid, pid, busno, address, ret);
	  continue;
	}

      for (interface = 0; (interface < config0->bNumInterfaces) && !found; interface++)
	{
	  switch (desc.bDeviceClass)
	    {
	      case LIBUSB_CLASS_VENDOR_SPEC:
		found = SANE_TRUE;
		break;

	      case LIBUSB_CLASS_PER_INTERFACE:
		if ((config0->interface[interface].num_altsetting == 0)
		    || !config0->interface[interface].altsetting)
		  {
		    DBG (1, "sanei_usb_init: device 0x%04x/0x%04x doesn't "
			 "have an altsetting for interface %d\n",
			 vid, pid, interface);
		    continue;
		  }

		switch (config0->interface[interface].altsetting[0].bInterfaceClass)
		  {
		    case LIBUSB_CLASS_VENDOR_SPEC:
		    case LIBUSB_CLASS_PER_INTERFACE:
		    case LIBUSB_CLASS_PTP:
		    case 16:	/* data? */
		      found = SANE_TRUE;
		      break;
		  }
		break;
	    }

	  if (!found)
	    DBG (5,
		 "sanei_usb_init: device 0x%04x/0x%04x, interface %d "
		 "doesn't look like a scanner (%d/%d)\n",
		 vid, pid, interface, desc.bDeviceClass,
		 (config0->interface[interface].altsetting != 0)
		 ? config0->interface[interface].altsetting[0].bInterfaceClass : -1);
	}

      libusb_free_config_descriptor (config0);

      interface--;

      if (!found)
	{
	  DBG (5,
	       "sanei_usb_init: device 0x%04x/0x%04x at %03d:%03d: no suitable interfaces\n",
	       vid, pid, busno, address);
	  continue;
	}

      memset (&device, 0, sizeof (device));
      device.lu_device = libusb_ref_device(dev);
      snprintf (devname, sizeof (devname), "libusb:%03d:%03d",
		busno, address);
      device.devname = strdup (devname);
      if (!device.devname)
	return;
      device.vendor = vid;
      device.product = pid;
      device.method = sanei_usb_method_libusb;
      device.interface_nr = interface;
      DBG (4,
	   "sanei_usb_init: found libusb-1.0 device (0x%04x/0x%04x) interface "
	   "%d at %s\n",
	   vid, pid, interface, devname);

      store_device (device);
    }

  libusb_free_device_list (devlist, 1);

 failed_libusb_1_0:
  /* libusb 1.0 failed to initialize */

#endif /* HAVE_LIBUSB_1_0 */

#if !defined(HAVE_LIBUSB) && !defined(HAVE_LIBUSB_1_0)
  DBG (4, "sanei_usb_init: SANE is built without support for libusb\n");
#endif


#ifdef HAVE_USBCALLS
  /* Check for devices using OS/2 USBCALLS Interface */

   CHAR ucData[2048];
   struct usb_device_descriptor *pDevDesc;
   struct usb_config_descriptor   *pCfgDesc;
   struct usb_interface_descriptor *intf;
   struct usb_endpoint_descriptor  *ep;
   struct usb_descriptor_header    *pDescHead;

   APIRET rc;
   ULONG ulNumDev, ulDev, ulBufLen;

   ulBufLen = sizeof(ucData);
   memset(&ucData,0,sizeof(ucData));
   rc = UsbQueryNumberDevices( &ulNumDev);

   if(rc==0 && ulNumDev)
   {
       for (ulDev=1; ulDev<=ulNumDev; ulDev++)
       {
         rc = UsbQueryDeviceReport( ulDev,
                                  &ulBufLen,
                                  ucData);

         pDevDesc = (struct usb_device_descriptor*)ucData;
         pCfgDesc = (struct usb_config_descriptor*) (ucData+sizeof(struct usb_device_descriptor));
	  int interface=0;
	  SANE_Bool found;
	  if (!pCfgDesc->bConfigurationValue)
	    {
	      DBG (1, "sanei_usb_init: device 0x%04x/0x%04x is not configured\n",
		   pDevDesc->idVendor, pDevDesc->idProduct);
	      continue;
	    }
	  if (pDevDesc->idVendor == 0 || pDevDesc->idProduct == 0)
	    {
	      DBG (5, "sanei_usb_init: device 0x%04x/0x%04x looks like a root hub\n",
		   pDevDesc->idVendor, pDevDesc->idProduct);
	      continue;
	    }
	  found = SANE_FALSE;
          
          if (pDevDesc->bDeviceClass == USB_CLASS_VENDOR_SPEC)
           {
             found = SANE_TRUE;
           }

	  if (!found)
	    {
	      DBG (5, "sanei_usb_init: device 0x%04x/0x%04x: no suitable interfaces\n",
		   pDevDesc->idVendor, pDevDesc->idProduct);
	      continue;
	    }

	  snprintf (devname, sizeof (devname), "usbcalls:%d",
		    ulDev);
    	  memset (&device, 0, sizeof (device));
	  device.devname = strdup (devname);
          device.fd = ulDev; /* store usbcalls device number */
	  device.vendor = pDevDesc->idVendor;
	  device.product = pDevDesc->idProduct;
	  device.method = sanei_usb_method_usbcalls;
	  device.interface_nr = interface;
	  DBG (4, "sanei_usb_init: found usbcalls device (0x%04x/0x%04x) as device number %s\n",
	       pDevDesc->idVendor, pDevDesc->idProduct,device.devname);
	  store_device(device);
       }
   }

#endif /* HAVE_USBCALLS */

  DBG (5, "sanei_usb_init: found %d devices\n", device_number);
  if (debug_level > 5)
    {
      for (i = 0; i < device_number; i++)
        {
          if(devices[i].missing)
            continue;
	  DBG (6, "sanei_usb_init: device %02d is %s\n", i, devices[i].devname);
        }
    }
}


/* This logically belongs to sanei_config.c but not every backend that
   uses sanei_config() wants to depend on sanei_usb.  */
void
sanei_usb_attach_matching_devices (const char *name,
				   SANE_Status (*attach) (const char *dev))
{
  char *vendor, *product;

  if (strncmp (name, "usb", 3) == 0)
    {
      SANE_Word vendorID = 0, productID = 0;

      name += 3;

      name = sanei_config_skip_whitespace (name);
      if (*name)
	{
	  name = sanei_config_get_string (name, &vendor);
	  if (vendor)
	    {
	      vendorID = strtol (vendor, 0, 0);
	      free (vendor);
	    }
	  name = sanei_config_skip_whitespace (name);
	}

      name = sanei_config_skip_whitespace (name);
      if (*name)
	{
	  name = sanei_config_get_string (name, &product);
	  if (product)
	    {
	      productID = strtol (product, 0, 0);
	      free (product);
	    }
	}
      sanei_usb_find_devices (vendorID, productID, attach);
    }
  else
    (*attach) (name);
}

SANE_Status
sanei_usb_get_vendor_product_byname (SANE_String_Const devname,
				     SANE_Word * vendor, SANE_Word * product)
{
  int i;
  SANE_Bool found = SANE_FALSE;

  for (i = 0; i < device_number && devices[i].devname; i++)
    {
      if(devices[i].missing)
        continue;

      if (strcmp (devices[i].devname, devname) == 0)
	{
	  found = SANE_TRUE;
	  break;
	}
    }

  if (!found)
    {
      DBG (1, "sanei_usb_get_vendor_product_byname: can't find device `%s' in list\n", devname);
      return SANE_STATUS_INVAL;
    }

  if ((devices[i].vendor == 0) && (devices[i].product == 0))
    {
      DBG (1, "sanei_usb_get_vendor_product_byname: not support for this method\n");
      return SANE_STATUS_UNSUPPORTED;
    }

  if (vendor)
    *vendor = devices[i].vendor;

  if (product)
    *product = devices[i].product;

  return SANE_STATUS_GOOD;
}

SANE_Status
sanei_usb_get_vendor_product (SANE_Int dn, SANE_Word * vendor,
			      SANE_Word * product)
{
  SANE_Word vendorID = 0;
  SANE_Word productID = 0;

  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_get_vendor_product: dn >= device number || dn < 0\n");
      return SANE_STATUS_INVAL;
    }

  if (devices[dn].method == sanei_usb_method_scanner_driver)
    kernel_get_vendor_product (devices[dn].fd, devices[dn].devname, &vendorID, &productID);
  else if (devices[dn].method == sanei_usb_method_libusb)
    {
#ifdef HAVE_LIBUSB
      vendorID = usb_device (devices[dn].libusb_handle)->descriptor.idVendor;
      productID =
	usb_device (devices[dn].libusb_handle)->descriptor.idProduct;
#elif defined(HAVE_LIBUSB_1_0)
      vendorID = devices[dn].vendor;
      productID = devices[dn].product;
#else
      DBG (1, "sanei_usb_get_vendor_product: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
#endif /* HAVE_LIBUSB */
    }
  else if (devices[dn].method == sanei_usb_method_usbcalls)
   {
#ifdef HAVE_USBCALLS
     vendorID = devices[dn].vendor;
     productID = devices[dn].product;
#else
      DBG (1, "sanei_usb_get_vendor_product: usbcalls support missing\n");
      return SANE_STATUS_UNSUPPORTED;
#endif /* HAVE_USBCALLS */
   }
  else
    {
      DBG (1, "sanei_usb_get_vendor_product: access method %d not "
	   "implemented\n", devices[dn].method);
      return SANE_STATUS_INVAL;
    }
  if (vendor)
    *vendor = vendorID;
  if (product)
    *product = productID;

  if (!vendorID || !productID)
    {
      DBG (3, "sanei_usb_get_vendor_product: device %d: Your OS doesn't "
	   "seem to support detection of vendor+product ids\n", dn);
      return SANE_STATUS_UNSUPPORTED;
    }
  else
    {
      DBG (3, "sanei_usb_get_vendor_product: device %d: vendorID: 0x%04x, "
	   "productID: 0x%04x\n", dn, vendorID, productID);
      return SANE_STATUS_GOOD;
    }
}

SANE_Status
sanei_usb_find_devices (SANE_Int vendor, SANE_Int product,
			SANE_Status (*attach) (SANE_String_Const dev))
{
  SANE_Int dn = 0;

  DBG (3,
       "sanei_usb_find_devices: vendor=0x%04x, product=0x%04x\n",
       vendor, product);

  while (devices[dn].devname && dn < device_number)
    {
      if (devices[dn].vendor == vendor
        && devices[dn].product == product
        && !devices[dn].missing
	&& attach)
	  attach (devices[dn].devname);
      dn++;
    }
  return SANE_STATUS_GOOD;
}

void
sanei_usb_set_endpoint (SANE_Int dn, SANE_Int ep_type, SANE_Int ep)
{
  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_set_endpoint: dn >= device number || dn < 0\n");
      return;
    }

  DBG (5, "sanei_usb_set_endpoint: Setting endpoint of type 0x%02x to 0x%02x\n", ep_type, ep);
  switch (ep_type)
    {
      case USB_DIR_IN|USB_ENDPOINT_TYPE_BULK:
	    devices[dn].bulk_in_ep  = ep;
	    break;
      case USB_DIR_OUT|USB_ENDPOINT_TYPE_BULK:
	    devices[dn].bulk_out_ep = ep;
	    break;
      case USB_DIR_IN|USB_ENDPOINT_TYPE_ISOCHRONOUS:
	    devices[dn].iso_in_ep = ep;
	    break;
      case USB_DIR_OUT|USB_ENDPOINT_TYPE_ISOCHRONOUS:
	    devices[dn].iso_out_ep = ep;
	    break;
      case USB_DIR_IN|USB_ENDPOINT_TYPE_INTERRUPT:
	    devices[dn].int_in_ep = ep;
	    break;
      case USB_DIR_OUT|USB_ENDPOINT_TYPE_INTERRUPT:
	    devices[dn].int_out_ep = ep;
	    break;
      case USB_DIR_IN|USB_ENDPOINT_TYPE_CONTROL:
	    devices[dn].control_in_ep = ep;
	    break;
      case USB_DIR_OUT|USB_ENDPOINT_TYPE_CONTROL:
	    devices[dn].control_out_ep = ep;
	    break;
    }
}

SANE_Int
sanei_usb_get_endpoint (SANE_Int dn, SANE_Int ep_type)
{
  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_get_endpoint: dn >= device number || dn < 0\n");
      return 0;
    }

  switch (ep_type)
    {
      case USB_DIR_IN|USB_ENDPOINT_TYPE_BULK:
	    return devices[dn].bulk_in_ep;
      case USB_DIR_OUT|USB_ENDPOINT_TYPE_BULK:
	    return devices[dn].bulk_out_ep;
      case USB_DIR_IN|USB_ENDPOINT_TYPE_ISOCHRONOUS:
	    return devices[dn].iso_in_ep;
      case USB_DIR_OUT|USB_ENDPOINT_TYPE_ISOCHRONOUS:
	    return devices[dn].iso_out_ep;
      case USB_DIR_IN|USB_ENDPOINT_TYPE_INTERRUPT:
	    return devices[dn].int_in_ep;
      case USB_DIR_OUT|USB_ENDPOINT_TYPE_INTERRUPT:
	    return devices[dn].int_out_ep;
      case USB_DIR_IN|USB_ENDPOINT_TYPE_CONTROL:
	    return devices[dn].control_in_ep;
      case USB_DIR_OUT|USB_ENDPOINT_TYPE_CONTROL:
	    return devices[dn].control_out_ep;
      default:
	    return 0;
    }
}

SANE_Status
sanei_usb_open (SANE_String_Const devname, SANE_Int * dn)
{
  int devcount;
  SANE_Bool found = SANE_FALSE;
  int c, i, a;

  DBG (5, "sanei_usb_open: trying to open device `%s'\n", devname);
  if (!dn)
    {
      DBG (1, "sanei_usb_open: can't open `%s': dn == NULL\n", devname);
      return SANE_STATUS_INVAL;
    }

  for (devcount = 0;
       devcount < device_number && devices[devcount].devname != 0;
       devcount++)
    {
      if(devices[devcount].missing)
        continue;

      if (strcmp (devices[devcount].devname, devname) == 0)
	{
	  if (devices[devcount].open)
	    {
	      DBG (1, "sanei_usb_open: device `%s' already open\n", devname);
	      return SANE_STATUS_INVAL;
	    }
	  found = SANE_TRUE;
	  break;
	}
    }

  if (!found)
    {
      DBG (1, "sanei_usb_open: can't find device `%s' in list\n", devname);
      return SANE_STATUS_INVAL;
    }

  if (devices[devcount].method == sanei_usb_method_libusb)
    {
#ifdef HAVE_LIBUSB
      struct usb_device *dev;
      struct usb_interface_descriptor *interface;
      int result, num;

      devices[devcount].libusb_handle =
	usb_open (devices[devcount].libusb_device);
      if (!devices[devcount].libusb_handle)
	{
	  SANE_Status status = SANE_STATUS_INVAL;

	  DBG (1, "sanei_usb_open: can't open device `%s': %s\n",
	       devname, strerror (errno));
	  if (errno == EPERM)
	    {
	      DBG (1, "Make sure you run as root or set appropriate "
		   "permissions\n");
	      status = SANE_STATUS_ACCESS_DENIED;
	    }
	  else if (errno == EBUSY)
	    {
	      DBG (1, "Maybe the kernel scanner driver claims the "
		   "scanner's interface?\n");
	      status = SANE_STATUS_DEVICE_BUSY;
	    }
	  return status;
	}

      dev = usb_device (devices[devcount].libusb_handle);

      /* Set the configuration */
      if (!dev->config)
	{
	  DBG (1, "sanei_usb_open: device `%s' not configured?\n", devname);
	  return SANE_STATUS_INVAL;
	}
      if (dev->descriptor.bNumConfigurations > 1)
	{
	  DBG (3, "sanei_usb_open: more than one "
	       "configuration (%d), choosing first config (%d)\n",
	       dev->descriptor.bNumConfigurations,
	       dev->config[0].bConfigurationValue);
	}
      result = usb_set_configuration (devices[devcount].libusb_handle,
				      dev->config[0].bConfigurationValue);
      if (result < 0)
	{
	  SANE_Status status = SANE_STATUS_INVAL;

	  DBG (1, "sanei_usb_open: libusb complained: %s\n", usb_strerror ());
	  if (errno == EPERM)
	    {
	      DBG (1, "Make sure you run as root or set appropriate "
		   "permissions\n");
	      status = SANE_STATUS_ACCESS_DENIED;
	    }
	  else if (errno == EBUSY)
	    {
	      DBG (3, "Maybe the kernel scanner driver or usblp claims the "
		   "interface? Ignoring this error...\n");
	      status = SANE_STATUS_GOOD;
	    }
	  if (status != SANE_STATUS_GOOD)
	    {
	      usb_close (devices[devcount].libusb_handle);
	      return status;
	    }
	}

      /* Claim the interface */
      result = usb_claim_interface (devices[devcount].libusb_handle,
				    devices[devcount].interface_nr);
      if (result < 0)
	{
	  SANE_Status status = SANE_STATUS_INVAL;

	  DBG (1, "sanei_usb_open: libusb complained: %s\n", usb_strerror ());
	  if (errno == EPERM)
	    {
	      DBG (1, "Make sure you run as root or set appropriate "
		   "permissions\n");
	      status = SANE_STATUS_ACCESS_DENIED;
	    }
	  else if (errno == EBUSY)
	    {
	      DBG (1, "Maybe the kernel scanner driver claims the "
		   "scanner's interface?\n");
	      status = SANE_STATUS_DEVICE_BUSY;
	    }
	  usb_close (devices[devcount].libusb_handle);
	  return status;
	}

      /* Loop through all of the configurations */
      for (c = 0; c < dev->descriptor.bNumConfigurations; c++)
	{
	  /* Loop through all of the interfaces */
	  for (i = 0; i < dev->config[c].bNumInterfaces; i++)
	    {
	      /* Loop through all of the alternate settings */
	      for (a = 0; a < dev->config[c].interface[i].num_altsetting; a++)
		{
		  DBG (5, "sanei_usb_open: configuration nr: %d\n", c);
		  DBG (5, "sanei_usb_open:     interface nr: %d\n", i);
		  DBG (5, "sanei_usb_open:   alt_setting nr: %d\n", a);

                  /* Start by interfaces found in sanei_usb_init */
                  if (c == 0 && i != devices[devcount].interface_nr)
                    {
                      DBG (5, "sanei_usb_open: interface %d not detected as "
                        "a scanner by sanei_usb_init, ignoring.\n", i);
                      continue;
                     }

		  interface = &dev->config[c].interface[i].altsetting[a];

		  /* Now we look for usable endpoints */
		  for (num = 0; num < interface->bNumEndpoints; num++)
		    {
		      struct usb_endpoint_descriptor *endpoint;
		      int address, direction, transfer_type;

		      endpoint = &interface->endpoint[num];
		      DBG (5, "sanei_usb_open: endpoint nr: %d\n", num);
		      transfer_type =
			endpoint->bmAttributes & USB_ENDPOINT_TYPE_MASK;
		      address =
			endpoint->
			bEndpointAddress & USB_ENDPOINT_ADDRESS_MASK;
		      direction =
			endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK;

		      DBG (5, "sanei_usb_open: direction: %d\n", direction);
		      transfer_type =
			endpoint->bmAttributes & USB_ENDPOINT_TYPE_MASK;

		      DBG (5,
			   "sanei_usb_open: address: %d transfertype: %d\n",
			   address, transfer_type);


		      /* save the endpoints we need later */
		      if (transfer_type == USB_ENDPOINT_TYPE_INTERRUPT)
			{
			  DBG (5,
			       "sanei_usb_open: found interrupt-%s endpoint (address 0x%02x)\n",
			       direction ? "in" : "out", address);
			  if (direction)	/* in */
			    {
			      if (devices[devcount].int_in_ep)
				DBG (3,
				     "sanei_usb_open: we already have a int-in endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].int_in_ep);
			      else
				devices[devcount].int_in_ep =
				  endpoint->bEndpointAddress;
			    }
			  else
			    {
			      if (devices[devcount].int_out_ep)
				DBG (3,
				     "sanei_usb_open: we already have a int-out endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].int_out_ep);
			      else
				devices[devcount].int_out_ep =
				  endpoint->bEndpointAddress;
			    }
			}
		      else if (transfer_type == USB_ENDPOINT_TYPE_BULK)
			{
			  DBG (5,
			       "sanei_usb_open: found bulk-%s endpoint (address 0x%02x)\n",
			       direction ? "in" : "out", address);
			  if (direction)	/* in */
			    {
			      if (devices[devcount].bulk_in_ep)
				DBG (3,
				     "sanei_usb_open: we already have a bulk-in endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].bulk_in_ep);
			      else
				devices[devcount].bulk_in_ep =
				  endpoint->bEndpointAddress;
			    }
			  else
			    {
			      if (devices[devcount].bulk_out_ep)
				DBG (3,
				     "sanei_usb_open: we already have a bulk-out endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].bulk_out_ep);
			      else
				devices[devcount].bulk_out_ep =
				  endpoint->bEndpointAddress;
			    }
			}
		      else if (transfer_type == USB_ENDPOINT_TYPE_ISOCHRONOUS)
			{
			  DBG (5,
			       "sanei_usb_open: found isochronous-%s endpoint (address 0x%02x)\n",
			       direction ? "in" : "out", address);
			  if (direction)	/* in */
			    {
			      if (devices[devcount].iso_in_ep)
				DBG (3,
				     "sanei_usb_open: we already have a isochronous-in endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].iso_in_ep);
			      else
				devices[devcount].iso_in_ep =
				  endpoint->bEndpointAddress;
			    }
			  else
			    {
			      if (devices[devcount].iso_out_ep)
				DBG (3,
				     "sanei_usb_open: we already have a isochronous-out endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].iso_out_ep);
			      else
				devices[devcount].iso_out_ep =
				  endpoint->bEndpointAddress;
			    }
			}
		      else if (transfer_type == USB_ENDPOINT_TYPE_CONTROL)
			{
			  DBG (5,
			       "sanei_usb_open: found control-%s endpoint (address 0x%02x)\n",
			       direction ? "in" : "out", address);
			  if (direction)	/* in */
			    {
			      if (devices[devcount].control_in_ep)
				DBG (3,
				     "sanei_usb_open: we already have a control-in endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].control_in_ep);
			      else
				devices[devcount].control_in_ep =
				  endpoint->bEndpointAddress;
			    }
			  else
			    {
			      if (devices[devcount].control_out_ep)
				DBG (3,
				     "sanei_usb_open: we already have a control-out endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].control_out_ep);
			      else
				devices[devcount].control_out_ep =
				  endpoint->bEndpointAddress;
			    }
			}
		    }
		}
	    }
	}

#elif defined(HAVE_LIBUSB_1_0) /* libusb-1.0 */

      int config;
      libusb_device *dev;
      struct libusb_device_descriptor desc;
      struct libusb_config_descriptor *config0;
      int result, num;

      dev = devices[devcount].lu_device;

      result = libusb_open (dev, &devices[devcount].lu_handle);
      if (result < 0)
	{
	  SANE_Status status = SANE_STATUS_INVAL;

	  DBG (1, "sanei_usb_open: can't open device `%s': %s\n",
	       devname, sanei_libusb_strerror (result));
	  if (result == LIBUSB_ERROR_ACCESS)
	    {
	      DBG (1, "Make sure you run as root or set appropriate "
		   "permissions\n");
	      status = SANE_STATUS_ACCESS_DENIED;
	    }
	  else if (result == LIBUSB_ERROR_BUSY)
	    {
	      DBG (1, "Maybe the kernel scanner driver claims the "
		   "scanner's interface?\n");
	      status = SANE_STATUS_DEVICE_BUSY;
	    }
	  else if (result == LIBUSB_ERROR_NO_MEM)
	    {
	      status = SANE_STATUS_NO_MEM;
	    }
	  return status;
	}

      result = libusb_get_configuration (devices[devcount].lu_handle, &config);
      if (result < 0)
	{
	  DBG (1,
	       "sanei_usb_open: could not get configuration for device `%s' (err %d)\n",
	       devname, result);
	  return SANE_STATUS_INVAL;
	}

      if (config == 0)
	{
	  DBG (1, "sanei_usb_open: device `%s' not configured?\n", devname);
	  return SANE_STATUS_INVAL;
	}

      result = libusb_get_device_descriptor (dev, &desc);
      if (result < 0)
	{
	  DBG (1,
	       "sanei_usb_open: could not get device descriptor for device `%s' (err %d)\n",
	       devname, result);
	  return SANE_STATUS_INVAL;
	}

      result = libusb_get_config_descriptor (dev, 0, &config0);
      if (result < 0)
	{
	  DBG (1,
	       "sanei_usb_open: could not get config[0] descriptor for device `%s' (err %d)\n",
	       devname, result);
	  return SANE_STATUS_INVAL;
	}

      /* Set the configuration */
      if (desc.bNumConfigurations > 1)
	{
	  DBG (3, "sanei_usb_open: more than one "
	       "configuration (%d), choosing first config (%d)\n",
	       desc.bNumConfigurations,
	       config0->bConfigurationValue);
	}
      result = libusb_set_configuration (devices[devcount].lu_handle,
					 config0->bConfigurationValue);

      libusb_free_config_descriptor (config0);

      if (result < 0)
	{
	  SANE_Status status = SANE_STATUS_INVAL;

	  DBG (1, "sanei_usb_open: libusb complained: %s\n",
	       sanei_libusb_strerror (result));
	  if (result == LIBUSB_ERROR_ACCESS)
	    {
	      DBG (1, "Make sure you run as root or set appropriate "
		   "permissions\n");
	      status = SANE_STATUS_ACCESS_DENIED;
	    }
	  else if (result == LIBUSB_ERROR_BUSY)
	    {
	      DBG (3, "Maybe the kernel scanner driver or usblp claims the "
		   "interface? Ignoring this error...\n");
	      status = SANE_STATUS_GOOD;
	    }

	  if (status != SANE_STATUS_GOOD)
	    {
	      libusb_close (devices[devcount].lu_handle);
	      return status;
	    }
	}

      /* Claim the interface */
      result = libusb_claim_interface (devices[devcount].lu_handle,
				       devices[devcount].interface_nr);
      if (result < 0)
	{
	  SANE_Status status = SANE_STATUS_INVAL;

	  DBG (1, "sanei_usb_open: libusb complained: %s\n",
	       sanei_libusb_strerror (result));
	  if (result == LIBUSB_ERROR_ACCESS)
	    {
	      DBG (1, "Make sure you run as root or set appropriate "
		   "permissions\n");
	      status = SANE_STATUS_ACCESS_DENIED;
	    }
	  else if (result == LIBUSB_ERROR_BUSY)
	    {
	      DBG (1, "Maybe the kernel scanner driver claims the "
		   "scanner's interface?\n");
	      status = SANE_STATUS_DEVICE_BUSY;
	    }

	  libusb_close (devices[devcount].lu_handle);
	  return status;
	}

      /* Loop through all of the configurations */
      for (c = 0; c < desc.bNumConfigurations; c++)
	{
	  struct libusb_config_descriptor *config;

	  result = libusb_get_config_descriptor (dev, c, &config);
	  if (result < 0)
	    {
	      DBG (1,
		   "sanei_usb_open: could not get config[%d] descriptor for device `%s' (err %d)\n",
		   c, devname, result);
	      continue;
	    }

	  /* Loop through all of the interfaces */
	  for (i = 0; i < config->bNumInterfaces; i++)
	    {
	      /* Loop through all of the alternate settings */
	      for (a = 0; a < config->interface[i].num_altsetting; a++)
		{
		  const struct libusb_interface_descriptor *interface;

		  DBG (5, "sanei_usb_open: configuration nr: %d\n", c);
		  DBG (5, "sanei_usb_open:     interface nr: %d\n", i);
		  DBG (5, "sanei_usb_open:   alt_setting nr: %d\n", a);

                  /* Start by interfaces found in sanei_usb_init */
                  if (c == 0 && i != devices[devcount].interface_nr)
                    {
                      DBG (5, "sanei_usb_open: interface %d not detected as "
                        "a scanner by sanei_usb_init, ignoring.\n", i);
                      continue;
                     }

		  interface = &config->interface[i].altsetting[a];

		  /* Now we look for usable endpoints */
		  for (num = 0; num < interface->bNumEndpoints; num++)
		    {
		      const struct libusb_endpoint_descriptor *endpoint;
		      int address, direction, transfer_type;

		      endpoint = &interface->endpoint[num];
		      DBG (5, "sanei_usb_open: endpoint nr: %d\n", num);

		      transfer_type = endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK;
		      address = endpoint->bEndpointAddress & LIBUSB_ENDPOINT_ADDRESS_MASK;
		      direction = endpoint->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK;

		      DBG (5, "sanei_usb_open: direction: %d\n", direction);
		      DBG (5, "sanei_usb_open: address: %d transfertype: %d\n",
			   address, transfer_type);

		      /* save the endpoints we need later */
		      if (transfer_type == LIBUSB_TRANSFER_TYPE_INTERRUPT)
			{
			  DBG (5,
			       "sanei_usb_open: found interrupt-%s endpoint (address 0x%02x)\n",
			       direction ? "in" : "out", address);
			  if (direction)	/* in */
			    {
			      if (devices[devcount].int_in_ep)
				DBG (3,
				     "sanei_usb_open: we already have a int-in endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].int_in_ep);
			      else
				devices[devcount].int_in_ep = endpoint->bEndpointAddress;
			    }
			  else
			    {
			      if (devices[devcount].int_out_ep)
				DBG (3,
				     "sanei_usb_open: we already have a int-out endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].int_out_ep);
			      else
				devices[devcount].int_out_ep = endpoint->bEndpointAddress;
			    }
			}
		      else if (transfer_type == LIBUSB_TRANSFER_TYPE_BULK)
			{
			  DBG (5,
			       "sanei_usb_open: found bulk-%s endpoint (address 0x%02x)\n",
			       direction ? "in" : "out", address);
			  if (direction)	/* in */
			    {
			      if (devices[devcount].bulk_in_ep)
				DBG (3,
				     "sanei_usb_open: we already have a bulk-in endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].bulk_in_ep);
			      else
				devices[devcount].bulk_in_ep = endpoint->bEndpointAddress;
			    }
			  else
			    {
			      if (devices[devcount].bulk_out_ep)
				DBG (3,
				     "sanei_usb_open: we already have a bulk-out endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].bulk_out_ep);
			      else
				devices[devcount].bulk_out_ep = endpoint->bEndpointAddress;
			    }
			}
		      else if (transfer_type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
			{
			  DBG (5,
			       "sanei_usb_open: found isochronous-%s endpoint (address 0x%02x)\n",
			       direction ? "in" : "out", address);
			  if (direction)	/* in */
			    {
			      if (devices[devcount].iso_in_ep)
				DBG (3,
				     "sanei_usb_open: we already have a isochronous-in endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].iso_in_ep);
			      else
				devices[devcount].iso_in_ep = endpoint->bEndpointAddress;
			    }
			  else
			    {
			      if (devices[devcount].iso_out_ep)
				DBG (3,
				     "sanei_usb_open: we already have a isochronous-out endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].iso_out_ep);
			      else
				devices[devcount].iso_out_ep = endpoint->bEndpointAddress;
			    }
			}
		      else if (transfer_type == LIBUSB_TRANSFER_TYPE_CONTROL)
			{
			  DBG (5,
			       "sanei_usb_open: found control-%s endpoint (address 0x%02x)\n",
			       direction ? "in" : "out", address);
			  if (direction)	/* in */
			    {
			      if (devices[devcount].control_in_ep)
				DBG (3,
				     "sanei_usb_open: we already have a control-in endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].control_in_ep);
			      else
				devices[devcount].control_in_ep = endpoint->bEndpointAddress;
			    }
			  else
			    {
			      if (devices[devcount].control_out_ep)
				DBG (3,
				     "sanei_usb_open: we already have a control-out endpoint "
				     "(address: 0x%02x), ignoring the new one\n",
				     devices[devcount].control_out_ep);
			      else
				devices[devcount].control_out_ep = endpoint->bEndpointAddress;
			    }
			}
		    }
		}
	    }

	  libusb_free_config_descriptor (config);
	}

#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
      DBG (1, "sanei_usb_open: can't open device `%s': "
	   "libusb support missing\n", devname);
      return SANE_STATUS_UNSUPPORTED;
#endif /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    }
  else if (devices[devcount].method == sanei_usb_method_scanner_driver)
    {
      long int flag;
      /* Using kernel scanner driver */
      devices[devcount].fd = -1;
#ifdef HAVE_RESMGR
      devices[devcount].fd = rsm_open_device (devname, O_RDWR);
#endif
      if (devices[devcount].fd == -1)
	devices[devcount].fd = open (devname, O_RDWR);
      if (devices[devcount].fd < 0)
	{
	  SANE_Status status = SANE_STATUS_INVAL;

	  if (errno == EACCES)
	    status = SANE_STATUS_ACCESS_DENIED;
	  else if (errno == ENOENT)
	    {
	      DBG (5, "sanei_usb_open: open of `%s' failed: %s\n",
		   devname, strerror (errno));
	      return status;
	    }
	  DBG (1, "sanei_usb_open: open of `%s' failed: %s\n",
	       devname, strerror (errno));
	  return status;
	}
#ifdef FD_CLOEXEC
      flag = fcntl (devices[devcount].fd, F_GETFD);
      if (flag >= 0)
	{
	  if (fcntl (devices[devcount].fd, F_SETFD, flag | FD_CLOEXEC) < 0)
	    DBG (1, "sanei_usb_open: fcntl of `%s' failed: %s\n",
		 devname, strerror (errno));
	}
#endif
    }
  else if (devices[devcount].method == sanei_usb_method_usbcalls)
    {
#ifdef HAVE_USBCALLS
      CHAR ucData[2048];
      struct usb_device_descriptor *pDevDesc;
      struct usb_config_descriptor   *pCfgDesc;
      struct usb_interface_descriptor *interface;
      struct usb_endpoint_descriptor  *endpoint;
      struct usb_descriptor_header    *pDescHead;

      ULONG  ulBufLen;
      ulBufLen = sizeof(ucData);
      memset(&ucData,0,sizeof(ucData));

      int result, num,rc;
      int address, direction, transfer_type;

      DBG (5, "devname = %s, devcount = %d\n",devices[devcount].devname,devcount);
      DBG (5, "USBCalls device number to open = %d\n",devices[devcount].fd);
      DBG (5, "USBCalls Vendor/Product to open = 0x%04x/0x%04x\n",
               devices[devcount].vendor,devices[devcount].product);
      
      rc = UsbOpen (&dh, 
			devices[devcount].vendor,
			devices[devcount].product,
			USB_ANY_PRODUCTVERSION,
			USB_OPEN_FIRST_UNUSED);
      DBG (1, "sanei_usb_open: UsbOpen rc = %d\n",rc);
      if (rc!=0)
	{
	  SANE_Status status = SANE_STATUS_INVAL;
	  DBG (1, "sanei_usb_open: can't open device `%s': %s\n",
	       devname, strerror (rc));
	  return status;
	}
      rc = UsbQueryDeviceReport( devices[devcount].fd,
                                  &ulBufLen,
                                  ucData);
      DBG (1, "sanei_usb_open: UsbQueryDeviceReport rc = %d\n",rc);
      pDevDesc = (struct usb_device_descriptor*)ucData;
      pCfgDesc = (struct usb_config_descriptor*) (ucData+sizeof(struct usb_device_descriptor));
      UCHAR *pCurPtr = (UCHAR*) pCfgDesc;
      UCHAR *pEndPtr = pCurPtr+ pCfgDesc->wTotalLength;
      pDescHead = (struct usb_descriptor_header *) (pCurPtr+pCfgDesc->bLength);
      /* Set the configuration */
      if (pDevDesc->bNumConfigurations > 1)
	{
	  DBG (3, "sanei_usb_open: more than one "
	       "configuration (%d), choosing first config (%d)\n",
	       pDevDesc->bNumConfigurations, 
	       pCfgDesc->bConfigurationValue);
	}
      DBG (5, "UsbDeviceSetConfiguration parameters: dh = %p, bConfigurationValue = %d\n",
               dh,pCfgDesc->bConfigurationValue);
      result = UsbDeviceSetConfiguration (dh,
				      pCfgDesc->bConfigurationValue);
      DBG (1, "sanei_usb_open: UsbDeviceSetConfiguration rc = %d\n",result);
      if (result)
	{
	  SANE_Status status = SANE_STATUS_INVAL;
	  DBG (1, "sanei_usb_open: usbcalls complained on UsbDeviceSetConfiguration, rc= %d\n", result);
	  status = SANE_STATUS_ACCESS_DENIED;
	  UsbClose (dh);
	  return status;
	}
      
      /* Now we look for usable endpoints */
      
      for (pDescHead = (struct usb_descriptor_header *) (pCurPtr+pCfgDesc->bLength);
            pDescHead;pDescHead = GetNextDescriptor(pDescHead,pEndPtr) )
	{
          switch(pDescHead->bDescriptorType)
          {
            case USB_DT_INTERFACE:
              interface = (struct usb_interface_descriptor *) pDescHead;
              DBG (5, "Found %d endpoints\n",interface->bNumEndpoints);
              DBG (5, "bAlternateSetting = %d\n",interface->bAlternateSetting);
              break;
            case USB_DT_ENDPOINT:
	      endpoint = (struct usb_endpoint_descriptor*)pDescHead;
              address = endpoint->bEndpointAddress;
	      direction = endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK;
	      transfer_type = endpoint->bmAttributes & USB_ENDPOINT_TYPE_MASK;
	      /* save the endpoints we need later */
	      if (transfer_type == USB_ENDPOINT_TYPE_INTERRUPT)
	      {
   	       DBG (5, "sanei_usb_open: found interupt-%s endpoint (address %2x)\n",
  	            direction ? "in" : "out", address);
	       if (direction)	/* in */
	       {
	         if (devices[devcount].int_in_ep)
		   DBG (3, "sanei_usb_open: we already have a int-in endpoint "
		        "(address: %d), ignoring the new one\n",
		        devices[devcount].int_in_ep);
	         else
		   devices[devcount].int_in_ep = endpoint->bEndpointAddress;
	       }
	       else
	         if (devices[devcount].int_out_ep)
		   DBG (3, "sanei_usb_open: we already have a int-out endpoint "
		        "(address: %d), ignoring the new one\n",
		        devices[devcount].int_out_ep);
	         else
		   devices[devcount].int_out_ep = endpoint->bEndpointAddress;
	     }
	     else if (transfer_type == USB_ENDPOINT_TYPE_BULK)
	     {
	       DBG (5, "sanei_usb_open: found bulk-%s endpoint (address %2x)\n",
	            direction ? "in" : "out", address);
	       if (direction)	/* in */
	         {
		   if (devices[devcount].bulk_in_ep)
		     DBG (3, "sanei_usb_open: we already have a bulk-in endpoint "
		          "(address: %d), ignoring the new one\n",
		          devices[devcount].bulk_in_ep);
		   else
		     devices[devcount].bulk_in_ep = endpoint->bEndpointAddress;
	         }
	       else
	         {
	           if (devices[devcount].bulk_out_ep)
		     DBG (3, "sanei_usb_open: we already have a bulk-out endpoint "
		          "(address: %d), ignoring the new one\n",
		          devices[devcount].bulk_out_ep);
	           else
		     devices[devcount].bulk_out_ep = endpoint->bEndpointAddress;
	         }
	       }
	     /* ignore currently unsupported endpoints */
	     else {
	         DBG (5, "sanei_usb_open: ignoring %s-%s endpoint "
		      "(address: %d)\n",
		      transfer_type == USB_ENDPOINT_TYPE_CONTROL ? "control" :
		      transfer_type == USB_ENDPOINT_TYPE_ISOCHRONOUS
		      ? "isochronous" : "interrupt",
		      direction ? "in" : "out", address);
	         continue;
	          }
          break;
          }
        }
#else
      DBG (1, "sanei_usb_open: can't open device `%s': "
	   "usbcalls support missing\n", devname);
      return SANE_STATUS_UNSUPPORTED;
#endif /* HAVE_USBCALLS */
    }
  else
    {
      DBG (1, "sanei_usb_open: access method %d not implemented\n",
	   devices[devcount].method);
      return SANE_STATUS_INVAL;
    }

  devices[devcount].open = SANE_TRUE;
  *dn = devcount;
  DBG (3, "sanei_usb_open: opened usb device `%s' (*dn=%d)\n",
       devname, devcount);
  return SANE_STATUS_GOOD;
}

void
sanei_usb_close (SANE_Int dn)
{
  DBG (5, "sanei_usb_close: closing device %d\n", dn);
  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_close: dn >= device number || dn < 0\n");
      return;
    }
  if (!devices[dn].open)
    {
      DBG (1, "sanei_usb_close: device %d already closed or never opened\n",
	   dn);
      return;
    }
  if (devices[dn].method == sanei_usb_method_scanner_driver)
    close (devices[dn].fd);
  else if (devices[dn].method == sanei_usb_method_usbcalls)
    {
#ifdef HAVE_USBCALLS
      int rc;
      rc=UsbClose (dh);
      DBG (5,"rc of UsbClose = %d\n",rc);
#else
    DBG (1, "sanei_usb_close: usbcalls support missing\n");
#endif
    }
  else
#ifdef HAVE_LIBUSB
    {
#if 0
      /* Should only be done in case of a stall */
      usb_clear_halt (devices[dn].libusb_handle, devices[dn].bulk_in_ep);
      usb_clear_halt (devices[dn].libusb_handle, devices[dn].bulk_out_ep);
      usb_clear_halt (devices[dn].libusb_handle, devices[dn].iso_in_ep);
      /* be careful, we don't know if we are in DATA0 stage now */
      usb_resetep (devices[dn].libusb_handle, devices[dn].bulk_in_ep);
      usb_resetep (devices[dn].libusb_handle, devices[dn].bulk_out_ep);
      usb_resetep (devices[dn].libusb_handle, devices[dn].iso_in_ep);
#endif /* 0 */
      usb_release_interface (devices[dn].libusb_handle,
			     devices[dn].interface_nr);
      usb_close (devices[dn].libusb_handle);
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      libusb_release_interface (devices[dn].lu_handle,
				devices[dn].interface_nr);
      libusb_close (devices[dn].lu_handle);
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    DBG (1, "sanei_usb_close: libusb support missing\n");
#endif
  devices[dn].open = SANE_FALSE;
  return;
}

void
sanei_usb_set_timeout (SANE_Int timeout)
{
#if defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0)
  libusb_timeout = timeout;
#else
  DBG (1, "sanei_usb_set_timeout: libusb support missing\n");
#endif /* HAVE_LIBUSB || HAVE_LIBUSB_1_0 */
}

SANE_Status
sanei_usb_clear_halt (SANE_Int dn)
{
#ifdef HAVE_LIBUSB
  int ret;

  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_clear_halt: dn >= device number || dn < 0\n");
      return SANE_STATUS_INVAL;
    }

  ret = usb_clear_halt (devices[dn].libusb_handle, devices[dn].bulk_in_ep);
  if (ret){
    DBG (1, "sanei_usb_clear_halt: BULK_IN ret=%d\n", ret);
    return SANE_STATUS_INVAL;
  }

  ret = usb_clear_halt (devices[dn].libusb_handle, devices[dn].bulk_out_ep);
  if (ret){
    DBG (1, "sanei_usb_clear_halt: BULK_OUT ret=%d\n", ret);
    return SANE_STATUS_INVAL;
  }

  /* be careful, we don't know if we are in DATA0 stage now
  ret = usb_resetep (devices[dn].libusb_handle, devices[dn].bulk_in_ep);
  ret = usb_resetep (devices[dn].libusb_handle, devices[dn].bulk_out_ep);
  */
#elif defined(HAVE_LIBUSB_1_0)
  int ret;

  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_clear_halt: dn >= device number || dn < 0\n");
      return SANE_STATUS_INVAL;
    }

  ret = libusb_clear_halt (devices[dn].lu_handle, devices[dn].bulk_in_ep);
  if (ret){
    DBG (1, "sanei_usb_clear_halt: BULK_IN ret=%d\n", ret);
    return SANE_STATUS_INVAL;
  }

  ret = libusb_clear_halt (devices[dn].lu_handle, devices[dn].bulk_out_ep);
  if (ret){
    DBG (1, "sanei_usb_clear_halt: BULK_OUT ret=%d\n", ret);
    return SANE_STATUS_INVAL;
  }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
  DBG (1, "sanei_usb_clear_halt: libusb support missing\n");
#endif /* HAVE_LIBUSB || HAVE_LIBUSB_1_0 */

  return SANE_STATUS_GOOD;
}

SANE_Status
sanei_usb_reset (SANE_Int dn)
{
#ifdef HAVE_LIBUSB
  int ret;

  ret = usb_reset (devices[dn].libusb_handle);
  if (ret){
    DBG (1, "sanei_usb_reset: ret=%d\n", ret);
    return SANE_STATUS_INVAL;
  }

#elif defined(HAVE_LIBUSB_1_0)
  int ret;

  ret = libusb_reset_device (devices[dn].lu_handle);
  if (ret){
    DBG (1, "sanei_usb_reset: ret=%d\n", ret);
    return SANE_STATUS_INVAL;
  }
  
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
  DBG (1, "sanei_usb_reset: libusb support missing\n");
#endif /* HAVE_LIBUSB || HAVE_LIBUSB_1_0 */

  return SANE_STATUS_GOOD;
}

SANE_Status
sanei_usb_read_bulk (SANE_Int dn, SANE_Byte * buffer, size_t * size)
{
  ssize_t read_size = 0;

  if (!size)
    {
      DBG (1, "sanei_usb_read_bulk: size == NULL\n");
      return SANE_STATUS_INVAL;
    }

  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_read_bulk: dn >= device number || dn < 0\n");
      return SANE_STATUS_INVAL;
    }
  DBG (5, "sanei_usb_read_bulk: trying to read %lu bytes\n",
       (unsigned long) *size);

  if (devices[dn].method == sanei_usb_method_scanner_driver)
    {
      read_size = read (devices[dn].fd, buffer, *size);

      if (read_size < 0)
	DBG (1, "sanei_usb_read_bulk: read failed: %s\n",
	     strerror (errno));
    }
  else if (devices[dn].method == sanei_usb_method_libusb)
#ifdef HAVE_LIBUSB
    {
      if (devices[dn].bulk_in_ep)
	{
	  read_size = usb_bulk_read (devices[dn].libusb_handle,
				     devices[dn].bulk_in_ep, (char *) buffer,
				     (int) *size, libusb_timeout);

	  if (read_size < 0)
	    DBG (1, "sanei_usb_read_bulk: read failed: %s\n",
		 strerror (errno));
	}
      else
	{
	  DBG (1, "sanei_usb_read_bulk: can't read without a bulk-in "
	       "endpoint\n");
	  return SANE_STATUS_INVAL;
	}
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      if (devices[dn].bulk_in_ep)
	{
	  int ret;
	  ret = libusb_bulk_transfer (devices[dn].lu_handle,
				      devices[dn].bulk_in_ep, buffer,
				      (int) *size, (int *) &read_size,
				      libusb_timeout);

	  if (ret < 0)
	    {
	      DBG (1, "sanei_usb_read_bulk: read failed: %s\n",
		   sanei_libusb_strerror (ret));

	      read_size = -1;
	    }
	}
      else
	{
	  DBG (1, "sanei_usb_read_bulk: can't read without a bulk-in "
	       "endpoint\n");
	  return SANE_STATUS_INVAL;
	}
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    {
      DBG (1, "sanei_usb_read_bulk: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_LIBUSB */
  else if (devices[dn].method == sanei_usb_method_usbcalls)
  {
#ifdef HAVE_USBCALLS
    int rc;
    char* buffer_ptr = (char*) buffer;
    while (*size)
    {
      ULONG ulToRead = (*size>MAX_RW)?MAX_RW:*size;
      ULONG ulNum = ulToRead;
      DBG (5, "Entered usbcalls UsbBulkRead with dn = %d\n",dn);
      DBG (5, "Entered usbcalls UsbBulkRead with dh = %p\n",dh);
      DBG (5, "Entered usbcalls UsbBulkRead with bulk_in_ep = 0x%02x\n",devices[dn].bulk_in_ep);
      DBG (5, "Entered usbcalls UsbBulkRead with interface_nr = %d\n",devices[dn].interface_nr);
      DBG (5, "Entered usbcalls UsbBulkRead with usbcalls_timeout = %d\n",usbcalls_timeout);

      if (devices[dn].bulk_in_ep){
        rc = UsbBulkRead (dh, devices[dn].bulk_in_ep, devices[dn].interface_nr,
                               &ulToRead, buffer_ptr, usbcalls_timeout);
        DBG (1, "sanei_usb_read_bulk: rc = %d\n",rc);}
      else
      {
          DBG (1, "sanei_usb_read_bulk: can't read without a bulk-in endpoint\n");
          return SANE_STATUS_INVAL;
      }
      if (rc || (ulNum!=ulToRead)) return SANE_STATUS_INVAL;
      *size -=ulToRead;
      buffer_ptr += ulToRead;
      read_size += ulToRead;
    }
#else /* not HAVE_USBCALLS */
    {
      DBG (1, "sanei_usb_read_bulk: usbcalls support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_USBCALLS */
  }
  else
    {
      DBG (1, "sanei_usb_read_bulk: access method %d not implemented\n",
	   devices[dn].method);
      return SANE_STATUS_INVAL;
    }

  if (read_size < 0)
    {
#ifdef HAVE_LIBUSB
      if (devices[dn].method == sanei_usb_method_libusb)
	usb_clear_halt (devices[dn].libusb_handle, devices[dn].bulk_in_ep);
#elif defined(HAVE_LIBUSB_1_0)
      if (devices[dn].method == sanei_usb_method_libusb)
	libusb_clear_halt (devices[dn].lu_handle, devices[dn].bulk_in_ep);
#endif
      *size = 0;
      return SANE_STATUS_IO_ERROR;
    }
  if (read_size == 0)
    {
      DBG (3, "sanei_usb_read_bulk: read returned EOF\n");
      *size = 0;
      return SANE_STATUS_EOF;
    }
  if (debug_level > 10)
    print_buffer (buffer, read_size);
  DBG (5, "sanei_usb_read_bulk: wanted %lu bytes, got %ld bytes\n",
       (unsigned long) *size, (unsigned long) read_size);
  *size = read_size;

  return SANE_STATUS_GOOD;
}

SANE_Status
sanei_usb_write_bulk (SANE_Int dn, const SANE_Byte * buffer, size_t * size)
{
  ssize_t write_size = 0;

  if (!size)
    {
      DBG (1, "sanei_usb_write_bulk: size == NULL\n");
      return SANE_STATUS_INVAL;
    }

  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_write_bulk: dn >= device number || dn < 0\n");
      return SANE_STATUS_INVAL;
    }
  DBG (5, "sanei_usb_write_bulk: trying to write %lu bytes\n",
       (unsigned long) *size);
  if (debug_level > 10)
    print_buffer (buffer, *size);

  if (devices[dn].method == sanei_usb_method_scanner_driver)
    {
      write_size = write (devices[dn].fd, buffer, *size);

      if (write_size < 0)
	DBG (1, "sanei_usb_write_bulk: write failed: %s\n",
	     strerror (errno));
    }
  else if (devices[dn].method == sanei_usb_method_libusb)
#ifdef HAVE_LIBUSB
    {
      if (devices[dn].bulk_out_ep)
	{
	  write_size = usb_bulk_write (devices[dn].libusb_handle,
				       devices[dn].bulk_out_ep,
				       (const char *) buffer,
				       (int) *size, libusb_timeout);
	  if (write_size < 0)
	    DBG (1, "sanei_usb_write_bulk: write failed: %s\n",
		 strerror (errno));
	}
      else
	{
	  DBG (1, "sanei_usb_write_bulk: can't write without a bulk-out "
	       "endpoint\n");
	  return SANE_STATUS_INVAL;
	}
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      if (devices[dn].bulk_out_ep)
	{
	  int ret;
	  int trans_bytes;
	  ret = libusb_bulk_transfer (devices[dn].lu_handle,
				      devices[dn].bulk_out_ep,
				      buffer,
				      (int) *size, &trans_bytes,
				      libusb_timeout);
	  if (ret < 0)
	    {
	      DBG (1, "sanei_usb_write_bulk: write failed: %s\n",
		   sanei_libusb_strerror (ret));

	      write_size = -1;
	    }
	  else
	    write_size = trans_bytes;
	}
      else
	{
	  DBG (1, "sanei_usb_write_bulk: can't write without a bulk-out "
	       "endpoint\n");
	  return SANE_STATUS_INVAL;
	}
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    {
      DBG (1, "sanei_usb_write_bulk: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
  else if (devices[dn].method == sanei_usb_method_usbcalls)
  {
#ifdef HAVE_USBCALLS
    int rc;
    DBG (5, "Entered usbcalls UsbBulkWrite with dn = %d\n",dn);
    DBG (5, "Entered usbcalls UsbBulkWrite with dh = %p\n",dh);
    DBG (5, "Entered usbcalls UsbBulkWrite with bulk_out_ep = 0x%02x\n",devices[dn].bulk_out_ep);
    DBG (5, "Entered usbcalls UsbBulkWrite with interface_nr = %d\n",devices[dn].interface_nr);
    DBG (5, "Entered usbcalls UsbBulkWrite with usbcalls_timeout = %d\n",usbcalls_timeout);
    while (*size)
    {
      ULONG ulToWrite = (*size>MAX_RW)?MAX_RW:*size;

      DBG (5, "size requested to write = %lu, ulToWrite = %lu\n",(unsigned long) *size,ulToWrite);
      if (devices[dn].bulk_out_ep){
        rc = UsbBulkWrite (dh, devices[dn].bulk_out_ep, devices[dn].interface_nr,
                               ulToWrite, (char*) buffer, usbcalls_timeout);
        DBG (1, "sanei_usb_write_bulk: rc = %d\n",rc);
      }
      else
      {
          DBG (1, "sanei_usb_write_bulk: can't read without a bulk-out endpoint\n");
          return SANE_STATUS_INVAL;
      }
      if (rc) return SANE_STATUS_INVAL;
      *size -=ulToWrite;
      buffer += ulToWrite;
      write_size += ulToWrite;
      DBG (5, "size = %d, write_size = %d\n",*size, write_size);
    }
#else /* not HAVE_USBCALLS */
    {
      DBG (1, "sanei_usb_write_bulk: usbcalls support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_USBCALLS */
  }
  else
    {
      DBG (1, "sanei_usb_write_bulk: access method %d not implemented\n",
	   devices[dn].method);
      return SANE_STATUS_INVAL;
    }

  if (write_size < 0)
    {
      *size = 0;
#ifdef HAVE_LIBUSB
      if (devices[dn].method == sanei_usb_method_libusb)
	usb_clear_halt (devices[dn].libusb_handle, devices[dn].bulk_out_ep);
#elif defined(HAVE_LIBUSB_1_0)
      if (devices[dn].method == sanei_usb_method_libusb)
	libusb_clear_halt (devices[dn].lu_handle, devices[dn].bulk_out_ep);
#endif
      return SANE_STATUS_IO_ERROR;
    }
  DBG (5, "sanei_usb_write_bulk: wanted %lu bytes, wrote %ld bytes\n",
       (unsigned long) *size, (unsigned long) write_size);
  *size = write_size;
  return SANE_STATUS_GOOD;
}

SANE_Status
sanei_usb_control_msg (SANE_Int dn, SANE_Int rtype, SANE_Int req,
		       SANE_Int value, SANE_Int index, SANE_Int len,
		       SANE_Byte * data)
{
  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_control_msg: dn >= device number || dn < 0, dn=%d\n",
	   dn);
      return SANE_STATUS_INVAL;
    }

  DBG (5, "sanei_usb_control_msg: rtype = 0x%02x, req = %d, value = %d, "
       "index = %d, len = %d\n", rtype, req, value, index, len);
  if (!(rtype & 0x80) && debug_level > 10)
    print_buffer (data, len);

  if (devices[dn].method == sanei_usb_method_scanner_driver)
    {
#if defined(__linux__)
      struct ctrlmsg_ioctl c;

      c.req.requesttype = rtype;
      c.req.request = req;
      c.req.value = value;
      c.req.index = index;
      c.req.length = len;
      c.data = data;

      if (ioctl (devices[dn].fd, SCANNER_IOCTL_CTRLMSG, &c) < 0)
	{
	  DBG (5, "sanei_usb_control_msg: SCANNER_IOCTL_CTRLMSG error - %s\n",
	       strerror (errno));
	  return SANE_STATUS_IO_ERROR;
	}
      if ((rtype & 0x80) && debug_level > 10)
	print_buffer (data, len);
      return SANE_STATUS_GOOD;
#elif defined(__BEOS__)
      struct usb_scanner_ioctl_ctrlmsg c;

      c.req.request_type = rtype;
      c.req.request = req;
      c.req.value = value;
      c.req.index = index;
      c.req.length = len;
      c.data = data;

      if (ioctl (devices[dn].fd, B_SCANNER_IOCTL_CTRLMSG, &c) < 0)
	{
	  DBG (5, "sanei_usb_control_msg: SCANNER_IOCTL_CTRLMSG error - %s\n",
	       strerror (errno));
	  return SANE_STATUS_IO_ERROR;
	}
	if ((rtype & 0x80) && debug_level > 10)
		print_buffer (data, len);
	
	return SANE_STATUS_GOOD;
#else /* not __linux__ */
      DBG (5, "sanei_usb_control_msg: not supported on this OS\n");
      return SANE_STATUS_UNSUPPORTED;
#endif /* not __linux__ */
    }
  else if (devices[dn].method == sanei_usb_method_libusb)
#ifdef HAVE_LIBUSB
    {
      int result;

      result = usb_control_msg (devices[dn].libusb_handle, rtype, req,
				value, index, (char *) data, len,
				libusb_timeout);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_control_msg: libusb complained: %s\n",
	       usb_strerror ());
	  return SANE_STATUS_INVAL;
	}
      if ((rtype & 0x80) && debug_level > 10)
	print_buffer (data, len);
      return SANE_STATUS_GOOD;
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      int result;

      result = libusb_control_transfer (devices[dn].lu_handle, rtype, req,
					value, index, data, len,
					libusb_timeout);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_control_msg: libusb complained: %s\n",
	       sanei_libusb_strerror (result));
	  return SANE_STATUS_INVAL;
	}
      if ((rtype & 0x80) && debug_level > 10)
	print_buffer (data, len);
      return SANE_STATUS_GOOD;
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0*/
    {
      DBG (1, "sanei_usb_control_msg: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
  else if (devices[dn].method == sanei_usb_method_usbcalls)
     {
#ifdef HAVE_USBCALLS
      int result;

      result = UsbCtrlMessage (dh, rtype, req,
				value, index, len, (char *) data, 
				usbcalls_timeout);
      DBG (5, "rc of usb_control_msg = %d\n",result);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_control_msg: usbcalls complained: %d\n",result);
	  return SANE_STATUS_INVAL;
	}
      if ((rtype & 0x80) && debug_level > 10)
	print_buffer (data, len);
      return SANE_STATUS_GOOD;
#else /* not HAVE_USBCALLS */
    {
      DBG (1, "sanei_usb_control_msg: usbcalls support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_USBCALLS */
     }
  else
    {
      DBG (1, "sanei_usb_control_msg: access method %d not implemented\n",
	   devices[dn].method);
      return SANE_STATUS_UNSUPPORTED;
    }
}

SANE_Status
sanei_usb_read_int (SANE_Int dn, SANE_Byte * buffer, size_t * size)
{
  ssize_t read_size = 0;
#if defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0)
  SANE_Bool stalled = SANE_FALSE;
#endif

  if (!size)
    {
      DBG (1, "sanei_usb_read_int: size == NULL\n");
      return SANE_STATUS_INVAL;
    }

  if (dn >= device_number || dn < 0)
    {
      DBG (1, "sanei_usb_read_int: dn >= device number || dn < 0\n");
      return SANE_STATUS_INVAL;
    }

  DBG (5, "sanei_usb_read_int: trying to read %lu bytes\n",
       (unsigned long) *size);
  if (devices[dn].method == sanei_usb_method_scanner_driver)
    {
      DBG (1, "sanei_usb_read_int: access method %d not implemented\n",
	   devices[dn].method);
      return SANE_STATUS_INVAL;
    }
  else if (devices[dn].method == sanei_usb_method_libusb)
#ifdef HAVE_LIBUSB
    {
      if (devices[dn].int_in_ep)
	{
	  read_size = usb_interrupt_read (devices[dn].libusb_handle,
					  devices[dn].int_in_ep,
					  (char *) buffer, (int) *size,
					  libusb_timeout);

	  if (read_size < 0)
	    DBG (1, "sanei_usb_read_int: read failed: %s\n",
		 strerror (errno));

	  stalled = (read_size == -EPIPE);
	}
      else
	{
	  DBG (1, "sanei_usb_read_int: can't read without an int "
	       "endpoint\n");
	  return SANE_STATUS_INVAL;
	}
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      if (devices[dn].int_in_ep)
	{
	  int ret;
	  int trans_bytes;
	  ret = libusb_interrupt_transfer (devices[dn].lu_handle,
					   devices[dn].int_in_ep,
					   buffer, (int) *size,
					   &trans_bytes, libusb_timeout);

	  if (ret < 0)
	    read_size = -1;
	  else
	    read_size = trans_bytes;

	  stalled = (ret == LIBUSB_ERROR_PIPE);
	}
      else
	{
	  DBG (1, "sanei_usb_read_int: can't read without an int "
	       "endpoint\n");
	  return SANE_STATUS_INVAL;
	}
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    {
      DBG (1, "sanei_usb_read_int: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
  else if (devices[dn].method == sanei_usb_method_usbcalls)
    {
#ifdef HAVE_USBCALLS
      int rc;
      USHORT usNumBytes=*size; 
      DBG (5, "Entered usbcalls UsbIrqStart with dn = %d\n",dn);
      DBG (5, "Entered usbcalls UsbIrqStart with dh = %p\n",dh);
      DBG (5, "Entered usbcalls UsbIrqStart with int_in_ep = 0x%02x\n",devices[dn].int_in_ep);
      DBG (5, "Entered usbcalls UsbIrqStart with interface_nr = %d\n",devices[dn].interface_nr);
      DBG (5, "Entered usbcalls UsbIrqStart with bytes to read = %u\n",usNumBytes);

      if (devices[dn].int_in_ep){
         rc = UsbIrqStart (dh,devices[dn].int_in_ep,devices[dn].interface_nr,
			usNumBytes, (char *) buffer, pUsbIrqStartHev);
         DBG (5, "rc of UsbIrqStart = %d\n",rc);
        }
      else
	{
	  DBG (1, "sanei_usb_read_int: can't read without an int "
	       "endpoint\n");
	  return SANE_STATUS_INVAL;
	}
      if (rc) return SANE_STATUS_INVAL;
      read_size += usNumBytes;
#else
      DBG (1, "sanei_usb_read_int: usbcalls support missing\n");
      return SANE_STATUS_UNSUPPORTED;
#endif /* HAVE_USBCALLS */
    }
  else
    {
      DBG (1, "sanei_usb_read_int: access method %d not implemented\n",
	   devices[dn].method);
      return SANE_STATUS_INVAL;
    }

  if (read_size < 0)
    {
#ifdef HAVE_LIBUSB
      if (devices[dn].method == sanei_usb_method_libusb)
        if (stalled)
	  usb_clear_halt (devices[dn].libusb_handle, devices[dn].int_in_ep);
#elif defined(HAVE_LIBUSB_1_0)
      if (devices[dn].method == sanei_usb_method_libusb)
        if (stalled)
	  libusb_clear_halt (devices[dn].lu_handle, devices[dn].int_in_ep);
#endif
      *size = 0;
      return SANE_STATUS_IO_ERROR;
    }
  if (read_size == 0)
    {
      DBG (3, "sanei_usb_read_int: read returned EOF\n");
      *size = 0;
      return SANE_STATUS_EOF;
    }
  DBG (5, "sanei_usb_read_int: wanted %lu bytes, got %ld bytes\n",
       (unsigned long) *size, (unsigned long) read_size);
  *size = read_size;
  if (debug_level > 10)
    print_buffer (buffer, read_size);

  return SANE_STATUS_GOOD;
}

SANE_Status
sanei_usb_set_configuration (SANE_Int dn, SANE_Int configuration)
{
  if (dn >= device_number || dn < 0)
    {
      DBG (1,
	   "sanei_usb_set_configuration: dn >= device number || dn < 0, dn=%d\n",
	   dn);
      return SANE_STATUS_INVAL;
    }

  DBG (5, "sanei_usb_set_configuration: configuration = %d\n", configuration);

  if (devices[dn].method == sanei_usb_method_scanner_driver)
    {
#if defined(__linux__)
      return SANE_STATUS_GOOD;
#else /* not __linux__ */
      DBG (5, "sanei_usb_set_configuration: not supported on this OS\n");
      return SANE_STATUS_UNSUPPORTED;
#endif /* not __linux__ */
    }
  else if (devices[dn].method == sanei_usb_method_libusb)
#ifdef HAVE_LIBUSB
    {
      int result;

      result =
	usb_set_configuration (devices[dn].libusb_handle, configuration);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_set_configuration: libusb complained: %s\n",
	       usb_strerror ());
	  return SANE_STATUS_INVAL;
	}
      return SANE_STATUS_GOOD;
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      int result;

      result = libusb_set_configuration (devices[dn].lu_handle, configuration);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_set_configuration: libusb complained: %s\n",
	       sanei_libusb_strerror (result));
	  return SANE_STATUS_INVAL;
	}
      return SANE_STATUS_GOOD;
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    {
      DBG (1, "sanei_usb_set_configuration: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
  else
    {
      DBG (1,
	   "sanei_usb_set_configuration: access method %d not implemented\n",
	   devices[dn].method);
      return SANE_STATUS_UNSUPPORTED;
    }
}

SANE_Status
sanei_usb_claim_interface (SANE_Int dn, SANE_Int interface_number)
{
  if (dn >= device_number || dn < 0)
    {
      DBG (1,
	   "sanei_usb_claim_interface: dn >= device number || dn < 0, dn=%d\n",
	   dn);
      return SANE_STATUS_INVAL;
    }

  DBG (5, "sanei_usb_claim_interface: interface_number = %d\n", interface_number);

  if (devices[dn].method == sanei_usb_method_scanner_driver)
    {
#if defined(__linux__)
      return SANE_STATUS_GOOD;
#else /* not __linux__ */
      DBG (5, "sanei_usb_claim_interface: not supported on this OS\n");
      return SANE_STATUS_UNSUPPORTED;
#endif /* not __linux__ */
    }
  else if (devices[dn].method == sanei_usb_method_libusb)
#ifdef HAVE_LIBUSB
    {
      int result;

      result = usb_claim_interface (devices[dn].libusb_handle, interface_number);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_claim_interface: libusb complained: %s\n",
	       usb_strerror ());
	  return SANE_STATUS_INVAL;
	}
      return SANE_STATUS_GOOD;
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      int result;

      result = libusb_claim_interface (devices[dn].lu_handle, interface_number);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_claim_interface: libusb complained: %s\n",
	       sanei_libusb_strerror (result));
	  return SANE_STATUS_INVAL;
	}
      return SANE_STATUS_GOOD;
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    {
      DBG (1, "sanei_usb_claim_interface: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
  else
    {
      DBG (1, "sanei_usb_claim_interface: access method %d not implemented\n",
	   devices[dn].method);
      return SANE_STATUS_UNSUPPORTED;
    }
}

SANE_Status
sanei_usb_release_interface (SANE_Int dn, SANE_Int interface_number)
{
  if (dn >= device_number || dn < 0)
    {
      DBG (1,
	   "sanei_usb_release_interface: dn >= device number || dn < 0, dn=%d\n",
	   dn);
      return SANE_STATUS_INVAL;
    }

  DBG (5, "sanei_usb_release_interface: interface_number = %d\n", interface_number);

  if (devices[dn].method == sanei_usb_method_scanner_driver)
    {
#if defined(__linux__)
      return SANE_STATUS_GOOD;
#else /* not __linux__ */
      DBG (5, "sanei_usb_release_interface: not supported on this OS\n");
      return SANE_STATUS_UNSUPPORTED;
#endif /* not __linux__ */
    }
  else if (devices[dn].method == sanei_usb_method_libusb)
#ifdef HAVE_LIBUSB
    {
      int result;

      result = usb_release_interface (devices[dn].libusb_handle, interface_number);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_release_interface: libusb complained: %s\n",
	       usb_strerror ());
	  return SANE_STATUS_INVAL;
	}
      return SANE_STATUS_GOOD;
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      int result;

      result = libusb_release_interface (devices[dn].lu_handle, interface_number);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_release_interface: libusb complained: %s\n",
	       sanei_libusb_strerror (result));
	  return SANE_STATUS_INVAL;
	}
      return SANE_STATUS_GOOD;
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    {
      DBG (1, "sanei_usb_release_interface: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
  else
    {
      DBG (1,
	   "sanei_usb_release_interface: access method %d not implemented\n",
	   devices[dn].method);
      return SANE_STATUS_UNSUPPORTED;
    }
}

SANE_Status
sanei_usb_set_altinterface (SANE_Int dn, SANE_Int alternate)
{
  if (dn >= device_number || dn < 0)
    {
      DBG (1,
	   "sanei_usb_set_altinterface: dn >= device number || dn < 0, dn=%d\n",
	   dn);
      return SANE_STATUS_INVAL;
    }

  DBG (5, "sanei_usb_set_altinterface: alternate = %d\n", alternate);

  if (devices[dn].method == sanei_usb_method_scanner_driver)
    {
#if defined(__linux__)
      return SANE_STATUS_GOOD;
#else /* not __linux__ */
      DBG (5, "sanei_usb_set_altinterface: not supported on this OS\n");
      return SANE_STATUS_UNSUPPORTED;
#endif /* not __linux__ */
    }
  else if (devices[dn].method == sanei_usb_method_libusb)
#ifdef HAVE_LIBUSB
    {
      int result;

      result = usb_set_altinterface (devices[dn].libusb_handle, alternate);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_set_altinterface: libusb complained: %s\n",
	       usb_strerror ());
	  return SANE_STATUS_INVAL;
	}
      return SANE_STATUS_GOOD;
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      int result;

      result = libusb_set_interface_alt_setting (devices[dn].lu_handle,
						 devices[dn].interface_nr, alternate);
      if (result < 0)
	{
	  DBG (1, "sanei_usb_set_altinterface: libusb complained: %s\n",
	       sanei_libusb_strerror (result));
	  return SANE_STATUS_INVAL;
	}
      return SANE_STATUS_GOOD;
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    {
      DBG (1, "sanei_set_altinterface: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
  else
    {
      DBG (1,
	   "sanei_usb_set_altinterface: access method %d not implemented\n",
	   devices[dn].method);
      return SANE_STATUS_UNSUPPORTED;
    }
}

extern SANE_Status
sanei_usb_get_descriptor( SANE_Int dn, struct sanei_usb_dev_descriptor *desc )
{
  if (dn >= device_number || dn < 0)
    {
      DBG (1,
	   "sanei_usb_get_descriptor: dn >= device number || dn < 0, dn=%d\n",
	   dn);
      return SANE_STATUS_INVAL;
    }

  DBG (5, "sanei_usb_get_descriptor\n");
#ifdef HAVE_LIBUSB
    {
	  struct usb_device_descriptor *usb_descr;

	  usb_descr = &(devices[dn].libusb_device->descriptor);
	  desc->desc_type = usb_descr->bDescriptorType;
	  desc->bcd_usb   = usb_descr->bcdUSB;
	  desc->bcd_dev   = usb_descr->bcdDevice;
	  desc->dev_class = usb_descr->bDeviceClass;

	  desc->dev_sub_class   = usb_descr->bDeviceSubClass;
	  desc->dev_protocol    = usb_descr->bDeviceProtocol;
	  desc->max_packet_size = usb_descr->bMaxPacketSize0;
	  return SANE_STATUS_GOOD;
    }
#elif defined(HAVE_LIBUSB_1_0)
    {
      struct libusb_device_descriptor lu_desc;
      int ret;

      ret = libusb_get_device_descriptor (devices[dn].lu_device, &lu_desc);
      if (ret < 0)
	{
	  DBG (1,
	       "sanei_usb_get_descriptor: libusb error: %s\n",
	       sanei_libusb_strerror (ret));

	  return SANE_STATUS_INVAL;
	}

      desc->desc_type = lu_desc.bDescriptorType;
      desc->bcd_usb   = lu_desc.bcdUSB;
      desc->bcd_dev   = lu_desc.bcdDevice;
      desc->dev_class = lu_desc.bDeviceClass;

      desc->dev_sub_class   = lu_desc.bDeviceSubClass;
      desc->dev_protocol    = lu_desc.bDeviceProtocol;
      desc->max_packet_size = lu_desc.bMaxPacketSize0;
      return SANE_STATUS_GOOD;
    }
#else /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
    {
      DBG (1, "sanei_usb_get_descriptor: libusb support missing\n");
      return SANE_STATUS_UNSUPPORTED;
    }
#endif /* not HAVE_LIBUSB && not HAVE_LIBUSB_1_0 */
}
