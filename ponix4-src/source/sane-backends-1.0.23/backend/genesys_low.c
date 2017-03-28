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
#define BACKEND_NAME genesys_low

#include "genesys_low.h"

/* ------------------------------------------------------------------------ */
/*                  functions calling ASIC specific functions               */
/* ------------------------------------------------------------------------ */

/**
 * setup the hardware dependent functions
 */
SANE_Status
sanei_genesys_init_cmd_set (Genesys_Device * dev)
{
  DBG_INIT ();
  switch (dev->model->asic_type)
    {
    case GENESYS_GL646:
      return sanei_gl646_init_cmd_set (dev);
    case GENESYS_GL841:
      return sanei_gl841_init_cmd_set (dev);
    case GENESYS_GL843:
      return sanei_gl843_init_cmd_set (dev);
    case GENESYS_GL847:
      return sanei_gl847_init_cmd_set (dev);
    case GENESYS_GL124:
      return sanei_gl124_init_cmd_set (dev);
    default:
      return SANE_STATUS_INVAL;
    }
}

/* ------------------------------------------------------------------------ */
/*                  General IO and debugging functions                      */
/* ------------------------------------------------------------------------ */

/* Write data to a pnm file (e.g. calibration). For debugging only */
/* data is RGB or grey, with little endian byte order */
SANE_Status
sanei_genesys_write_pnm_file (char *filename, uint8_t * data, int depth,
			      int channels, int pixels_per_line, int lines)
{
  FILE *out;
  int count;

  DBG (DBG_info,
       "sanei_genesys_write_pnm_file: depth=%d, channels=%d, ppl=%d, lines=%d\n",
       depth, channels, pixels_per_line, lines);

  out = fopen (filename, "w");
  if (!out)
    {
      DBG (DBG_error,
	   "sanei_genesys_write_pnm_file: could nor open %s for writing: %s\n",
	   filename, strerror (errno));
      return SANE_STATUS_INVAL;
    }
  if(depth==1)
    {
      fprintf (out, "P4\n%d\n%d\n", pixels_per_line, lines);
    }
  else
    {
      fprintf (out, "P%c\n%d\n%d\n%d\n", channels == 1 ? '5' : '6',
	   pixels_per_line, lines, (int) pow (2, depth) - 1);
    }
  if (channels == 3)
    {
      for (count = 0; count < (pixels_per_line * lines * 3); count++)
	{
	  if (depth == 16)
	    fputc (*(data + 1), out);
	  fputc (*(data++), out);
	  if (depth == 16)
	    data++;
	}
    }
  else
    {
      if (depth==1)
        {
          pixels_per_line/=8;
        }
      for (count = 0; count < (pixels_per_line * lines); count++)
	{
          switch (depth)
            {
              case 8:
	        fputc (*(data + count), out);
                break;
              case 16:
	        fputc (*(data + 1), out);
	        fputc (*(data), out);
	        data += 2;
                break;
              default:
                fputc(data[count], out);
                break;
            }
	}
    }
  fclose (out);

  DBG (DBG_proc, "sanei_genesys_write_pnm_file: finished\n");
  return SANE_STATUS_GOOD;
}

/* the following 2 functions are used to handle registers in a
   way that doesn't depend on the actual ASIC type */

/* Reads a register from a register set */
SANE_Byte
sanei_genesys_read_reg_from_set (Genesys_Register_Set * reg,
				 SANE_Byte address)
{
  SANE_Int i;

  for (i = 0; i < GENESYS_MAX_REGS && reg[i].address; i++)
    {
      if (reg[i].address == address)
	{
	  return reg[i].value;
	}
    }
  return 0;
}

/* Reads a register from a register set */
void
sanei_genesys_set_reg_from_set (Genesys_Register_Set * reg, SANE_Byte address,
				SANE_Byte value)
{
  SANE_Int i;

  for (i = 0; i < GENESYS_MAX_REGS && reg[i].address; i++)
    {
      if (reg[i].address == address)
	{
	  reg[i].value = value;
	  break;
	}
    }
}


/* ------------------------------------------------------------------------ */
/*                  Read and write RAM, registers and AFE                   */
/* ------------------------------------------------------------------------ */

/** @brief write to one high (addr >= 0x100) register
 * write to a register which address is higher than 0xff.
 * @param dev opened device to write to
 * @param reg LSB of register address
 * @param val value to write
 */
SANE_Status
sanei_genesys_write_hregister (Genesys_Device * dev, uint8_t reg, uint8_t val)
{
  SANE_Status status;
  uint8_t buffer[2];

  buffer[0]=reg;
  buffer[1]=val;
  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_OUT, REQUEST_BUFFER,
			   0x100 | VALUE_SET_REGISTER, INDEX, 2, buffer);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "sanei_genesys_write_hregister (0x%02x, 0x%02x): failed : %s\n", reg, val, sane_strstatus (status));
      return status;
    }

  DBG (DBG_io, "sanei_genesys_write_hregister (0x%02x, 0x%02x) completed\n",
       reg, val);

  return status;
}

/** @brief read from one high (addr >= 0x100) register
 * Read to a register which address is higher than 0xff. Second byte is check to detect
 * physical link errors.
 * @param dev opened device to read from
 * @param reg LSB of register address
 * @param val value to write
 */
