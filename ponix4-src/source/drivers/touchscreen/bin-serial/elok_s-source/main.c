// main.c : Main file for elocontrol driver project.
//
// Copyright(c) 2013  Elo Touch Solutions, All rights reserved.
//
// Linux control driver model
//
// NOTE: '_KERNEL_2_6_' must be defined in elocontrol.h for Linux 2.6 kernels
// -----------------------------------------------------------------------------

#include "elocontrol.h"

// -----------------------------------------------------------------------------
//				Static Data
//--------------------------------------------------------------------------------
static unsigned long bytes_in_buffer=0;

#define ELO_QUEUE_SZ	4000

static ELO_X_TOUCH_PACKET elodatabuffer[ELO_QUEUE_SZ] ;
ELO_X_TOUCH_PACKET previousTouchData;
static unsigned int databufferhead=0 ;	// head is read position
static unsigned int databuffertail=0 ;	// tail is write position

static wait_queue_head_t elocontrol_wq ;
struct fasync_struct *gfasync;

int elocontrol_major=elocontrol_MAJOR;
static char *elocontrol_name="/dev/input/elo_ser";

// timer
struct timer_list timer, reset_data_timer;
unsigned long up_seconds = 0 ;
static BOOL signaled_data = FALSE ;
			
// debug
int debug = 0;

unsigned long NoOfOpen=0  ;

// -----------------------------------------------------------------------------
//			Command line params
// -----------------------------------------------------------------------------

//Driver Information
#define DRIVER_VERSION	"3.3.2"
#define DRIVER_AUTHOR 	"Elo Touch Solutions"
#define DRIVER_DESC 	"Elo Touch Solutions Serial Smartset Driver Module"
#define DRIVER_LICENSE	"Elo Touch Solutions, All rights reserved. Copyright (c) 2013"

// module information
#ifdef _KERNEL_2_6_
MODULE_VERSION(DRIVER_VERSION);
#endif
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);

#ifdef _KERNEL_2_6_
module_param(debug,int,0);
#else
MODULE_PARM(debug,"i");
#endif
MODULE_PARM_DESC(debug, "Debug print for the device" ) ;

#ifdef _KERNEL_2_6_
module_param(elocontrol_major,int,0);
#else
MODULE_PARM(elocontrol_major,"i");
#endif
MODULE_PARM_DESC(elocontrol_major, "Major Number for the data device" ) ;

// -----------------------------------------------------------------------------
//			Function Prototype
// -----------------------------------------------------------------------------
int elocontrol_writetouch( const char *buf, size_t count ) ;
static ssize_t elocontrol_read( struct file *file, char *buf, size_t count, loff_t *offset);
static ssize_t elocontrol_write( struct file *file, const char *buf, size_t count, loff_t *offset) ;
static int elocontrol_fasync(int fd, struct file *file, int on) ;

static unsigned int elocontrol_poll(struct file *file, poll_table *wait) ;
static int elocontrol_open( struct inode *inode, struct file *file);
static int elocontrol_release( struct inode *inode, struct file *file ) ;

void my_timer_function(unsigned long data);

//----------------------------------------------------------------------------
static struct file_operations elocontrol_fops = {
	//owner:	THIS_MODULE,
	read:	elocontrol_read,
	write:	elocontrol_write,
	poll:	elocontrol_poll,
	open:	elocontrol_open,
	release:elocontrol_release,
	fasync:	elocontrol_fasync,	
};

//----------------------------------------------------------------------------
void debugprintk( int level, char *szBuff )
{
	// If debugging is enabled print it
	// KERN_INFO , KERN_WARNING, KERN_CRIT
	BOOL bPrint=FALSE ;

	if( debug == DBG_NONE )
		return ;
	
	if( DBG_ALL == debug )
		bPrint=TRUE;
	else
	if( debug >= level )
		bPrint=TRUE;
		
	if( bPrint == TRUE )
		printk( "\n%s", szBuff );
}

 //----------------------------------------------------------------------------
// Implements open entry point for elocontrol /dev/input/elo device
static int elocontrol_open( struct inode *inode, struct file *file)
{
	char szBuff[MAX_STR];

#ifndef _KERNEL_2_6_
	// Increment module usage cnt
	// In 2.4 kernel base module count was incremented using this mechanism.
	// 2.6 automatically increments as per usage
	MOD_INC_USE_COUNT;
#else

    try_module_get(THIS_MODULE);

#endif

	if( file->f_mode & FMODE_READ )
	{
		debugprintk(DBG_INPUT_PIPE, "elocontrol_open: opened in read mode\n" );
	}
	
	// No of opens
	NoOfOpen++;
	
	if( file->f_mode & FMODE_WRITE )
	{
		debugprintk( DBG_INPUT_PIPE,"elocontrol_open: opened in write mode\n" );
	}	

	sprintf( szBuff, "Major no %u minor no %u \n" , MAJOR(inode->i_rdev), 
		MINOR( inode->i_rdev));
	debugprintk( DBG_INPUT_PIPE,szBuff ) ;


	return 0;
}



