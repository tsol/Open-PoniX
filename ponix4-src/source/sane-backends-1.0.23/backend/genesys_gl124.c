/* sane - Scanner Access Now Easy.

   Copyright (C) 2010-2012 Stéphane Voltz <stef.dev@free.fr>
   
    
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
*/

#undef BACKEND_NAME
#define BACKEND_NAME genesys_gl124

#include "genesys_gl124.h"

/****************************************************************************
 Low level function
 ****************************************************************************/

/* ------------------------------------------------------------------------ */
/*                  Read and write RAM, registers and AFE                   */
/* ------------------------------------------------------------------------ */

/**
 * Write to many GL124 registers at once
 * Note: sequential call to write register, no effective
 * bulk write implemented.
 * @param dev device to write to
 * @param reg pointer to an array of registers
 * @param elems size of the array
 */
#ifndef UNIT_TESTING
static
#endif
  SANE_Status
gl124_bulk_write_register (Genesys_Device * dev, Genesys_Register_Set * reg,
			   size_t elems)
{
  SANE_Status status = SANE_STATUS_GOOD;
  size_t i;

  for (i = 0; i < elems && status == SANE_STATUS_GOOD; i++)
    {
      if (reg[i].address != 0)
	{
	  status =
	    sanei_genesys_write_register (dev, reg[i].address, reg[i].value);
	}
    }

  DBG (DBG_io, "gl124_bulk_write_register: wrote %lu registers\n",
       (u_long) elems);
  return status;
}

/** @brief read scanned data
 * Read in 0xeff0 maximum sized blocks. This read is done in 2
 * parts if not multple of 512. First read is rounded to a multiple of 512 bytes, last read fetches the 
 * remainder. Read addr is always 0x10000000 with the memory layout setup.
 * @param dev device to read data from
 * @param addr address within ASIC emory space
 * @param data pointer where to store the read data
 * @param len size to read
 */
static SANE_Status
gl124_bulk_read_data (Genesys_Device * dev, uint8_t addr,
		      uint8_t * data, size_t len)
{
  SANE_Status status;
  size_t size, target, read, done;
  uint8_t outdata[8];

  DBG (DBG_io, "gl124_bulk_read_data: requesting %lu bytes (unused addr=0x%02x)\n", (u_long) len,addr);

  if (len == 0)
    return SANE_STATUS_GOOD;

  target = len;

  /* loop until computed data size is read */
  while (target)
    {
      if (target > 0xeff0)
	{
	  size = 0xeff0;
	}
      else
	{
	  size = target;
	}

      /* hard coded 0x10000000 addr */
      outdata[0] = 0;
      outdata[1] = 0;
      outdata[2] = 0;
      outdata[3] = 0x10;

      /* data size to transfer */
      outdata[4] = (size & 0xff);
      outdata[5] = ((size >> 8) & 0xff);
      outdata[6] = ((size >> 16) & 0xff);
      outdata[7] = ((size >> 24) & 0xff);

      status =
	sanei_usb_control_msg (dev->dn, REQUEST_TYPE_OUT, REQUEST_BUFFER,
			       VALUE_BUFFER, 0x00, sizeof (outdata),
			       outdata);
      if (status != SANE_STATUS_GOOD)
	{
	  DBG (DBG_error, "%s failed while writing command: %s\n",
	       __FUNCTION__, sane_strstatus (status));
	  return status;
	}

      /* blocks must be multiple of 512 but not last block */
      read = size;
      read /= 512;
      read *= 512;
    
      if(read>0)
        {
          DBG (DBG_io2,
               "gl124_bulk_read_data: trying to read %lu bytes of data\n",
               (u_long) read);
          status = sanei_usb_read_bulk (dev->dn, data, &read);
          if (status != SANE_STATUS_GOOD)
            {
              DBG (DBG_error,
                   "gl124_bulk_read_data failed while reading bulk data: %s\n",
                   sane_strstatus (status));
              return status;
            }
        }

      /* read less than 512 bytes remainder */
      if (read < size)
	{
          done = read;
	  read = size - read;
	  DBG (DBG_io2,
	       "gl124_bulk_read_data: trying to read remaining %lu bytes of data\n",
	       (u_long) read);
	  status = sanei_usb_read_bulk (dev->dn, data+done, &read);
	  if (status != SANE_STATUS_GOOD)
	    {
	      DBG (DBG_error,
		   "gl124_bulk_read_data failed while reading bulk data: %s\n",
		   sane_strstatus (status));
	      return status;
	    }
	}

      DBG (DBG_io2, "%s: read %lu bytes, %lu remaining\n", __FUNCTION__,
	   (u_long) size, (u_long) (target - size));

      target -= size;
      data += size;
    }

  DBGCOMPLETED;

  return SANE_STATUS_GOOD;
}

/****************************************************************************
 Mid level functions 
 ****************************************************************************/

static SANE_Bool
gl124_get_fast_feed_bit (Genesys_Register_Set * regs)
{
  Genesys_Register_Set *r = NULL;

  r = sanei_genesys_get_address (regs, REG02);
  if (r && (r->value & REG02_FASTFED))
    return SANE_TRUE;
  return SANE_FALSE;
}

static SANE_Bool
gl124_get_filter_bit (Genesys_Register_Set * regs)
{
  Genesys_Register_Set *r = NULL;

  r = sanei_genesys_get_address (regs, REG04);
  if (r && (r->value & REG04_FILTER))
    return SANE_TRUE;
  return SANE_FALSE;
}

static SANE_Bool
gl124_get_lineart_bit (Genesys_Register_Set * regs)
{
  Genesys_Register_Set *r = NULL;

  r = sanei_genesys_get_address (regs, REG04);
  if (r && (r->value & REG04_LINEART))
    return SANE_TRUE;
  return SANE_FALSE;
}

static SANE_Bool
gl124_get_bitset_bit (Genesys_Register_Set * regs)
{
  Genesys_Register_Set *r = NULL;

  r = sanei_genesys_get_address (regs, REG04);
  if (r && (r->value & REG04_BITSET))
    return SANE_TRUE;
  return SANE_FALSE;
}

static SANE_Bool
gl124_get_gain4_bit (Genesys_Register_Set * regs)
{
  Genesys_Register_Set *r = NULL;

  r = sanei_genesys_get_address (regs, REG06);
  if (r && (r->value & REG06_GAIN4))
    return SANE_TRUE;
  return SANE_FALSE;
}

static SANE_Bool
gl124_test_buffer_empty_bit (SANE_Byte val)
{
  if (val & BUFEMPTY)
    return SANE_TRUE;
  return SANE_FALSE;
}

static SANE_Bool
gl124_test_motor_flag_bit (SANE_Byte val)
{
  if (val & MOTORENB)
    return SANE_TRUE;
  return SANE_FALSE;
}

/** @get sensor profile
 * search for the database of motor profiles and get the best one. Each
 * profile is at a specific dpihw. Use LiDE 110 table by default.
 * @param sensor_type sensor id
 * @param dpi hardware dpi for the scan
 * @return a pointer to a Sensor_Profile struct
 */
static Sensor_Profile *get_sensor_profile(int sensor_type, int dpi)
{
  unsigned int i;
  int idx;

  i=0;
  idx=-1;
  while(i<sizeof(sensors)/sizeof(Sensor_Profile))
    {
      /* exact match */
      if(sensors[i].sensor_type==sensor_type && sensors[i].dpi==dpi)
        {
          return &(sensors[i]);
        }

      /* closest match */
      if(sensors[i].sensor_type==sensor_type)
        {
          if(idx<0)
            {
              idx=i;
            }
          else
            {
              if(sensors[i].dpi>=dpi 
              && sensors[i].dpi<sensors[idx].dpi)
                {
                  idx=i;
                }
            }
        }
      i++;
    }

  /* default fallback */
  if(idx<0)
    {
      DBG (DBG_warn,"%s: using default sensor profile\n",__FUNCTION__);
      idx=0;
    }

  return &(sensors[idx]);
}

/** @get motor profile
 * search for the database of motor profiles and get the best one. Each
 * profile is at full step and at a reference exposure. Use LiDE 110 table
 * by default.
 * @param motor_type motor id
 * @param exposure exposure time
 * @return a pointer to a Motor_Profile struct
 */
static Motor_Profile *get_motor_profile(int motor_type, int exposure)
{
  unsigned int i;
  int idx;

  i=0;
  idx=-1;
  while(i<sizeof(motors)/sizeof(Motor_Profile))
    {
      /* exact match */
      if(motors[i].motor_type==motor_type && motors[i].exposure==exposure)
        {
          return &(motors[i]);
        }

      /* closest match */
      if(motors[i].motor_type==motor_type)
        {
          if(idx<0)
            {
              idx=i;
            }
          else
            {
              if(motors[i].exposure>=exposure 
              && motors[i].exposure<motors[idx].exposure)
                {
                  idx=i;
                }
            }
        }
      i++;
    }

  /* default fallback */
  if(idx<0)
    {
      DBG (DBG_warn,"%s: using default motor profile\n",__FUNCTION__);
      idx=0;
    }

  return &(motors[idx]);
}

/** @brief generate slope table
 * Generate the slope table to use for the scan using a reference slope
 * table.
 * @param slope pointer to the slope table to fill
 * @param steps pointer to return used step number
 * @param dpi   desired motor resolution
 * @param exposure exposure used
 * @param base_dpi base resolution of the motor
 * @param step_type step type used for scan
 * @param factor shrink factor for the slope
 * @param motor_type motor id
 */
static int gl124_slope_table(uint16_t *slope,
		             int       *steps,
			     int       dpi,
			     int       exposure,
			     int       base_dpi,
			     int       step_type,
			     int       factor,
                             int       motor_type)
{
int sum, i;
uint16_t target,current;
Motor_Profile *profile;

	/* required speed */
	target=((exposure * dpi) / base_dpi)>>step_type;
	
	/* fill result with target speed */
        for(i=0;i<SLOPE_TABLE_SIZE;i++)
          slope[i]=target;

        profile=get_motor_profile(motor_type,exposure);

	/* use profile to build table */
        i=0;
	sum=0;

        /* first step is used unmodified */
        current=profile->table[0];

        /* loop on profile copying and apply step type */
        while(i<SLOPE_TABLE_SIZE && current>=target)
          {
            slope[i]=current;
            sum+=slope[i];
            i++;
            current=profile->table[i*factor]>>step_type;
          }
        if(i<3 && DBG_LEVEL >= DBG_warn)
          {
            DBG (DBG_warn,"%s: short slope table, failed to reach %d\n",__FUNCTION__,target);
          }

        /* ensure minimal slope size */
        while(i<8)
          {
            sum+=slope[i];
            i++;
          }

        /* return used steps and acceleration sum */
        *steps=i;
	return sum;
}

/* returns the max register bulk size */
static int
gl124_bulk_full_size (void)
{
  return GENESYS_GL124_MAX_REGS;
}

static SANE_Status
gl124_homsnr_gpio(Genesys_Device *dev)
{
uint8_t val;
SANE_Status status=SANE_STATUS_GOOD;

  RIE (sanei_genesys_read_register (dev, REG32, &val));
  val &= ~REG32_GPIO10;
  RIE (sanei_genesys_write_register (dev, REG32, val));
  return status;
}

/** @brief set all registers to default values .
 * This function is called only once at the beginning and
 * fills register startup values for registers reused across scans.
 * Those that are rarely modified or not modified are written
 * individually.
 * @param dev device structure holding register set to initialize
 */