SANE_Status
sanei_genesys_read_hregister (Genesys_Device * dev, uint8_t reg, uint8_t * val)
{
  SANE_Status status;
  SANE_Byte value[2];

  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_IN, REQUEST_BUFFER,
			   0x100 | VALUE_GET_REGISTER, 0x22+(reg<<8), 2, value);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_read_hregister (0x%02x): failed while reading register: %s\n",
	   reg, sane_strstatus (status));
      return status;
    }
  *val=value[0];
  DBG( DBG_io2, "sanei_genesys_read_hregister(0x%02x)=0x%02x\n",reg,*val);

  /* check usb link status */
  if((value[1] & 0xff) != 0x55)
    {
      DBG (DBG_error,"sanei_genesys_read_hregister: invalid read, scanner unplugged ?\n");
      status=SANE_STATUS_IO_ERROR;
    }
  return status;
}

/**
 * Write to one GL847 ASIC register
URB    10  control  0x40 0x04 0x83 0x00 len     2 wrote 0xa6 0x04 
 */
static SANE_Status
sanei_genesys_write_gl847_register (Genesys_Device * dev, uint8_t reg, uint8_t val)
{
  SANE_Status status;
  uint8_t buffer[2];

  buffer[0]=reg;
  buffer[1]=val;
  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_OUT, REQUEST_BUFFER,
			   VALUE_SET_REGISTER, INDEX, 2, buffer);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "sanei_genesys_write_gl847_register (0x%02x, 0x%02x): failed : %s\n", reg, val, sane_strstatus (status));
      return status;
    }

  DBG (DBG_io, "sanei_genesys_write_gl847_register (0x%02x, 0x%02x) completed\n",
       reg, val);

  return status;
}

/**
 * Write to one ASIC register
 */
SANE_Status
sanei_genesys_write_register (Genesys_Device * dev, uint8_t reg, uint8_t val)
{
  SANE_Status status;

  /* route to gl847 function if needed */
  if(dev->model->asic_type==GENESYS_GL847 || dev->model->asic_type==GENESYS_GL124)
    return sanei_genesys_write_gl847_register(dev, reg, val);

  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_OUT, REQUEST_REGISTER,
			   VALUE_SET_REGISTER, INDEX, 1, &reg);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_write_register (0x%02x, 0x%02x): failed while setting register: %s\n",
	   reg, val, sane_strstatus (status));
      return status;
    }

  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_OUT, REQUEST_REGISTER,
			   VALUE_WRITE_REGISTER, INDEX, 1, &val);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_write_register (0x%02x, 0x%02x): failed while writing register value: %s\n",
	   reg, val, sane_strstatus (status));
      return status;
    }

  DBG (DBG_io, "sanei_genesys_write_register (0x%02x, 0x%02x) completed\n",
       reg, val);

  return status;
}

/** 
 * write command to 0x8c endpoint
 * @param dev device to write to
 * @param index index of the command
 * @param val value to write
 */
SANE_Status
sanei_genesys_write_0x8c (Genesys_Device * dev, uint8_t index, uint8_t val)
{
  SANE_Status status;

  DBG (DBG_io, "sanei_genesys_write_0x8c: 0x%02x,0x%02x\n", index, val);

  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_OUT, REQUEST_REGISTER,
			   VALUE_BUF_ENDACCESS, index, 1, &val);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_write_0x8c: failed %s\n", sane_strstatus (status));
    }
  return status;
}

/* read reg 0x41:
 * URB   164  control  0xc0 0x04 0x8e 0x4122 len     2 read  0xfc 0x55
 */
static SANE_Status
sanei_genesys_read_gl847_register (Genesys_Device * dev, uint8_t reg, uint8_t * val)
{
  SANE_Status status;
  SANE_Byte value[2];

  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_IN, REQUEST_BUFFER,
			   VALUE_GET_REGISTER, 0x22+(reg<<8), 2, value);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_read_gl847_register (0x%02x): failed while setting register: %s\n",
	   reg, sane_strstatus (status));
      return status;
    }
  *val=value[0];
  DBG( DBG_io2, "sanei_genesys_read_gl847_register(0x%02x)=0x%02x\n",reg,*val);

  /* check usb link status */
  if((value[1] & 0xff) != 0x55)
    {
      DBG (DBG_error,"sanei_genesys_read_gl847_register: invalid read, scanner unplugged ?\n");
      status=SANE_STATUS_IO_ERROR;
    }
  return status;
}

/* Read from one register */
SANE_Status
sanei_genesys_read_register (Genesys_Device * dev, uint8_t reg, uint8_t * val)
{
  SANE_Status status;

  /* route to gl847 function if needed */
  if(dev->model->asic_type==GENESYS_GL847 || dev->model->asic_type==GENESYS_GL124)
    return sanei_genesys_read_gl847_register(dev, reg, val);

  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_OUT, REQUEST_REGISTER,
			   VALUE_SET_REGISTER, INDEX, 1, &reg);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_read_register (0x%02x, 0x%02x): failed while setting register: %s\n",
	   reg, *val, sane_strstatus (status));
      return status;
    }

  *val = 0;

  status =
    sanei_usb_control_msg (dev->dn, REQUEST_TYPE_IN, REQUEST_REGISTER,
			   VALUE_READ_REGISTER, INDEX, 1, val);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_read_register (0x%02x, 0x%02x): failed while reading register value: %s\n",
	   reg, *val, sane_strstatus (status));
      return status;
    }

  DBG (DBG_io, "sanei_genesys_read_register (0x%02x, 0x%02x) completed\n",
       reg, *val);

  return status;
}

