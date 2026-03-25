#include "l3gd20.h"
#include "stm32f4xx_hal.h"
#include <stm32f429xx.h>


static SPI_HandleTypeDef *hspi5ptr;
volatile int16_t *x_axisptr;
volatile int16_t *y_axisptr;
volatile int16_t *z_axisptr;
volatile float *xdps_ptr, *ydps_ptr, *zdps_ptr;
static uint8_t spi_tx_buf[7]; //1 tx and 7 garbage values
static uint8_t spi_rx_buf[7]; //1 garbage value and 7 axes values. Axes values in indices 1-6


/*
HAL_StatusTypeDef L3GD20_Reg_Read(uint8_t regaddr, volatile uint8_t *pRX) {
	uint8_t tx[2];
	uint8_t rx[2];
	tx[0] = regaddr | 0x80; //MSB set to 1 to read
	tx[1] = 0x00; //dummy
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); //reset CS to start communication

	HAL_StatusTypeDef res =  HAL_SPI_TransmitReceive(
			hspi5ptr,
			tx,
			rx,
			2,
			HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); //set CS to stop communication
	*pRX = rx[1];
	return res;
}

HAL_StatusTypeDef L3GD20_Reg_ReadMultiBytes(uint8_t baseaddr, unsigned int len, volatile uint8_t *pBuffer) {
	if (len == 1) {
		return L3GD20_Reg_Read(baseaddr, pBuffer);
	}
	uint8_t tx[len + 1]; //1 data + (len)*dummy
	for(unsigned int i = 1; i < len + 1; i++){
			tx[i] = 0x00; //reset tx
	}
	uint8_t rx[len + 1];
	tx[0] = baseaddr | 0xC0;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

	HAL_StatusTypeDef res = HAL_SPI_TransmitReceive(
			hspi5ptr,
			tx,
			rx,
			len + 1,
			HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

	for (unsigned int i = 0; i < len; i++) {
		pBuffer[i] = rx[i + 1];
	}
	return res;
}

void readX(void) {
	uint8_t rx[2];
	L3GD20_Reg_ReadMultiBytes(OUT_X_L, 2, rx);
	*x_axisptr = (int16_t) ((rx[1] << 8) | rx[0]);
}
void readY(void) {
	uint8_t rx[2];
	L3GD20_Reg_ReadMultiBytes(OUT_Y_L, 2, rx);
	*y_axisptr = (int16_t) ((rx[1] << 8) | rx[0]);
}
void readZ(void) {
	uint8_t rx[2];
	L3GD20_Reg_ReadMultiBytes(OUT_Z_L, 2, rx);
	*z_axisptr = (int16_t) ((rx[1] << 8) | rx[0]);
}

HAL_StatusTypeDef L3GD20_ReadAxes(void) {
	volatile uint8_t axesBuffer[6];
	HAL_StatusTypeDef res = L3GD20_Reg_ReadMultiBytes(OUT_X_L, 6, axesBuffer);

	*x_axisptr = (int16_t) ((axesBuffer[1] << 8) | axesBuffer[0]);
	*y_axisptr = (int16_t) ((axesBuffer[3] << 8) | axesBuffer[2]);
	*z_axisptr = (int16_t) ((axesBuffer[5] << 8) | axesBuffer[4]);

	return res;
}*/

HAL_StatusTypeDef L3GD20_Reg_Write(uint8_t regaddr, uint8_t data) {
	uint8_t tx[2]; // A total of 2 bytes transmitted
	tx[0] = regaddr & 0x7F; //MSB set to 0 to write
	tx[1] = data;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); //reset CS to start communication

	HAL_StatusTypeDef res = HAL_SPI_Transmit(hspi5ptr,
			tx,
			2, // 2 bytes total
			HAL_MAX_DELAY);


	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); //set CS to stop communication
	return res;
}
void L3GD20_Init(SPI_HandleTypeDef *hspi5, volatile int16_t *x_axis, volatile int16_t *y_axis, volatile int16_t *z_axis,
		volatile float *xdps, volatile float *ydps, volatile float *zdps) {
	hspi5ptr = hspi5;
	x_axisptr = x_axis;
	y_axisptr = y_axis;
	z_axisptr = z_axis;
	xdps_ptr = xdps;
	ydps_ptr = ydps;
	zdps_ptr = zdps;

	uint8_t ctrl1 = 0x3F; //normal mode
	L3GD20_Reg_Write(CTRL_REG1, ctrl1);

	uint8_t ctrl2 = 0x05; //reset reading HP_RESET_FILTER, cut off freq: 0.18 Hz
	L3GD20_Reg_Write(CTRL_REG2, ctrl2);

	uint8_t ctrl4 = 0x00; //no highpass filter
	L3GD20_Reg_Write(CTRL_REG4, ctrl4);

	uint8_t ctrl5 = 0x00; //no highpass filter
	L3GD20_Reg_Write(CTRL_REG5, ctrl5);

	spi_tx_buf[0] = OUT_X_L | 0xC0;
	for (unsigned short i = 1; i < 7; i++) {
		spi_tx_buf[i] = 0;
	}

	HAL_Delay(250);
}

void L3GD20_ReadAxes_DMA(void) {
	//Set CS pin low to start SPI
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive_DMA(
			hspi5ptr,
			spi_tx_buf,
			spi_rx_buf,
			7);
}
void L3GD20_ReadValuesFromRx(void) {
	*x_axisptr = (int16_t) ((spi_rx_buf[2] << 8) | spi_rx_buf[1]);
	*y_axisptr = (int16_t) ((spi_rx_buf[4] << 8) | spi_rx_buf[3]);
	*z_axisptr = (int16_t) ((spi_rx_buf[6] << 8) | spi_rx_buf[5]);
	*xdps_ptr = (float)*x_axisptr * (0.00875);
	*ydps_ptr = (float)*y_axisptr * (0.00875);
	*zdps_ptr = (float)*z_axisptr * (0.00875);

}


