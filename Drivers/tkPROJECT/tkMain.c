/* tkMain.c
 *  Created on: 27 лют. 2018 р.
 *      Author: Volodymyr.Kozak
 */
//#include "stm32f1xx_hal.h"
#include "adc.h"
#include "tkConfig.h"
//#include "i2c.h"
#include "gpio.h"
#include "tim.h"
#include <stdio.h>
#include "stm32f1xx_it.h"
//#include "iwdg.h"
//#include "iwdg.h"
#include "vv300set.h"
//#include <tkConfig.h>
#include "tkGrid.h"
#include "tkMain.h"
#include "tk0spv.h"
//#include "tkSystemSet.h"
//#include "tkMotor_modes.h"
//#include "tkTest1Flash.h"
//#include "tkTEST1menu.h" /* лише для відлагоджування */

//#include "tk2_remote_control.h"
//#include "tk2n2.h"
//#include  "wk_atime64.h"
//#include "u16set.h"
#include "pr_OVEN_cmi.h"
#ifdef WINSTAR_Display_WO1602G
#include "tk_wo1602g.h"
#endif //WINSTAR_Display_WO1602G
#ifdef WINSTAR_Display_WH1602B3
#include "tk_wh1602b3.h"
#endif //WINSTAR_Display_WH1602B3

//#include "tk0flash_base.h"
//#include "tkButtonsADC.h"
//#include "tkErrLib.h"
//#include "mb_forTK2_master.h"
//
//#include "tkErr_Handler.h"
#include "stdbool.h"
#include "mdGRD_test.h"
//#include "usart.h"
#include "string.h"
#include "wk_atime64.h"
#include "tkErr_Handler.h"
//#include "tk0spv.h"
/* Зглажена частота в герцах для управління таймером відліків Фур'є	  */

//#include <pr_OVEN_cmi.h>
uint8_t tick100ms_counter=0;


//button_output_t keyboard = none;

uint32_t adc_buttons_in =2000;
float float_a = (float)(40000);


uint32_t loop_counter = 0;
uint32_t iwdgTick =0;
uint32_t loopTick =0;
uint32_t volatile loopDuration=0;
uint32_t volatile loopSDuration=0;
uint32_t volatile iwdgDuration = 0;


/****************************************************************/
tkMAIN_status_t tkInit(void){
/****************************************************************/
	volatile tkMAIN_status_t tkI=tkMAIN_UNKNOWN_Err;

	while (tkI != tkMAIN_OK){
		flash_status_t fs = FLASH_UNKNOWN_Err;
		/* Якщо ініціалізація tk4 здійснюється не вперше */
		if (f_ZeroTest() == ZERO_INIT_Cplt){
			/* зчитати з регістрів збережений час і дату*/
			HALRTC_AverageInit();
			/* зчитати з флеш в память налаштування виробника*/
			fs = f_VV300_FactorySets_Read();
			if (fs != FLASH_OK){
				f_PutErrRecord(ERR fs);
				tkI=tkMAIN_FLASHErr;
				break;
			}
			/* зчитати з флеш в память налаштування користувача */
			fs =f_VV300_UserSets_Read();
			if (fs != FLASH_OK){
				f_PutErrRecord(ERR fs);
				tkI=tkMAIN_FLASHErr;break;
			}
		}
		else {
			/* Якщо ініціалізація tk4 здійснюється вперше */
			/* Iніціювати дату і час датою і часом компіляції */
			HALRTC_ZeroInit();
			/* Ініціювати налаштування виробника і користувача за замовчуванням */
			f_FSsetInit();
			f_UsetInit();
			/* Записати на флеш
			 *  налаштування виробника і користувача за замовчуванням */
			fs=f_VV300_FactorySets_Write();
			if (fs != FLASH_OK){
				f_PutErrRecord(ERR fs);
				tkI=tkMAIN_FLASHErr;break;
			}
			fs=f_VV300_UserSets_Write();
			if (f_VV300_UserSets_Write() != FLASH_OK){
				f_PutErrRecord(ERR fs);
				tkI=tkMAIN_FLASHErr;
				break;
			}
			/* Записати у регіст мітку про завершену первинну ініціалізацію контролера */
			f_ZeroLabelWrite(ZERO_INIT_Cplt);
		}
		/* дата та час у форматі 64 */
	    f_wk64_time_Init();
	    /* Дисплей */
#ifdef WINSTAR_Display_WH1602B3
	    wh1602b3_status_t wh = WH1602b3_Init();
		if (wh != WH1602B3_OK){
			f_PutErrRecord(ERR wh);
			tkI=tkMAIN_wh1602b3Err;break;
		}
#endif //WINSTAR_Display_WH1602B3

#ifdef WINSTAR_Display_WO1602G
		if (WO1602_Init(&hi2c1, 0x3E) != WO1602G_OK){
			tkI=tkMAIN_wo1602gErr;break;
			break;
		}
#endif //WINSTAR_Display_WO1602G

		spv_status_t spvs = f_0UI_Init();
		if (spvs != spv_OK){
			f_PutErrRecord(ERR spvs);
			tkI=tkMAIN_MenuSPV_Err;break;
		}
#ifdef MODBUS_MASTER_for_OVEN_INDICATOR
#ifdef MODBUS_MASTER_for_OVEN_INDICATOR_TESTING
		f_oventest_Init();
//		f_ovenSMI2_Init (&huart4,16);
//		f_ovenSMI2_Init (&huart4,18);
//		f_ovenSMI2_Init (&huart5,17);
#endif MODBUS_MASTER_for_OVEN_INDICATOR_TESTING
#endif //MODBUS_MASTER_for_OVEN_INDICATOR

#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
		MB_Slave_Receive_Init();
#endif //defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
//#ifdef GRD_FURIE
/* Вимірювання параметрів електромережі*/
		f_GRD_Init();

//#endif //GRD_FURIE
//		if (tkFLASH_Init() != FLASH_OK){
//			tkI=tkMAIN_FLASHErr;
//			GET_tkERR();
//			woErrPrint ();
//			break;
//		}





//		 MB_Slave_Receive_Init ();


#ifdef DRIVES_REMOTE_CONTROL
		tk2_remote_control_Init ();
#endif //DRIVES_REMOTE_CONTROL




	iwdgTick=HAL_GetTick();
#if defined GRD_FURIE
	ADC_ConvInit();
	SysTick_afterInit=Loop_SysTick;
#endif //defined GRD_FURIE
	tkI=tkMAIN_OK;
	}
	return tkI;
}
/***********************************************************************************************
 *  БАЗОВИЙ ЦИКЛ
 ***********************************************************************************************/
