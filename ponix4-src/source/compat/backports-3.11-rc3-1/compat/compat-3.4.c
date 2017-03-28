/*
 * Copyright 2012  Luis R. Rodriguez <mcgrof@frijolero.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Backport functionality introduced in Linux 3.4.
 */

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/wait.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
#include <linux/regmap.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
#if defined(CPTCFG_VIDEO_DEV_MODULE)
#include <media/soc_camera.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-dev.h>
#include <media/videobuf-core.h>
#include <media/videobuf2-core.h>
#include <media/soc_mediabus.h>
#include <linux/regulator/consumer.h>
#endif /* defined(CPTCFG_VIDEO_DEV_MODULE) */
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)) */
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)) */
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)) */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))

#if defined(CPTCFG_VIDEO_V4L2_MODULE)
#include <media/v4l2-clk.h>
int soc_camera_power_on(struct device *dev,
			struct soc_camera_subdev_desc *ssdd,
			struct v4l2_clk *clk)
{
	int ret = clk ? v4l2_clk_enable(clk) : 0;
	if (ret < 0) {
		dev_err(dev, "Cannot enable clock: %d\n", ret);
		return ret;
	}
	ret = regulator_bulk_enable(ssdd->num_regulators,
					ssdd->regulators);
	if (ret < 0) {
		dev_err(dev, "Cannot enable regulators\n");
		goto eregenable;
	}

	if (ssdd->power) {
		ret = ssdd->power(dev, 1);
		if (ret < 0) {
			dev_err(dev,
				"Platform failed to power-on the camera.\n");
			goto epwron;
		}
	}

	return 0;

epwron:
	regulator_bulk_disable(ssdd->num_regulators,
			       ssdd->regulators);
eregenable:
	if (clk)
		v4l2_clk_disable(clk);

	return ret;
}
EXPORT_SYMBOL_GPL(soc_camera_power_on);

int soc_camera_power_off(struct device *dev,
			 struct soc_camera_subdev_desc *ssdd,
			 struct v4l2_clk *clk)
{
	int ret = 0;
	int err;

	if (ssdd->power) {
		err = ssdd->power(dev, 0);
		if (err < 0) {
			dev_err(dev,
			        "Platform failed to power-off the camera.\n");
			ret = err;
		}
	}

	err = regulator_bulk_disable(ssdd->num_regulators,
				     ssdd->regulators);
	if (err < 0) {
		dev_err(dev, "Cannot disable regulators\n");
		ret = ret ? : err;
	}

	if (clk)
		v4l2_clk_disable(clk);

	return ret;
}
EXPORT_SYMBOL_GPL(soc_camera_power_off);
#endif /* defined(CPTCFG_VIDEO_V4L2_MODULE) */

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)) */
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)) */

#if defined(CONFIG_REGMAP)
static void devm_regmap_release(struct device *dev, void *res)
{
	regmap_exit(*(struct regmap **)res);
}

#if defined(CONFIG_REGMAP_I2C)
static int regmap_i2c_write(
			    struct device *dev,
			    const void *data,
			    size_t count)
{
	struct i2c_client *i2c = to_i2c_client(dev);
	int ret;

	ret = i2c_master_send(i2c, data, count);
	if (ret == count)
		return 0;
	else if (ret < 0)
		return ret;
	else
		return -EIO;
}

static int regmap_i2c_gather_write(
				   struct device *dev,
				   const void *reg, size_t reg_size,
				   const void *val, size_t val_size)
{
	struct i2c_client *i2c = to_i2c_client(dev);
	struct i2c_msg xfer[2];
	int ret;

	/* If the I2C controller can't do a gather tell the core, it
	 * will substitute in a linear write for us.
	 */
	if (!i2c_check_functionality(i2c->adapter, I2C_FUNC_NOSTART))
		return -ENOTSUPP;

	xfer[0].addr = i2c->addr;
	xfer[0].flags = 0;
	xfer[0].len = reg_size;
	xfer[0].buf = (void *)reg;

	xfer[1].addr = i2c->addr;
	xfer[1].flags = I2C_M_NOSTART;
	xfer[1].len = val_size;
	xfer[1].buf = (void *)val;

	ret = i2c_transfer(i2c->adapter, xfer, 2);
	if (ret == 2)
		return 0;
	if (ret < 0)
		return ret;
	else
		return -EIO;
}

