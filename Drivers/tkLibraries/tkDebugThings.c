/*
 * tkDebugThings.c
 *
 *  Created on: 10 жовт. 2017 р.
 *      Author: Volodymyr.Kozak
 */
#include "tkDebugThings.h"

#include "i2c.h"
#include "stm32f1xx_hal.h"

uint32_t delay3_5bytes=3700;
uint32_t delay1_5bytes=1590;


/************************************************************************
Мікросекундомір
Якщо ми хочемо виміряти час, який витрачає наша програма на ansi_c на
виконання якоїсь кількості інструкцій просто пишемо:

timing1= getUs()-start;
			* ОПЕРАТОРИ ПРОГРАМИ *
		  start=getUs();
і отримуємо час в мікросекундах
максимальний час коректної роботи - 1 год.12 хв :-),будьте обережні
**************************************************************************/
uint32_t getUs (void) {
	uint32_t ms_us=0;
	uint32_t usTicks = HAL_RCC_GetSysClockFreq() / 1000000;
	register uint32_t ms;
	register uint32_t cycle_cnt;
	do {
		ms = HAL_GetTick();
		cycle_cnt = SysTick->VAL;
	} while (ms != HAL_GetTick());
	ms_us = (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
	return (ms_us);
}
/***************************************************************************/
/*затримка в мікросекундах, аналог HAL_Delay (), */
void delayUs(uint32_t micros) {
	uint32_t start = getUs();
	while (getUs()-start < (uint32_t) micros) {
		asm("nop");
	}
}
/****************************************************************************
 * **************************************************************************
 */
uint32_t flash_read(uint32_t address) {

	return (*(__IO uint32_t*) address);

	}
/*********************/
uint8_t f_I2C_ScanBus(void) {
	uint8_t i_out=0;
	uint8_t i = 0;
	/* Scan only for 112 allowed addresses */
	volatile HAL_StatusTypeDef i2cStatus = HAL_OK;
	for (i = 0x07; i < 0x78; i++) {
		i2cStatus=HAL_I2C_IsDeviceReady(&hi2c1, i << 1, 10, 100);
			if (i2cStatus==HAL_BUSY){
				/**I2C1 GPIO Configuration
				 PB6     ------> I2C1_SCL
				 PB7     ------> I2C1_SDA
				 */

				HAL_I2C_ClearBusyFlagErrata_2_14_7(
						&hi2c1,
						GPIOB,
						GPIO_PIN_6,
						GPIO_PIN_7
					);
				HAL_I2C_MspDeInit(&hi2c1);
				HAL_I2C_MspInit(&hi2c1);
				MX_I2C1_Init();
			}
		if (i2cStatus == HAL_OK) {
			i_out=i;break;
		}
	}
	return i_out;
}

void i2c1BUSY_Doctor (void){
/**I2C1 GPIO Configuration
 PB6     ------> I2C1_SCL
 PB7     ------> I2C1_SDA
 */

//	HAL_I2C_ClearBusyFlagErrata_2_14_7(
//			&hi2c1,
//			GPIOB,
//			GPIO_PIN_6,
//			GPIO_PIN_7
//	);
//	MX_I2C1_Init();
//	HAL_I2C_MspDeInit(&hi2c1);
//	HAL_I2C_MspInit(&hi2c1);

	HAL_I2C_ClearBusyFlagErrata_2_14_7(
			&hi2c1,
			GPIOB,
			GPIO_PIN_6,
			GPIO_PIN_7
	);
	HAL_I2C_MspInit(&hi2c1);
	MX_I2C1_Init();


}
	/* USER CODE BEGIN 1 */
	/**
	 * @brief Clears the I2C busy flags, based on STM32F103xC/D/E Errata section 2.14.7
	 * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
	           the configuration information for I2C module
	 * @param  I2C_Port: I2C GPIO peripheral port
	 * @param  SCL_Pin: I2C SCL Pin
	 * @param  SDA_Pin: I2C SDA Pin
	 * @retval HAL status
	 */
	/* Errata sheet (section 2.14.7)  */
	HAL_StatusTypeDef HAL_I2C_ClearBusyFlagErrata_2_14_7(I2C_HandleTypeDef * hi2c, GPIO_TypeDef * I2C_Port, uint16_t SCL_Pin, uint16_t SDA_Pin) {
		GPIO_InitTypeDef GPIO_InitStruct;
		/* 1. Disable the I2C peripheral by clearing the PE bit in I2Cx_CR1 register. */
		__HAL_I2C_DISABLE(hi2c);
		/* 2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR). */
	//	HAL_GPIO_WritePin(I2C_Port, SCL_Pin, GPIO_PIN_SET);
	//	HAL_GPIO_WritePin(I2C_Port, SDA_Pin, GPIO_PIN_SET);
		GPIO_InitStruct.Pin = SCL_Pin | SDA_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(I2C_Port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(I2C_Port, SCL_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(I2C_Port, SDA_Pin, GPIO_PIN_SET);
		/* 3. Check SCL and SDA High level in GPIOx_IDR. */
		while(HAL_GPIO_ReadPin(I2C_Port, SCL_Pin) != GPIO_PIN_SET ||
				HAL_GPIO_ReadPin(I2C_Port, SDA_Pin) != GPIO_PIN_SET);
		/* 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR). */
		HAL_GPIO_WritePin(I2C_Port, SDA_Pin, GPIO_PIN_RESET);
		/* 5. Check SDA Low level in GPIOx_IDR. */
		while(HAL_GPIO_ReadPin(I2C_Port, SDA_Pin) != GPIO_PIN_RESET);
		/* 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR). */
		HAL_GPIO_WritePin(I2C_Port, SCL_Pin, GPIO_PIN_RESET);
		/* 7. Check SCL Low level in GPIOx_IDR. */
		while(HAL_GPIO_ReadPin(I2C_Port, SCL_Pin) != GPIO_PIN_RESET);
		/* 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR). */
		HAL_GPIO_WritePin(I2C_Port, SCL_Pin, GPIO_PIN_SET);
		/* 9. Check SCL High level in GPIOx_IDR. */
		while(HAL_GPIO_ReadPin(I2C_Port, SCL_Pin) != GPIO_PIN_SET);
		/* 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR). */
		HAL_GPIO_WritePin(I2C_Port, SDA_Pin, GPIO_PIN_SET);
		/* 11. Check SDA High level in GPIOx_IDR. */
		while(HAL_GPIO_ReadPin(I2C_Port, SDA_Pin) != GPIO_PIN_SET);
		/* 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain. */
		GPIO_InitStruct.Pin = SCL_Pin | SDA_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(I2C_Port, &GPIO_InitStruct);
		/* 13. Set SWRST bit in I2Cx_CR1 register. */
		hi2c->Instance->CR1 |= I2C_CR1_SWRST;
		/* 14. Clear SWRST bit in I2Cx_CR1 register. */
		hi2c->Instance->CR1 &= ~I2C_CR1_SWRST;
		/* 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register. */
		__HAL_I2C_ENABLE(hi2c);

		if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY)) {
			return HAL_ERROR;
		} else {
			return HAL_OK;
		}
	}
