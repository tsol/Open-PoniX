/*
 * Copyright (C) 2003-2006 by XGI Technology, Taiwan.
 * (c) Copyright IBM Corporation 2008
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation on the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL XGI AND/OR
 * ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file
 * I2C bus interface support routines.
 *
 * \note
 * These routines currently only support I2C for the CRT port.
 *
 * \author Ian Romanick <idr@us.ibm.com>
 * \author Jennifer Weng
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"
#include "xf86_OSproc.h"

#include "xgi.h"
#include "xgi_regs.h"

#include "xg47_i2c.h"


struct xg47_i2c_private {
    XGIPtr pXGI;
    unsigned port;
};


#define XGIPTR_FROM_I2CBUS(b) \
    ((struct xg47_i2c_private *) b->DriverPrivate.ptr)->pXGI

#define IOPORT_FROM_I2CBUS(b) \
    ((struct xg47_i2c_private *) b->DriverPrivate.ptr)->port


/**
 * Write a single value to the I2C bus
 *
 * \param input  Value to write.  Includes clock, data, and mode.
 *
 * \returns
 * Actual bits in I2C I/O port
 */
static uint8_t
i2c_Write(I2CBusPtr b, uint8_t input)
{
    static const uint8_t mask = ~(I2C_SCL_HIGH | I2C_WRITE_MODE
				  | I2C_SDA_HIGH);
    XGIPtr pXGI = XGIPTR_FROM_I2CBUS(b);
    const unsigned port = IOPORT_FROM_I2CBUS(b);
    uint8_t    output;

    output = (IN3X5B(port) & mask) | input;
    OUT3X5B(port, output);
    b->I2CUDelay(b, b->RiseFallTime);

    return output;
}


/**
 * Read a single value from the I2C bus
 *
 * \returns
 * Actual bits in I2C I/O port.  Includes clock, data, and mode.
 */
static uint8_t
i2c_Read(I2CBusPtr b)
{
    XGIPtr pXGI = XGIPTR_FROM_I2CBUS(b);
    const unsigned port = IOPORT_FROM_I2CBUS(b);

    return IN3X5B(port);
}


/**
 * Send a single byte to the device.
 *
 * \return
 * \c TRUE is always returned because no timeout checking is done.
 */
static Bool
xg47_i2c_PutByte(I2CDevPtr d, I2CByte data)
{
    XGIPtr pXGI = XGIPTR_FROM_I2CBUS(d->pI2CBus);
    uint8_t   output;
    uint8_t   input;
    int  i;

    for (i = 7; i >= 0; i--) {
        output = I2C_WRITE_MODE | ((data >> i) & 0x01);

        output = i2c_Write(d->pI2CBus, output);
        output = i2c_Write(d->pI2CBus, output | I2C_SCL_HIGH);
	(void)   i2c_Write(d->pI2CBus, output & ~I2C_SCL_HIGH);
    }

    i2c_Write(d->pI2CBus, I2C_WRITE_MODE | I2C_SDA_HIGH);
    output = i2c_Write(d->pI2CBus, 
		       I2C_SCL_HIGH | I2C_WRITE_MODE | I2C_SDA_HIGH);
    i2c_Write(d->pI2CBus, output &  ~I2C_WRITE_MODE);


    for (i = 0; i < 10; i++) {
        input = i2c_Read(d->pI2CBus);
        if ((input & 0x01) == 0) {
            break;
        }
    }
    i2c_Write(d->pI2CBus, 
	      (input & ~I2C_SCL_HIGH) | (I2C_WRITE_MODE | I2C_SDA_HIGH));

    return TRUE;
}


/**
 * Read a single byte from the device.
 *
 * \return
 * \c TRUE is always returned because no timeout checking is done.
 */
static Bool
xg47_i2c_GetByte(I2CDevPtr d, I2CByte *data, Bool last)
{
    XGIPtr pXGI = d->pI2CBus->DriverPrivate.ptr;
    uint8_t   output;
    uint8_t   input;
    int  i;

    *data = 0;
    for (i = 7; i >= 0; i--) {
        i2c_Write(d->pI2CBus, I2C_READ_MODE);
        i2c_Write(d->pI2CBus, I2C_SCL_HIGH);

        input = i2c_Read(d->pI2CBus);
        *data = *data | ((input & 0x01) << i);

        i2c_Write(d->pI2CBus, input & ~0x01 & ~I2C_SCL_HIGH);
    }

    /* Send ACK.  If this is the last value to be read, send NAK.
     */
    output = (last) ? (I2C_WRITE_MODE | I2C_SDA_HIGH) : I2C_WRITE_MODE;
    i2c_Write(d->pI2CBus, output);

    output = i2c_Write(d->pI2CBus, output | I2C_SCL_HIGH);
    i2c_Write(d->pI2CBus, output & ~I2C_SCL_HIGH);

    return TRUE;
}


/**
 * Signal devices that a transation has finished
 */