/* Set address for writing data */
SANE_Status
sanei_genesys_set_buffer_address (Genesys_Device * dev, uint32_t addr)
{
  SANE_Status status;
  
  if(dev->model->asic_type==GENESYS_GL847 || dev->model->asic_type==GENESYS_GL124)
    {
      DBG (DBG_warn,
	   "sanei_genesys_set_buffer_address: shouldn't be used for GL847 or GL124\n");
      return SANE_STATUS_GOOD;
    }

  DBG (DBG_io,
       "sanei_genesys_set_buffer_address: setting address to 0x%05x\n",
       addr & 0xfffffff0);

  addr = addr >> 4;

  status = sanei_genesys_write_register (dev, 0x2b, (addr & 0xff));
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_set_buffer_address: failed while writing low byte: %s\n",
	   sane_strstatus (status));
      return status;
    }

  addr = addr >> 8;
  status = sanei_genesys_write_register (dev, 0x2a, (addr & 0xff));
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_set_buffer_address: failed while writing high byte: %s\n",
	   sane_strstatus (status));
      return status;
    }

  DBG (DBG_io, "sanei_genesys_set_buffer_address: completed\n");

  return status;
}

/** read data for analog frontend
 * @param dev device owning the AFE
 * @param addr register address to read
 * @param data placeholder for the result
 * @return SANE_STATUS_GOOD is OK, else the error code
 */
SANE_Status
sanei_genesys_fe_read_data (Genesys_Device * dev, uint8_t addr,
			     uint16_t *data)
{
  SANE_Status status;
  uint8_t value;
  Genesys_Register_Set reg[1];


  DBG (DBG_proc, "sanei_genesys_fe_read_data: start\n");

  reg[0].address = 0x50;
  reg[0].value = addr;

  /* set up read address */
  status = dev->model->cmd_set->bulk_write_register (dev, reg, 1);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_fe_read_data: failed while bulk writing registers: %s\n",
	   sane_strstatus (status));
      return status;
    }

  /* read data */
  RIE (sanei_genesys_read_register (dev, 0x46, &value));
  *data=256*value;
  RIE (sanei_genesys_read_register (dev, 0x47, &value));
  *data+=value;

  DBG (DBG_io, "sanei_genesys_fe_read_data (0x%02x, 0x%04x)\n", addr, *data);
  DBG (DBG_proc, "sanei_genesys_fe_read_data: completed\n");

  return status;
}

/* Write data for analog frontend */
SANE_Status
sanei_genesys_fe_write_data (Genesys_Device * dev, uint8_t addr,
			     uint16_t data)
{
  SANE_Status status;
  Genesys_Register_Set reg[3];

  DBG (DBG_io, "sanei_genesys_fe_write_data (0x%02x, 0x%04x)\n", addr, data);

  reg[0].address = 0x51;
  reg[0].value = addr;
  reg[1].address = 0x3a;
  reg[1].value = (data / 256) & 0xff;
  reg[2].address = 0x3b;
  reg[2].value = data & 0xff;
  if (dev->model->asic_type == GENESYS_GL124)
    {
     reg[1].address = 0x5d;
     reg[2].address = 0x5e;
    }

  status = dev->model->cmd_set->bulk_write_register (dev, reg, 3);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_fe_write_data: failed while bulk writing registers: %s\n",
	   sane_strstatus (status));
      return status;
    }

  DBG (DBG_io, "sanei_genesys_fe_write_data: completed\n");

  return status;
}

/* ------------------------------------------------------------------------ */
/*                       Medium level functions                             */
/* ------------------------------------------------------------------------ */

/** read the status register
 */
SANE_Status
sanei_genesys_get_status (Genesys_Device * dev, uint8_t * status)
{
  if(dev->model->asic_type==GENESYS_GL124)
    return sanei_genesys_read_hregister(dev, 0x01, status);
  return sanei_genesys_read_register (dev, 0x41, status);
}

/**
 * decodes and prints content of status register
 * @param val value read from status register
 */
void sanei_genesys_print_status (uint8_t val)
{
  char msg[80];

  sprintf (msg, "%s%s%s%s%s%s%s%s",
	   val & PWRBIT ? "PWRBIT " : "",
	   val & BUFEMPTY ? "BUFEMPTY " : "",
	   val & FEEDFSH ? "FEEDFSH " : "",
	   val & SCANFSH ? "SCANFSH " : "",
	   val & HOMESNR ? "HOMESNR " : "",
	   val & LAMPSTS ? "LAMPSTS " : "",
	   val & FEBUSY ? "FEBUSY " : "",
	   val & MOTORENB ? "MOTORENB" : "");
  DBG (DBG_info, "status=%s\n", msg);
}

