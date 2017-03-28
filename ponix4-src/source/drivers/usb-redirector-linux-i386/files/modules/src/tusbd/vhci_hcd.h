#if defined(_USBD_HCD_H_INC_)
  #include <linux/usb/hcd.h>
#elif defined(_USBD_HCD_H_PATH_)
  #include _USBD_HCD_H_PATH_
#elif RHEL_RELEASE_EQ(6,0)
  #warning hcd.h or hub.h file not found - using cached version (rhel6.0)
  #include "kernel-headers/rhel6.0/hcd.h"
#elif KERNEL_EQ(2,6,15)
  #warning hcd.h or hub.h file not found - using cached version (2.6.15)
  #include "kernel-headers/2.6.15/hcd.h"
#elif KERNEL_EQ(2,6,16)
  #warning hcd.h or hub.h file not found - using cached version (2.6.16)
  #include "kernel-headers/2.6.16/hcd.h"
#elif KERNEL_EQ(2,6,17)
  #warning hcd.h or hub.h file not found - using cached version (2.6.17)
  #include "kernel-headers/2.6.17/hcd.h"
#elif KERNEL_EQ(2,6,18)
  #warning hcd.h or hub.h file not found - using cached version (2.6.18)
  #include "kernel-headers/2.6.18/hcd.h"
#elif KERNEL_EQ(2,6,19)
  #warning hcd.h or hub.h file not found - using cached version (2.6.19)
  #include "kernel-headers/2.6.19/hcd.h"
#elif KERNEL_EQ(2,6,20)
  #warning hcd.h or hub.h file not found - using cached version (2.6.20)
  #include "kernel-headers/2.6.20/hcd.h"
#elif KERNEL_EQ(2,6,21)
  #warning hcd.h or hub.h file not found - using cached version (2.6.21)
  #include "kernel-headers/2.6.21/hcd.h"
#elif KERNEL_EQ(2,6,22)
  #warning hcd.h or hub.h file not found - using cached version (2.6.22)
  #include "kernel-headers/2.6.22/hcd.h"
#elif KERNEL_EQ(2,6,23)
  #warning hcd.h or hub.h file not found - using cached version (2.6.23)
  #include "kernel-headers/2.6.23/hcd.h"
#elif KERNEL_EQ(2,6,24)
  #warning hcd.h or hub.h file not found - using cached version (2.6.24)
  #include "kernel-headers/2.6.24/hcd.h"
#elif KERNEL_EQ(2,6,25)
  #warning hcd.h or hub.h file not found - using cached version (2.6.25)
  #include "kernel-headers/2.6.25/hcd.h"
#elif KERNEL_EQ(2,6,26)
  #warning hcd.h or hub.h file not found - using cached version (2.6.26)
  #include "kernel-headers/2.6.26/hcd.h"
#elif KERNEL_EQ(2,6,27)
  #warning hcd.h or hub.h file not found - using cached version (2.6.27)
  #include "kernel-headers/2.6.27/hcd.h"
#elif KERNEL_EQ(2,6,28)
  #warning hcd.h or hub.h file not found - using cached version (2.6.28)
  #include "kernel-headers/2.6.28/hcd.h"
#elif KERNEL_EQ(2,6,29)
  #warning hcd.h or hub.h file not found - using cached version (2.6.29)
  #include "kernel-headers/2.6.29/hcd.h"
#elif KERNEL_EQ(2,6,30)
  #warning hcd.h or hub.h file not found - using cached version (2.6.30)
  #include "kernel-headers/2.6.30/hcd.h"
#elif KERNEL_EQ(2,6,31)
  #warning hcd.h or hub.h file not found - using cached version (2.6.31)
  #include "kernel-headers/2.6.31/hcd.h"
#elif KERNEL_EQ(2,6,32)
  #warning hcd.h or hub.h file not found - using cached version (2.6.32)
  #include "kernel-headers/2.6.32/hcd.h"
#elif KERNEL_EQ(2,6,33)
  #warning hcd.h or hub.h file not found - using cached version (2.6.33)
  #include "kernel-headers/2.6.33/hcd.h"
#elif KERNEL_EQ(2,6,34)
  #warning hcd.h or hub.h file not found - using cached version (2.6.34)
  #include "kernel-headers/2.6.34/hcd.h"
#elif KERNEL_GT_EQ(2,6,35)
  #include <linux/usb/hcd.h>
#else
  #error Seems like your kernel is not supported! hcd.h or hub.h file not found.
#endif

