#include <adxl345_spi.h>
#include "stm32f4xx_hal.h"
#include <stm32f429xx.h>


static SPI_HandleTypeDef *hspi5ptr;
static uint8_t spi_tx_buf[7]; //1 tx and 7 garbage values
static uint8_t spi_rx_buf[7]; //1 garbage value and 7 axes values. Axes values in indices 1-6

HAL_StatusTypeDef ADXL345_Reg_Write(uint8_t regaddr, uint8_t data) {
	uint8_t tx[2];
	uint8_t rx[2];

	tx[0] = regaddr & 0x7F;
	tx[1] = data;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET); // CS LOW

	HAL_StatusTypeDef res = HAL_SPI_TransmitReceive(hspi5ptr, tx, rx, 2, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET); // CS HIGH

	return res;
}

void ADXL345_Init(SPI_HandleTypeDef *hspi5) {
	hspi5ptr = hspi5;
	ADXL345_Reg_Write(ADXL345_POWER_CTL, 0x08);
	ADXL345_Reg_Write(ADXL345_DUR, 0x00); //disable tap detection
	ADXL345_Reg_Write(ADXL345_LATENT, 0x00); //disable double tap detection
	ADXL345_Reg_Write(ADXL345_WINDOW, 0x00); //disable double tap detection
	ADXL345_Reg_Write(ADXL345_ACT_INACT_CTL, 0x00); //disable act/inact interrupts
	ADXL345_Reg_Write(ADXL345_BW_RATE, 0x0B); //normal power mode, odr 200Hz, bw 100Hz
	ADXL345_Reg_Write(ADXL345_INT_ENABLE, 0x80); //enable data ready interrupt only
	ADXL345_Reg_Write(ADXL345_INT_MAP, 0x00); //map data ready interrupt to INT1 (0XXXXXXX -> 0 and 7 don't cares)
	ADXL345_Reg_Write(ADXL345_DATA_FORMAT, 0x0B); //full res, +/-16g, right justified w/ sign extension
	ADXL345_Reg_Write(ADXL345_FIFO_CTL, 0x00); //bypass fifo


	HAL_Delay(10);
}
HAL_StatusTypeDef ADXL345_Reg_Read(uint8_t regaddr, volatile uint8_t *pRX) {
	uint8_t tx[2];
	uint8_t rx[2];
	tx[0] = regaddr | 0x80; //MSB set to 1 to read
	tx[1] = 0x00; //dummy
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET); //reset CS to start communication

	HAL_StatusTypeDef res =  HAL_SPI_TransmitReceive(
			hspi5ptr,
			tx,
			rx,
			2,
			HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET); //set CS to stop communication
	*pRX = rx[1];
	return res;
}