static int regmap_i2c_read(
			   struct device *dev,
			   const void *reg, size_t reg_size,
			   void *val, size_t val_size)
{
	struct i2c_client *i2c = to_i2c_client(dev);
	struct i2c_msg xfer[2];
	int ret;

	xfer[0].addr = i2c->addr;
	xfer[0].flags = 0;
	xfer[0].len = reg_size;
	xfer[0].buf = (void *)reg;

	xfer[1].addr = i2c->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = val_size;
	xfer[1].buf = val;

	ret = i2c_transfer(i2c->adapter, xfer, 2);
	if (ret == 2)
		return 0;
	else if (ret < 0)
		return ret;
	else
		return -EIO;
}

static struct regmap_bus regmap_i2c = {
	.write = regmap_i2c_write,
	.gather_write = regmap_i2c_gather_write,
	.read = regmap_i2c_read,
};
#endif /* defined(CONFIG_REGMAP_I2C) */

/**
 * devm_regmap_init(): Initialise managed register map
 *
 * @dev: Device that will be interacted with
 * @bus: Bus-specific callbacks to use with device
 * @bus_context: Data passed to bus-specific callbacks
 * @config: Configuration for register map
 *
 * The return value will be an ERR_PTR() on error or a valid pointer
 * to a struct regmap.  This function should generally not be called
 * directly, it should be called by bus-specific init functions.  The
 * map will be automatically freed by the device management code.
 */
struct regmap *devm_regmap_init(struct device *dev,
				const struct regmap_bus *bus,
				const struct regmap_config *config)
{
	struct regmap **ptr, *regmap;

	ptr = devres_alloc(devm_regmap_release, sizeof(*ptr), GFP_KERNEL);
	if (!ptr)
		return ERR_PTR(-ENOMEM);

	regmap = regmap_init(dev,
			     bus,
			     config);
	if (!IS_ERR(regmap)) {
		*ptr = regmap;
		devres_add(dev, ptr);
	} else {
		devres_free(ptr);
	}

	return regmap;
}
EXPORT_SYMBOL_GPL(devm_regmap_init);

#if defined(CONFIG_REGMAP_I2C)
/**
 * devm_regmap_init_i2c(): Initialise managed register map
 *
 * @i2c: Device that will be interacted with
 * @config: Configuration for register map
 *
 * The return value will be an ERR_PTR() on error or a valid pointer
 * to a struct regmap.  The regmap will be automatically freed by the
 * device management code.
 */
struct regmap *devm_regmap_init_i2c(struct i2c_client *i2c,
				    const struct regmap_config *config)
{
	return devm_regmap_init(&i2c->dev, &regmap_i2c, config);
}
EXPORT_SYMBOL_GPL(devm_regmap_init_i2c);
#endif /* defined(CONFIG_REGMAP_I2C) */

#if defined(CONFIG_REGMAP_SPI)
static int regmap_spi_write(
			    struct device *dev,
			    const void *data, size_t count)
{
	struct spi_device *spi = to_spi_device(dev);

	return spi_write(spi, data, count);
}

static int regmap_spi_gather_write(
				   struct device *dev,
				   const void *reg, size_t reg_len,
				   const void *val, size_t val_len)
{
	struct spi_device *spi = to_spi_device(dev);
	struct spi_message m;
	struct spi_transfer t[2] = { { .tx_buf = reg, .len = reg_len, },
				     { .tx_buf = val, .len = val_len, }, };

	spi_message_init(&m);
	spi_message_add_tail(&t[0], &m);
	spi_message_add_tail(&t[1], &m);

	return spi_sync(spi, &m);
}

static int regmap_spi_read(
			   struct device *dev,
			   const void *reg, size_t reg_size,
			   void *val, size_t val_size)
{
	struct spi_device *spi = to_spi_device(dev);

	return spi_write_then_read(spi, reg, reg_size, val, val_size);
}