#if 0
/* returns pixels per line from register set */
/*candidate for moving into chip specific files?*/
static int
genesys_pixels_per_line (Genesys_Register_Set * reg)
{
  int pixels_per_line;

  pixels_per_line =
    sanei_genesys_read_reg_from_set (reg,
				     0x32) * 256 +
    sanei_genesys_read_reg_from_set (reg, 0x33);
  pixels_per_line -=
    (sanei_genesys_read_reg_from_set (reg, 0x30) * 256 +
     sanei_genesys_read_reg_from_set (reg, 0x31));

  return pixels_per_line;
}

/* returns dpiset from register set */
/*candidate for moving into chip specific files?*/
static int
genesys_dpiset (Genesys_Register_Set * reg)
{
  int dpiset;

  dpiset =
    sanei_genesys_read_reg_from_set (reg,
				     0x2c) * 256 +
    sanei_genesys_read_reg_from_set (reg, 0x2d);

  return dpiset;
}
#endif

/** read the number of valid words in scanner's RAM
 * ie registers 42-43-44
 */
/*candidate for moving into chip specific files?*/
SANE_Status
sanei_genesys_read_valid_words (Genesys_Device * dev, unsigned int *words)
{
  SANE_Status status;
  uint8_t value;

  DBG (DBG_proc, "sanei_genesys_read_valid_words\n");
  if (dev->model->asic_type == GENESYS_GL124)
    {
      RIE (sanei_genesys_read_hregister (dev, 0x02, &value));
      *words=(value & 0x03);
      RIE (sanei_genesys_read_hregister (dev, 0x03, &value));
      *words=*words*256+value;
      RIE (sanei_genesys_read_hregister (dev, 0x04, &value));
      *words=*words*256+value;
      RIE (sanei_genesys_read_hregister (dev, 0x05, &value));
      *words=*words*256+value;
    }
  else if (dev->model->asic_type == GENESYS_GL847)
    {
      RIE (sanei_genesys_read_register (dev, 0x42, &value));
      *words=(value & 0x03);
      RIE (sanei_genesys_read_register (dev, 0x43, &value));
      *words=*words*256+value;
      RIE (sanei_genesys_read_register (dev, 0x44, &value));
      *words=*words*256+value;
      RIE (sanei_genesys_read_register (dev, 0x45, &value));
      *words=*words*256+value;
    }
  else
    {
      RIE (sanei_genesys_read_register (dev, 0x44, &value));
      *words = value;
      RIE (sanei_genesys_read_register (dev, 0x43, &value));
      *words += (value * 256);
      RIE (sanei_genesys_read_register (dev, 0x42, &value));
      if (dev->model->asic_type == GENESYS_GL646)
        *words += ((value & 0x03) * 256 * 256);
      else
        *words += ((value & 0x0f) * 256 * 256);
    }

  DBG (DBG_proc, "sanei_genesys_read_valid_words: %d words\n", *words);
  return SANE_STATUS_GOOD;
}

/** read the number of lines scanned
 * ie registers 4b-4c-4d
 */
SANE_Status
sanei_genesys_read_scancnt (Genesys_Device * dev, unsigned int *words)
{
  SANE_Status status;
  uint8_t value;

  DBG (DBG_proc, "sanei_genesys_read_scancnt: start\n");

  if (dev->model->asic_type == GENESYS_GL124)
    {
      RIE (sanei_genesys_read_hregister (dev, 0x0b, &value));
      *words = (value & 0x0f) << 16;
      RIE (sanei_genesys_read_hregister (dev, 0x0c, &value));
      *words += (value << 8);
      RIE (sanei_genesys_read_hregister (dev, 0x0d, &value));
      *words += value;
    }
  else
    {
      RIE (sanei_genesys_read_register (dev, 0x4d, &value));
      *words = value;
      RIE (sanei_genesys_read_register (dev, 0x4c, &value));
      *words += (value * 256);
      RIE (sanei_genesys_read_register (dev, 0x4b, &value));
      if (dev->model->asic_type == GENESYS_GL646)
        *words += ((value & 0x03) * 256 * 256);
      else
        *words += ((value & 0x0f) * 256 * 256);
    }

  DBG (DBG_proc, "sanei_genesys_read_scancnt: %d lines\n", *words);
  return SANE_STATUS_GOOD;
}

/**
 * Find register in set
 * @param regs register set to search
 * @param addr addres of the searched register
 * @return a Genesys_Register_Set pointer corresponding to the required
 * address in ASIC space. Or NULL if not found.
 */
Genesys_Register_Set *
sanei_genesys_get_address (Genesys_Register_Set * regs, SANE_Byte addr)
{
  int i;
  for (i = 0; i < GENESYS_MAX_REGS; i++)
    {
      if (regs[i].address == addr)
	return &regs[i];
    }
  DBG (DBG_error, "sanei_genesys_get_address: failed to find address for register 0x%02x, crash expected !\n",addr);
  return NULL;
}

/**
 * set a 16 bit value in the given register set.
 * @param regs register set where to set values
 * @param addr address of the first register index to set
 * @param value value to set
 * @return SANE_STATUS_INVAL if the index doesn't exist in register set
 */