static void
gl124_init_registers (Genesys_Device * dev)
{
  DBGSTART;

  memset (dev->reg, 0, GENESYS_GL124_MAX_REGS * sizeof (Genesys_Register_Set));

  /* default to LiDE 110 */
  SETREG (0x01,0xa2); /* + REG01_SHDAREA */
  SETREG (0x02,0x90);
  SETREG (0x03,0x50);
  SETREG (0x03,0x50 & ~REG03_AVEENB);
  SETREG (0x04,0x03);
  SETREG (0x05,0x00);
  SETREG (0x06,0x50 | REG06_GAIN4); /* causes calibration trouble */
  SETREG (0x06,0x50);
  /* SETREG (0x07,0x00); */
  /* SETREG (0x08,0x10); */
  SETREG (0x09,0x00);
  SETREG (0x0a,0xc0);
  SETREG (0x0b,0x2a);
  SETREG (0x0c,0x12);
  SETREG (0x11,0x00);
  SETREG (0x12,0x00);
  SETREG (0x13,0x0f);
  SETREG (0x14,0x00);
  SETREG (0x15,0x80);
  SETREG (0x16,0x10);
  SETREG (0x17,0x04);
  SETREG (0x18,0x00);
  SETREG (0x19,0x01);
  SETREG (0x1a,0x30);
  SETREG (0x1b,0x00);
  SETREG (0x1c,0x00);
  SETREG (0x1d,0x01);
  SETREG (0x1e,0x10);
  SETREG (0x1f,0x00);
  SETREG (0x20,0x15);
  SETREG (0x21,0x00);
  SETREG (0x22,0x02);
  SETREG (0x23,0x00);
  SETREG (0x24,0x00);
  SETREG (0x25,0x00);
  SETREG (0x26,0x0d);
  SETREG (0x27,0x48);
  SETREG (0x28,0x00);
  SETREG (0x29,0x56);
  SETREG (0x2a,0x5e);
  SETREG (0x2b,0x02);
  SETREG (0x2c,0x02);
  SETREG (0x2d,0x58);
  SETREG (0x3b,0x00);
  SETREG (0x3c,0x00);
  SETREG (0x3d,0x00);
  SETREG (0x3e,0x00);
  SETREG (0x3f,0x02);
  SETREG (0x40,0x00);
  SETREG (0x41,0x00);
  SETREG (0x42,0x00);
  SETREG (0x43,0x00);
  SETREG (0x44,0x00);
  SETREG (0x45,0x00);
  SETREG (0x46,0x00);
  SETREG (0x47,0x00);
  SETREG (0x48,0x00);
  SETREG (0x49,0x00);
  SETREG (0x4f,0x00);
  SETREG (0x52,0x00);
  SETREG (0x53,0x02);
  SETREG (0x54,0x04);
  SETREG (0x55,0x06);
  SETREG (0x56,0x04);
  SETREG (0x57,0x04);
  SETREG (0x58,0x04);
  SETREG (0x59,0x04);
  SETREG (0x5a,0x1a);
  SETREG (0x5b,0x00);
  SETREG (0x5c,0xc0);
  SETREG (0x5f,0x00);
  SETREG (0x60,0x02);
  SETREG (0x61,0x00);
  SETREG (0x62,0x00);
  SETREG (0x63,0x00);
  SETREG (0x64,0x00);
  SETREG (0x65,0x00);
  SETREG (0x66,0x00);
  SETREG (0x67,0x00);
  SETREG (0x68,0x00);
  SETREG (0x69,0x00);
  SETREG (0x6a,0x00);
  SETREG (0x6b,0x00);
  SETREG (0x6c,0x00);
  SETREG (0x6d,0xd0);
  SETREG (0x6e,0x00);
  SETREG (0x6f,0x00);
  SETREG (0x70,0x06);
  SETREG (0x71,0x08);
  SETREG (0x72,0x08);
  SETREG (0x73,0x0a);

  /* CKxMAP */
  SETREG (0x74,0x00);
  SETREG (0x75,0x00);
  SETREG (0x76,0x3c);
  SETREG (0x77,0x00);
  SETREG (0x78,0x00);
  SETREG (0x79,0x9f);
  SETREG (0x7a,0x00);
  SETREG (0x7b,0x00);
  SETREG (0x7c,0x55);

  SETREG (0x7d,0x00);
  SETREG (0x7e,0x08);
  SETREG (0x7f,0x58);
  SETREG (0x80,0x00);
  SETREG (0x81,0x14);

  /* STRPIXEL */
  SETREG (0x82,0x00);
  SETREG (0x83,0x00);
  SETREG (0x84,0x00);
  /* ENDPIXEL */
  SETREG (0x85,0x00);
  SETREG (0x86,0x00);
  SETREG (0x87,0x00);

  SETREG (0x88,0x00);
  SETREG (0x89,0x65);
  SETREG (0x8a,0x00);
  SETREG (0x8b,0x00);
  SETREG (0x8c,0x00);
  SETREG (0x8d,0x00);
  SETREG (0x8e,0x00);
  SETREG (0x8f,0x00);
  SETREG (0x90,0x00);
  SETREG (0x91,0x00);
  SETREG (0x92,0x00);
  SETREG (0x93,0x00);
  SETREG (0x94,0x14);
  SETREG (0x95,0x30);
  SETREG (0x96,0x00);
  SETREG (0x97,0x90);
  SETREG (0x98,0x01);
  SETREG (0x99,0x1f);
  SETREG (0x9a,0x00);
  SETREG (0x9b,0x80);
  SETREG (0x9c,0x80);
  SETREG (0x9d,0x3f);
  SETREG (0x9e,0x00);
  SETREG (0x9f,0x00);
  SETREG (0xa0,0x20);
  SETREG (0xa1,0x30);
  SETREG (0xa2,0x00);
  SETREG (0xa3,0x20);
  SETREG (0xa4,0x01);
  SETREG (0xa5,0x00);
  SETREG (0xa6,0x00);
  SETREG (0xa7,0x08);
  SETREG (0xa8,0x00);
  SETREG (0xa9,0x08);
  SETREG (0xaa,0x01);
  SETREG (0xab,0x00);
  SETREG (0xac,0x00);
  SETREG (0xad,0x40);
  SETREG (0xae,0x01);
  SETREG (0xaf,0x00);
  SETREG (0xb0,0x00);
  SETREG (0xb1,0x40);
  SETREG (0xb2,0x00);
  SETREG (0xb3,0x09);
  SETREG (0xb4,0x5b);
  SETREG (0xb5,0x00);
  SETREG (0xb6,0x10);
  SETREG (0xb7,0x3f);
  SETREG (0xb8,0x00);
  SETREG (0xbb,0x00);
  SETREG (0xbc,0xff);
  SETREG (0xbd,0x00);
  SETREG (0xbe,0x07);
  SETREG (0xc3,0x00);
  SETREG (0xc4,0x00);

  /* gamma 
  SETREG (0xc5,0x00);
  SETREG (0xc6,0x00);
  SETREG (0xc7,0x00);
  SETREG (0xc8,0x00);
  SETREG (0xc9,0x00);
  SETREG (0xca,0x00);
  SETREG (0xcb,0x00);
  SETREG (0xcc,0x00);
  SETREG (0xcd,0x00);
  SETREG (0xce,0x00);
  */

  /* memory layout 
  SETREG (0xd0,0x0a);
  SETREG (0xd1,0x1f);
  SETREG (0xd2,0x34); */
  SETREG (0xd3,0x00);
  SETREG (0xd4,0x00);
  SETREG (0xd5,0x00);
  SETREG (0xd6,0x00);
  SETREG (0xd7,0x00);
  SETREG (0xd8,0x00);
  SETREG (0xd9,0x00);

  /* memory layout
  SETREG (0xe0,0x00);
  SETREG (0xe1,0x48);
  SETREG (0xe2,0x15);
  SETREG (0xe3,0x90);
  SETREG (0xe4,0x15);
  SETREG (0xe5,0x91);
  SETREG (0xe6,0x2a);
  SETREG (0xe7,0xd9);
  SETREG (0xe8,0x2a);
  SETREG (0xe9,0xad);
  SETREG (0xea,0x40);
  SETREG (0xeb,0x22);
  SETREG (0xec,0x40);
  SETREG (0xed,0x23);
  SETREG (0xee,0x55);
  SETREG (0xef,0x6b);
  SETREG (0xf0,0x55);
  SETREG (0xf1,0x6c);
  SETREG (0xf2,0x6a);
  SETREG (0xf3,0xb4);
  SETREG (0xf4,0x6a);
  SETREG (0xf5,0xb5);
  SETREG (0xf6,0x7f);
  SETREG (0xf7,0xfd);*/

  SETREG (0xf8,0x01);   /* other value is 0x05 */
  SETREG (0xf9,0x00);
  SETREG (0xfa,0x00);
  SETREG (0xfb,0x00);
  SETREG (0xfc,0x00);
  SETREG (0xff,0x00);

  /* fine tune upon device description */
  dev->reg[reg_0x05].value &= ~REG05_DPIHW;
  switch (dev->sensor.optical_res)
    {
    case 600:
      dev->reg[reg_0x05].value |= REG05_DPIHW_600;
      break;
    case 1200:
      dev->reg[reg_0x05].value |= REG05_DPIHW_1200;
      break;
    case 2400:
      dev->reg[reg_0x05].value |= REG05_DPIHW_2400;
      break;
    case 4800:
      dev->reg[reg_0x05].value |= REG05_DPIHW_4800;
      break;
    }

  /* initalize calibration reg */
  memcpy (dev->calib_reg, dev->reg,
	  GENESYS_GL124_MAX_REGS * sizeof (Genesys_Register_Set));

  DBGCOMPLETED;
}

/**@brief send slope table for motor movement 
 * Send slope_table in machine byte order
 * @param dev device to send slope table
 * @param table_nr index of the slope table in ASIC memory
 * Must be in the [0-4] range.
 * @param slope_table pointer to 16 bit values array of the slope table
 * @param steps number of elemnts in the slope table
 */
static SANE_Status
gl124_send_slope_table (Genesys_Device * dev, int table_nr,
			uint16_t * slope_table, int steps)
{
  SANE_Status status;
  uint8_t *table;
  int i;
  char msg[2048];

  DBG (DBG_proc, "%s (table_nr = %d, steps = %d)\n", __FUNCTION__,
       table_nr, steps);

  /* sanity check */
  if(table_nr<0 || table_nr>4)
    {
      DBG (DBG_error, "%s: invalid table number %d!\n", __FUNCTION__, table_nr);
      return SANE_STATUS_INVAL;
    }

  table = (uint8_t *) malloc (steps * 2);
  for (i = 0; i < steps; i++)
    {
      table[i * 2] = slope_table[i] & 0xff;
      table[i * 2 + 1] = slope_table[i] >> 8;
    }

  if (DBG_LEVEL >= DBG_io)
    {
      sprintf (msg, "write slope %d (%d)=", table_nr, steps);
      for (i = 0; i < steps; i++)
	{
	  sprintf (msg, "%s,%d", msg, slope_table[i]);
	}
      DBG (DBG_io, "%s: %s\n", __FUNCTION__, msg);
    }

  /* slope table addresses are fixed */
  status =
    sanei_genesys_write_ahb (dev->dn, 0x10000000 + 0x4000 * table_nr, steps * 2, table);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "%s: write to AHB failed writing slope table %d (%s)\n",
	   __FUNCTION__, table_nr, sane_strstatus (status));
    }

  free (table);
  DBGCOMPLETED;
  return status;
}

/**
 * Set register values of 'special' type frontend
 * */
static SANE_Status
gl124_set_ti_fe (Genesys_Device * dev, uint8_t set)
{
  SANE_Status status = SANE_STATUS_GOOD;
  int i;
  uint16_t val;

  DBGSTART;
  if (set == AFE_INIT)
    {
      DBG (DBG_proc, "%s: setting DAC %u\n", __FUNCTION__,
	   dev->model->dac_type);

      /* sets to default values */
      sanei_genesys_init_fe (dev);
    }

  /* start writing to DAC */
  status = sanei_genesys_fe_write_data (dev, 0x00, 0x80);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "%s: failed to write reg0: %s\n", __FUNCTION__,
	   sane_strstatus (status));
      return status;
    }

  /* write values to analog frontend */
  for (i = 1; i < 4; i++)
    {
      val = dev->frontend.reg[i];
      status = sanei_genesys_fe_write_data (dev, i, val);
      if (status != SANE_STATUS_GOOD)
	{
	  DBG (DBG_error,
	       "%s: failed to write reg %d: %s\n", __FUNCTION__, i,
	       sane_strstatus (status));
	  return status;
	}
    }

  status = sanei_genesys_fe_write_data (dev, 0x04, 0x00);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "%s: failed to write reg4: %s\n", __FUNCTION__,
	   sane_strstatus (status));
      return status;
    }

  /* these are not really sign */
  for (i = 0; i < 3; i++)
    {
      val = dev->frontend.sign[i];
      status = sanei_genesys_fe_write_data (dev, 0x05 + i, val);
      if (status != SANE_STATUS_GOOD)
	{
	  DBG (DBG_error,
	       "%s: failed to write reg %d: %s\n", __FUNCTION__, i+5,
	       sane_strstatus (status));
	  return status;
	}
    }

  /* close writing to DAC */
  status = sanei_genesys_fe_write_data (dev, 0x00, 0x11);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "%s: failed to write reg0: %s\n", __FUNCTION__,
	   sane_strstatus (status));
      return status;
    }

  DBGCOMPLETED;

  return status;
}


/* Set values of analog frontend */
static SANE_Status
gl124_set_fe (Genesys_Device * dev, uint8_t set)
{
  SANE_Status status;
  uint8_t val;

  DBG (DBG_proc, "gl124_set_fe (%s)\n",
       set == AFE_INIT ? "init" : set == AFE_SET ? "set" : set ==
       AFE_POWER_SAVE ? "powersave" : "huh?");

  if (set == AFE_INIT)
    {
      DBG (DBG_proc, "gl124_set_fe(): setting DAC %u\n",
	   dev->model->dac_type);
      sanei_genesys_init_fe (dev);
    }

  RIE (sanei_genesys_read_register (dev, REG0A, &val));

  /* route to correct analog FE */
  switch ((val & REG0A_SIFSEL)>>REG0AS_SIFSEL)
    {
    case 3:
      status=gl124_set_ti_fe (dev, set);
      break;
    case 0:
    case 1:
    case 2:
    default:
      DBG (DBG_error, "%s: unsupported anlog FE 0x%02x\n",__FUNCTION__,val);
      status=SANE_STATUS_INVAL;
      break;
    }

  DBGCOMPLETED;
  return status;
}


/**@brief compute exposure to use
 * compute the sensor exposure based on target resolution
 */
static int gl124_compute_exposure(Genesys_Device *dev, int xres)
{
  Sensor_Profile *sensor;

  sensor=get_sensor_profile(dev->model->ccd_type, xres);
  return sensor->exposure;
}

/**@brief compute motor step type to use
 * compute the step type (full, half, quarter, ...) to use based
 * on target resolution
 * @param dev device description
 * @param exposure sensor exposure
 * @return 0 for full step
 *         1 for half step
 *         2 for quarter step
 *         3 for eighth step
 */
static int gl124_compute_step_type(Genesys_Device *dev, int exposure)
{
Motor_Profile *profile;

    profile=get_motor_profile(dev->model->motor_type,exposure);
    return profile->step_type;
}


static SANE_Status
gl124_init_motor_regs_scan (Genesys_Device * dev,
                            Genesys_Register_Set * reg,
                            unsigned int scan_exposure_time,
			    float scan_yres,
			    int scan_step_type,
			    unsigned int scan_lines,
			    unsigned int scan_dummy,
			    unsigned int feed_steps,
			    int scan_mode,
                            unsigned int flags)
{
  SANE_Status status;
  int use_fast_fed;
  unsigned int fast_time;
  unsigned int slow_time;
  unsigned int lincnt, fast_dpi;
  uint16_t scan_table[SLOPE_TABLE_SIZE];
  uint16_t fast_table[SLOPE_TABLE_SIZE];
  int scan_steps,fast_steps,factor;
  unsigned int feedl,dist;
  Genesys_Register_Set *r;
  uint32_t z1, z2;
  float yres;

  DBGSTART;
  DBG (DBG_info, "gl124_init_motor_regs_scan : scan_exposure_time=%d, "
       "scan_yres=%g, scan_step_type=%d, scan_lines=%d, scan_dummy=%d, "
       "feed_steps=%d, scan_mode=%d, flags=%x\n",
       scan_exposure_time,
       scan_yres,
       scan_step_type,
       scan_lines, scan_dummy, feed_steps, scan_mode, flags);

  /* we never use fast fed since we do manual feed for the scans */
  use_fast_fed=0;
  factor=1;

  if(dev->line_interp>0)
    {
      lincnt=scan_lines*dev->line_interp;
    }
  else
    {
      lincnt=scan_lines;
    }
      
  yres=scan_yres;
  if ((scan_mode == SCAN_MODE_COLOR) && (yres<900))
    {
      yres=900;
    }
  if ((scan_mode != SCAN_MODE_COLOR) && (yres<300))
    {
      scan_dummy=dev->line_interp-1;
      dev->line_interp=0;
      yres=300;
    }

  sanei_genesys_set_triple(reg,REG_LINCNT,lincnt);
  DBG (DBG_io, "%s: lincnt=%d\n", __FUNCTION__, lincnt);

  /* compute register 02 value */
  r = sanei_genesys_get_address (reg, REG02);
  r->value = REG02_NOTHOME;
  r->value |= REG02_MTRPWR;

  if (use_fast_fed)
    r->value |= REG02_FASTFED;
  else
    r->value &= ~REG02_FASTFED;

  if (flags & MOTOR_FLAG_AUTO_GO_HOME)
    r->value |= REG02_AGOHOME;

  if ((flags & MOTOR_FLAG_DISABLE_BUFFER_FULL_MOVE)
    ||(yres>=dev->sensor.optical_res))
    r->value |= REG02_ACDCDIS;

  /* SCANFED */
  sanei_genesys_set_double(reg,REG_SCANFED,4);

  /* scan and backtracking slope table */
  slow_time=gl124_slope_table(scan_table,
                              &scan_steps,
                              yres,
                              scan_exposure_time,
                              dev->motor.base_ydpi,
                              scan_step_type,
                              factor,
                              dev->model->motor_type);
  RIE(gl124_send_slope_table (dev, SCAN_TABLE, scan_table, scan_steps));
  RIE(gl124_send_slope_table (dev, BACKTRACK_TABLE, scan_table, scan_steps));

  /* STEPNO */
  sanei_genesys_set_double(reg,REG_STEPNO,scan_steps);

  /* fast table */
  fast_dpi=yres;
  if (scan_mode != SCAN_MODE_COLOR)
    {
      fast_dpi*=3;
    }
  fast_time=gl124_slope_table(fast_table,
                              &fast_steps,
                              fast_dpi,
                              scan_exposure_time,
                              dev->motor.base_ydpi,
                              scan_step_type,
                              factor,
                              dev->model->motor_type);
  RIE(gl124_send_slope_table (dev, STOP_TABLE, fast_table, fast_steps));
  RIE(gl124_send_slope_table (dev, FAST_TABLE, fast_table, fast_steps));

  /* FASTNO */
  sanei_genesys_set_double(reg,REG_FASTNO,fast_steps);

  /* FSHDEC */
  sanei_genesys_set_double(reg,REG_FSHDEC,fast_steps);

  /* FMOVNO */
  sanei_genesys_set_double(reg,REG_FMOVNO,fast_steps);

  /* substract acceleration distance from feedl */
  feedl=feed_steps;
  feedl<<=scan_step_type;

  dist = scan_steps;
  if (flags & MOTOR_FLAG_FEED)
    dist *=2;
  if (use_fast_fed) 
    {
        dist += fast_steps*2;
    }
  DBG (DBG_io2, "%s: acceleration distance=%d\n", __FUNCTION__, dist);

  /* get sure we don't use insane value */
  if(dist<feedl)
    feedl -= dist;
  else
    feedl = 0;

  sanei_genesys_set_triple(reg,REG_FEEDL,feedl);
  DBG (DBG_io, "%s: feedl=%d\n", __FUNCTION__, feedl);

  /* doesn't seem to matter that much */
  sanei_genesys_calculate_zmode2 (use_fast_fed,
				  scan_exposure_time,
				  scan_table,
				  scan_steps,
				  feedl,
                                  scan_steps,
                                  &z1,
                                  &z2);

  sanei_genesys_set_triple(reg,REG_Z1MOD,z1);
  DBG (DBG_info, "gl124_init_motor_regs_scan: z1 = %d\n", z1);

  sanei_genesys_set_triple(reg,REG_Z2MOD,z2);
  DBG (DBG_info, "gl124_init_motor_regs_scan: z2 = %d\n", z2);

  /* LINESEL */
  r = sanei_genesys_get_address (reg, REG1D);
  r->value = (r->value & ~REG1D_LINESEL) | scan_dummy;

  r = sanei_genesys_get_address (reg, REGA0);
  r->value = (scan_step_type << REGA0S_STEPSEL) | (scan_step_type << REGA0S_FSTPSEL);
  
  /* FMOVDEC */
  sanei_genesys_set_double(reg,REG_FMOVDEC,fast_steps);

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}