static struct regmap_bus regmap_spi = {
	.write = regmap_spi_write,
	.gather_write = regmap_spi_gather_write,
/*
 * See commit 0d509f2b112b
 * only 3.9 kernels have this we'll ignore it
 * given I have not seen drivers use these we
 * are backporting. We'll -EINVAL these.
 */
#if 0
	.async_write = regmap_spi_async_write,
	.async_alloc = regmap_spi_async_alloc,
#endif
	.read = regmap_spi_read,
	.read_flag_mask = 0x80,

};

/**
 * devm_regmap_init_spi(): Initialise register map
 *
 * @spi: Device that will be interacted with
 * @config: Configuration for register map
 *
 * The return value will be an ERR_PTR() on error or a valid pointer
 * to a struct regmap.  The map will be automatically freed by the
 * device management code.
 */
struct regmap *devm_regmap_init_spi(struct spi_device *spi,
				    const struct regmap_config *config)
{
	return devm_regmap_init(&spi->dev, &regmap_spi, config);
}
EXPORT_SYMBOL_GPL(devm_regmap_init_spi);
#endif /* defined(CONFIG_REGMAP_SPI) */

#endif /* defined(CONFIG_REGMAP) */
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)) */

/* __wake_up_common was declared as part of the wait.h until
 * 2.6.31 in which they made it private to the scheduler. Prefix it with
 * compat to avoid double declaration issues.
 */
static void compat_wake_up_common(wait_queue_head_t *q, unsigned int mode,
			int nr_exclusive, int wake_flags, void *key)
{
	wait_queue_t *curr, *next;

	list_for_each_entry_safe(curr, next, &q->task_list, task_list) {
		unsigned flags = curr->flags;

		if (curr->func(curr, mode, wake_flags, key) &&
				(flags & WQ_FLAG_EXCLUSIVE) && !--nr_exclusive)
			break;
	}
}

/* The last 'nr' parameter was added to the __wake_up_locked() function
 * in 3.4 kernel. Define a new one prefixed with compat_ for the new API.
 */
void compat_wake_up_locked(wait_queue_head_t *q, unsigned int mode, int nr)
{
	compat_wake_up_common(q, mode, nr, 0, NULL);
}
EXPORT_SYMBOL_GPL(compat_wake_up_locked);


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/delay.h>

#define setsda(adap, val)	adap->setsda(adap->data, val)
#define setscl(adap, val)	adap->setscl(adap->data, val)
#define getsda(adap)		adap->getsda(adap->data)
#define getscl(adap)		adap->getscl(adap->data)

#define bit_dbg(level, dev, format, args...) \
	do {} while (0)

static inline void sdalo(struct i2c_algo_bit_data *adap)
{
	setsda(adap, 0);
	udelay((adap->udelay + 1) / 2);
}

static inline void sdahi(struct i2c_algo_bit_data *adap)
{
	setsda(adap, 1);
	udelay((adap->udelay + 1) / 2);
}

static inline void scllo(struct i2c_algo_bit_data *adap)
{
	setscl(adap, 0);
	udelay(adap->udelay / 2);
}

static int sclhi(struct i2c_algo_bit_data *adap)
{
	unsigned long start;

	setscl(adap, 1);

	/* Not all adapters have scl sense line... */
	if (!adap->getscl)
		goto done;

	start = jiffies;
	while (!getscl(adap)) {
		/* This hw knows how to read the clock line, so we wait
		 * until it actually gets high.  This is safer as some
		 * chips may hold it low ("clock stretching") while they
		 * are processing data internally.
		 */
		if (time_after(jiffies, start + adap->timeout)) {
			/* Test one last time, as we may have been preempted
			 * between last check and timeout test.
			 */
			if (getscl(adap))
				break;
			return -ETIMEDOUT;
		}
		cpu_relax();
	}
#ifdef DEBUG
	if (jiffies != start && i2c_debug >= 3)
		pr_debug("i2c-algo-bit: needed %ld jiffies for SCL to go "
			 "high\n", jiffies - start);
#endif

done:
	udelay(adap->udelay);
	return 0;
}

static void i2c_start(struct i2c_algo_bit_data *adap)
{
	/* assert: scl, sda are high */
	setsda(adap, 0);
	udelay(adap->udelay);
	scllo(adap);
}