//----------------------------------------------------------------------------
// Write touch data to the /dev/input/elo data pipe
int elocontrol_writetouch( const char *buf, size_t count )
{
	char szBuff[MAX_STR];

	sprintf( szBuff, "++elocontrol_write: databuffertail = %d \n", (unsigned)databuffertail );
	debugprintk(DBG_INPUT_PIPE, szBuff);

	if( count != ELO_TOUCH_DATA_BUFFER_SIZE ) return 0;

	memcpy( ( elodatabuffer+databuffertail ) , buf , count ) ;

	// circular buffer
	databuffertail++ ;
	if ( databuffertail >= ELO_QUEUE_SZ ) databuffertail=0 ;

	bytes_in_buffer += count ;
	if ( bytes_in_buffer > ( ELO_QUEUE_SZ * ELO_TOUCH_DATA_BUFFER_SIZE ) ) 
		bytes_in_buffer = ELO_QUEUE_SZ * ELO_TOUCH_DATA_BUFFER_SIZE ;

	if( gfasync )
		kill_fasync(&gfasync, SIGIO, POLL_IN);

	wake_up_interruptible( &elocontrol_wq ) ;
	signaled_data = TRUE ;
	return (int)count;
}



//----------------------------------------------------------------------------
// touch data read pipe is /dev/input/elo
// Touch data read entry point 
// return data available from the touch data buffer
static ssize_t elocontrol_read( struct file *file, char *buf, size_t count, 
								loff_t *offset)
{
	char szBuff[MAX_STR];
	int retval = 0;
	PELO_X_TOUCH_PACKET tmpbuffer ;
	int readcount, tmpRead  ;
	char *tmpreadbuf;
		
	// If debugging is enabled print the function param
        #if defined __x86_64__ 
         // size_t data type is 8 bytes in x86_64 platform
	 sprintf(szBuff,"elocontrol_read: In Count=%ld bytes_in_buffer=%u databufferhead=%d\n", count , (int)bytes_in_buffer, databuffertail);
        #elif defined __i386__
         // size_t data type is 4 bytes in i386 platform
	sprintf(szBuff,"elocontrol_read: In Count=%d bytes_in_buffer=%u databufferhead=%d\n", count , (int)bytes_in_buffer, databuffertail);
        #endif
	debugprintk(DBG_INPUT_PIPE,szBuff);
	
#ifndef _KERNEL_2_6_
	if(ELO_TOUCH_DATA_BUFFER_SIZE > bytes_in_buffer)
	{
	
		while ( ELO_TOUCH_DATA_BUFFER_SIZE > bytes_in_buffer) {

			if (file->f_flags & O_NONBLOCK) {
				debugprintk( DBG_INPUT_PIPE, "Flag nonblock\n" ) ;
				retval = -EAGAIN;
				break;
			}
			if (signal_pending(current)) {
				debugprintk( DBG_INPUT_PIPE, "signal pending current\n") ;
				retval = -ERESTARTSYS;
				break;
			}
			interruptible_sleep_on(&elocontrol_wq);
		}
		
		current->state = TASK_RUNNING;
	}
#else
	if(ELO_TOUCH_DATA_BUFFER_SIZE > bytes_in_buffer)
	{
		if (file->f_flags & O_NONBLOCK) {
			debugprintk( DBG_INPUT_PIPE, "Flag nonblock\n" ) ;
			retval = -EAGAIN;
		}
		else
		retval=wait_event_interruptible(elocontrol_wq,	(ELO_TOUCH_DATA_BUFFER_SIZE < bytes_in_buffer)) ;
	}

#endif	
	if (retval)
	{
		debugprintk( DBG_INPUT_PIPE, "read: Error from retval \n" ) ;
		return retval;
	}

	tmpRead =0 ;
	
	if( count > bytes_in_buffer )
	{
		count = readcount = bytes_in_buffer ;
	}
	else
		readcount = count ;
		
	tmpreadbuf=buf;
	while( readcount >= ELO_TOUCH_DATA_BUFFER_SIZE )
	{
		tmpbuffer = elodatabuffer + databufferhead ;
		if( copy_to_user(tmpreadbuf,(elodatabuffer + databufferhead), ELO_TOUCH_DATA_BUFFER_SIZE))
		{
			debugprintk(DBG_INPUT_PIPE,"elocontrol error in copy_to_user success\n" );
			return -EFAULT;
		} 
		databufferhead ++ ;
		if ( databufferhead >= ELO_QUEUE_SZ ) databufferhead=0 ;
		readcount -= ELO_TOUCH_DATA_BUFFER_SIZE ;
		tmpRead += ELO_TOUCH_DATA_BUFFER_SIZE ;
		tmpreadbuf += ELO_TOUCH_DATA_BUFFER_SIZE ;
	}
	sprintf( szBuff, "After read tmpRead=%d, bytes_in_buffer=%d \n", (int)tmpRead, (int)bytes_in_buffer ) ;
	debugprintk( DBG_INPUT_PIPE, szBuff ) ;
	{
	int iLoop ; 
	int cnt ;
	cnt=tmpRead/ELO_TOUCH_DATA_BUFFER_SIZE;
	for( iLoop=0;iLoop<cnt;iLoop++)
	{
		tmpbuffer = (PELO_X_TOUCH_PACKET)(buf + ( ELO_TOUCH_DATA_BUFFER_SIZE * iLoop )) ;	
		sprintf( szBuff, "elocontrol: Buff state=%x screen_no=%x  x=%x y=%x \n", tmpbuffer->status, tmpbuffer->vScreenNum, tmpbuffer->x , tmpbuffer->y );
		debugprintk(DBG_INPUT_PIPE,szBuff);
	}
	}
	count=tmpRead;
	bytes_in_buffer -= tmpRead;
	if( bytes_in_buffer < 0 )
		bytes_in_buffer = 0 ;
	return tmpRead; 
}