/** @brief copy sensor specific settings
 * Set up register set for the given sensor resolution.
 * @param dev device to set up
 * @param regs register set to modify
 * @param dpi resolution of the sensor during scan
 * */
static void
gl124_setup_sensor (Genesys_Device * dev, Genesys_Register_Set * regs, int dpi)
{
  Genesys_Register_Set *r;
  int i;
  Sensor_Profile *sensor;
  int dpihw;
  uint32_t exp;

  DBGSTART;

  /* we start at 6, 0-5 is a 16 bits cache for exposure */
  for (i = 0x06; i < 0x0e; i++)
    {
      r = sanei_genesys_get_address (regs, 0x10 + i);
      if (r)
	r->value = dev->sensor.regs_0x10_0x1d[i];
    }

  for (i = 0; i < 11; i++)
    {
      r = sanei_genesys_get_address (regs, 0x52 + i);
      if (r)
	r->value = dev->sensor.regs_0x52_0x5e[i];
    }

  /* set EXPDUMMY and CKxMAP */
  dpihw=sanei_genesys_compute_dpihw(dev,dpi);
  sensor=get_sensor_profile(dev->model->ccd_type, dpihw);
      
  r = sanei_genesys_get_address (regs, 0x18);
  if (r)
    {
      r->value = sensor->reg18;
    }
  r = sanei_genesys_get_address (regs, 0x20);
  if (r)
    {
      r->value = sensor->reg20;
    }
  r = sanei_genesys_get_address (regs, 0x61);
  if (r)
    {
      r->value = sensor->reg61;
    }
  r = sanei_genesys_get_address (regs, 0x98);
  if (r)
    {
      r->value = sensor->reg98;
    }

  sanei_genesys_set_triple(regs,REG_SEGCNT,sensor->segcnt);
  sanei_genesys_set_double(regs,REG_TG0CNT,sensor->tg0cnt);
  sanei_genesys_set_double(regs,REG_EXPDMY,sensor->expdummy);

  /* if no calibration has been done, set default values for exposures */
  exp=dev->sensor.regs_0x10_0x1d[0]*256+dev->sensor.regs_0x10_0x1d[1];
  if(exp==0)
    {
      exp=sensor->expr;
    }
  sanei_genesys_set_triple(regs,REG_EXPR,exp);

  exp=dev->sensor.regs_0x10_0x1d[2]*256+dev->sensor.regs_0x10_0x1d[3];
  if(exp==0)
    {
      exp=sensor->expg;
    }
  sanei_genesys_set_triple(regs,REG_EXPG,exp);

  exp=dev->sensor.regs_0x10_0x1d[4]*256+dev->sensor.regs_0x10_0x1d[5];
  if(exp==0)
    {
      exp=sensor->expb;
    }
  sanei_genesys_set_triple(regs,REG_EXPB,exp);

  sanei_genesys_set_triple(regs,REG_CK1MAP,sensor->ck1map);
  sanei_genesys_set_triple(regs,REG_CK3MAP,sensor->ck3map);
  sanei_genesys_set_triple(regs,REG_CK4MAP,sensor->ck4map);

  /* order of the sub-segments */
  dev->order=sensor->order;

  DBGCOMPLETED;
}

/** @brief setup optical related registers
 * start and pixels are expressed in optical sensor resolution coordinate
 * space.
 * @param exposure_time exposure time to use
 * @param used_res scanning resolution used, may differ from
 *        scan's one
 * @param start logical start pixel coordinate
 * @param pixels logical number of pixels to use
 * @return SANE_STATUS_GOOD if OK
 */
static SANE_Status
gl124_init_optical_regs_scan (Genesys_Device * dev,
			      Genesys_Register_Set * reg,
			      unsigned int exposure_time,
			      int used_res,
			      unsigned int start,
			      unsigned int pixels,
			      int channels,
			      int depth,
			      SANE_Bool half_ccd,
                              int color_filter,
                              int flags)
{
  unsigned int words_per_line, segcnt;
  unsigned int startx, endx, used_pixels, segnb;
  unsigned int dpiset, cksel, dpihw, factor;
  unsigned int bytes;
  Genesys_Register_Set *r;
  SANE_Status status;

  DBG (DBG_proc, "%s :  exposure_time=%d, "
       "used_res=%d, start=%d, pixels=%d, channels=%d, depth=%d, "
       "half_ccd=%d, flags=%x\n", __FUNCTION__, exposure_time,
       used_res, start, pixels, channels, depth, half_ccd, flags);

  /* resolution is divided according to CKSEL */ 
  r = sanei_genesys_get_address (reg, REG18);
  cksel= (r->value & REG18_CKSEL)+1;
  DBG (DBG_io2, "%s: cksel=%d\n", __FUNCTION__, cksel);

  /* to manage high resolution device while keeping good
   * low resolution scanning speed, we make hardware dpi vary */
  dpihw=sanei_genesys_compute_dpihw(dev, used_res * cksel);
  factor=dev->sensor.optical_res/dpihw;
  DBG (DBG_io2, "%s: dpihw=%d (factor=%d)\n", __FUNCTION__, dpihw, factor);

  /* sensor parameters */
  gl124_setup_sensor (dev, reg, dpihw);
  dpiset = used_res * cksel;

  /* start and end coordinate in optical dpi coordinates */
  /* startx = start/cksel + dev->sensor.dummy_pixel; XXX STEF XXX */
  startx = start/cksel;
  used_pixels=pixels/cksel;
  endx = startx + used_pixels;

  /* pixel coordinate factor correction when used dpihw is not maximal one */
  startx/=factor;
  endx/=factor;
  used_pixels=endx-startx;

  status = gl124_set_fe (dev, AFE_SET);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "%s: failed to set frontend: %s\n", __FUNCTION__,
	   sane_strstatus (status));
      return status;
    }

  /* enable shading */
  r = sanei_genesys_get_address (reg, REG01);
  r->value &= ~REG01_SCAN;
  if ((flags & OPTICAL_FLAG_DISABLE_SHADING) ||
      (dev->model->flags & GENESYS_FLAG_NO_CALIBRATION))
    {
      r->value &= ~REG01_DVDSET;
    }
  else
    {
      r->value |= REG01_DVDSET;
    }
  r->value &= ~REG01_SCAN;

  r = sanei_genesys_get_address (reg, REG03);
  r->value &= ~REG03_AVEENB;

  if (flags & OPTICAL_FLAG_DISABLE_LAMP)
    r->value &= ~REG03_LAMPPWR;
  else
    r->value |= REG03_LAMPPWR;

  /* BW threshold */
  RIE (sanei_genesys_write_hregister (dev, REG114, dev->settings.threshold));
  RIE (sanei_genesys_write_hregister (dev, REG115, dev->settings.threshold));

  /* monochrome / color scan */
  r = sanei_genesys_get_address (reg, REG04);
  switch (depth)
    {
    case 1:
      r->value &= ~REG04_BITSET;
      r->value |= REG04_LINEART;
      break;
    case 8:
      r->value &= ~(REG04_LINEART | REG04_BITSET);
      break;
    case 16:
      r->value &= ~REG04_LINEART;
      r->value |= REG04_BITSET;
      break;
    }

  r->value &= ~REG04_FILTER;
  if (channels == 1)
    {
      switch (color_filter)
	{
	case 0:
	  r->value |= 0x10;	/* red filter */
	  break;
	case 2:
	  r->value |= 0x30;	/* blue filter */
	  break;
	default:
	  r->value |= 0x20;	/* green filter */
	  break;
	}
    }

  /* register 05 */
  r = sanei_genesys_get_address (reg, REG05);

  /* set up dpihw */
  r->value &= ~REG05_DPIHW;
  switch(dpihw)
    {
      case 600:
        r->value |= REG05_DPIHW_600;
        break;
      case 1200:
        r->value |= REG05_DPIHW_1200;
        break;
      case 2400:
        r->value |= REG05_DPIHW_2400;
        break;
      case 4800:
        r->value |= REG05_DPIHW_4800;
        break;
    }

  /* enable gamma tables */
  if (flags & OPTICAL_FLAG_DISABLE_GAMMA)
    r->value &= ~REG05_GMMENB;
  else
    r->value |= REG05_GMMENB;

  sanei_genesys_set_double(reg,REG_DPISET,dpiset);
  DBG (DBG_io2, "%s: dpiset used=%d\n", __FUNCTION__, dpiset);

  /* segment number */
  r = sanei_genesys_get_address (reg, 0x98);
  segnb = r->value & 0x0f;

  sanei_genesys_set_triple(reg,REG_STRPIXEL,startx/segnb);
  sanei_genesys_get_triple(reg,REG_SEGCNT,&segcnt);
  if(endx/segnb==segcnt)
    {
      endx=0;
    }
  sanei_genesys_set_triple(reg,REG_ENDPIXEL,endx/segnb);
  
  /* words(16bit) before gamma, conversion to 8 bit or lineart */
  words_per_line = (used_pixels * dpiset) / dpihw;
  bytes = depth / 8;
  if (depth == 1)
    {
      words_per_line = (words_per_line >> 3) + ((words_per_line & 7) ? 1 : 0);
    }
  else
    {
      words_per_line *= bytes;
    }

  dev->bpl = words_per_line;
  dev->cur = 0;
  dev->skip = 0;
  dev->len = dev->bpl/segnb;
  dev->dist = dev->bpl/segnb;
  dev->segnb = segnb;
  dev->line_count = 0;
  if(dpiset>=300)
    {
      dev->line_interp = 0;
    }
  else
    {
      /* line interpolation, we are scanning at higher
       * motor dpi then discard lines to match taget
       * resolution, so lincnt has to be updated
       */
      dev->line_interp = 300/dpiset;
    }

  DBG (DBG_io2, "%s: used_pixels=%d\n", __FUNCTION__, used_pixels);
  DBG (DBG_io2, "%s: pixels     =%d\n", __FUNCTION__, pixels);
  DBG (DBG_io2, "%s: depth      =%d\n", __FUNCTION__, depth);
  DBG (DBG_io2, "%s: dev->bpl   =%lu\n", __FUNCTION__, (unsigned long)dev->bpl);
  DBG (DBG_io2, "%s: dev->len   =%lu\n", __FUNCTION__, (unsigned long)dev->len);
  DBG (DBG_io2, "%s: dev->dist  =%lu\n", __FUNCTION__, (unsigned long)dev->dist);

  words_per_line *= channels;
  dev->wpl = words_per_line;

  /* allocate buffer for odd/even pixels handling */
  if(dev->oe_buffer.buffer!=NULL)
    {
      sanei_genesys_buffer_free (&(dev->oe_buffer));
    }
  RIE (sanei_genesys_buffer_alloc (&(dev->oe_buffer), dev->wpl));

  /* MAXWD is expressed in 2 words unit */
  sanei_genesys_set_triple(reg,REG_MAXWD,(words_per_line));
  DBG (DBG_io2, "%s: words_per_line used=%d\n", __FUNCTION__, words_per_line);

  sanei_genesys_set_triple(reg,REG_LPERIOD,exposure_time);
  DBG (DBG_io2, "%s: exposure_time used=%d\n", __FUNCTION__, exposure_time);

  sanei_genesys_set_double(reg,REG_DUMMY,dev->sensor.dummy_pixel);

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

/* set up registers for an actual scan
 *
 * this function sets up the scanner to scan in normal or single line mode
 */
#ifndef UNIT_TESTING
static
#endif
  SANE_Status