tkMAIN_status_t tkLoop(void){
	loopTick=HAL_GetTick();
//	HAL_IWDG_Refresh(&hiwdg); /* 6550 ms Реально - 3200 */
	loop_counter++;


	/* Start the timer in interrupt mode */

//	HAL_TIM_Base_Start_IT(&htim7);
//	loopDuration = HAL_GetTick()-loopTick;
//	if (loopDuration<25){
//		HAL_Delay (25-loopDuration);
//	}
//	HAL_TIM_Base_Stop_IT(&htim7);

	loopDuration = HAL_GetTick()-loopTick;
//	HAL_TIM_Base_Start_IT(&htim6);

//	if (loopDuration<50){
//		HAL_Delay (50-loopDuration);
//	}
//	HAL_TIM_Base_Stop_IT(&htim6);
	loopTick=HAL_GetTick();
#if defined GRD_FURIE
	if (flag_grdCalc==enable_GRD_Calc){
		f_GRD_Events();
////	f_GRD_Print();
		flag_grdCalc=GRD_Calc_finished;
	}
	loopDuration = HAL_GetTick()-loopTick;
#endif //GRD_FURIE
#if  defined FURIE_TEST
		f_furie_Model(Frequency);
		f_GRD_Calc();
//		f_GRD_Print();
		f_GRID_Power();

#endif //defined FURIE_TEST

	f_wk64_time_Loop();
	int8_t st[17]={0};
 	snprintf((char*)st, 17, "%4d/%02d/%02d %02d:%02d",
 			p_wk64_tm->tm_year+1900,
			p_wk64_tm->tm_mon+1,
			p_wk64_tm->tm_mday,
			p_wk64_tm->tm_hour,
			p_wk64_tm->tm_min);
 	tkDisplay_PutHighStr(st);
 	snprintf((char*)st, 17, "%02d:%02d:%02d",
 			p_wk64_tm->tm_hour,
 			p_wk64_tm->tm_min,
			p_wk64_tm->tm_sec);
 	tkDisplay_PutLowStr(st);
//	keyboard = buttons_pool ();                                           /* опитування клавіш */
//	if (keyboard 	!= none){							  /* якщо виявлено натиснуту кнопку контролера - обробка меню */
//		WOmenuDoing (keyboard);
//	}
//	f_btn_debug (keyboard);

#ifdef MODBUS_MASTER_for_OVEN_INDICATOR
#ifdef MODBUS_MASTER_for_OVEN_INDICATOR_TESTING
	f_oventest1();
#endif MODBUS_MASTER_for_OVEN_INDICATOR_TESTING
#endif //MODBUS_MASTER_for_OVEN_INDICATOR

#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
//	MB_Slave_Handling ();
#endif //defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
//

//	f_oventest1();
#ifdef DRIVES_REMOTE_CONTROL
//		tk2_remote_control_LOOP ();
#endif //DRIVES_REMOTE_CONTROL



//	two_pump_control_loop ();
													/* оновлення лічильника сторожового таймера */
//	 ++tick100ms_counter;
//	 if (tick100ms_counter==10){
//		 if (f_TIME_tick () != TIME_OK){											/**/
//			 tkErrHandler();
//		 }
//		 tick100ms_counter=0;
//	 }
//	date_time_t t_getted={0};
//	if (f_TIME_get (&t_getted)!=TIME_OK){
//		 tkErrHandler();
//	}
//	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_SET);

//	vect_float.value[0] = float_a;
//	vect_float.value[1] = float_a*0.5;
//	f_ovenSMI2_DisplayBroadCast (	0x20,
//									2,
//									&vect_float
//								);
//
//if (float_a > 0){
//	if (float_a > 0.0001){
//		float_a= float_a*((float)(0.7));
//	}
//	else {
//		(float_a = float_a *(-1.0));
//	}
//}
//if(float_a<0){
//	if (float_a>-10999){
//	float_a= float_a*((float)(1.3));
//	}
//	else {
//		(float_a = 10000);
//
//		f_ovenSMI2_TASKS_06(0x20, 0x0023, 0x00BB); /* ключить мигание		*/
//	}
//}

//
//	volatile uint16_t value_from_register = 0;
//	volatile oven_session_status_t oven_session_status = oven_UnknownERR;
//	oven_session_status = f_ovenSMI2_TASKS_17 (0x20);
//
//	oven_session_status = oven_UnknownERR;
//	HAL_Delay (500);
//	oven_session_status = f_ovenSMI2_TASKS_03 (0x20, 19, &value_from_register);
//
//	oven_session_status = oven_UnknownERR;
//	HAL_Delay (500);
//	oven_session_status = f_ovenSMI2_TASKS_03 (0x20, 8, &value_from_register);

	// f_MB_PC_DiagnosticFE(0x01, msg_number,0);
	//	if (msg_number==255){
	//		msg_number=0;
	//	}
	//	else {
	//		msg_number++;
	//	}

//	tk2_remote_control_LOOP ();


//	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_RESET);
//	PARAM_Cycle ();
//	f_MBtk2_Diagnostic_108(0x01);
//	HAL_Delay(100);
//iwdgDuration=HAL_GetTick()-iwdgTick;
//uint8_t tt[17]={0};
//snprintf(tt, 17, "%6d",iwdgDuration);
//WH1602b3_WriteHighString(tt);

//		  f_PutErrRecord ((int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__, (uint32_t)tkMAIN_UNKNOWN_Err);
//		  f_PutErrRecord ((int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__, (uint32_t)tkMAIN_UNKNOWN_Err);
//		  f_PutErrRecord ((int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__, (uint32_t)tkMAIN_UNKNOWN_Err);
//
//		  f_PutErrRecord ((int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__, (uint32_t)tkMAIN_UNKNOWN_Err);
//		  f_PutErrRecord ((int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__, (uint32_t)tkMAIN_UNKNOWN_Err);
//		  f_PutErrRecord ((int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__, (uint32_t)tkMAIN_UNKNOWN_Err);
//
//		  f_PutErrRecord ((int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__, (uint32_t)tkMAIN_UNKNOWN_Err);
//		  f_PutErrRecord ((int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__, (uint32_t)tkMAIN_UNKNOWN_Err);
//		  f_PutErrRecord ((int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__, (uint32_t)tkMAIN_UNKNOWN_Err);

    /* Це невдала спроба поблимати світлодіодом на платі  */
	if ((loop_counter % 2) == 0){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	}
	if (loop_counter==1200){loop_counter=0;}

	loopDuration = HAL_GetTick()-loopTick;

//#ifdef FURIE_TEST
//	f_vk1000_Loop();
//	f_GRD_Calc();
//#endif //FURIE_TEST
//	f_0UI_Spv();

//	loopDuration = HAL_GetTick()-loopTick;
//	uint32_t ms_to_wait = 99-loopDuration;
//	if (loopDuration<99){
//		HAL_Delay (ms_to_wait);
//	}
//	loopDuration = HAL_GetTick()-loopTick;
//	loopSDuration+=loopDuration;
	return 0;
}

