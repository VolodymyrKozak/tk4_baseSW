/*
 * tk_oven_smi2.h
 *
 *  Created on: 23 трав. 2018 р.
 *      Author: KozakVF
 */

#ifndef TK_OVEN_SMI2_H_
#define TK_OVEN_SMI2_H_
#include <stdio.h>
#include <tkConfig.h>
#include "usart.h"
#ifdef   MODBUS_MASTER_for_OVEN_INDICATOR

#define SMI1_DEFAULT_MODBUSS_ADRESS_1 0x20
#define SMI1_DEFAULT_MODBUSS_ADRESS_2 0x21


/* Если используется широковещательной выдачи параметров
 * функцией f_ovenSMI2_DisplayBroadCast
 * на много приборов одновременно
 * предварительно должен быть определен и заполнен
 * структура/массив параметров с плавающей точкой.
 * Если f_ovenSMI2_DisplayBroadCast не используется - не обращать внимания	*/
typedef struct {
	float value[31];
}vector_of_float_t;

/*  Если хочется, можно использовать возвращаемое всеми функциями значение
 *  - код ошибки 															*/
typedef enum {
	oven_OK,
	oven_UnknownERR,
	oven_Modbus_ERR,
	oven_WrongDevice,
	oven_WrongeFunction,
	oven_WrongeRegime,
	oven_WrongeTask,
	oven_NULL_POINTER,
	oven_AnswerLOST
} oven_session_status_t;


/* Инициализация. После запуска контроллера выполнить функцию
 * для последующего использования OVEN SMI2,
 * если приборов много - выполнить инициализацию каждого прибора..
 * По умолчанию МИГАНИЕ выключено.
 * ovenSMI_ADDRESS - адресс Модбас прибора*/
oven_session_status_t f_ovenSMI2_Init (UART_HandleTypeDef *phuart,
									   uint8_t ovenSMI_ADDRESS);

/**********************************************************************************************************************
 * ШИРОКОВЕЩАТЕЛЬНАЯ ВЫДАЧА НА ИНДИКАЦИЮ ОПЕРАТИВНЫХ ПАРАМЕТРОВ
 * 1. Модбас-адреса нескольких (до 31, по моему, индикаторов СМИ2 должны быть подряд,
 * например 0х20, 0х21, ... , 0х51. Указывается лишь адрес первого регистра и количчество регистров.
 * 2. Массив плавающих чисел должен быть объявлен и определен.
 * 3. Каждое отдельное число выводится как и в функции f_ovenSMI2_DisplayFloat
 **********************************************************************************************************************/

oven_session_status_t f_ovenSMI2_DisplayBroadCast (	UART_HandleTypeDef *phuart,
													uint8_t fist_ovenSMI_ADDRESS,
													uint8_t number_of_values,
													vector_of_float_t *pv);
/*ВЫДАЧА НА ИНДИКАЦИЮ ПАРАМЕТРА - ЧИСЛА С ПЛАВАЮЩЕЙ ТОЧКОЙ
 * Если value
 * 				>  999.0 		выводится 'Err '
 * 				< -999.0 		выводится '-Err'
 * Если -999.0 ...+999.0  		выводится значение, место точки определяется автоматически
 * Если -0.01  ... 0.001		выводится '0000'
 * */
oven_session_status_t f_ovenSMI2_DisplayFloat (	UART_HandleTypeDef *phuart,
												uint8_t ovenSMI_ADDRESS,
												float value
												);
/* ВКЛЮЧИТЬ МИГАНИЕ */
oven_session_status_t f_ovenSMI2_DisplayBlinkingON (UART_HandleTypeDef *phuart,
													uint8_t ovenSMI_ADDRESS);
/* ВЫКЛЮЧИТЬ МИГАНИЕ */
oven_session_status_t f_ovenSMI2_DisplayBlinkingOFF (UART_HandleTypeDef *phuart,
													 uint8_t ovenSMI_ADDRESS);



oven_session_status_t f_ovenSMI2_Init_String (	UART_HandleTypeDef *phuart,
												uint8_t ovenSMI_ADDRESS);
/*************************************************************************************/
/* СЛУЖЕБНЫЕ ФУНКЦИИ. Не нужны - не трогать. Нужны - разбираться зачем
 * в соотв. с руководством по эксплуатации ОВЕН СМИ2 Индикатор RS-485 АРАВ.421449.004РЭ																				*/
/* Запись нескольких регистров */
oven_session_status_t f_ovenSMI2_TASKS_16 (UART_HandleTypeDef *phuart,
										   uint8_t ovenSMI_ADDRESS,
									       uint8_t register_to_BEGIN,
										   uint8_t N_of_registers,
										   uint8_t N2_of_bytes,
										   uint8_t *str_to_oven);
/* Запись одного регистра */
oven_session_status_t f_ovenSMI2_TASKS_06 (UART_HandleTypeDef *phuart,
										   uint8_t ovenSMI_ADDRESS,
										   uint16_t register_to_PRESET,
										   uint16_t register_value);
/* Чтение регистра */
#ifdef MODBUS_MASTER_for_OVEN_INDICATOR_TESTING
oven_session_status_t f_ovenSMI2_TASKS_03 ( UART_HandleTypeDef *phuart,
											uint8_t ovenSMI_ADDRESS,
											uint16_t register_to_READ,
											uint16_t *ptr_received_values);
#endif //MODBUS_MASTER_for_OVEN_INDICATOR_TESTING
/* Чтение имени прибора и версии программы															*/
oven_session_status_t f_ovenSMI2_TASKS_17 (	UART_HandleTypeDef *phuart,
											uint8_t ovenSMI_ADDRESS);
#endif /*  MODBUS_MASTER_for_OVEN_INDICATOR */

#endif /* TK_OVEN_SMI2_H_*/

