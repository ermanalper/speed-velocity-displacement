#include "adxl345_i2c.h"
#include "stm32f4xx_hal.h"
#include <stm32f429xx.h>

#define I2C_PORT I2C_NUM_0

static I2C_HandleTypeDef *hi2c3;
int16_t *x, *y, *z;
uint8_t i2c_rx_buf[6];
HAL_StatusTypeDef ADXL345_Reg_Read_I2C(uint8_t regaddr, uint8_t *pRx) {
	return HAL_I2C_Mem_Read(
			hi2c3,
			(ADXL345_ADDR << 1),
			regaddr,
			I2C_MEMADD_SIZE_8BIT,
			pRx,
			1,
			HAL_MAX_DELAY);

}
void ADXL345_Init_I2C(I2C_HandleTypeDef *hi2c3ptr, int16_t *px, int16_t *py, int16_t *pz) {
	hi2c3 = hi2c3ptr;
	x = px;
	y = py;
	z = pz;
	//ADXL345_Reg_Write_I2C(ADXL345_DUR, 0x00); //disable tap detection
//	ADXL345_Reg_Write_I2C(ADXL345_LATENT, 0x00); //disable double tap detection
	//ADXL345_Reg_Write_I2C(ADXL345_WINDOW, 0x00); //disable double tap detection
	//ADXL345_Reg_Write_I2C(ADXL345_ACT_INACT_CTL, 0x00); //disable act/inact interrupts
	ADXL345_Reg_Write_I2C(ADXL345_BW_RATE, 0x0B); //normal power mode, odr 200Hz, bw 100Hz
	ADXL345_Reg_Write_I2C(ADXL345_INT_ENABLE, 0x80); //disable all interrupts
	//ADXL345_Reg_Write_I2C(ADXL345_INT_MAP, 0x00); //map data ready interrupt to INT1 (0XXXXXXX -> 0 and 7 don't cares)
	ADXL345_Reg_Write_I2C(ADXL345_DATA_FORMAT, 0x0B); //full res, +/-16g, right justified w/ sign extension
	//ADXL345_Reg_Write_I2C(ADXL345_FIFO_CTL, 0x00); //bypass fifo
	HAL_StatusTypeDef status = ADXL345_Reg_Write_I2C(ADXL345_POWER_CTL, 0x08);

	HAL_Delay(500);
	uint8_t int_enable_reg = 99;
	uint8_t int_map_reg = 99;
	ADXL345_Reg_Read_I2C(ADXL345_INT_ENABLE, &int_enable_reg);
	ADXL345_Reg_Read_I2C(ADXL345_INT_MAP, &int_map_reg);
}

HAL_StatusTypeDef ADXL345_Reg_Write_I2C(uint8_t regaddr, uint8_t data) {
	return HAL_I2C_Mem_Write(
			hi2c3,
			(ADXL345_ADDR << 1) ,
			regaddr,
			I2C_MEMADD_SIZE_8BIT,
			&data,
			1,
			HAL_MAX_DELAY);

}
HAL_StatusTypeDef ADXL345_Reg_ReadMulti_I2C(uint8_t baseaddr, uint8_t *pBuffer, uint16_t len) {
	return HAL_I2C_Mem_Read(
			hi2c3,
			(ADXL345_ADDR << 1),
			baseaddr,
			I2C_MEMADD_SIZE_8BIT,
			pBuffer,
			len,
			HAL_MAX_DELAY);
}

void ADXL345_ReadAxes_I2C() {
	uint8_t rx_buf[6];
	ADXL345_Reg_ReadMulti_I2C(ADXL345_DATAX0, rx_buf, 6);

	*x = (int16_t) ((rx_buf[1] << 8) | rx_buf[0]);
	*y = (int16_t) ((rx_buf[3] << 8) | rx_buf[2]);
	*z = (int16_t) ((rx_buf[5] << 8) | rx_buf[4]);
}

HAL_StatusTypeDef ADXL345_ReadAxes_DMA_I2C() {
	return HAL_I2C_Mem_Read_DMA(
			hi2c3,
			(ADXL345_ADDR) << 1,
			ADXL345_DATAX0,
			I2C_MEMADD_SIZE_8BIT,
			i2c_rx_buf,
			6);

}
void ADXL345_ReadValuesFromRx() {
	*x = (i2c_rx_buf[1] << 8) | i2c_rx_buf[0];
	*y = (i2c_rx_buf[3] << 8) | i2c_rx_buf[2];
	*z = (i2c_rx_buf[5] << 8) | i2c_rx_buf[4];
}
float adxl345_convert_to_g(int16_t raw_value) {
    // For full resolution mode, the scale factor is 4mg/LSB
    return raw_value * 0.004f; // Convert to g
}
