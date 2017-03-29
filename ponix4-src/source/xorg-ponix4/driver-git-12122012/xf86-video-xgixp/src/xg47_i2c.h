#ifndef XG47_I2C_H
#define XG47_I2C_H

#define I2C_SDA_HIGH   0x01
#define I2C_SCL_HIGH   0x02

#define I2C_READ_MODE  0x00
#define I2C_WRITE_MODE 0x08

extern Bool xg47_InitI2C(ScrnInfoPtr pScrn);

#endif /* XG47_I2C_H */