static void i2c_repstart(struct i2c_algo_bit_data *adap)
{
	/* assert: scl is low */
	sdahi(adap);
	sclhi(adap);
	setsda(adap, 0);
	udelay(adap->udelay);
	scllo(adap);
}


static void i2c_stop(struct i2c_algo_bit_data *adap)
{
	/* assert: scl is low */
	sdalo(adap);
	sclhi(adap);
	setsda(adap, 1);
	udelay(adap->udelay);
}

static int i2c_outb(struct i2c_adapter *i2c_adap, unsigned char c)
{
	int i;
	int sb;
	int ack;
	struct i2c_algo_bit_data *adap = i2c_adap->algo_data;

	/* assert: scl is low */
	for (i = 7; i >= 0; i--) {
		sb = (c >> i) & 1;
		setsda(adap, sb);
		udelay((adap->udelay + 1) / 2);
		if (sclhi(adap) < 0) { /* timed out */
			bit_dbg(1, &i2c_adap->dev, "i2c_outb: 0x%02x, "
				"timeout at bit #%d\n", (int)c, i);
			return -ETIMEDOUT;
		}
		/* FIXME do arbitration here:
		 * if (sb && !getsda(adap)) -> ouch! Get out of here.
		 *
		 * Report a unique code, so higher level code can retry
		 * the whole (combined) message and *NOT* issue STOP.
		 */
		scllo(adap);
	}
	sdahi(adap);
	if (sclhi(adap) < 0) { /* timeout */
		bit_dbg(1, &i2c_adap->dev, "i2c_outb: 0x%02x, "
			"timeout at ack\n", (int)c);
		return -ETIMEDOUT;
	}

	/* read ack: SDA should be pulled down by slave, or it may
	 * NAK (usually to report problems with the data we wrote).
	 */
	ack = !getsda(adap);    /* ack: sda is pulled low -> success */
	bit_dbg(2, &i2c_adap->dev, "i2c_outb: 0x%02x %s\n", (int)c,
		ack ? "A" : "NA");

	scllo(adap);
	return ack;
	/* assert: scl is low (sda undef) */
}

static int i2c_inb(struct i2c_adapter *i2c_adap)
{
	/* read byte via i2c port, without start/stop sequence	*/
	/* acknowledge is sent in i2c_read.			*/
	int i;
	unsigned char indata = 0;
	struct i2c_algo_bit_data *adap = i2c_adap->algo_data;

	/* assert: scl is low */
	sdahi(adap);
	for (i = 0; i < 8; i++) {
		if (sclhi(adap) < 0) { /* timeout */
			bit_dbg(1, &i2c_adap->dev, "i2c_inb: timeout at bit "
				"#%d\n", 7 - i);
			return -ETIMEDOUT;
		}
		indata *= 2;
		if (getsda(adap))
			indata |= 0x01;
		setscl(adap, 0);
		udelay(i == 7 ? adap->udelay / 2 : adap->udelay);
	}
	/* assert: scl is low */
	return indata;
}

static int try_address(struct i2c_adapter *i2c_adap,
		       unsigned char addr, int retries)
{
	struct i2c_algo_bit_data *adap = i2c_adap->algo_data;
	int i, ret = 0;

	for (i = 0; i <= retries; i++) {
		ret = i2c_outb(i2c_adap, addr);
		if (ret == 1 || i == retries)
			break;
		bit_dbg(3, &i2c_adap->dev, "emitting stop condition\n");
		i2c_stop(adap);
		udelay(adap->udelay);
		yield();
		bit_dbg(3, &i2c_adap->dev, "emitting start condition\n");
		i2c_start(adap);
	}
	if (i && ret)
		bit_dbg(1, &i2c_adap->dev, "Used %d tries to %s client at "
			"0x%02x: %s\n", i + 1,
			addr & 1 ? "read from" : "write to", addr >> 1,
			ret == 1 ? "success" : "failed, timeout?");
	return ret;
}