gl124_init_scan_regs (Genesys_Device * dev,
                      Genesys_Register_Set * reg,
                      float xres,	/*dpi */
		      float yres,	/*dpi */
		      float startx,	/*optical_res, from dummy_pixel+1 */
		      float starty,	/*base_ydpi, from home! */
		      float pixels,
		      float lines,
		      unsigned int depth,
		      unsigned int channels,
		      int color_filter,
                      unsigned int flags)
{
  int used_res;
  int start, used_pixels;
  int bytes_per_line;
  int move;
  unsigned int lincnt;
  unsigned int oflags, mflags; /**> optical and motor flags */
  int exposure_time;
  int stagger;

  int slope_dpi = 0;
  int dummy = 0;
  int scan_step_type = 1;
  int max_shift;
  size_t requested_buffer_size, read_buffer_size;

  SANE_Bool half_ccd;		/* false: full CCD res is used, true, half max CCD res is used */
  int optical_res;
  SANE_Status status;

  DBG (DBG_info,
       "gl124_init_scan_regs settings:\n"
       "Resolution    : %gDPI/%gDPI\n"
       "Lines         : %g\n"
       "PPL           : %g\n"
       "Startpos      : %g/%g\n"
       "Depth/Channels: %u/%u\n"
       "Flags         : %x\n\n",
       xres, yres, lines, pixels, startx, starty, depth, channels, flags);

  /* we have 2 domains for ccd: xres below or above half ccd max dpi */
  if (dev->sensor.optical_res < 2 * xres ||
      !(dev->model->flags & GENESYS_FLAG_HALF_CCD_MODE))
    {
      half_ccd = SANE_FALSE;
    }
  else
    {
      half_ccd = SANE_TRUE;
    }

  /* optical_res */
  optical_res = dev->sensor.optical_res;
  if (half_ccd)
    optical_res /= 2;

  /* stagger */
  if ((!half_ccd) && (dev->model->flags & GENESYS_FLAG_STAGGERED_LINE))
    stagger = (4 * yres) / dev->motor.base_ydpi;
  else
    stagger = 0;
  DBG (DBG_info, "gl124_init_scan_regs : stagger=%d lines\n", stagger);

  /** @brief compute used resolution */
  if (flags & SCAN_FLAG_USE_OPTICAL_RES)
    {
      used_res = optical_res;
    }
  else
    {
      /* resolution is choosen from a fixed list and can be used directly,
       * unless we have ydpi higher than sensor's maximum one */
      if(xres>optical_res)
        used_res=optical_res;
      else
        used_res = xres;
    }

  /* compute scan parameters values */
  /* pixels are allways given at full optical resolution */
  /* use detected left margin and fixed value */
  /* start */
  /* add x coordinates */
  start = startx;

  if (stagger > 0)
    start |= 1;

  /* compute correct pixels number */
  used_pixels = (pixels * optical_res) / xres;
  DBG (DBG_info, "%s: used_pixels=%d\n", __FUNCTION__, used_pixels);

  /* round up pixels number if needed */
  if (used_pixels * xres < pixels * optical_res)
    used_pixels++;

  /* we want even number of pixels here */
  if(used_pixels & 1)
    used_pixels++;

  dummy = 0;

  /* slope_dpi */
  /* cis color scan is effectively a gray scan with 3 gray lines per color line and a FILTER of 0 */
  if (dev->model->is_cis)
    slope_dpi = yres * channels;
  else
    slope_dpi = yres;

  /* scan_step_type */
  if(flags & SCAN_FLAG_FEEDING)
    {
      scan_step_type=0;
      exposure_time=MOVE_EXPOSURE;
    }
  else
    {
      exposure_time = gl124_compute_exposure (dev, used_res);
      scan_step_type = gl124_compute_step_type(dev, exposure_time);
    }

  DBG (DBG_info, "gl124_init_scan_regs : exposure_time=%d pixels\n", exposure_time);
  DBG (DBG_info, "gl124_init_scan_regs : scan_step_type=%d\n", scan_step_type);

  /*** optical parameters ***/
  /* in case of dynamic lineart, we use an internal 8 bit gray scan
   * to generate 1 lineart data */
  if ((flags & SCAN_FLAG_DYNAMIC_LINEART)
      && (dev->settings.scan_mode == SCAN_MODE_LINEART))
    {
      depth = 8;
    }

  /* we enable true gray for cis scanners only, and just when doing 
   * scan since color calibration is OK for this mode
   */
  oflags = 0;
  if (flags & SCAN_FLAG_DISABLE_SHADING)
    oflags |= OPTICAL_FLAG_DISABLE_SHADING;
  if (flags & SCAN_FLAG_DISABLE_GAMMA)
    oflags |= OPTICAL_FLAG_DISABLE_GAMMA;
  if (flags & SCAN_FLAG_DISABLE_LAMP)
    oflags |= OPTICAL_FLAG_DISABLE_LAMP;
  if (flags & SCAN_FLAG_CALIBRATION)
    oflags |= OPTICAL_FLAG_DISABLE_DOUBLE;

  /* now _LOGICAL_ optical values used are known, setup registers */
  status = gl124_init_optical_regs_scan (dev,
					 reg,
					 exposure_time,
					 used_res,
					 start,
					 used_pixels,
					 channels,
					 depth,
					 half_ccd,
                                         color_filter,
                                         oflags);
  if (status != SANE_STATUS_GOOD)
    return status;

  /*** motor parameters ***/

  /* max_shift */
  max_shift=sanei_genesys_compute_max_shift(dev,channels,yres,flags);

  /* lines to scan */
  lincnt = lines + max_shift + stagger;

  /* add tl_y to base movement */
  move = starty;
  DBG (DBG_info, "gl124_init_scan_regs: move=%d steps\n", move);

  mflags=0;
  if(flags & SCAN_FLAG_DISABLE_BUFFER_FULL_MOVE)
    mflags|=MOTOR_FLAG_DISABLE_BUFFER_FULL_MOVE;
  if(flags & SCAN_FLAG_FEEDING)
    mflags|=MOTOR_FLAG_FEED;

    status = gl124_init_motor_regs_scan (dev,
					 reg,
					 exposure_time,
					 slope_dpi,
					 scan_step_type,
					 dev->model->is_cis ? lincnt * channels : lincnt,
					 dummy,
					 move,
					 dev->settings.scan_mode,
					 mflags);
  if (status != SANE_STATUS_GOOD)
    return status;

  /*** prepares data reordering ***/

  /* words_per_line */
  bytes_per_line = (used_pixels * used_res) / optical_res;
  bytes_per_line = (bytes_per_line * channels * depth) / 8;

  /* since we don't have sheetfed scanners to handle,
   * use huge read buffer */
  /* TODO find the best size according to settings */
  requested_buffer_size = 16 * bytes_per_line;

  read_buffer_size =
    2 * requested_buffer_size +
    ((max_shift + stagger) * used_pixels * channels * depth) / 8;

  RIE (sanei_genesys_buffer_free (&(dev->read_buffer)));
  RIE (sanei_genesys_buffer_alloc (&(dev->read_buffer), read_buffer_size));

  RIE (sanei_genesys_buffer_free (&(dev->lines_buffer)));
  RIE (sanei_genesys_buffer_alloc (&(dev->lines_buffer), read_buffer_size));

  RIE (sanei_genesys_buffer_free (&(dev->shrink_buffer)));
  RIE (sanei_genesys_buffer_alloc (&(dev->shrink_buffer),
				   requested_buffer_size));

  RIE (sanei_genesys_buffer_free (&(dev->out_buffer)));
  RIE (sanei_genesys_buffer_alloc (&(dev->out_buffer),
				   (8 * dev->settings.pixels * channels *
				    depth) / 8));


  dev->read_bytes_left = bytes_per_line * lincnt;

  DBG (DBG_info,
       "gl124_init_scan_regs: physical bytes to read = %lu\n",
       (u_long) dev->read_bytes_left);
  dev->read_active = SANE_TRUE;


  dev->current_setup.pixels = (used_pixels * used_res) / optical_res;
  DBG (DBG_info, "%s: current_setup.pixels=%d\n", __FUNCTION__, dev->current_setup.pixels);
  dev->current_setup.lines = lincnt;
  dev->current_setup.depth = depth;
  dev->current_setup.channels = channels;
  dev->current_setup.exposure_time = exposure_time;
  dev->current_setup.xres = used_res;
  dev->current_setup.yres = yres;
  dev->current_setup.half_ccd = half_ccd;
  dev->current_setup.stagger = stagger;
  dev->current_setup.max_shift = max_shift + stagger;

  dev->total_bytes_read = 0;
  if (depth == 1)
    dev->total_bytes_to_read =
      ((dev->settings.pixels * dev->settings.lines) / 8 +
       (((dev->settings.pixels * dev->settings.lines) % 8) ? 1 : 0)) *
      channels;
  else
    dev->total_bytes_to_read =
      dev->settings.pixels * dev->settings.lines * channels * (depth / 8);

  DBG (DBG_info, "gl124_init_scan_regs: total bytes to send = %lu\n",
       (u_long) dev->total_bytes_to_read);

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

static SANE_Status
gl124_calculate_current_setup (Genesys_Device * dev)
{
  int channels;
  int depth;
  int start;

  float xres;			/*dpi */
  float yres;			/*dpi */
  float startx;			/*optical_res, from dummy_pixel+1 */
  float pixels;
  float lines;
  int color_filter;

  int used_res;
  int used_pixels;
  unsigned int lincnt;
  int exposure_time;
  int stagger;

  int slope_dpi = 0;
  int dummy = 0;
  int scan_step_type = 1;
  int max_shift, dpihw;
  Sensor_Profile *sensor;

  SANE_Bool half_ccd;		/* false: full CCD res is used, true, half max CCD res is used */
  int optical_res;

  DBG (DBG_info,
       "gl124_calculate_current_setup settings:\n"
       "Resolution: %ux%uDPI\n"
       "Lines     : %u\n"
       "PPL       : %u\n"
       "Startpos  : %.3f/%.3f\n"
       "Scan mode : %d\n\n",
       dev->settings.xres,
       dev->settings.yres, dev->settings.lines, dev->settings.pixels,
       dev->settings.tl_x, dev->settings.tl_y, dev->settings.scan_mode);

  /* channels */
  if (dev->settings.scan_mode == 4)	/* single pass color */
    channels = 3;
  else
    channels = 1;

  /* depth */
  depth = dev->settings.depth;
  if (dev->settings.scan_mode == 0)
    depth = 1;

  /* start */
  start = SANE_UNFIX (dev->model->x_offset);
  start += dev->settings.tl_x;
  start = (start * dev->sensor.optical_res) / MM_PER_INCH;


  xres = dev->settings.xres;
  yres = dev->settings.yres;
  startx = start;
  pixels = dev->settings.pixels;
  lines = dev->settings.lines;
  color_filter = dev->settings.color_filter;


  DBG (DBG_info,
       "gl124_calculate_current_setup settings:\n"
       "Resolution    : %gDPI/%gDPI\n"
       "Lines         : %g\n"
       "PPL           : %g\n"
       "Startpos      : %g\n"
       "Depth/Channels: %u/%u\n\n",
       xres, yres, lines, pixels, startx, depth, channels);

/* half_ccd */
  /* we have 2 domains for ccd: xres below or above half ccd max dpi */
  if ((dev->sensor.optical_res < 2 * xres) ||
      !(dev->model->flags & GENESYS_FLAG_HALF_CCD_MODE))
    {
      half_ccd = SANE_FALSE;
    }
  else
    {
      half_ccd = SANE_TRUE;
    }


  /* optical_res */
  optical_res = dev->sensor.optical_res;
  if (half_ccd)
    optical_res /= 2;

  /* stagger */
  if ((!half_ccd) && (dev->model->flags & GENESYS_FLAG_STAGGERED_LINE))
    stagger = (4 * yres) / dev->motor.base_ydpi;
  else
    stagger = 0;
  DBG (DBG_info, "%s: stagger=%d lines\n", __FUNCTION__, stagger);

  if(xres<=optical_res)
    used_res = xres;
  else
    used_res=optical_res;

  /* compute scan parameters values */
  /* pixels are allways given at half or full CCD optical resolution */
  /* use detected left margin  and fixed value */

  /* compute correct pixels number */
  used_pixels = (pixels * optical_res) / xres;
  DBG (DBG_info, "%s: used_pixels=%d\n", __FUNCTION__, used_pixels);
  dummy = 0;

  /* slope_dpi */
  /* cis color scan is effectively a gray scan with 3 gray lines per color
   line and a FILTER of 0 */
  if (dev->model->is_cis)
    slope_dpi = yres * channels;
  else
    slope_dpi = yres;

  /* scan_step_type and exposure */
  exposure_time = gl124_compute_exposure (dev, xres);
  scan_step_type = gl124_compute_step_type(dev, exposure_time);
  DBG (DBG_info, "%s : exposure_time=%d pixels\n", __FUNCTION__, exposure_time);

  /* max_shift */
  max_shift=sanei_genesys_compute_max_shift(dev,channels,yres,0);
  
  dpihw=sanei_genesys_compute_dpihw(dev,used_res);
  sensor=get_sensor_profile(dev->model->ccd_type, dpihw);
  dev->segnb=sensor->reg98 & 0x0f;

  /* lincnt */
  lincnt = lines + max_shift + stagger;

  dev->current_setup.pixels = (used_pixels * used_res) / optical_res;
  DBG (DBG_info, "%s: current_setup.pixels=%d\n", __FUNCTION__, dev->current_setup.pixels);
  dev->current_setup.lines = lincnt;
  dev->current_setup.depth = depth;
  dev->current_setup.channels = channels;
  dev->current_setup.exposure_time = exposure_time;
  dev->current_setup.xres = used_res;
  dev->current_setup.yres = yres;
  dev->current_setup.half_ccd = half_ccd;
  dev->current_setup.stagger = stagger;
  dev->current_setup.max_shift = max_shift + stagger;

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

static void
gl124_set_motor_power (Genesys_Register_Set * regs, SANE_Bool set)
{

  DBG (DBG_proc, "gl124_set_motor_power\n");

  if (set)
    {
      sanei_genesys_set_reg_from_set (regs, REG02,
				      sanei_genesys_read_reg_from_set (regs,
								       REG02)
				      | REG02_MTRPWR);
    }
  else
    {
      sanei_genesys_set_reg_from_set (regs, REG02,
				      sanei_genesys_read_reg_from_set (regs,
								       REG02)
				      & ~REG02_MTRPWR);
    }
}

static void
gl124_set_lamp_power (Genesys_Device * dev,
		      Genesys_Register_Set * regs, SANE_Bool set)
{
  if (dev == NULL || regs==NULL)
    return;

  if (set)
    {
      sanei_genesys_set_reg_from_set (regs, 0x03,
				      sanei_genesys_read_reg_from_set (regs,
								       0x03)
				      | REG03_LAMPPWR);
    }
  else
    {
      sanei_genesys_set_reg_from_set (regs, 0x03,
				      sanei_genesys_read_reg_from_set (regs,
								       0x03)
				      & ~REG03_LAMPPWR);
    }
}

/**
 * for fast power saving methods only, like disabling certain amplifiers
 * @param device device to use
 * @param enable true to set inot powersaving
 * */
static SANE_Status
gl124_save_power (Genesys_Device * dev, SANE_Bool enable)
{
  DBG (DBG_proc, "gl124_save_power: enable = %d\n", enable);
  if (dev == NULL)
    return SANE_STATUS_INVAL;

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

static SANE_Status
gl124_set_powersaving (Genesys_Device * dev, int delay /* in minutes */ )
{
  Genesys_Register_Set *r;

  DBG (DBG_proc, "gl124_set_powersaving (delay = %d)\n", delay);
  
  r = sanei_genesys_get_address (dev->reg, REG03);
  r->value &= ~0xf0;
  if(delay<15)
    {
      r->value |= delay;
    }
  else
    {
      r->value |= 0x0f;
    }

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

#ifndef UNIT_TESTING
static
#endif
  SANE_Status
gl124_start_action (Genesys_Device * dev)
{
  return sanei_genesys_write_register (dev, 0x0f, 0x01);
}

#ifndef UNIT_TESTING
static
#endif
SANE_Status
gl124_stop_action (Genesys_Device * dev)
{
  SANE_Status status;
  uint8_t val40, val;
  unsigned int loop;

  DBGSTART;

  /* post scan gpio : without that HOMSNR is unreliable */
  gl124_homsnr_gpio(dev);

  status = sanei_genesys_get_status (dev, &val);
  if (DBG_LEVEL >= DBG_io)
    {
      sanei_genesys_print_status (val);
    }

  status = sanei_genesys_read_hregister (dev, REG100, &val40);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "%s: failed to read reg100: %s\n", __FUNCTION__,
	   sane_strstatus (status));
      DBGCOMPLETED;
      return status;
    }

  /* only stop action if needed */
  if (!(val40 & REG100_DATAENB) && !(val40 & REG100_MOTMFLG))
    {
      DBG (DBG_info, "%s: already stopped\n", __FUNCTION__);
      DBGCOMPLETED;
      return SANE_STATUS_GOOD;
    }

  /* ends scan */
  val = sanei_genesys_read_reg_from_set (dev->reg, REG01);
  val &= ~REG01_SCAN;
  sanei_genesys_set_reg_from_set (dev->reg, REG01, val);
  status = sanei_genesys_write_register (dev, REG01, val);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "%s: failed to write register 01: %s\n", __FUNCTION__,
	   sane_strstatus (status));
      return status;
    }
  usleep (100 * 1000);

  loop = 10;
  while (loop > 0)
    {
      status = sanei_genesys_get_status (dev, &val);
      if (DBG_LEVEL >= DBG_io)
	{
	  sanei_genesys_print_status (val);
	}
      status = sanei_genesys_read_hregister (dev, REG100, &val40);
      if (status != SANE_STATUS_GOOD)
	{
	  DBG (DBG_error,
	       "%s: failed to read home sensor: %s\n", __FUNCTION__,
	       sane_strstatus (status));
	  DBGCOMPLETED;
	  return status;
	}

      /* if scanner is in command mode, we are done */
      if (!(val40 & REG100_DATAENB) && !(val40 & REG100_MOTMFLG)
	  && !(val & MOTORENB))
	{
	  DBGCOMPLETED;
	  return SANE_STATUS_GOOD;
	}

      usleep (100 * 1000);
      loop--;
    }

  DBGCOMPLETED;
  return SANE_STATUS_IO_ERROR;
}

/* Send the low-level scan command */
/* todo : is this that useful ? */
#ifndef UNIT_TESTING
static
#endif
  SANE_Status