SANE_Status
sanei_genesys_set_double(Genesys_Register_Set *regs, SANE_Byte addr, uint16_t value)
{
  Genesys_Register_Set *r;

  /* high byte */
  r = sanei_genesys_get_address (regs, addr);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  r->value = HIBYTE (value);

  /* low byte */
  r = sanei_genesys_get_address (regs, addr+1);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  r->value = LOBYTE (value);

  return SANE_STATUS_GOOD;
}

/**
 * set a 24 bit value in the given register set.
 * @param regs register set where to set values
 * @param addr address of the first register index to set
 * @param value value to set
 * @return SANE_STATUS_INVAL if the index doesn't exist in register set
 */
SANE_Status
sanei_genesys_set_triple(Genesys_Register_Set *regs, SANE_Byte addr, uint32_t value)
{
  Genesys_Register_Set *r;

  /* low byte of highword */
  r = sanei_genesys_get_address (regs, addr);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  r->value = LOBYTE (HIWORD(value));

  /* high byte of low word */
  r = sanei_genesys_get_address (regs, addr+1);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  r->value = HIBYTE (LOWORD(value));

  /* low byte of low word */
  r = sanei_genesys_get_address (regs, addr+2);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  r->value = LOBYTE (LOWORD(value));

  return SANE_STATUS_GOOD;
}

/**
 * get a 16 bit value in the given register set.
 * @param regs register set where to read values
 * @param addr address of the first register index to read
 * @param value value to set
 * @return SANE_STATUS_INVAL if the index doesn't exist in register set
 */
SANE_Status
sanei_genesys_get_double(Genesys_Register_Set *regs, SANE_Byte addr, uint16_t *value)
{
  Genesys_Register_Set *r;
  uint16_t result=0;

  /* high byte */
  r = sanei_genesys_get_address (regs, addr);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  result=r->value<<8;

  /* low byte */
  r = sanei_genesys_get_address (regs, addr+1);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  result+=r->value;

  *value=result;
  return SANE_STATUS_GOOD;
}

/**
 * get a 24 bit value in the given register set.
 * @param regs register set where to read values
 * @param addr address of the first register index to read
 * @param value value to set
 * @return SANE_STATUS_INVAL if the index doesn't exist in register set
 */
SANE_Status
sanei_genesys_get_triple(Genesys_Register_Set *regs, SANE_Byte addr, uint32_t *value)
{
  Genesys_Register_Set *r;
  uint32_t result=0;

  /* low byte of highword */
  r = sanei_genesys_get_address (regs, addr);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  result=r->value<<16;

  /* high byte of low word */
  r = sanei_genesys_get_address (regs, addr+1);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  result+=(r->value<<8);

  /* low byte of low word */
  r = sanei_genesys_get_address (regs, addr+2);
  if(r==NULL)
    {
      return SANE_STATUS_INVAL;
    }
  result+=r->value;

  *value=result;
  return SANE_STATUS_GOOD;
}

/* Checks if the scan buffer is empty */
SANE_Status
sanei_genesys_test_buffer_empty (Genesys_Device * dev, SANE_Bool * empty)
{
  uint8_t val = 0;
  SANE_Status status;

  status = sanei_genesys_get_status (dev, &val);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_test_buffer_empty: failed to read buffer status: %s\n",
	   sane_strstatus (status));
      return status;
    }

  if (dev->model->cmd_set->test_buffer_empty_bit (val))
    {
      DBG (DBG_io2, "sanei_genesys_test_buffer_empty: buffer is empty\n");
      *empty = SANE_TRUE;
      return SANE_STATUS_GOOD;
    }

  *empty = SANE_FALSE;

  DBG (DBG_io, "sanei_genesys_test_buffer_empty: buffer is filled\n");
  return SANE_STATUS_GOOD;
}


