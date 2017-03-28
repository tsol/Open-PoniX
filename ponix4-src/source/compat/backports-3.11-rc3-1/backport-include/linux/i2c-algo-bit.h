#ifndef __BACKPORT_LINUX_I2C_ALGO_BIT_H
#define __BACKPORT_LINUX_I2C_ALGO_BIT_H
#include_next <linux/i2c-algo-bit.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0) && \
    LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
#define i2c_bit_algo LINUX_BACKPORT(i2c_bit_algo)
extern const struct i2c_algorithm i2c_bit_algo;
#endif

#endif /* __BACKPORT_LINUX_I2C_ALGO_BIT_H */
