#include <linux/poll.h>
//#include <linux/config.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
 #include <generated/autoconf.h>
#else
 #include <linux/autoconf.h>
#endif

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>

// --------------------------------------------------------------------------------
//	    For Linux 2.6 Kernels, '_KERNEL_2_6_' should be defined below
// --------------------------------------------------------------------------------
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
 #define _KERNEL_2_6_
#else
 #undef _KERNEL_2_6_
#endif

enum
{
 FALSE=0,
 TRUE=1
} ;

// device major should be generated automatically, hence 0
#define elocontrol_MAJOR 0
#define SECONDS_COUNT 1
#define RESET_DATA_SECONDS_COUNT 1000
#define ELO_TOUCH_DATA_BUFFER_SIZE 10 
#define MAX_STR	256

#define BOOL int

typedef struct elo_X_touch_packet
{
  short status;
  short vScreenNum;
  short x;
  short y;
  short z;
} ELO_X_TOUCH_PACKET, *PELO_X_TOUCH_PACKET;

#define ELO_X_TOUCH_PACKET_SIZE		sizeof(ELO_X_TOUCH_PACKET)  // 10 bytes

enum
{
	DBG_NONE=0,
	DBG_INPUT_PIPE,
	DBG_PROC_PIPE,
	DBG_SERIALCMD_PIPE,
	DBG_CONFIG_PIPE,
	DBG_ALL
};