/* Read data (e.g scanned image) from scan buffer */
SANE_Status
sanei_genesys_read_data_from_scanner (Genesys_Device * dev, uint8_t * data,
				      size_t size)
{
  SANE_Status status;
  int time_count = 0;
  unsigned int words = 0;

  DBG (DBG_proc, "sanei_genesys_read_data_from_scanner (size = %lu bytes)\n",
       (u_long) size);

  if (size & 1)
    DBG (DBG_info,
	 "WARNING sanei_genesys_read_data_from_scanner: odd number of bytes\n");

  /* wait until buffer not empty for up to 5 seconds */
  do
    {
      status = sanei_genesys_read_valid_words (dev, &words);
      if (status != SANE_STATUS_GOOD)
	{
	  DBG (DBG_error,
	       "sanei_genesys_read_data_from_scanner: checking for empty buffer failed: %s\n",
	       sane_strstatus (status));
	  return status;
	}
      if (words == 0)
	{
	  usleep (10000);	/* wait 10 msec */
	  time_count++;
	}
    }
  while ((time_count < 2500*2) && (words == 0));

  if (words == 0)		/* timeout, buffer does not get filled */
    {
      DBG (DBG_error,
	   "sanei_genesys_read_data_from_scanner: timeout, buffer does not get filled\n");
      return SANE_STATUS_IO_ERROR;
    }

  status = dev->model->cmd_set->bulk_read_data (dev, 0x45, data, size);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_read_data_from_scanner: reading bulk data failed: %s\n",
	   sane_strstatus (status));
      return status;
    }

  DBG (DBG_proc, "sanei_genesys_read_data_from_scanner: completed\n");
  return SANE_STATUS_GOOD;
}
SANE_Status
sanei_genesys_read_feed_steps (Genesys_Device * dev, unsigned int *steps)
{
  SANE_Status status;
  uint8_t value;

  DBG (DBG_proc, "sanei_genesys_read_feed_steps\n");

  if (dev->model->asic_type == GENESYS_GL124)
    {
      RIE (sanei_genesys_read_hregister (dev, 0x08, &value));
      *steps = (value & 0x1f) << 16;
      RIE (sanei_genesys_read_hregister (dev, 0x09, &value));
      *steps += (value << 8);
      RIE (sanei_genesys_read_hregister (dev, 0x0a, &value));
      *steps += value;
    }
  else
    {
      RIE (sanei_genesys_read_register (dev, 0x4a, &value));
      *steps = value;
      RIE (sanei_genesys_read_register (dev, 0x49, &value));
      *steps += (value * 256);
      RIE (sanei_genesys_read_register (dev, 0x48, &value));
      if (dev->model->asic_type == GENESYS_GL646)
        *steps += ((value & 0x03) * 256 * 256);
      else if (dev->model->asic_type == GENESYS_GL841)
        *steps += ((value & 0x0f) * 256 * 256);
      else
        *steps += ((value & 0x1f) * 256 * 256);
    }

  DBG (DBG_proc, "sanei_genesys_read_feed_steps: %d steps\n", *steps);
  return SANE_STATUS_GOOD;
}


/**
 * writes a block of data to AHB
 * @param dn USB device index
 * @param addr AHB address to write to
 * @param size size of the chunk of data
 * @param data pointer to the data to write
 */
SANE_Status
sanei_genesys_write_ahb (SANE_Int dn, uint32_t addr, uint32_t size, uint8_t * data)
{
  uint8_t outdata[8];
  size_t written,blksize;
  SANE_Status status = SANE_STATUS_GOOD;
  int i;
  char msg[60]="AHB=";

  outdata[0] = addr & 0xff;
  outdata[1] = ((addr >> 8) & 0xff);
  outdata[2] = ((addr >> 16) & 0xff);
  outdata[3] = ((addr >> 24) & 0xff);
  outdata[4] = (size & 0xff);
  outdata[5] = ((size >> 8) & 0xff);
  outdata[6] = ((size >> 16) & 0xff);
  outdata[7] = ((size >> 24) & 0xff);

  if (DBG_LEVEL >= DBG_io)
    {
      for (i = 0; i < 8; i++)
	{
          sprintf (msg, "%s 0x%02x", msg, outdata[i]);
	}
      DBG (DBG_io, "%s: write(0x%08x,0x%08x)\n", __FUNCTION__, addr,size);
      DBG (DBG_io, "%s: %s\n", __FUNCTION__, msg);
    }

  /* write addr and size for AHB */
  status =
    sanei_usb_control_msg (dn, REQUEST_TYPE_OUT, REQUEST_BUFFER, VALUE_BUFFER,
			   0x01, 8, outdata);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error, "sanei_genesys_write_ahb: failed while setting addr and size: %s\n",
	   sane_strstatus (status));
      return status;
    }

  /* write actual data */
  written = 0;
  do
    {
      if (size - written > BULKOUT_MAXSIZE)
        {
          blksize = BULKOUT_MAXSIZE;
        }
      else
        {
          blksize = size - written;
        }
      status = sanei_usb_write_bulk (dn, data + written, &blksize);
      if (status != SANE_STATUS_GOOD)
        {
          DBG (DBG_error,
               "sanei_genesys_write_ahb: failed while writing bulk data: %s\n",
               sane_strstatus (status));
          return status;
        }
      written += blksize;
    }
  while (written < size);

  return status;
}

/**
 * Wait for the scanning head to park 
 */
SANE_Status
sanei_genesys_wait_for_home (Genesys_Device * dev)
{
  SANE_Status status;
  uint8_t val;
  int loop;

  DBGSTART;

  /* clear the parking status whatever the outcome of the function */
  dev->parking=SANE_FALSE;

  /* read initial status, if head isn't at home and motor is on
   * we are parking, so we wait.
   * gl847/gl124 need 2 reads for reliable results */
  status = sanei_genesys_get_status (dev, &val);
  usleep (10000);
  status = sanei_genesys_get_status (dev, &val);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "%s: failed to read home sensor: %s\n", __FUNCTION__,
	   sane_strstatus (status));
      return status;
    }

  /* if at home, return */
  if(val & HOMESNR)
    {
	  DBG (DBG_info,
	       "%s: already at home\n", __FUNCTION__);
	  return status;
    }

  /* loop for 30 s max, polling home sensor */
  loop = 0;
  do
    {
      /* wait 100 ms */
      usleep (100000);
      status = sanei_genesys_get_status (dev, &val);
      if (status != SANE_STATUS_GOOD)
	{
	  DBG (DBG_error,
	       "%s: failed to read home sensor: %s\n", __FUNCTION__,
	       sane_strstatus (status));
	  return status;
	}
          if (DBG_LEVEL >= DBG_io2)
            {
              sanei_genesys_print_status (val);
            }
      ++loop;
    }
  while (loop < 300 && !(val & HOMESNR) && status == SANE_STATUS_GOOD);

  DBGCOMPLETED;
  return status;
}