gl124_begin_scan (Genesys_Device * dev, Genesys_Register_Set * reg,
		  SANE_Bool start_motor)
{
  SANE_Status status;
  uint8_t val;

  DBGSTART;
  if (reg == NULL)
    return SANE_STATUS_INVAL;

  /* set up GPIO for scan */
  RIE (sanei_genesys_read_register (dev, REG32, &val));
  if(dev->settings.yres>=dev->motor.base_ydpi/2)
    {
      val &= 0xf7;
    }
  else if(dev->settings.yres>=dev->motor.base_ydpi/4)
    {
      val &= 0xef;
    }
  else
    {
      val |= 0x10;
    }
  val |= 0x02;
  RIE (sanei_genesys_write_register (dev, REG32, val));

  /* clear scan and feed count */
  RIE (sanei_genesys_write_register (dev, REG0D, REG0D_CLRLNCNT | REG0D_CLRMCNT));

  /* enable scan and motor */
  RIE (sanei_genesys_read_register (dev, REG01, &val));
  val |= REG01_SCAN;
  RIE (sanei_genesys_write_register (dev, REG01, val));

  if (start_motor)
    {
      RIE (sanei_genesys_write_register (dev, REG0F, 1));
    }
  else
    {
      RIE (sanei_genesys_write_register (dev, REG0F, 0));
    }

  DBGCOMPLETED;
  return status;
}


/* Send the stop scan command */
#ifndef UNIT_TESTING
static
#endif
  SANE_Status
gl124_end_scan (Genesys_Device * dev, Genesys_Register_Set * reg,
		SANE_Bool check_stop)
{
  SANE_Status status;

  DBG (DBG_proc, "gl124_end_scan (check_stop = %d)\n", check_stop);
  if (reg == NULL)
    return SANE_STATUS_INVAL;

  if (dev->model->is_sheetfed == SANE_TRUE)
    {
      status = SANE_STATUS_GOOD;
    }
  else				/* flat bed scanners */
    {
      status = gl124_stop_action (dev);
      if (status != SANE_STATUS_GOOD)
	{
	  DBG (DBG_error,
	       "gl124_end_scan: failed to stop: %s\n",
	       sane_strstatus (status));
	  return status;
	}
    }

  DBGCOMPLETED;
  return status;
}


/** @brief Moves the slider to the home (top) position slowly
 * */
#ifndef UNIT_TESTING
static
#endif
  SANE_Status
gl124_slow_back_home (Genesys_Device * dev, SANE_Bool wait_until_home)
{
  Genesys_Register_Set local_reg[GENESYS_GL124_MAX_REGS];
  SANE_Status status;
  Genesys_Register_Set *r;
  uint8_t val;
  float resolution;
  int loop = 0;
  int scan_mode;

  DBG (DBG_proc, "gl124_slow_back_home (wait_until_home = %d)\n",
       wait_until_home);

  /* post scan gpio : without that HOMSNR is unreliable */
  gl124_homsnr_gpio(dev);

  /* first read gives HOME_SENSOR true */
  status = sanei_genesys_get_status (dev, &val);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_slow_back_home: failed to read home sensor: %s\n",
	   sane_strstatus (status));
      return status;
    }
  if (DBG_LEVEL >= DBG_io)
    {
      sanei_genesys_print_status (val);
    }
  usleep (100000);		/* sleep 100 ms */

  /* second is reliable */
  status = sanei_genesys_get_status (dev, &val);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_slow_back_home: failed to read home sensor: %s\n",
	   sane_strstatus (status));
      return status;
    }
  if (DBG_LEVEL >= DBG_io)
    {
      sanei_genesys_print_status (val);
    }
  
  /* is sensor at home? */
  if (val & HOMESNR)
    {
      DBG (DBG_info, "%s: already at home, completed\n", __FUNCTION__);
      dev->scanhead_position_in_steps = 0;
      DBGCOMPLETED;
      return SANE_STATUS_GOOD;
    }

  memset (local_reg, 0, sizeof (local_reg));
  memcpy (local_reg, dev->reg, GENESYS_GL124_MAX_REGS * sizeof (Genesys_Register_Set));
  resolution=sanei_genesys_get_lowest_dpi(dev);

  /* TODO add scan_mode to the API */
  scan_mode= dev->settings.scan_mode;
  dev->settings.scan_mode=SCAN_MODE_LINEART;
  gl124_init_scan_regs (dev,
			local_reg,
			resolution,
			resolution,
			100,
			30000,
			100,
			100,
			8,
			1,
			0,
			SCAN_FLAG_DISABLE_SHADING |
			SCAN_FLAG_DISABLE_GAMMA |
			SCAN_FLAG_IGNORE_LINE_DISTANCE);
  dev->settings.scan_mode=scan_mode;

  /* clear scan and feed count */
  RIE (sanei_genesys_write_register (dev, REG0D, REG0D_CLRLNCNT | REG0D_CLRMCNT));
  
  /* set up for reverse and no scan */
  r = sanei_genesys_get_address (local_reg, REG02);
  r->value |= REG02_MTRREV;

  RIE (gl124_bulk_write_register (dev, local_reg, GENESYS_GL124_MAX_REGS));

  status = gl124_start_action (dev);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_slow_back_home: failed to start motor: %s\n",
	   sane_strstatus (status));
      gl124_stop_action (dev);
      /* restore original registers */
      gl124_bulk_write_register (dev, dev->reg, GENESYS_GL124_MAX_REGS);
      return status;
    }

  /* post scan gpio : without that HOMSNR is unreliable */
  gl124_homsnr_gpio(dev);

  if (wait_until_home)
    {

      while (loop < 300)	/* do not wait longer then 30 seconds */
	{
	  status = sanei_genesys_get_status (dev, &val);
	  if (status != SANE_STATUS_GOOD)
	    {
	      DBG (DBG_error,
		   "gl124_slow_back_home: failed to read home sensor: %s\n",
		   sane_strstatus (status));
	      return status;
	    }

	  if (val & HOMESNR)	/* home sensor */
	    {
	      DBG (DBG_info, "gl124_slow_back_home: reached home position\n");
	      DBGCOMPLETED;
              dev->scanhead_position_in_steps = 0;
	      return SANE_STATUS_GOOD;
	    }
	  usleep (100000);	/* sleep 100 ms */
	  ++loop;
	}

      /* when we come here then the scanner needed too much time for this, so we better stop the motor */
      gl124_stop_action (dev);
      DBG (DBG_error,
	   "gl124_slow_back_home: timeout while waiting for scanhead to go home\n");
      return SANE_STATUS_IO_ERROR;
    }

  DBG (DBG_info, "gl124_slow_back_home: scanhead is still moving\n");
  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

/** @brief moves the slider to steps at motor base dpi
 * @param dev device to work on
 * @param steps number of steps to move
 * */
#ifndef UNIT_TESTING
static
#endif
SANE_Status
gl124_feed (Genesys_Device * dev, unsigned int steps)
{
  Genesys_Register_Set local_reg[GENESYS_GL124_MAX_REGS];
  SANE_Status status;
  Genesys_Register_Set *r;
  float resolution;
  uint8_t val;

  DBGSTART;
  DBG (DBG_io, "%s: steps=%d\n", __FUNCTION__, steps);

  /* prepare local registers */
  memset (local_reg, 0, sizeof (local_reg));
  memcpy (local_reg, dev->reg, GENESYS_GL124_MAX_REGS * sizeof (Genesys_Register_Set));

  resolution=sanei_genesys_get_lowest_ydpi(dev);
  gl124_init_scan_regs (dev,
			local_reg,
			resolution,
			resolution,
			0,
			steps,
			100,
			3,
			8,
			3,
			dev->settings.color_filter,
			SCAN_FLAG_DISABLE_SHADING |
			SCAN_FLAG_DISABLE_GAMMA |
                        SCAN_FLAG_FEEDING |
			SCAN_FLAG_DISABLE_BUFFER_FULL_MOVE |
			SCAN_FLAG_IGNORE_LINE_DISTANCE);

  /* set exposure to zero */
  sanei_genesys_set_triple(local_reg,REG_EXPR,0);
  sanei_genesys_set_triple(local_reg,REG_EXPG,0);
  sanei_genesys_set_triple(local_reg,REG_EXPB,0);

  /* clear scan and feed count */
  RIE (sanei_genesys_write_register (dev, REG0D, REG0D_CLRLNCNT));
  RIE (sanei_genesys_write_register (dev, REG0D, REG0D_CLRMCNT));
  
  /* set up for no scan */
  r = sanei_genesys_get_address (local_reg, REG01);
  r->value &= ~REG01_SCAN;
  
  /* send registers */
  RIE (gl124_bulk_write_register (dev, local_reg, GENESYS_GL124_MAX_REGS));

  status = gl124_start_action (dev);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "%s: failed to start motor: %s\n", __FUNCTION__, sane_strstatus (status));
      gl124_stop_action (dev);

      /* restore original registers */
      gl124_bulk_write_register (dev, dev->reg, GENESYS_GL124_MAX_REGS);
      return status;
    }

  /* wait until feed count reaches the required value, but do not
   * exceed 30s */
  do
    {
          status = sanei_genesys_get_status (dev, &val);
    }
  while (status == SANE_STATUS_GOOD && !(val & FEEDFSH));

  /* then stop scanning */
  RIE(gl124_stop_action (dev));
  
  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}


/* Automatically set top-left edge of the scan area by scanning a 200x200 pixels
   area at 600 dpi from very top of scanner */
static SANE_Status
gl124_search_start_position (Genesys_Device * dev)
{
  int size;
  SANE_Status status;
  uint8_t *data;
  Genesys_Register_Set local_reg[GENESYS_GL124_MAX_REGS];
  int steps;

  int pixels = 600;
  int dpi = 300;

  DBG (DBG_proc, "gl124_search_start_position\n");

  memset (local_reg, 0, sizeof (local_reg));
  memcpy (local_reg, dev->reg,
	  GENESYS_GL124_MAX_REGS * sizeof (Genesys_Register_Set));

  /* sets for a 200 lines * 600 pixels */
  /* normal scan with no shading */

  status = gl124_init_scan_regs (dev, local_reg, dpi, dpi, 0, 0,	/*we should give a small offset here~60 steps */
				 600, dev->model->search_lines, 8, 1, 1,	/*green */
				 SCAN_FLAG_DISABLE_SHADING |
				 SCAN_FLAG_DISABLE_GAMMA |
				 SCAN_FLAG_IGNORE_LINE_DISTANCE |
				 SCAN_FLAG_DISABLE_BUFFER_FULL_MOVE);

  /* send to scanner */
  status = gl124_bulk_write_register (dev, local_reg, GENESYS_GL124_MAX_REGS);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_search_start_position: failed to bulk write registers: %s\n",
	   sane_strstatus (status));
      return status;
    }

  size = pixels * dev->model->search_lines;

  data = malloc (size);
  if (!data)
    {
      DBG (DBG_error,
	   "gl124_search_start_position: failed to allocate memory\n");
      return SANE_STATUS_NO_MEM;
    }

  status = gl124_begin_scan (dev, local_reg, SANE_TRUE);
  if (status != SANE_STATUS_GOOD)
    {
      free (data);
      DBG (DBG_error,
	   "gl124_search_start_position: failed to begin scan: %s\n",
	   sane_strstatus (status));
      return status;
    }

  /* waits for valid data */
  do
    sanei_genesys_test_buffer_empty (dev, &steps);
  while (steps);

  /* now we're on target, we can read data */
  status = sanei_genesys_read_data_from_scanner (dev, data, size);
  if (status != SANE_STATUS_GOOD)
    {
      free (data);
      DBG (DBG_error,
	   "gl124_search_start_position: failed to read data: %s\n",
	   sane_strstatus (status));
      return status;
    }

  if (DBG_LEVEL >= DBG_data)
    sanei_genesys_write_pnm_file ("search_position.pnm", data, 8, 1, pixels,
				  dev->model->search_lines);

  status = gl124_end_scan (dev, local_reg, SANE_TRUE);
  if (status != SANE_STATUS_GOOD)
    {
      free (data);
      DBG (DBG_error,
	   "gl124_search_start_position: failed to end scan: %s\n",
	   sane_strstatus (status));
      return status;
    }

  /* update regs to copy ASIC internal state */
  memcpy (dev->reg, local_reg,
	  GENESYS_GL124_MAX_REGS * sizeof (Genesys_Register_Set));

  status =
    sanei_genesys_search_reference_point (dev, data, 0, dpi, pixels,
					  dev->model->search_lines);
  if (status != SANE_STATUS_GOOD)
    {
      free (data);
      DBG (DBG_error,
	   "gl124_search_start_position: failed to set search reference point: %s\n",
	   sane_strstatus (status));
      return status;
    }

  free (data);
  return SANE_STATUS_GOOD;
}

/* 
 * sets up register for coarse gain calibration
 * todo: check it for scanners using it */
static SANE_Status
gl124_init_regs_for_coarse_calibration (Genesys_Device * dev)
{
  SANE_Status status;
  uint8_t channels;
  uint8_t cksel;

  DBGSTART;
  cksel = (dev->calib_reg[reg_0x18].value & REG18_CKSEL) + 1;	/* clock speed = 1..4 clocks */

  /* set line size */
  if (dev->settings.scan_mode == SCAN_MODE_COLOR)	/* single pass color */
    channels = 3;
  else
    channels = 1;

  status = gl124_init_scan_regs (dev,
				 dev->calib_reg,
				 dev->settings.xres,
				 dev->settings.yres,
				 0,
				 0,
				 dev->sensor.optical_res / cksel,
				 20,
				 16,
				 channels,
				 dev->settings.color_filter,
				 SCAN_FLAG_DISABLE_SHADING |
				 SCAN_FLAG_DISABLE_GAMMA |
				 SCAN_FLAG_SINGLE_LINE |
				 SCAN_FLAG_FEEDING |
				 SCAN_FLAG_IGNORE_LINE_DISTANCE);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_init_register_for_coarse_calibration: failed to setup scan: %s\n",
	   sane_strstatus (status));
      return status;
    }
  gl124_set_motor_power (dev->calib_reg, SANE_FALSE);

  DBG (DBG_info,
       "gl124_init_register_for_coarse_calibration: optical sensor res: %d dpi, actual res: %d\n",
       dev->sensor.optical_res / cksel, dev->settings.xres);

  status = gl124_bulk_write_register (dev, dev->calib_reg, GENESYS_GL124_MAX_REGS);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_init_register_for_coarse_calibration: failed to bulk write registers: %s\n",
	   sane_strstatus (status));
      return status;
    }

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}


/* init registers for shading calibration */
/* shading calibration is done at dpihw */
static SANE_Status
gl124_init_regs_for_shading (Genesys_Device * dev)
{
  SANE_Status status;
  int move, resolution, dpihw, factor;

  DBGSTART;
  
  /* initial calibration reg values */
  memcpy (dev->calib_reg, dev->reg, GENESYS_GL124_MAX_REGS * sizeof (Genesys_Register_Set));

  dev->calib_channels = 3;
  dev->calib_lines = dev->model->shading_lines;
  dpihw=sanei_genesys_compute_dpihw(dev,dev->settings.xres);
  if(dpihw>=2400)
    {
      dev->calib_lines *= 2;
    }
  factor=dev->sensor.optical_res/dpihw;
  resolution=dpihw;
  dev->calib_resolution = resolution;
  dev->calib_pixels = dev->sensor.sensor_pixels/factor;

  /* distance to move to reach white target at high resolution */
  move=0;
  if(dev->settings.yres>=1200)
    {
      move = SANE_UNFIX (dev->model->y_offset_calib);
      move = (move * (dev->motor.base_ydpi/4)) / MM_PER_INCH;
    }
  DBG (DBG_io, "%s: move=%d steps\n", __FUNCTION__, move);

  status = gl124_init_scan_regs (dev,
				 dev->calib_reg,
				 resolution,
				 resolution,
				 0,
				 move,
				 dev->calib_pixels,
				 dev->calib_lines,
				 16,
				 dev->calib_channels,
				 0,
				 SCAN_FLAG_DISABLE_SHADING |
				 SCAN_FLAG_DISABLE_GAMMA |
				 SCAN_FLAG_DISABLE_BUFFER_FULL_MOVE |
				 SCAN_FLAG_IGNORE_LINE_DISTANCE);
  gl124_set_motor_power (dev->calib_reg, SANE_FALSE);

  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "%s: failed to setup scan: %s\n", __FUNCTION__, 
	   sane_strstatus (status));
      return status;
    }

  dev->scanhead_position_in_steps += dev->calib_lines + move;

  status = gl124_bulk_write_register (dev, dev->calib_reg, GENESYS_GL124_MAX_REGS);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "%s: failed to bulk write registers: %s\n", __FUNCTION__, 
	   sane_strstatus (status));
      return status;
    }

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

