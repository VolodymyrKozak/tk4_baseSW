/*
 * tkDebugThings.h
 *
 *  Created on: 10 жовт. 2017 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKDEBUGTHINGS_H_
#define TKDEBUGTHINGS_H_
#include <stdint.h>
#include "i2c.h"
extern uint32_t delay3_5bytes;
extern uint32_t delay1_5bytes;



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
void delayUs(uint32_t micros);
/***************************************************************************/
/*затримка в мікросекундах, аналог HAL_Delay (), */
uint32_t getUs (void);
/***************************************************************************/

uint8_t f_I2C_ScanBus(void);
void i2c1BUSY_Doctor (void);
HAL_StatusTypeDef HAL_I2C_ClearBusyFlagErrata_2_14_7(I2C_HandleTypeDef * hi2c, GPIO_TypeDef * I2C_Port, uint16_t SCL_Pin, uint16_t SDA_Pin);
#endif /* TKDEBUGTHINGS_H_ */