//---------------------------------------------------------------------------------
// Serial touch data is written into the /dev/input/elo 
// We dont write it to the read data stream but use it internally to get touch from user mode
// serial component
static ssize_t elocontrol_write( struct file *file, const char *buf, size_t count, 
								loff_t *offset)
{
	// This is a write from user mode elo serial driver to the control module.
	// Check if the data size is correct, if so write it to the conversion routine
	char szBuff[MAX_STR];
	ELO_X_TOUCH_PACKET datapkt ;
	size_t ret = count ;

	sprintf(szBuff,"elocontrol_write: In count=%u, offset=%u\n", (unsigned)count, 
		(unsigned) *offset);
	debugprintk(DBG_INPUT_PIPE,szBuff);

	if( count < ELO_TOUCH_DATA_BUFFER_SIZE )
		return 0 ; 
	
	ret=0 ;	
	while( count >= ELO_TOUCH_DATA_BUFFER_SIZE )
	{
		if( copy_from_user((void *)&datapkt, (void *)(buf+ret), ELO_TOUCH_DATA_BUFFER_SIZE ))
		{
			printk( KERN_EMERG "copy_from_user failed\n" ) ;
			return -EFAULT;
		}

         //printk( KERN_INFO "Previous Tpacket=[%d %d %d %d], Current Tpacket=[%d %d %d %d]\n", previousTouchData.status, previousTouchData.vScreenNum, previousTouchData.x, previousTouchData.y, datapkt.status, datapkt.vScreenNum, datapkt.x, datapkt.y);

         if((previousTouchData.status != 4) && (previousTouchData.vScreenNum != datapkt.vScreenNum))
          {
           //printk( KERN_INFO "Ignoring second touch on video screen[%d]. Valid touch active on video Screen [%d]. Discarding Tpacket=[%d %d %d %d]\n", datapkt.vScreenNum, previousTouchData.vScreenNum, datapkt.status, datapkt.vScreenNum, datapkt.x, datapkt.y);
	   return ret;
          }

         if((previousTouchData.status == 4) && (datapkt.status != 1))
          {
           //printk( KERN_INFO "Ignoring invalid touch packet, waiting for new touch down. Discarding Tpacket=[%d %d %d %d]\n", datapkt.status, datapkt.vScreenNum, datapkt.x, datapkt.y);
	       return ret;
          }

         if((datapkt.status == 1) && (previousTouchData.status != 4))
          {
           //printk( KERN_INFO "Ignoring invalid touch packet, waiting for new touch up. Discarding Tpacket=[%d %d %d %d]\n", datapkt.status, datapkt.vScreenNum, datapkt.x, datapkt.y);
	       return ret;
          }

         elocontrol_writetouch( (char *)&datapkt, ELO_TOUCH_DATA_BUFFER_SIZE );
	 memcpy(&previousTouchData, &datapkt, ELO_TOUCH_DATA_BUFFER_SIZE) ;
		count -= ELO_TOUCH_DATA_BUFFER_SIZE ;
		ret += ELO_TOUCH_DATA_BUFFER_SIZE ;
	}
	
	return ret;
}

//----------------------------------------------------------------------------
// Implements asynchronous io for elocontrol /dev/input/elo pipe
static int elocontrol_fasync(int fd, struct file *file, int on)
{
	int retval;
	retval = fasync_helper(fd, file, on, &gfasync);
	return retval < 0 ? retval : 0;
}