/** @brief set up registers for the actual scan
 */
static SANE_Status
gl124_init_regs_for_scan (Genesys_Device * dev)
{
  int channels;
  int flags;
  int depth;
  float move;
  int move_dpi;
  float start;
  uint8_t val40,val;

  SANE_Status status;

  DBG (DBG_info,
       "gl124_init_regs_for_scan settings:\nResolution: %ux%uDPI\n"
       "Lines     : %u\npixels    : %u\nStartpos  : %.3f/%.3f\nScan mode : %d\n\n",
       dev->settings.xres,
       dev->settings.yres,
       dev->settings.lines,
       dev->settings.pixels,
       dev->settings.tl_x,
       dev->settings.tl_y,
       dev->settings.scan_mode);

  /* wait for motor to stop first */
  status = sanei_genesys_get_status (dev, &val);
  if (status != SANE_STATUS_GOOD)
   {
     DBG (DBG_error, "%s: failed to read status: %s\n", __FUNCTION__, sane_strstatus (status));
     DBGCOMPLETED;
     return status;
   }
  status = sanei_genesys_read_hregister (dev, REG100, &val40);
  if (status != SANE_STATUS_GOOD)
   {
     DBG (DBG_error, "%s: failed to read reg100: %s\n", __FUNCTION__, sane_strstatus (status));
     DBGCOMPLETED;
     return status;
   }
  if((val & MOTORENB) || (val40 & REG100_MOTMFLG))
    {
      do
        {
          usleep(10000);
          status = sanei_genesys_get_status (dev, &val);
          if (status != SANE_STATUS_GOOD)
           {
             DBG (DBG_error, "%s: failed to read status: %s\n", __FUNCTION__, sane_strstatus (status));
             DBGCOMPLETED;
             return status;
           }
          status = sanei_genesys_read_hregister (dev, REG100, &val40);
          if (status != SANE_STATUS_GOOD)
           {
             DBG (DBG_error, "%s: failed to read reg100: %s\n", __FUNCTION__, sane_strstatus (status));
             DBGCOMPLETED;
             return status;
           }
        } while ((val & MOTORENB) || (val40 & REG100_MOTMFLG));
        usleep(50000);
    }

  /* ensure head is parked in case of calibration */
  RIE (gl124_slow_back_home (dev, SANE_TRUE));

  /* channels */
  if (dev->settings.scan_mode == SCAN_MODE_COLOR)
    channels = 3;
  else
    channels = 1;

  /* depth */
  depth = dev->settings.depth;
  if (dev->settings.scan_mode == SCAN_MODE_LINEART)
    depth = 1;

  /* y (motor) distance to move to reach scanned area */
  move_dpi = dev->motor.base_ydpi/4;
  move = SANE_UNFIX (dev->model->y_offset);
  move += dev->settings.tl_y;
  move = (move * move_dpi) / MM_PER_INCH;
  DBG (DBG_info, "%s: move=%f steps\n", __FUNCTION__, move);

  if(channels*dev->settings.yres>=600 && move>700)
    {
      status = gl124_feed (dev, move-500);
      if (status != SANE_STATUS_GOOD)
        {
          DBG (DBG_error, "%s: failed to move to scan area\n",__FUNCTION__);
          return status;
        }
      move=500;
    }
  DBG (DBG_info, "gl124_init_regs_for_scan: move=%f steps\n", move);

  /* start */
  start = SANE_UNFIX (dev->model->x_offset);
  start += dev->settings.tl_x;
  start = (start * dev->sensor.optical_res) / MM_PER_INCH;

  flags = 0;

  /* enable emulated lineart from gray data */
  if(dev->settings.scan_mode == SCAN_MODE_LINEART 
     && dev->settings.dynamic_lineart)
    {
      flags |= SCAN_FLAG_DYNAMIC_LINEART;
    }

  status = gl124_init_scan_regs (dev,
				 dev->reg,
				 dev->settings.xres,
				 dev->settings.yres,
				 start,
				 move,
				 dev->settings.pixels,
				 dev->settings.lines,
				 depth,
				 channels,
                                 dev->settings.color_filter,
                                 flags);

  if (status != SANE_STATUS_GOOD)
    return status;

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

/**
 * Send shading calibration data. The buffer is considered to always hold values
 * for all the channels.
 */
#ifndef UNIT_TESTING
static
#endif
SANE_Status
gl124_send_shading_data (Genesys_Device * dev, uint8_t * data, int size)
{
  SANE_Status status = SANE_STATUS_GOOD;
  uint32_t addr, length, strpixel ,endpixel, x, factor, segcnt, pixels, i;
  uint16_t dpiset,dpihw;
  uint8_t val,*buffer,*ptr,*src;

  DBGSTART;
  DBG( DBG_io2, "%s: writing %d bytes of shading data\n",__FUNCTION__,size);

  /* logical size of a color as seen by generic code of the frontend */
  length = (uint32_t) (size / 3);
  sanei_genesys_get_triple(dev->reg,REG_STRPIXEL,&strpixel);
  sanei_genesys_get_triple(dev->reg,REG_ENDPIXEL,&endpixel);
  sanei_genesys_get_triple(dev->reg,REG_SEGCNT,&segcnt);
  if(endpixel==0)
    {
      endpixel=segcnt;
    }
  DBG( DBG_io2, "%s: STRPIXEL=%d, ENDPIXEL=%d, PIXELS=%d, SEGCNT=%d\n",__FUNCTION__,strpixel,endpixel,endpixel-strpixel,segcnt);
 
  /* turn pixel value into bytes 2x16 bits words */
  strpixel*=2*2; /* 2 words of 2 bytes */
  endpixel*=2*2;
  segcnt*=2*2;
  pixels=endpixel-strpixel;

  DBG( DBG_io2, "%s: using chunks of %d bytes (%d shading data pixels)\n",__FUNCTION__,length, length/4);
  buffer=(uint8_t *)malloc(pixels*dev->segnb);
  memset(buffer,0,pixels*dev->segnb);

  /* compute deletion factor */
  sanei_genesys_get_double(dev->reg,REG_DPISET,&dpiset);
  dpihw=sanei_genesys_compute_dpihw(dev,dpiset);
  factor=dpihw/dpiset;
  DBG( DBG_io2, "%s: factor=%d\n",__FUNCTION__,factor);

  /* write actual red data */
  for(i=0;i<3;i++)
    {
      /* copy data to work buffer and process it */
          /* coefficent destination */
      ptr=buffer;

      /* iterate on both sensor segment */
      for(x=0;x<pixels;x+=4*factor)
        {
          /* coefficient source */
          src=data+x+strpixel+i*length;

          /* iterate over all the segments */
          switch(dev->segnb)
            {
            case 1:
              ptr[0+pixels*0]=src[0+segcnt*0];
              ptr[1+pixels*0]=src[1+segcnt*0];
              ptr[2+pixels*0]=src[2+segcnt*0];
              ptr[3+pixels*0]=src[3+segcnt*0];
              break;
            case 2:
              ptr[0+pixels*0]=src[0+segcnt*0];
              ptr[1+pixels*0]=src[1+segcnt*0];
              ptr[2+pixels*0]=src[2+segcnt*0];
              ptr[3+pixels*0]=src[3+segcnt*0];
              ptr[0+pixels*1]=src[0+segcnt*1];
              ptr[1+pixels*1]=src[1+segcnt*1];
              ptr[2+pixels*1]=src[2+segcnt*1];
              ptr[3+pixels*1]=src[3+segcnt*1];
              break;
            case 4:
              ptr[0+pixels*0]=src[0+segcnt*0];
              ptr[1+pixels*0]=src[1+segcnt*0];
              ptr[2+pixels*0]=src[2+segcnt*0];
              ptr[3+pixels*0]=src[3+segcnt*0];
              ptr[0+pixels*1]=src[0+segcnt*2];
              ptr[1+pixels*1]=src[1+segcnt*2];
              ptr[2+pixels*1]=src[2+segcnt*2];
              ptr[3+pixels*1]=src[3+segcnt*2];
              ptr[0+pixels*2]=src[0+segcnt*1];
              ptr[1+pixels*2]=src[1+segcnt*1];
              ptr[2+pixels*2]=src[2+segcnt*1];
              ptr[3+pixels*2]=src[3+segcnt*1];
              ptr[0+pixels*3]=src[0+segcnt*3];
              ptr[1+pixels*3]=src[1+segcnt*3];
              ptr[2+pixels*3]=src[2+segcnt*3];
              ptr[3+pixels*3]=src[3+segcnt*3];
              break;
            }

          /* next shading coefficient */
          ptr+=4;
        }
      RIE (sanei_genesys_read_register (dev, 0xd0+i, &val));
      addr = val * 8192 + 0x10000000;
      status = sanei_genesys_write_ahb (dev->dn, addr, pixels*dev->segnb, buffer);
      if (status != SANE_STATUS_GOOD)
        {
          DBG (DBG_error, "gl124_send_shading_data; write to AHB failed (%s)\n",
               sane_strstatus (status));
          return status;
        }
    }

  free(buffer);
  DBGCOMPLETED;

  return status;
}

/** @brief send gamma table to scanner
 * This function sends generic gamma table (ie ones built with
 * provided gamma) or the user defined one if provided by 
 * fontend.
 * @param dev device to write to
 * @param generic flag for using generic gamma tables
 */
static SANE_Status
gl124_send_gamma_table (Genesys_Device * dev, SANE_Bool generic)
{
  int size;
  int status;
  uint8_t *gamma, val;
  int i, gmmval;

  DBG (DBG_proc, "gl124_send_gamma_table\n");

  /* don't send anything if no specific gamma table defined */
  if (!generic
      && (dev->sensor.red_gamma_table == NULL
	  || dev->sensor.green_gamma_table == NULL
	  || dev->sensor.blue_gamma_table == NULL))
    {
      DBG (DBG_proc, "gl124_send_gamma_table: nothing to send, skipping\n");
      return SANE_STATUS_GOOD;
    }

  size = 256;

  /* allocate temporary gamma tables: 16 bits words, 3 channels */
  gamma = (uint8_t *) malloc (size * 2 * 3);
  if (!gamma)
    return SANE_STATUS_NO_MEM;

  /* take care off generic/specific data */
  if (generic)
    {
      /* fill with default values */
      for (i = 0; i < size; i++)
	{
	  gmmval = i * 256;
	  gamma[i * 2 + size * 0 + 0] = gmmval & 0xff;
	  gamma[i * 2 + size * 0 + 1] = (gmmval >> 8) & 0xff;
	  gamma[i * 2 + size * 2 + 0] = gmmval & 0xff;
	  gamma[i * 2 + size * 2 + 1] = (gmmval >> 8) & 0xff;
	  gamma[i * 2 + size * 4 + 0] = gmmval & 0xff;
	  gamma[i * 2 + size * 4 + 1] = (gmmval >> 8) & 0xff;
	}
    }
  else
    {
      /* copy sensor specific's gamma tables */
      for (i = 0; i < size; i++)
	{
	  gamma[i * 2 + size * 0 + 0] = dev->sensor.red_gamma_table[i] & 0xff;
	  gamma[i * 2 + size * 0 + 1] =
	    (dev->sensor.red_gamma_table[i] >> 8) & 0xff;
	  gamma[i * 2 + size * 2 + 0] =
	    dev->sensor.green_gamma_table[i] & 0xff;
	  gamma[i * 2 + size * 2 + 1] =
	    (dev->sensor.green_gamma_table[i] >> 8) & 0xff;
	  gamma[i * 2 + size * 4 + 0] =
	    dev->sensor.blue_gamma_table[i] & 0xff;
	  gamma[i * 2 + size * 4 + 1] =
	    (dev->sensor.blue_gamma_table[i] >> 8) & 0xff;
	}
    }

  /* loop sending gamma tables NOTE: 0x01000000 not 0x10000000 */
  for (i = 0; i < 3; i++)
    {
      /* clear corresponding GMM_N bit */
      RIE (sanei_genesys_read_register (dev, 0xbd, &val));
      /* val &= ~(0x01 << i); */
      val = 0x00;
      RIE (sanei_genesys_write_register (dev, 0xbd, val));

      /* clear corresponding GMM_F bit */
      RIE (sanei_genesys_read_register (dev, 0xbe, &val));
      /* val &= ~(0x01 << i); */
      val = 0x07;
      RIE (sanei_genesys_write_register (dev, 0xbe, val));

      /* set GMM_Z */
      RIE (sanei_genesys_write_register (dev, 0xc5+2*i, 0x00));
      RIE (sanei_genesys_write_register (dev, 0xc6+2*i, 0x00));

      status =
	sanei_genesys_write_ahb (dev->dn, 0x01000000 + 0x200 * i, size * 2,
		   gamma + i * size * 2);
      if (status != SANE_STATUS_GOOD)
	{
	  DBG (DBG_error,
	       "gl124_send_gamma_table: write to AHB failed writing table %d (%s)\n",
	       i, sane_strstatus (status));
	}
    }

  free (gamma);
  DBGCOMPLETED;
  return status;
}

/* this function does the led calibration by scanning one line of the calibration
   area below scanner's top on white strip.

-needs working coarse/gain
*/
static SANE_Status
gl124_led_calibration (Genesys_Device * dev)
{
  int num_pixels;
  int total_size;
  int used_res;
  uint8_t *line;
  int i, j;
  SANE_Status status = SANE_STATUS_GOOD;
  int val;
  int channels, depth;
  int avg[3], top[3], bottom[3];
  int turn;
  char fn[20];
  uint16_t exp[3],target;
  Sensor_Profile *sensor;
  SANE_Bool acceptable;

  DBGSTART;

  /* offset calibration is always done in color mode */
  channels = 3;
  depth=16;
  used_res=sanei_genesys_compute_dpihw(dev,dev->settings.xres);
  sensor=get_sensor_profile(dev->model->ccd_type, used_res);
  num_pixels = (dev->sensor.sensor_pixels*used_res)/dev->sensor.optical_res;
  
  /* initial calibration reg values */
  memcpy (dev->calib_reg, dev->reg, GENESYS_GL124_MAX_REGS * sizeof (Genesys_Register_Set));

  /* set up for the calibration scan */
  status = gl124_init_scan_regs (dev,
				 dev->calib_reg,
				 used_res,
				 used_res,
				 0,
				 0,
				 num_pixels,
                                 1,
                                 depth,
                                 channels,
				 dev->settings.color_filter,
				 SCAN_FLAG_DISABLE_SHADING |
				 SCAN_FLAG_DISABLE_GAMMA |
				 SCAN_FLAG_SINGLE_LINE |
				 SCAN_FLAG_IGNORE_LINE_DISTANCE);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "%s: failed to setup scan: %s\n", __FUNCTION__, sane_strstatus (status));
      return status;
    }

  total_size = num_pixels * channels * (depth/8) * 1;	/* colors * bytes_per_color * scan lines */
  line = malloc (total_size);
  if (!line)
    return SANE_STATUS_NO_MEM;

  /* initial loop values and boundaries */
  exp[0]=sensor->expr;
  exp[1]=sensor->expg;
  exp[2]=sensor->expb;
  target=dev->sensor.gain_white_ref*256;

  for(i=0;i<3;i++)
    {
      bottom[i]=exp[i]/2;
      top[i]=exp[i]*1.5;
    }

  turn = 0;

  /* no move during led calibration */
  gl124_set_motor_power (dev->calib_reg, SANE_FALSE);
  do
    {
      /* set up exposure */
      sanei_genesys_set_triple(dev->calib_reg,REG_EXPR,exp[0]);
      sanei_genesys_set_triple(dev->calib_reg,REG_EXPG,exp[1]);
      sanei_genesys_set_triple(dev->calib_reg,REG_EXPB,exp[2]);

      /* write registers and scan data */
      RIE (gl124_bulk_write_register (dev, dev->calib_reg, GENESYS_GL124_MAX_REGS));

      DBG (DBG_info, "gl124_led_calibration: starting line reading\n");
      RIE (gl124_begin_scan (dev, dev->calib_reg, SANE_TRUE));
      RIE (sanei_genesys_read_data_from_scanner (dev, line, total_size));

      /* stop scanning */
      RIE (gl124_stop_action (dev));

      if (DBG_LEVEL >= DBG_data)
	{
	  snprintf (fn, 20, "led_%02d.pnm", turn);
	  sanei_genesys_write_pnm_file (fn, line, depth, channels, num_pixels, 1);
	}

      /* compute average */
      for (j = 0; j < channels; j++)
	{
	  avg[j] = 0;
	  for (i = 0; i < num_pixels; i++)
	    {
	      if (dev->model->is_cis)
		val =
		  line[i * 2 + j * 2 * num_pixels + 1] * 256 +
		  line[i * 2 + j * 2 * num_pixels];
	      else
		val =
		  line[i * 2 * channels + 2 * j + 1] * 256 +
		  line[i * 2 * channels + 2 * j];
	      avg[j] += val;
	    }

	  avg[j] /= num_pixels;
	}

      DBG (DBG_info, "gl124_led_calibration: average: %d,%d,%d\n", avg[0], avg[1], avg[2]);

      /* check if exposure gives average within the boundaries */
      acceptable = SANE_TRUE;
      for(i=0;i<3;i++)
        {
          /* we accept +- 2% delta from target */
          if(abs(avg[i]-target)>target/50)
            {
              exp[i]=(exp[i]*target)/avg[i];
              acceptable = SANE_FALSE;
            }
        }

      turn++;
    }
  while (!acceptable && turn < 100);

  DBG (DBG_info, "gl124_led_calibration: acceptable exposure: %d,%d,%d\n", exp[0], exp[1], exp[2]);

  /* set these values as final ones for scan */
  sanei_genesys_set_triple(dev->reg,REG_EXPR,exp[0]);
  sanei_genesys_set_triple(dev->reg,REG_EXPG,exp[1]);
  sanei_genesys_set_triple(dev->reg,REG_EXPB,exp[2]);

  /* store in this struct since it is the one used by cache calibration */
  dev->sensor.regs_0x10_0x1d[0] = (exp[0] >> 8) & 0xff;
  dev->sensor.regs_0x10_0x1d[1] = exp[0] & 0xff;
  dev->sensor.regs_0x10_0x1d[2] = (exp[1] >> 8) & 0xff;
  dev->sensor.regs_0x10_0x1d[3] = exp[1] & 0xff;
  dev->sensor.regs_0x10_0x1d[4] = (exp[2] >> 8) & 0xff;
  dev->sensor.regs_0x10_0x1d[5] = exp[2] & 0xff;

  /* cleanup before return */
  free (line);
 
  DBGCOMPLETED;
  return status;
}