static int bit_doAddress(struct i2c_adapter *i2c_adap, struct i2c_msg *msg)
{
	unsigned short flags = msg->flags;
	unsigned short nak_ok = msg->flags & I2C_M_IGNORE_NAK;
	struct i2c_algo_bit_data *adap = i2c_adap->algo_data;

	unsigned char addr;
	int ret, retries;

	retries = nak_ok ? 0 : i2c_adap->retries;

	if (flags & I2C_M_TEN) {
		/* a ten bit address */
		addr = 0xf0 | ((msg->addr >> 7) & 0x06);
		bit_dbg(2, &i2c_adap->dev, "addr0: %d\n", addr);
		/* try extended address code...*/
		ret = try_address(i2c_adap, addr, retries);
		if ((ret != 1) && !nak_ok)  {
			dev_err(&i2c_adap->dev,
				"died at extended address code\n");
			return -ENXIO;
		}
		/* the remaining 8 bit address */
		ret = i2c_outb(i2c_adap, msg->addr & 0xff);
		if ((ret != 1) && !nak_ok) {
			/* the chip did not ack / xmission error occurred */
			dev_err(&i2c_adap->dev, "died at 2nd address code\n");
			return -ENXIO;
		}
		if (flags & I2C_M_RD) {
			bit_dbg(3, &i2c_adap->dev, "emitting repeated "
				"start condition\n");
			i2c_repstart(adap);
			/* okay, now switch into reading mode */
			addr |= 0x01;
			ret = try_address(i2c_adap, addr, retries);
			if ((ret != 1) && !nak_ok) {
				dev_err(&i2c_adap->dev,
					"died at repeated address code\n");
				return -EIO;
			}
		}
	} else {		/* normal 7bit address	*/
		addr = msg->addr << 1;
		if (flags & I2C_M_RD)
			addr |= 1;
		if (flags & I2C_M_REV_DIR_ADDR)
			addr ^= 1;
		ret = try_address(i2c_adap, addr, retries);
		if ((ret != 1) && !nak_ok)
			return -ENXIO;
	}

	return 0;
}

static int sendbytes(struct i2c_adapter *i2c_adap, struct i2c_msg *msg)
{
	const unsigned char *temp = msg->buf;
	int count = msg->len;
	unsigned short nak_ok = msg->flags & I2C_M_IGNORE_NAK;
	int retval;
	int wrcount = 0;

	while (count > 0) {
		retval = i2c_outb(i2c_adap, *temp);

		/* OK/ACK; or ignored NAK */
		if ((retval > 0) || (nak_ok && (retval == 0))) {
			count--;
			temp++;
			wrcount++;

		/* A slave NAKing the master means the slave didn't like
		 * something about the data it saw.  For example, maybe
		 * the SMBus PEC was wrong.
		 */
		} else if (retval == 0) {
			dev_err(&i2c_adap->dev, "sendbytes: NAK bailout.\n");
			return -EIO;

		/* Timeout; or (someday) lost arbitration
		 *
		 * FIXME Lost ARB implies retrying the transaction from
		 * the first message, after the "winning" master issues
		 * its STOP.  As a rule, upper layer code has no reason
		 * to know or care about this ... it is *NOT* an error.
		 */
		} else {
			dev_err(&i2c_adap->dev, "sendbytes: error %d\n",
					retval);
			return retval;
		}
	}
	return wrcount;
}

static int acknak(struct i2c_adapter *i2c_adap, int is_ack)
{
	struct i2c_algo_bit_data *adap = i2c_adap->algo_data;

	/* assert: sda is high */
	if (is_ack)		/* send ack */
		setsda(adap, 0);
	udelay((adap->udelay + 1) / 2);
	if (sclhi(adap) < 0) {	/* timeout */
		dev_err(&i2c_adap->dev, "readbytes: ack/nak timeout\n");
		return -ETIMEDOUT;
	}
	scllo(adap);
	return 0;
}

