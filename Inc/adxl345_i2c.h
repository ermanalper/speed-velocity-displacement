#ifndef ADXL345_I2C_H
#define ADXL345_I2C_H

#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx_hal.h"
#define ADXL345_ADDR 0x53 // I2C address of the ADXL345 when ALT ADDRESS (SDO) pin is low. Aware that this is a 7-bit address,
                          // so it must be shifted left and ORd with the read/write bit when used in I2C communication.
                          // (esp-idf's i2c_master_write_to_device and i2c_master_read_from_device functions handle this internally,
                          // so you can use the 7-bit address directly.)
#define ADXL345_DEVID 0x00
#define ADXL345_THRESH_TAP 0x1D
#define ADXL345_OFSX 0x1E
#define ADXL345_OFSY 0x1F
#define ADXL345_OFSZ 0x20
#define ADXL345_DUR 0x21
#define ADXL345_LATENT 0x22
#define ADXL345_WINDOW 0x23
#define ADXL345_THRESH_ACT 0x24
#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27
#define ADXL345_THRESH_FF 0x28
#define ADXL345_TIME_FF 0x29
#define ADXL345_TAP_AXES 0x2A
#define ADXL345_ACT_TAP_STATUS 0x2B
#define ADXL345_BW_RATE 0x2C
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_INT_ENABLE 0x2E
#define ADXL345_INT_MAP 0x2F
#define ADXL345_INT_SOURCE 0x30
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
#define ADXL345_DATAX1 0x33
#define ADXL345_DATAY0 0x34
#define ADXL345_DATAY1 0x35
#define ADXL345_DATAZ0 0x36
#define ADXL345_DATAZ1 0x37
#define ADXL345_FIFO_CTL 0x38
#define ADXL345_FIFO_STATUS 0x39                          

HAL_StatusTypeDef ADXL345_Reg_Read_I2C(uint8_t regaddr, uint8_t *pRx);
void ADXL345_Init_I2C(I2C_HandleTypeDef *hi2c3ptr, int16_t *px, int16_t *py, int16_t *pz);
void ADXL345_ReadAxes_I2C(void);
HAL_StatusTypeDef ADXL345_Reg_ReadMulti_I2C(uint8_t baseaddr, uint8_t *pBuffer, uint16_t len);
HAL_StatusTypeDef ADXL345_Reg_Write_I2C(uint8_t regaddr, uint8_t data);
float adxl345_convert_to_g(int16_t raw_value);


#endif