/**
 * average dark pixels of a 8 bits scan
 */
static int
dark_average (uint8_t * data, unsigned int pixels, unsigned int lines,
	      unsigned int channels, unsigned int black)
{
  unsigned int i, j, k, average, count;
  unsigned int avg[3];
  uint8_t val;

  /* computes average value on black margin */
  for (k = 0; k < channels; k++)
    {
      avg[k] = 0;
      count = 0;
      for (i = 0; i < lines; i++)
	{
	  for (j = 0; j < black; j++)
	    {
	      val = data[i * channels * pixels + j + k];
	      avg[k] += val;
	      count++;
	    }
	}
      if (count)
	avg[k] /= count;
      DBG (DBG_info, "dark_average: avg[%d] = %d\n", k, avg[k]);
    }
  average = 0;
  for (i = 0; i < channels; i++)
    average += avg[i];
  average /= channels;
  DBG (DBG_info, "dark_average: average = %d\n", average);
  return average;
}


static SANE_Status
gl124_offset_calibration (Genesys_Device * dev)
{
  SANE_Status status = SANE_STATUS_GOOD;
  uint8_t *first_line, *second_line, reg0a;
  unsigned int channels, bpp;
  char title[32];
  int pass = 0, avg, total_size;
  int topavg, bottomavg, resolution, lines;
  int top, bottom, black_pixels, pixels;

  DBGSTART;

  /* no gain nor offset for TI AFE */
  RIE (sanei_genesys_read_register (dev, REG0A, &reg0a));
  if(((reg0a & REG0A_SIFSEL)>>REG0AS_SIFSEL)==3)
    {
      DBGCOMPLETED;
      return status;
    }

  /* offset calibration is always done in color mode */
  channels = 3;
  resolution=dev->sensor.optical_res;
  dev->calib_pixels = dev->sensor.sensor_pixels;
  lines=1;
  bpp=8;
  pixels= (dev->sensor.sensor_pixels*resolution) / dev->sensor.optical_res;
  black_pixels = (dev->sensor.black_pixels * resolution) / dev->sensor.optical_res;
  DBG (DBG_io2, "gl124_offset_calibration: black_pixels=%d\n", black_pixels);

  status = gl124_init_scan_regs (dev,
				 dev->calib_reg,
				 resolution,
				 resolution,
				 0,
				 0,
				 pixels,
				 lines,
				 bpp,
				 channels,
				 dev->settings.color_filter,
				 SCAN_FLAG_DISABLE_SHADING |
				 SCAN_FLAG_DISABLE_GAMMA |
				 SCAN_FLAG_SINGLE_LINE |
				 SCAN_FLAG_IGNORE_LINE_DISTANCE);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_offset_calibration: failed to setup scan: %s\n",
	   sane_strstatus (status));
      return status;
    }
  gl124_set_motor_power (dev->calib_reg, SANE_FALSE);

  /* allocate memory for scans */
  total_size = pixels * channels * lines * (bpp/8);	/* colors * bytes_per_color * scan lines */

  first_line = malloc (total_size);
  if (!first_line)
    return SANE_STATUS_NO_MEM;

  second_line = malloc (total_size);
  if (!second_line)
    {
      free (first_line);
      return SANE_STATUS_NO_MEM;
    }

  /* init gain */
  dev->frontend.gain[0] = 0;
  dev->frontend.gain[1] = 0;
  dev->frontend.gain[2] = 0;

  /* scan with no move */
  bottom = 10;
  dev->frontend.offset[0] = bottom;
  dev->frontend.offset[1] = bottom;
  dev->frontend.offset[2] = bottom;

  RIE (gl124_set_fe(dev, AFE_SET));
  RIE (gl124_bulk_write_register (dev, dev->calib_reg, GENESYS_GL124_MAX_REGS));
  DBG (DBG_info, "gl124_offset_calibration: starting first line reading\n");
  RIE (gl124_begin_scan (dev, dev->calib_reg, SANE_TRUE));
  RIE (sanei_genesys_read_data_from_scanner (dev, first_line, total_size));
  if (DBG_LEVEL >= DBG_data)
   {
      snprintf(title,20,"offset%03d.pnm",bottom);
      sanei_genesys_write_pnm_file (title, first_line, bpp, channels, pixels, lines);
   }
     
  bottomavg = dark_average (first_line, pixels, lines, channels, black_pixels);
  DBG (DBG_io2, "gl124_offset_calibration: bottom avg=%d\n", bottomavg);

  /* now top value */
  top = 255;
  dev->frontend.offset[0] = top;
  dev->frontend.offset[1] = top;
  dev->frontend.offset[2] = top;
  RIE (gl124_set_fe(dev, AFE_SET));
  RIE (gl124_bulk_write_register (dev, dev->calib_reg, GENESYS_GL124_MAX_REGS));
  DBG (DBG_info, "gl124_offset_calibration: starting second line reading\n");
  RIE (gl124_begin_scan (dev, dev->calib_reg, SANE_TRUE));
  RIE (sanei_genesys_read_data_from_scanner (dev, second_line, total_size));
      
  topavg = dark_average (second_line, pixels, lines, channels, black_pixels);
  DBG (DBG_io2, "gl124_offset_calibration: top avg=%d\n", topavg);

  /* loop until acceptable level */
  while ((pass < 32) && (top - bottom > 1))
    {
      pass++;

      /* settings for new scan */
      dev->frontend.offset[0] = (top + bottom) / 2;
      dev->frontend.offset[1] = (top + bottom) / 2;
      dev->frontend.offset[2] = (top + bottom) / 2;

      /* scan with no move */
      RIE(gl124_set_fe(dev, AFE_SET));
      RIE (gl124_bulk_write_register (dev, dev->calib_reg, GENESYS_GL124_MAX_REGS));
      DBG (DBG_info, "gl124_offset_calibration: starting second line reading\n");
      RIE (gl124_begin_scan (dev, dev->calib_reg, SANE_TRUE));
      RIE (sanei_genesys_read_data_from_scanner (dev, second_line, total_size));

      if (DBG_LEVEL >= DBG_data)
	{
	  sprintf (title, "offset%03d.pnm", dev->frontend.offset[1]);
	  sanei_genesys_write_pnm_file (title, second_line, bpp, channels, pixels, lines);
	}

      avg = dark_average (second_line, pixels, lines, channels, black_pixels);
      DBG (DBG_info, "gl124_offset_calibration: avg=%d offset=%d\n", avg,
	   dev->frontend.offset[1]);

      /* compute new boundaries */
      if (topavg == avg)
	{
	  topavg = avg;
	  top = dev->frontend.offset[1];
	}
      else
	{
	  bottomavg = avg;
	  bottom = dev->frontend.offset[1];
	}
    }
  DBG (DBG_info, "gl124_offset_calibration: offset=(%d,%d,%d)\n", dev->frontend.offset[0], dev->frontend.offset[1], dev->frontend.offset[2]);

  /* cleanup before return */
  free (first_line);
  free (second_line);

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}


/* alternative coarse gain calibration 
   this on uses the settings from offset_calibration and
   uses only one scanline
 */
/*
  with offset and coarse calibration we only want to get our input range into
  a reasonable shape. the fine calibration of the upper and lower bounds will 
  be done with shading.
 */
static SANE_Status
gl124_coarse_gain_calibration (Genesys_Device * dev, int dpi)
{
  int pixels;
  int total_size;
  uint8_t *line, reg0a;
  int i, j, channels;
  SANE_Status status = SANE_STATUS_GOOD;
  int max[3];
  float gain[3],coeff;
  int val, code, lines;
  int resolution;
  int bpp;

  DBG (DBG_proc, "gl124_coarse_gain_calibration: dpi = %d\n", dpi);

  /* no gain nor offset for TI AFE */
  RIE (sanei_genesys_read_register (dev, REG0A, &reg0a));
  if(((reg0a & REG0A_SIFSEL)>>REG0AS_SIFSEL)==3)
    {
      DBGCOMPLETED;
      return status;
    }

  /* coarse gain calibration is always done in color mode */
  channels = 3;

  /* follow CKSEL */
  if(dev->settings.xres<dev->sensor.optical_res)
    {
      coeff=0.9;
      resolution=dev->sensor.optical_res/2;
      resolution=dev->sensor.optical_res;
    }
  else
    {
      resolution=dev->sensor.optical_res;
      coeff=1.0;
    }
  lines=10;
  bpp=8;
  pixels = (dev->sensor.sensor_pixels * resolution) / dev->sensor.optical_res;

  status = gl124_init_scan_regs (dev,
				 dev->calib_reg,
				 resolution,
				 resolution,
				 0,
				 0,
				 pixels,
                                 lines,
                                 bpp,
                                 channels,
				 dev->settings.color_filter,
				 SCAN_FLAG_DISABLE_SHADING |
				 SCAN_FLAG_DISABLE_GAMMA |
				 SCAN_FLAG_SINGLE_LINE |
				 SCAN_FLAG_IGNORE_LINE_DISTANCE);
  gl124_set_motor_power (dev->calib_reg, SANE_FALSE);

  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_coarse_calibration: failed to setup scan: %s\n",
	   sane_strstatus (status));
      return status;
    }

  RIE (gl124_bulk_write_register
       (dev, dev->calib_reg, GENESYS_GL124_MAX_REGS));

  total_size = pixels * channels * (16/bpp) * lines;

  line = malloc (total_size);
  if (!line)
    return SANE_STATUS_NO_MEM;

  RIE (gl124_set_fe(dev, AFE_SET));
  RIE (gl124_begin_scan (dev, dev->calib_reg, SANE_TRUE));
  RIE (sanei_genesys_read_data_from_scanner (dev, line, total_size));

  if (DBG_LEVEL >= DBG_data)
    sanei_genesys_write_pnm_file ("coarse.pnm", line, bpp, channels, pixels, lines);

  /* average value on each channel */
  for (j = 0; j < channels; j++)
    {
      max[j] = 0;
      for (i = pixels/4; i < (pixels*3/4); i++)
	{
          if(bpp==16)
            {
	  if (dev->model->is_cis)
	    val =
	      line[i * 2 + j * 2 * pixels + 1] * 256 +
	      line[i * 2 + j * 2 * pixels];
	  else
	    val =
	      line[i * 2 * channels + 2 * j + 1] * 256 +
	      line[i * 2 * channels + 2 * j];
            }
          else
            {
	  if (dev->model->is_cis)
	    val = line[i + j * pixels];
	  else
	    val = line[i * channels + j];
            }

	    max[j] += val;
	}
      max[j] = max[j] / (pixels/2);

      gain[j] = ((float) dev->sensor.gain_white_ref*coeff) / max[j];

      /* turn logical gain value into gain code, checking for overflow */
      code = 283 - 208 / gain[j];
      if (code > 255)
	code = 255;
      else if (code < 0)
	code = 0;
      dev->frontend.gain[j] = code;

      DBG (DBG_proc,
	   "gl124_coarse_gain_calibration: channel %d, max=%d, gain = %f, setting:%d\n",
	   j, max[j], gain[j], dev->frontend.gain[j]);
    }

  if (dev->model->is_cis)
    {
      if (dev->frontend.gain[0] > dev->frontend.gain[1])
	dev->frontend.gain[0] = dev->frontend.gain[1];
      if (dev->frontend.gain[0] > dev->frontend.gain[2])
	dev->frontend.gain[0] = dev->frontend.gain[2];
      dev->frontend.gain[2] = dev->frontend.gain[1] = dev->frontend.gain[0];
    }

  if (channels == 1)
    {
      dev->frontend.gain[0] = dev->frontend.gain[1];
      dev->frontend.gain[2] = dev->frontend.gain[1];
    }

  free (line);

  RIE (gl124_stop_action (dev));

  gl124_slow_back_home (dev, SANE_TRUE);

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

/*
 * wait for lamp warmup by scanning the same line until difference
 * between 2 scans is below a threshold
 */
static SANE_Status
gl124_init_regs_for_warmup (Genesys_Device * dev,
			    Genesys_Register_Set * reg,
			    int *channels, int *total_size)
{
  int num_pixels;
  SANE_Status status = SANE_STATUS_GOOD;

  DBGSTART;
  if (dev == NULL || reg == NULL || channels == NULL || total_size == NULL)
    return SANE_STATUS_INVAL;
  
  *channels=3;

  memcpy (reg, dev->reg, (GENESYS_GL124_MAX_REGS + 1) * sizeof (Genesys_Register_Set));
  status = gl124_init_scan_regs (dev,
				 reg,
				 dev->sensor.optical_res,
				 dev->motor.base_ydpi,
				 dev->sensor.sensor_pixels/4,
				 0,
				 dev->sensor.sensor_pixels/2,
				 1,
				 8,
				 *channels,
				 dev->settings.color_filter,
				 SCAN_FLAG_DISABLE_SHADING |
				 SCAN_FLAG_DISABLE_GAMMA |
				 SCAN_FLAG_SINGLE_LINE |
				 SCAN_FLAG_IGNORE_LINE_DISTANCE);

  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_init_regs_for_warmup: failed to setup scan: %s\n",
	   sane_strstatus (status));
      return status;
    }

  num_pixels = dev->current_setup.pixels;

  *total_size = num_pixels * 3 * 1;	/* colors * bytes_per_color * scan lines */

  gl124_set_motor_power (reg, SANE_FALSE);
  RIE (gl124_bulk_write_register (dev, reg, GENESYS_GL124_MAX_REGS));

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