//----------------------------------------------------------------------------
// Implements polling for elocontrol /dev/input/elo pipe
static unsigned int elocontrol_poll(struct file *file, poll_table *wait)
{
	poll_wait(file, &elocontrol_wq, wait);
	if (bytes_in_buffer >=  ELO_TOUCH_DATA_BUFFER_SIZE )
		return POLLIN | POLLRDNORM;
	return 0;
}



//----------------------------------------------------------------------------
// Implements close entry point for elocontrol /dev/input/elo device
static int elocontrol_release( struct inode *inode, struct file *file ) 
{
	// If debugging is enabled print the function param
	debugprintk(DBG_INPUT_PIPE, "elocontrol_release: In\n" ) ;

	elocontrol_fasync(-1, file, 0);

#ifndef _KERNEL_2_6_
	// Decrement module usage cnt
	// In 2.4 kernel base module count was decremented using this mechanism.
	// 2.6 automatically decrements as per usage
	MOD_DEC_USE_COUNT;

#else
    module_put(THIS_MODULE);
#endif
        

	// No of close 
	NoOfOpen--;

	return 0;
}

//----------------------------------------------------------------------------
// Implements timer function for clearing data in the touch data device
void my_timer_function( unsigned long data )
{
  // int DevCnt ;
	unsigned long *sec_count = (unsigned long *) data;
	*sec_count += SECONDS_COUNT ;

	// If debugging is enabled print the function param
	//if data was not read set this to 0	
	
	if( bytes_in_buffer > 0 )
	{
		if( gfasync )
		kill_fasync(&gfasync, SIGIO, POLL_IN);
		wake_up_interruptible( &elocontrol_wq ) ;
	}  

	if( bytes_in_buffer <= 0 )
		signaled_data = FALSE ;

	// if data is not picked up by any client reset the pointers
	// if anyone is intrested in reading data in the meanwhile then do not do this 
	// if device is opened for read then dont reset data
	if( ( NoOfOpen == 1 ) && ( bytes_in_buffer > 0 ) )
	{
		bytes_in_buffer=0;
		// head is read position & tail is write position
		databufferhead=databuffertail=0 ;	
		debugprintk(DBG_INPUT_PIPE, "Reset data buffer in reset_data_timer_function" );
	}  

	timer.expires = jiffies + HZ/33 ;
	add_timer(&timer);
}


//----------------------------------------------------------------------------
// Implements module load initialize
int proj_init_module(void)
{
	int status;
	char szBuff[MAX_STR];


	sprintf( szBuff, "Init module registering %s major %d \n", elocontrol_name, elocontrol_major ) ;
	debugprintk(DBG_INPUT_PIPE,szBuff);


	// register the data device
	status=register_chrdev(elocontrol_major, elocontrol_name, &elocontrol_fops );
	
	if( status <0 )
	{
		printk(KERN_EMERG "device registration failed\n");
		return status;			
	}

	if(elocontrol_major ==0 ) elocontrol_major=status;

	sprintf( szBuff, "major assigned = %d \n", elocontrol_major);
	debugprintk(DBG_INPUT_PIPE,szBuff);



	init_waitqueue_head(&elocontrol_wq);



	debugprintk( DBG_INPUT_PIPE, "init timer\n");

	init_timer(&timer);
	timer.expires = jiffies + SECONDS_COUNT ;
	timer.data = (unsigned long) &up_seconds;
	timer.function = my_timer_function ;
	add_timer( &timer );


	databufferhead = databuffertail = 0 ;		
        bytes_in_buffer = 0 ;

        previousTouchData.status = 4;
        previousTouchData.vScreenNum = 0;
        previousTouchData.x = 0;
        previousTouchData.y = 0;
        //printk( KERN_INFO "Initialize previous Touch packet=[%d %d %d %d]\n", previousTouchData.status, previousTouchData.vScreenNum, previousTouchData.x, previousTouchData.y);
	return 0;
}


//----------------------------------------------------------------------------
// Implements module unload free up
void proj_cleanup_module(void)
{
	int status ;
	char szBuff[ MAX_STR ] ;

	// If debugging is enabled print the function param
	sprintf( szBuff, "Cleanup module Unregistering %s %d \n", elocontrol_name, elocontrol_major );
	debugprintk(DBG_INPUT_PIPE, szBuff);

	// unregister the char driver
	unregister_chrdev(elocontrol_major,elocontrol_name);

	debugprintk(DBG_INPUT_PIPE, "after unregister\n");

	// unregister the timer
	status = del_timer( &timer ) ;
    return;
}

//----------------------------------------------------------------------------
module_init(proj_init_module);
module_exit(proj_cleanup_module);

//-------------------------------EOF---------------------------------------------