static int readbytes(struct i2c_adapter *i2c_adap, struct i2c_msg *msg)
{
	int inval;
	int rdcount = 0;	/* counts bytes read */
	unsigned char *temp = msg->buf;
	int count = msg->len;
	const unsigned flags = msg->flags;

	while (count > 0) {
		inval = i2c_inb(i2c_adap);
		if (inval >= 0) {
			*temp = inval;
			rdcount++;
		} else {   /* read timed out */
			break;
		}

		temp++;
		count--;

		/* Some SMBus transactions require that we receive the
		   transaction length as the first read byte. */
		if (rdcount == 1 && (flags & I2C_M_RECV_LEN)) {
			if (inval <= 0 || inval > I2C_SMBUS_BLOCK_MAX) {
				if (!(flags & I2C_M_NO_RD_ACK))
					acknak(i2c_adap, 0);
				dev_err(&i2c_adap->dev, "readbytes: invalid "
					"block length (%d)\n", inval);
				return -EPROTO;
			}
			/* The original count value accounts for the extra
			   bytes, that is, either 1 for a regular transaction,
			   or 2 for a PEC transaction. */
			count += inval;
			msg->len += inval;
		}

		bit_dbg(2, &i2c_adap->dev, "readbytes: 0x%02x %s\n",
			inval,
			(flags & I2C_M_NO_RD_ACK)
				? "(no ack/nak)"
				: (count ? "A" : "NA"));

		if (!(flags & I2C_M_NO_RD_ACK)) {
			inval = acknak(i2c_adap, count);
			if (inval < 0)
				return inval;
		}
	}
	return rdcount;
}


static u32 bit_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_NOSTART | I2C_FUNC_SMBUS_EMUL |
	       I2C_FUNC_SMBUS_READ_BLOCK_DATA |
	       I2C_FUNC_SMBUS_BLOCK_PROC_CALL |
	       I2C_FUNC_10BIT_ADDR | I2C_FUNC_PROTOCOL_MANGLING;
}

static int bit_xfer(struct i2c_adapter *i2c_adap,
		    struct i2c_msg msgs[], int num)
{
	struct i2c_msg *pmsg;
	struct i2c_algo_bit_data *adap = i2c_adap->algo_data;
	int i, ret;
	unsigned short nak_ok;

	if (adap->pre_xfer) {
		ret = adap->pre_xfer(i2c_adap);
		if (ret < 0)
			return ret;
	}

	bit_dbg(3, &i2c_adap->dev, "emitting start condition\n");
	i2c_start(adap);
	for (i = 0; i < num; i++) {
		pmsg = &msgs[i];
		nak_ok = pmsg->flags & I2C_M_IGNORE_NAK;
		if (!(pmsg->flags & I2C_M_NOSTART)) {
			if (i) {
				bit_dbg(3, &i2c_adap->dev, "emitting "
					"repeated start condition\n");
				i2c_repstart(adap);
			}
			ret = bit_doAddress(i2c_adap, pmsg);
			if ((ret != 0) && !nak_ok) {
				bit_dbg(1, &i2c_adap->dev, "NAK from "
					"device addr 0x%02x msg #%d\n",
					msgs[i].addr, i);
				goto bailout;
			}
		}
		if (pmsg->flags & I2C_M_RD) {
			/* read bytes into buffer*/
			ret = readbytes(i2c_adap, pmsg);
			if (ret >= 1)
				bit_dbg(2, &i2c_adap->dev, "read %d byte%s\n",
					ret, ret == 1 ? "" : "s");
			if (ret < pmsg->len) {
				if (ret >= 0)
					ret = -EIO;
				goto bailout;
			}
		} else {
			/* write bytes from buffer */
			ret = sendbytes(i2c_adap, pmsg);
			if (ret >= 1)
				bit_dbg(2, &i2c_adap->dev, "wrote %d byte%s\n",
					ret, ret == 1 ? "" : "s");
			if (ret < pmsg->len) {
				if (ret >= 0)
					ret = -EIO;
				goto bailout;
			}
		}
	}
	ret = i;

bailout:
	bit_dbg(3, &i2c_adap->dev, "emitting stop condition\n");
	i2c_stop(adap);

	if (adap->post_xfer)
		adap->post_xfer(i2c_adap);
	return ret;
}


const struct i2c_algorithm i2c_bit_algo = {
        .master_xfer    = bit_xfer,
	.functionality  = bit_func,
};
EXPORT_SYMBOL_GPL(i2c_bit_algo);
#endif

int simple_open(struct inode *inode, struct file *file)
{
	if (inode->i_private)
		file->private_data = inode->i_private;
	return 0;
}
EXPORT_SYMBOL_GPL(simple_open);