/** 
 * set up GPIO/GPOE for idle state
WRITE GPIO[17-21]= GPIO19
WRITE GPOE[17-21]= GPOE21 GPOE20 GPOE19 GPOE18
genesys_write_register(0xa8,0x3e)
GPIO(0xa8)=0x3e
 */
static SANE_Status
gl124_init_gpio (Genesys_Device * dev)
{
  SANE_Status status = SANE_STATUS_GOOD;
  int idx;

  DBGSTART;

  /* per model GPIO layout */
  if (strcmp (dev->model->name, "canon-lide-110") == 0)
    {
      idx = 0;
    }
  else
    {				/* canon LiDE 210 case */
      idx = 1;
    }

  RIE (sanei_genesys_write_register (dev, REG31, gpios[idx].r31));
  RIE (sanei_genesys_write_register (dev, REG32, gpios[idx].r32));
  RIE (sanei_genesys_write_register (dev, REG33, gpios[idx].r33));
  RIE (sanei_genesys_write_register (dev, REG34, gpios[idx].r34));
  RIE (sanei_genesys_write_register (dev, REG35, gpios[idx].r35));
  RIE (sanei_genesys_write_register (dev, REG36, gpios[idx].r36));
  RIE (sanei_genesys_write_register (dev, REG38, gpios[idx].r38));

  DBGCOMPLETED;
  return status;
}

/** 
 * set memory layout by filling values in dedicated registers
 */
static SANE_Status
gl124_init_memory_layout (Genesys_Device * dev)
{
  SANE_Status status = SANE_STATUS_GOOD;
  int idx = 0;

  DBGSTART;

  /* point to per model memory layout */
  if (strcmp (dev->model->name, "canon-lide-110") == 0)
    {
      idx = 0;
    }
  else
    {				/* canon LiDE 210 case */
      idx = 1;
    }

  /* setup base address for shading data. */
  /* values must be multiplied by 8192=0x4000 to give address on AHB */
  /* R-Channel shading bank0 address setting for CIS */
  sanei_genesys_write_register (dev, 0xd0, layouts[idx].rd0);
  /* G-Channel shading bank0 address setting for CIS */
  sanei_genesys_write_register (dev, 0xd1, layouts[idx].rd1);
  /* B-Channel shading bank0 address setting for CIS */
  sanei_genesys_write_register (dev, 0xd2, layouts[idx].rd2);

  /* setup base address for scanned data. */
  /* values must be multiplied by 1024*2=0x0800 to give address on AHB */
  /* R-Channel ODD image buffer 0x0124->0x92000 */
  /* size for each buffer is 0x16d*1k word */
  sanei_genesys_write_register (dev, 0xe0, layouts[idx].re0);
  sanei_genesys_write_register (dev, 0xe1, layouts[idx].re1);
/* R-Channel ODD image buffer end-address 0x0291->0x148800 => size=0xB6800*/
  sanei_genesys_write_register (dev, 0xe2, layouts[idx].re2);
  sanei_genesys_write_register (dev, 0xe3, layouts[idx].re3);

  /* R-Channel EVEN image buffer 0x0292 */
  sanei_genesys_write_register (dev, 0xe4, layouts[idx].re4);
  sanei_genesys_write_register (dev, 0xe5, layouts[idx].re5);
/* R-Channel EVEN image buffer end-address 0x03ff*/
  sanei_genesys_write_register (dev, 0xe6, layouts[idx].re6);
  sanei_genesys_write_register (dev, 0xe7, layouts[idx].re7);

/* same for green, since CIS, same addresses */
  sanei_genesys_write_register (dev, 0xe8, layouts[idx].re0);
  sanei_genesys_write_register (dev, 0xe9, layouts[idx].re1);
  sanei_genesys_write_register (dev, 0xea, layouts[idx].re2);
  sanei_genesys_write_register (dev, 0xeb, layouts[idx].re3);
  sanei_genesys_write_register (dev, 0xec, layouts[idx].re4);
  sanei_genesys_write_register (dev, 0xed, layouts[idx].re5);
  sanei_genesys_write_register (dev, 0xee, layouts[idx].re6);
  sanei_genesys_write_register (dev, 0xef, layouts[idx].re7);

/* same for blue, since CIS, same addresses */
  sanei_genesys_write_register (dev, 0xf0, layouts[idx].re0);
  sanei_genesys_write_register (dev, 0xf1, layouts[idx].re1);
  sanei_genesys_write_register (dev, 0xf2, layouts[idx].re2);
  sanei_genesys_write_register (dev, 0xf3, layouts[idx].re3);
  sanei_genesys_write_register (dev, 0xf4, layouts[idx].re4);
  sanei_genesys_write_register (dev, 0xf5, layouts[idx].re5);
  sanei_genesys_write_register (dev, 0xf6, layouts[idx].re6);
  sanei_genesys_write_register (dev, 0xf7, layouts[idx].re7);

  DBGCOMPLETED;
  return status;
}



/* *
 * initialize ASIC from power on condition
 */
static SANE_Status
gl124_cold_boot (Genesys_Device * dev)
{
  SANE_Status status;
  uint8_t val;

  DBGSTART;

  RIE (sanei_genesys_write_register (dev, 0x0e, 0x01));
  RIE (sanei_genesys_write_register (dev, 0x0e, 0x00));

  /* enable GPOE 17 */
  RIE (sanei_genesys_write_register (dev, 0x36, 0x01));

  /* set GPIO 17 */
  RIE (sanei_genesys_read_register (dev, 0x33, &val));
  val |= 0x01;
  RIE (sanei_genesys_write_register (dev, 0x33, val));

  /* test CHKVER */
  RIE (sanei_genesys_read_hregister (dev, REG100, &val));
  if (val & REG100_CHKVER)
    {
      RIE (sanei_genesys_read_register (dev, 0x00, &val));
      DBG (DBG_info,
	   "gl124_cold_boot: reported version for genesys chip is 0x%02x\n",
	   val);
    }

  /* Set default values for registers */
  gl124_init_registers (dev);

  /* Write initial registers */
  RIE (gl124_bulk_write_register (dev, dev->reg, GENESYS_GL124_MAX_REGS));

  /* tune reg 0B */
  val = REG0B_30MHZ | REG0B_ENBDRAM | REG0B_64M;
  RIE (sanei_genesys_write_register (dev, REG0B, val));
  dev->reg[reg_0x0b].address = 0x00;

  /* set up end access */
  RIE (sanei_genesys_write_0x8c (dev, 0x10, 0x0b));
  RIE (sanei_genesys_write_0x8c (dev, 0x13, 0x0e));

  /* CIS_LINE */
  SETREG (0x08, REG08_CIS_LINE);
  RIE (sanei_genesys_write_register (dev, 0x08, dev->reg[reg_0x08].value));

  /* setup gpio */
  RIE (gl124_init_gpio (dev));

  /* setup internal memory layout */
  RIE (gl124_init_memory_layout (dev));

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}

/* *
 * initialize backend and ASIC : registers, motor tables, and gamma tables
 * then ensure scanner's head is at home
 */
#ifndef UNIT_TESTING
static
#endif
SANE_Status
gl124_init (Genesys_Device * dev)
{
  SANE_Status status;
  uint8_t val;
  SANE_Bool cold = SANE_TRUE;
  int size;

  DBG_INIT ();
  DBGSTART;

  /* URB    16  control  0xc0 0x0c 0x8e 0x0b len     1 read  0x00 */
  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_IN, REQUEST_REGISTER,
			   VALUE_GET_REGISTER, 0x00, 1, &val);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "gl124_init: request register failed %s\n",
	   sane_strstatus (status));
      return status;
    }
  DBG (DBG_io2, "gl124_init: value=0x%02x\n", val);
  DBG (DBG_info, "%s: device is %s\n", __FUNCTION__,
       (val & 0x08) ? "USB 1.0" : "USB2.0");
  if (val & 0x08)
    {
      dev->usb_mode = 1;
    }
  else
    {
      dev->usb_mode = 2;
    }

  /* check if the device has already been initialized and powered up 
   * we read register 6 and check PWRBIT, if reset scanner has been
   * freshly powered up. This bit will be set to later so that following
   * reads can detect power down/up cycle*/
  RIE (sanei_genesys_read_register (dev, 0x06, &val));
  if (val & REG06_PWRBIT)
    {
      cold = SANE_FALSE;
    }
  DBG (DBG_info, "%s: device is %s\n", __FUNCTION__, cold ? "cold" : "warm");

  /* don't do anything if backend is initialized and hardware hasn't been
   * replug */
  if (dev->already_initialized && !cold)
    {
      DBG (DBG_info, "gl124_init: already initialized, nothing to do\n");
      return SANE_STATUS_GOOD;
    }

  /* set up hardware and registers */
  RIE (gl124_cold_boot (dev));

  /* now hardware part is OK, set up device struct */
  FREE_IFNOT_NULL (dev->white_average_data);
  FREE_IFNOT_NULL (dev->dark_average_data);
  FREE_IFNOT_NULL (dev->sensor.red_gamma_table);
  FREE_IFNOT_NULL (dev->sensor.green_gamma_table);
  FREE_IFNOT_NULL (dev->sensor.blue_gamma_table);

  dev->settings.color_filter = 0;

  memcpy (dev->calib_reg, dev->reg,
	  GENESYS_GL124_MAX_REGS * sizeof (Genesys_Register_Set));

  /* Set analog frontend */
  RIE (gl124_set_fe (dev, AFE_INIT));

  /* init gamma tables */
  size = 256;
  if (dev->sensor.red_gamma_table == NULL)
    {
      dev->sensor.red_gamma_table = (uint16_t *) malloc (2 * size);
      if (dev->sensor.red_gamma_table == NULL)
	{
	  DBG (DBG_error,
	       "gl124_init: could not allocate memory for gamma table\n");
	  return SANE_STATUS_NO_MEM;
	}
      sanei_genesys_create_gamma_table (dev->sensor.red_gamma_table, size,
					65535, 65535, dev->sensor.red_gamma);
    }
  if (dev->sensor.green_gamma_table == NULL)
    {
      dev->sensor.green_gamma_table = (uint16_t *) malloc (2 * size);
      if (dev->sensor.red_gamma_table == NULL)
	{
	  DBG (DBG_error,
	       "gl124_init: could not allocate memory for gamma table\n");
	  return SANE_STATUS_NO_MEM;
	}
      sanei_genesys_create_gamma_table (dev->sensor.green_gamma_table, size,
					65535, 65535,
					dev->sensor.green_gamma);
    }
  if (dev->sensor.blue_gamma_table == NULL)
    {
      dev->sensor.blue_gamma_table = (uint16_t *) malloc (2 * size);
      if (dev->sensor.red_gamma_table == NULL)
	{
	  DBG (DBG_error,
	       "gl124_init: could not allocate memory for gamma table\n");
	  return SANE_STATUS_NO_MEM;
	}
      sanei_genesys_create_gamma_table (dev->sensor.blue_gamma_table, size,
					65535, 65535, dev->sensor.blue_gamma);
    }

  dev->oe_buffer.buffer = NULL;
  dev->already_initialized = SANE_TRUE;

  /* Move home if needed */
  RIE (gl124_slow_back_home (dev, SANE_TRUE));
  dev->scanhead_position_in_steps = 0;

  /* Set powersaving (default = 15 minutes) */
  RIE (gl124_set_powersaving (dev, 15));

  DBGCOMPLETED;
  return status;
}

static SANE_Status
gl124_update_hardware_sensors (Genesys_Scanner * s)
{
  /* do what is needed to get a new set of events, but try to not loose
     any of them.
   */
  SANE_Status status = SANE_STATUS_GOOD;
  uint8_t val=0;

  RIE (sanei_genesys_read_register (s->dev, REG31, &val));

  /* TODO : for the next scanner special case,
   * add another per scanner button profile struct to avoid growing
   * hard-coded button mapping here. 
   */
  if(s->dev->model->gpo_type == GPO_CANONLIDE110)
    {
      if (s->val[OPT_SCAN_SW].b == s->last_val[OPT_SCAN_SW].b)
        s->val[OPT_SCAN_SW].b = (val & 0x01) == 0;
      if (s->val[OPT_FILE_SW].b == s->last_val[OPT_FILE_SW].b)
        s->val[OPT_FILE_SW].b = (val & 0x08) == 0;
      if (s->val[OPT_EMAIL_SW].b == s->last_val[OPT_EMAIL_SW].b)
        s->val[OPT_EMAIL_SW].b = (val & 0x04) == 0;
      if (s->val[OPT_COPY_SW].b == s->last_val[OPT_COPY_SW].b)
        s->val[OPT_COPY_SW].b = (val & 0x02) == 0;
    }
  else
    { /* LiDE 210 case */
      if (s->val[OPT_SCAN_SW].b == s->last_val[OPT_SCAN_SW].b)
        s->val[OPT_SCAN_SW].b = (val & 0x02) == 0;
      if (s->val[OPT_COPY_SW].b == s->last_val[OPT_COPY_SW].b)
        s->val[OPT_COPY_SW].b = (val & 0x04) == 0;
      if (s->val[OPT_EMAIL_SW].b == s->last_val[OPT_EMAIL_SW].b)
        s->val[OPT_EMAIL_SW].b = (val & 0x08) == 0;
      if (s->val[OPT_FILE_SW].b == s->last_val[OPT_FILE_SW].b)
        s->val[OPT_FILE_SW].b = (val & 0x10) == 0;
    }
  return status;
}


/** the gl124 command set */
static Genesys_Command_Set gl124_cmd_set = {
  "gl124-generic",		/* the name of this set */

  gl124_init,
  gl124_init_regs_for_warmup,
  gl124_init_regs_for_coarse_calibration,
  gl124_init_regs_for_shading,
  gl124_init_regs_for_scan,

  gl124_get_filter_bit,
  gl124_get_lineart_bit,
  gl124_get_bitset_bit,
  gl124_get_gain4_bit,
  gl124_get_fast_feed_bit,
  gl124_test_buffer_empty_bit,
  gl124_test_motor_flag_bit,

  gl124_bulk_full_size,

  gl124_set_fe,
  gl124_set_powersaving,
  gl124_save_power,

  gl124_set_motor_power,
  gl124_set_lamp_power,

  gl124_begin_scan,
  gl124_end_scan,

  gl124_send_gamma_table,

  gl124_search_start_position,

  gl124_offset_calibration,
  gl124_coarse_gain_calibration,
  gl124_led_calibration,

  gl124_slow_back_home,

  gl124_bulk_write_register,
  NULL,
  gl124_bulk_read_data,

  gl124_update_hardware_sensors,

  /* no sheetfed support for now */
  NULL,
  NULL,
  NULL,
  NULL,

  sanei_genesys_is_compatible_calibration,
  NULL,
  gl124_send_shading_data,
  gl124_calculate_current_setup
};

SANE_Status
sanei_gl124_init_cmd_set (Genesys_Device * dev)
{
  dev->model->cmd_set = &gl124_cmd_set;
  return SANE_STATUS_GOOD;
}

/* vim: set sw=2 cino=>2se-1sn-1s{s^-1st0(0u0 smarttab expandtab: */
