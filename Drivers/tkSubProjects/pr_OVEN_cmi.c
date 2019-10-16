/*
 * prg_OVEN_cmi.c
 *
 *  Created on: 24 груд. 2018 р.
 *      Author: KozakVF
 */

/*
 * prg_OVEN_SMI.c
 *
 *  Created on: 24 груд. 2018 р.
 *      Author: KozakVF
 */

#include "tkConfig.h"

#include "pr_OVEN_cmi.h"
#include "wk_atime64.h"
#include "tk_wh1602b3.h"

#include <stdio.h>
#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#ifdef MODBUS_MASTER_for_OVEN_INDICATOR
/* Тут писати функції конкретних проектів з використанням
 * OVEN_INDICATOR
 * */

#ifdef MODBUS_MASTER_for_OVEN_INDICATOR_TESTING
	static float k[11]  = {10, 1, 0.1, 0.01, 0.001,0.0001, -0.0001,-0.001,-0.01,-0.1, -1};
	static uint8_t tmpov_str[17]={0};
	static UART_HandleTypeDef *poven_master_huart;
	static void ctr_oprint(uint8_t *str, float float_a);

	void f_oventest_Init(void){
#ifdef MASTERRTU_HUART_4
		poven_master_huart =&huart4;
#endif
#ifdef MASTERRTU_HUART_5
		poven_master_huart =&huart5;
#endif

	}

	/* Демонстрація управління контролером роботою ОВЕН СМИ
	 * Ты самі числа виводяться і на ОВЕН СМИ і на дисплей.
	*/
	void f_oventest(uint32_t ov_counter){
		if (ov_counter < 300){
		/*	1. 5 хвилин виводимо випадкові числа з точкою у різних місцях,
			послідовно - на один та на другий дисплей
			(функція f_ovenSMI2_DisplayFloat(..) */

			f_oventest1();
		}
		else if ((ov_counter > 600)){
		/*	2. 5 Хвилин виводимо числа однією циркулярною функцєю на всі
						ОВЕН СМИ
						(функція f_ovenSMI2_DisplayBroadCast(..)
		*/
			f_oventest2();
		}
		else{}
		if ((ov_counter == 900)){
			/*3. Вмикаємо мигання на одному приладі*/

			f_ovenSMI2_DisplayBlinkingON (poven_master_huart,16);
		}
		if (ov_counter == 1200){
			/*4. Вимикаємо мигання на одному приладі*/
			f_ovenSMI2_DisplayBlinkingOFF (poven_master_huart,16);
		}
	}
	void f_oventest1(void){
//		volatile uint32_t dursession=0;
//		uint32_t ticksess=HAL_GetTick();
//		for (uint8_t i=0;i<10;i++){

			uint32_t seed = HAL_GetTick();
			srand (seed);
			uint32_t randseedh = rand()%10000;
			uint8_t rowh=rand()%11;
			volatile float a1 = (float)(randseedh)*k[rowh];
			/* випадкова адреса Модбас */
			uint8_t randaddress = rand()%128;

			f_ovenSMI2_DisplayFloat (poven_master_huart,randaddress,a1);

//			dursession = HAL_GetTick()-ticksess;

			ctr_oprint(tmpov_str,a1);
			WH1602b3_WriteHighString(tmpov_str);
//		}
		HAL_Delay(20);

//		uint32_t randseedl = rand()%10000;
//		uint8_t rowl=rand()%11;
//		volatile float a2 = (float)(randseedl)*k[rowl];
//		f_ovenSMI2_DisplayFloat (poven_master_huart,18,a2);
//		ctr_oprint(tmpov_str,a2);
//		WH1602b3_WriteLowString(tmpov_str);
//		HAL_Delay(20);
	}

	vector_of_float_t vect = {{0.001, 0.002, 0.003, 0.004}};
	void f_oventest2(void){
		f_ovenSMI2_DisplayBroadCast (	poven_master_huart,
										16,
										3,
										&vect);
		ctr_oprint(tmpov_str,vect.value[0]);
		WH1602b3_WriteHighString(tmpov_str);
		ctr_oprint(tmpov_str,vect.value[2]);
		WH1602b3_WriteLowString(tmpov_str);
		vect.value[0]=vect.value[0]+0.001;
		vect.value[2]=vect.value[2]+0.001;

	}

	void f_oventest3(void){

		uint32_t seed = wk_real64_value % 0x100000000;
		srand (seed);
		uint32_t randseedh = rand()%10000;
		uint8_t rowh=rand()%11;
		volatile float a1 = (float)(randseedh)*k[rowh];
		f_ovenSMI2_DisplayFloat (poven_master_huart,16,a1);
		ctr_oprint(tmpov_str,a1);
		WH1602b3_WriteHighString(tmpov_str);
		HAL_Delay(20);

		uint32_t randseedl = rand()%10000;
		uint8_t rowl=rand()%11;
		volatile float a2 = (float)(randseedl)*k[rowl];
		f_ovenSMI2_DisplayFloat (poven_master_huart,18,a2);
		ctr_oprint(tmpov_str,a2);
		WH1602b3_WriteLowString(tmpov_str);
		HAL_Delay(20);
	}



static void ctr_oprint(uint8_t *str, float float_a){
	volatile uint32_t int_ttt=0;
	volatile float fl = 0;
	volatile uint32_t fra_ttt=0;
	if (float_a>=0){
		int_ttt=(uint32_t)float_a;
		fl = (float_a-int_ttt)*1000;
		fra_ttt=(uint32_t)(fl);
		memset (str, 0, sizeof(str));
		snprintf((char*)(str), 17, "t = %4d.%03d ",int_ttt, fra_ttt);/*#include "string.h"*/


	}
	else {

		float ttt=-float_a;
		int_ttt=(uint32_t)ttt;
		fl = (ttt-int_ttt)*1000;
		fra_ttt=(uint32_t)(fl);
	}
	snprintf((char*)(&tmpov_str), 17, "t =-%4d.%03d ",int_ttt, fra_ttt);/*#include "string.h"*/
}
#endif // MODBUS_MASTER_for_OVEN_INDICATOR_TESTING
#endif // MODBUS_MASTER_for_OVEN_INDICATOR