static void
xg47_i2c_Stop(I2CDevPtr d)
{
    i2c_Write(d->pI2CBus, I2C_WRITE_MODE);
    i2c_Write(d->pI2CBus, I2C_SCL_HIGH | I2C_WRITE_MODE);
    i2c_Write(d->pI2CBus, I2C_SCL_HIGH | I2C_WRITE_MODE | I2C_SDA_HIGH);
}


/**
 * Send a start signal on the I2C bus
 */
static Bool
xg47_i2c_Start(I2CBusPtr b, int timeout)
{
    i2c_Write(b, I2C_SCL_HIGH | I2C_WRITE_MODE | I2C_SDA_HIGH);
    i2c_Write(b, I2C_SCL_HIGH | I2C_WRITE_MODE);
    i2c_Write(b, I2C_WRITE_MODE);

    return TRUE;
}


/**
 * Create a start condition and send a slave address
 *
 * \note
 * This function was lifted almost directly from X.org's xf86i2c.c.
 */
static Bool
xg47_i2c_Address(I2CDevPtr d, I2CSlaveAddr addr)
{
    if (xg47_i2c_Start(d->pI2CBus, d->StartTimeout)) {
        if (xg47_i2c_PutByte(d, addr & 0xFF)) {
            if ((addr & 0xF8) != 0xF0 && (addr & 0xFE) != 0x00) {
                return TRUE;
	    }

            if (xg47_i2c_PutByte(d, (addr >> 8) & 0xFF)) {
                return TRUE;
	    }
        }

        xg47_i2c_Stop(d);
    }

    return FALSE;
}


/**
 * Initialize I2C bus structures
 */
Bool xg47_InitI2C(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    struct xg47_i2c_private *i2c_priv;

    pXGI->pI2C = xf86CreateI2CBusRec();
    if (pXGI->pI2C == NULL) {
	goto fail;
    }

    pXGI->pI2C->BusName = "DDC (CRT)";
    pXGI->pI2C->scrnIndex = pScrn->scrnIndex;
    pXGI->pI2C->I2CPutBits = NULL;
    pXGI->pI2C->I2CGetBits = NULL;
    pXGI->pI2C->I2CPutByte = xg47_i2c_PutByte;
    pXGI->pI2C->I2CGetByte = xg47_i2c_GetByte;
    pXGI->pI2C->I2CAddress = xg47_i2c_Address;
    pXGI->pI2C->I2CStart   = xg47_i2c_Start;
    pXGI->pI2C->I2CStop    = xg47_i2c_Stop;
    pXGI->pI2C->AcknTimeout = 5;


    i2c_priv = malloc(sizeof(struct xg47_i2c_private));
    if (i2c_priv == NULL) {
	goto fail;
    }

    i2c_priv->pXGI = pXGI;
    i2c_priv->port = 0x37;
    pXGI->pI2C->DriverPrivate.ptr = i2c_priv;

    if (!xf86I2CBusInit(pXGI->pI2C)) {
	xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		   "xf86I2CBusInit (for CRT) failed.\n");
	goto fail;
    }


    pXGI->pI2C_dvi = xf86CreateI2CBusRec();
    if (pXGI->pI2C_dvi == NULL) {
	goto fail;
    }

    pXGI->pI2C_dvi->BusName = "DDC (DVI)";
    pXGI->pI2C_dvi->scrnIndex = pScrn->scrnIndex;
    pXGI->pI2C_dvi->I2CPutBits = NULL;
    pXGI->pI2C_dvi->I2CGetBits = NULL;
    pXGI->pI2C_dvi->I2CPutByte = xg47_i2c_PutByte;
    pXGI->pI2C_dvi->I2CGetByte = xg47_i2c_GetByte;
    pXGI->pI2C_dvi->I2CAddress = xg47_i2c_Address;
    pXGI->pI2C_dvi->I2CStart   = xg47_i2c_Start;
    pXGI->pI2C_dvi->I2CStop    = xg47_i2c_Stop;
    pXGI->pI2C_dvi->AcknTimeout = 5;


    i2c_priv = malloc(sizeof(struct xg47_i2c_private));
    if (i2c_priv == NULL) {
	goto fail;
    }

    i2c_priv->pXGI = pXGI;
    i2c_priv->port = 0x30;
    pXGI->pI2C_dvi->DriverPrivate.ptr = i2c_priv;

    if (!xf86I2CBusInit(pXGI->pI2C_dvi)) {
	xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		   "xf86I2CBusInit (for DVI) failed.\n");
	goto fail;
    }

    return TRUE; 


fail:
    if (pXGI->pI2C != NULL) {
	xf86DestroyI2CBusRec(pXGI->pI2C, TRUE, TRUE);	
	pXGI->pI2C = NULL;
    }

    if (pXGI->pI2C_dvi != NULL) {
	xf86DestroyI2CBusRec(pXGI->pI2C_dvi, TRUE, TRUE);	
	pXGI->pI2C_dvi = NULL;
    }

    return FALSE;
}
