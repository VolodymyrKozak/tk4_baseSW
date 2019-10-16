/*
 * zFF.h
 *
 *  Created on: 24 лют. 2019 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKLIBRARIES_TKMENU_ZFF_H_
#define TKLIBRARIES_TKMENU_ZFF_H_


#include "stdint.h"
#include "tkConfig.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <vkButtonsADC.h>
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "tk_wo1602g.h"
#include "tk_wh1602b3.h"

typedef enum {
	dsp_OK,						/* OK, без коментарів															*/
	dsp_UNKNOWN_Err,			/* невизначена помилка*/
	dsp_wo1602gErr,
	dsp_FLASHErr,
	dsp_I2C_Err,				/* помилка транспортування даних по I2C 										*/
	dsp_BUSY,
	dsp_WRONG_Arg,				/* на вході функції некоректні аргументи, отже і функції щось робила некоректно */
	dsp_NULL_Ptr,				/* нульовий показчик (рос. - указатель), аврійне припинення роботи 				*/
	dsp_Err						/* помилка при виконання функції, щось не так..*/
}dsp_status_t;


extern int8_t strHigh_menu[17];
extern int8_t strLow_menu[17];
extern int8_t strLow0_menu[17];

dsp_status_t tkDisplay_Init(void);
dsp_status_t tkDisplay_ClearScreen(void);
dsp_status_t tkDisplay_PutHighStr(int8_t *hstr);
dsp_status_t tkDisplay_PutLowStr(int8_t *hstr);


#endif /* TKLIBRARIES_TKMENU_ZFF_H_ */