/**@brief compute hardware sensor dpi to use
 * compute the sensor hardware dpi based on target resolution.
 * A lower dpihw enable faster scans.
 * @param dev device used for the scan
 * @param xres x resolution of the scan
 * @return the hardware dpi to use
 */
int sanei_genesys_compute_dpihw(Genesys_Device *dev, int xres)
{
  /* can't be below 600 dpi */
  if (xres <= 600)
    {
      return 600;
    }
  if (xres <= dev->sensor.optical_res / 4)
    {
      return dev->sensor.optical_res / 4;
    }
  if (xres <= dev->sensor.optical_res / 2)
    {
      return dev->sensor.optical_res / 2;
    }
  return dev->sensor.optical_res;
}

/** @get motor profile
 * search for the database of motor profiles and get the best one. Each
 * profile is at full step and at a reference exposure. Use first entry
 * by default.
 * @param motors motor profile database
 * @param motor_type motor id
 * @param exposure exposure time
 * @return a pointer to a Motor_Profile struct
 */
Motor_Profile *sanei_genesys_get_motor_profile(Motor_Profile *motors, int motor_type, int exposure)
{
  unsigned int i;
  int idx;

  i=0;
  idx=-1;
  while(motors[i].exposure!=0)
    {
      /* exact match */
      if(motors[i].motor_type==motor_type && motors[i].exposure==exposure)
        {
          return &(motors[i]);
        }

      /* closest match */
      if(motors[i].motor_type==motor_type)
        {
          /* if profile exposure is higher than the required one,
           * the entry is a candidate for the closest match */
          if(motors[i].exposure>=exposure)
            {
              if(idx<0)
                {
                  /* no match found yet */
                  idx=i;
                }
              else
                {
                  /* test for better match */
                  if(motors[i].exposure<motors[idx].exposure)
                    {
                      idx=i;
                    }
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

/**@brief compute motor step type to use
 * compute the step type (full, half, quarter, ...) to use based
 * on target resolution
 * @param motors motor profile database
 * @param motor_type motor id
 * @param exposure sensor exposure
 * @return 0 for full step
 *         1 for half step
 *         2 for quarter step
 *         3 for eighth step
 */
int sanei_genesys_compute_step_type(Motor_Profile *motors,
                                    int motor_type,
                                    int exposure)
{
Motor_Profile *profile;

    profile=sanei_genesys_get_motor_profile(motors, motor_type, exposure);
    return profile->step_type;
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
 * @param motors motor profile database
 */
int sanei_genesys_slope_table(uint16_t *slope,
		             int       *steps,
			     int       dpi,
			     int       exposure,
			     int       base_dpi,
			     int       step_type,
			     int       factor,
                             int       motor_type,
                             Motor_Profile *motors)
{
int sum, i;
uint16_t target,current;
Motor_Profile *profile;

	/* required speed */
	target=((exposure * dpi) / base_dpi)>>step_type;
        DBG (DBG_io2, "%s: target=%d\n", __FUNCTION__, target);
	
	/* fill result with target speed */
        for(i=0;i<SLOPE_TABLE_SIZE;i++)
          slope[i]=target;

        profile=sanei_genesys_get_motor_profile(motors, motor_type, exposure);

	/* use profile to build table */
        i=0;
	sum=0;

        /* first step is used unmodified */
        current=profile->table[0];

        /* loop on profile copying and apply step type */
        while(profile->table[i]!=0 && current>=target)
          {
            slope[i]=current;
            sum+=slope[i];
            i++;
            current=profile->table[i]>>step_type;
          }

        /* range checking */
        if(profile->table[i]==0 && DBG_LEVEL >= DBG_warn && current>target)
          {
            DBG (DBG_warn,"%s: short slope table, failed to reach %d. target too low ?\n",__FUNCTION__,target);
          }
        if(i<3 && DBG_LEVEL >= DBG_warn)
          {
            DBG (DBG_warn,"%s: short slope table, failed to reach %d. target too high ?\n",__FUNCTION__,target);
          }

        /* align on factor */
        while(i%factor!=0)
          {
            slope[i+1]=slope[i];
            sum+=slope[i];
            i++;
          }

        /* ensure minimal slope size */
        while(i<2*factor)
          {
            slope[i+1]=slope[i];
            sum+=slope[i];
            i++;
          }

        /* return used steps and acceleration sum */
        *steps=i/factor;
	return sum;
}

/** @brief returns the lowest possible ydpi for the device
 * Parses device entry to find lowest motor dpi.
 * @param dev device description
 * @return lowest motor resolution
 */
int sanei_genesys_get_lowest_ydpi(Genesys_Device *dev)
{
  int min=20000;
  int i=0;

  while(dev->model->ydpi_values[i]!=0)
    {
      if(dev->model->ydpi_values[i]<min)
        {
          min=dev->model->ydpi_values[i];
        }
      i++;
    }
  return min;
}

/** @brief returns the lowest possible dpi for the device
 * Parses device entry to find lowest motor or sensor dpi.
 * @param dev device description
 * @return lowest motor resolution
 */
int sanei_genesys_get_lowest_dpi(Genesys_Device *dev)
{
  int min=20000;
  int i=0;

  while(dev->model->ydpi_values[i]!=0)
    {
      if(dev->model->ydpi_values[i]<min)
        {
          min=dev->model->ydpi_values[i];
        }
      i++;
    }
  i=0;
  while(dev->model->xdpi_values[i]!=0)
    {
      if(dev->model->xdpi_values[i]<min)
        {
          min=dev->model->xdpi_values[i];
        }
      i++;
    }
  return min;
}

/** @brief check is a cache entry may be used
 * Compares current settings with the cache entry and return
 * SANE_TRUE if they are compatible.
 */
SANE_Status
sanei_genesys_is_compatible_calibration (Genesys_Device * dev,
				 Genesys_Calibration_Cache * cache,
				 int for_overwrite)
{
#ifdef HAVE_SYS_TIME_H
  struct timeval time;
#endif
  int compatible = 1, resolution;
  SANE_Status status;

  DBGSTART;
 
  if(dev->model->cmd_set->calculate_current_setup==NULL)
    {
      DBG (DBG_proc,
	   "sanei_genesys_is_compatible_calibration: no calculate_setup, non compatible cache\n");
      return SANE_STATUS_UNSUPPORTED;
    }

  status = dev->model->cmd_set->calculate_current_setup (dev);
  if (status != SANE_STATUS_GOOD)
    {
      DBG (DBG_error,
	   "sanei_genesys_is_compatible_calibration: failed to calculate current setup: %s\n",
	   sane_strstatus (status));
      return status;
    }
  dev->current_setup.scan_method = dev->settings.scan_method;

  DBG (DBG_proc, "sanei_genesys_is_compatible_calibration: checking\n");
  
  /* a calibration cache is compatible if color mode and x dpi match the user 
   * requested scan. In the case of CIS scanners, dpi isn't a criteria */
  if (dev->model->is_cis == SANE_FALSE)
    {
      resolution = dev->settings.xres;
      if(resolution>dev->sensor.optical_res)
        {
          resolution=dev->sensor.optical_res;
        }
      compatible = (resolution == ((int) cache->used_setup.xres));
    }
  else
    {
      resolution=sanei_genesys_compute_dpihw(dev,dev->settings.xres);
      compatible = (resolution == ((int) sanei_genesys_compute_dpihw(dev,cache->used_setup.xres)));
    }
  if (dev->current_setup.scan_method != cache->used_setup.scan_method)
    {
      DBG (DBG_io,
	   "sanei_genesys_is_compatible_calibration: current method=%d, used=%d\n",
	   dev->current_setup.scan_method, cache->used_setup.scan_method);
      compatible = 0;
    }
  if (!compatible)
    {
      DBG (DBG_proc,
	   "sanei_genesys_is_compatible_calibration: completed, non compatible cache\n");
      return SANE_STATUS_UNSUPPORTED;
    }

  /* a cache entry expires after 60 minutes for non sheetfed scanners */
  /* this is not taken into account when overwriting cache entries    */
#ifdef HAVE_SYS_TIME_H
  if(for_overwrite == SANE_FALSE)
    {
      gettimeofday (&time, NULL);
      if ((time.tv_sec - cache->last_calibration > 60 * 60)
          && (dev->model->is_sheetfed == SANE_FALSE)
          && (dev->settings.scan_method == SCAN_METHOD_FLATBED))
        {
          DBG (DBG_proc,
               "sanei_genesys_is_compatible_calibration: expired entry, non compatible cache\n");
          return SANE_STATUS_UNSUPPORTED;
        }
    }
#endif

  DBGCOMPLETED;
  return SANE_STATUS_GOOD;
}


/** @brief compute maximum line distance shift
 * compute maximum line distance shift for the motor and sensor
 * combination. Line distance shift is the distance between different
 * color component of CCD sensors. Since these components aren't at
 * the same physical place, they scan diffrent lines. Software must
 * take this into account to accurately mix color data.
 * @param dev device session to compute max_shift for
 * @param channels number of color channels for the scan
 * @param yres motor resolution used for the scan
 * @param flags scan flags
 * @return 0 or line distance shift
 */
int sanei_genesys_compute_max_shift(Genesys_Device *dev,
                                    int channels,
                                    int yres,
                                    int flags)
{
  int max_shift;

  max_shift=0;
  if (channels > 1 && !(flags & SCAN_FLAG_IGNORE_LINE_DISTANCE))
    {
      max_shift = dev->ld_shift_r;
      if (dev->ld_shift_b > max_shift)
	max_shift = dev->ld_shift_b;
      if (dev->ld_shift_g > max_shift)
	max_shift = dev->ld_shift_g;
      max_shift = (max_shift * yres) / dev->motor.base_ydpi;
    }
  return max_shift;
}



/* vim: set sw=2 cino=>2se-1sn-1s{s^-1st0(0u0 smarttab expandtab: */
