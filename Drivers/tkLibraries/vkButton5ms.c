///*
// * vkButton5ms.c
// *
// *  Created on: 26 лют. 2019 р.
// *      Author: Volodymyr.Kozak
// */
//
//#ifndef TKLIBRARIES_VKBUTTON5MS_C_
//#define TKLIBRARIES_VKBUTTON5MS_C_
//
//#include <vkButtonsADC.h>
//#include "string.h"
//#include "tkConfig.h"
//
///* DEBOUNCING PROTOTYPE see:
//https://pubweb.eng.utah.edu/~cs5780/debouncing.pdf
//   DETECT SHORT_LONG_BUTTON_PRESS see:
//https://www.baldengineer.com/detect-short-long-button-press.html
//*/
//
//
//
//
//static btn_limit_t L5ms = {0};
//static i_btn_t i5ms_btn={0};
//
//static int BTN_Pool5ms_Init( btn_limit_t* l);
//
//int f_BTNs_Pool5ms_Init(void){
//	BTN_Pool5ms_Init(&L5ms);
//	memset((&i5ms_btn), 0, sizeof(i5ms_btn));
//	i5ms_btn.s=pool_btw_btn_state;
//	return 0;
//}
//button_output_t f_BTN_Pool5ms (uint32_t adc_buttons_pool) {
//	/* ОПИТУВАННЯ КНОПОК ДИСПЛЕЯ */
//	button_output_t out_button_pushed = NONE_btn;
//	button_output_t iPool = NONE_btn;
//	iPool = buttonsADCpool(adc_buttons_pool);
//
//	if (i5ms_btn.s == pool_btw_btn_state){
//
//		if (iPool == NONE_btn){
//			i5ms_btn.iBtw++;
//			if (i5ms_btn.iBtw > 0xFFFFFFF0){
//				i5ms_btn.iBtw=0;
//			}
//		}
//		else if (iPool != NONE_btn){
//			i5ms_btn.sPool=iPool;
//			if (i5ms_btn.iBtw > L5ms.LBtw){
//				i5ms_btn.s = pool5s_before_start_state;
//				i5ms_btn.iYes++;
//			}
//			else{
//				i5ms_btn.iBtw++;
//			}
//		}
//		else{}
//	}
//	else if (i5ms_btn.s == pool5s_before_start_state){
//		if (i5ms_btn.sPool != iPool){
//			i5ms_btn.iYes=0;
//			i5ms_btn.s = pool_btw_btn_state;
//		}
//		else if (i5ms_btn.sPool == iPool){
//			i5ms_btn.iYes++;
//			i5ms_btn.s = pool_started_state;
//		}
//		else{}
//	}
//	/* Начался подсчет голосов :-) */
//	else if (i5ms_btn.s == pool_started_state){
//		if (iPool != i5ms_btn.sPool){
//			i5ms_btn.iReset++;
//			if (i5ms_btn.iReset>L5ms.LReset){
//				out_button_pushed=NONE_btn;
//				i5ms_btn.iReset=0;
//				i5ms_btn.iYes=0;
//				i5ms_btn.s = pool_btw_btn_state;
//			}
//		}
//		else if (iPool == i5ms_btn.sPool){
//			i5ms_btn.iYes++;
//			if (i5ms_btn.iYes>L5ms.LYes){
//				i5ms_btn.s = pool5s_before_LongPressed_state;
//				/* Набрано достатньо голосів. Щось є.*/
//				/* Це точно Pushed але чи LONG Pushed ? */
//				out_button_pushed=i5ms_btn.sPool;
////				prevBtnPressed   =i5ms_btn.sPool;
//			}
//		}
//		else{}
//	}
//	else if (i5ms_btn.s==pool5s_before_LongPressed_state){
//		if (iPool != i5ms_btn.sPool){
//			i5ms_btn.iReset++;
//			if (i5ms_btn.iReset>(L5ms.LReset)){
//				/* Це точно  PRESSED ! */
//				out_button_pushed=i5ms_btn.sPool;
//				i5ms_btn.s=pool_btw_btn_state;
//				memset(&i5ms_btn, 0, sizeof(i5ms_btn));
//			}
//		}
//		else if (iPool == i5ms_btn.sPool){
//			i5ms_btn.iYes++;
//			if (i5ms_btn.iYes>(L5ms.LYes*L5ms.xLYes)){
//				i5ms_btn.s = pool_LongPressed_state;
//				/* Це точно  LONG Pressed ! */
//				uint32_t tmpau = ((uint32_t)i5ms_btn.sPool) + 4;
//				out_button_pushed=(button_output_t)tmpau;
//				i5ms_btn.xLongBP=2;
//			}
//		}
//		else{}
//	}
//
//	else if (i5ms_btn.s==pool_LongPressed_state){
//		if (iPool != i5ms_btn.sPool){
//			i5ms_btn.iNo++;
//			if (i5ms_btn.iNo>(L5ms.LNo*i5ms_btn.xLongBP)){
//				/* закінчився LONG Pressed */
//				out_button_pushed=NONE_btn;
//				i5ms_btn.s = pool_btw_btn_state;
//				memset(&i5ms_btn, 0, sizeof(i5ms_btn));
//			}
//		}
//		else if (iPool == i5ms_btn.sPool){
//			i5ms_btn.iYes++;
//			if(i5ms_btn.iYes>(L5ms.LYes*i5ms_btn.xLongBP)){
//				/* LONG Pressed продовжується*/
//				uint32_t tmpau = ((uint32_t)i5ms_btn.sPool) + 4;
//				out_button_pushed=(button_output_t)tmpau;
//				i5ms_btn.xLongBP++;
//			}
//		}
//		else{}
//	}
//	else{}
//	return out_button_pushed;
//}
//
///********************************************************************************************************************
// *  ПРИВАТНі ФУНКЦІЇ
// ********************************************************************************************************************/
//static int BTN_Pool5ms_Init( btn_limit_t* l){
//
//		l->LBtw			= 10;  //l->LLONGYes;
//		l->LBeforeStart = 2;  /* кількість так до початку фіксації так;*/
//		l->LYes 		= 10;  //(uint8_t)(btn_pool_sec*10.0f*0.6f);
//		l->LNo 		    = 2;  //(uint8_t)(btn_pool_sec*10.0f*0.3f);
//		l->LReset 		= 5;  //(uint8_t)(btn_pool_sec*10.0f*0.5f);
//		l->xLYes        = (LONGBUTTONms/l->LYes)/5;/*10*/
//	return 0;
//}
//
//uint8_t FlagBTNdb =0;
//volatile uint16_t btnArr[100]={0};
//uint8_t iBTNi=0;
//button_output_t buttonsADCpool(uint32_t adc_buttons_pool){                                            /* опитування АЦП кнопок контролера */
////
//	button_output_t button_output = NONE_btn;
////  uint32_t adc_buttons_pool = ADC1_GetValue(FROM_BUTTONS_ADC_INPUT);
//
//	if (adc_buttons_pool<=400){
//		button_output =NONE_btn;
//	}
//	else if ((adc_buttons_pool > 400)  && (adc_buttons_pool <= 1200)) {
//		button_output=ESC_btn;;
//	}
//	else if ((adc_buttons_pool > 1200) && (adc_buttons_pool <= 2000)) {
//		button_output=UP_btn;
//	}
//	else if ((adc_buttons_pool > 2000) && (adc_buttons_pool <= 2900)) {
//		button_output=DOWN_btn;
//		}
//	else if ((adc_buttons_pool > 2900) && (adc_buttons_pool <= 4095)) {
//		button_output=ENTER_btn;
//		}
//	else {}
//
//if (adc_buttons_pool>200){
//	FlagBTNdb=1;
//}
//if(FlagBTNdb==1){
//	btnArr[iBTNi]=adc_buttons_pool;
//	iBTNi++;
//	if (iBTNi==100){
//		HAL_Delay(1);
//		iBTNi=0;
//		memset(&btnArr, 0, sizeof(btnArr));
//
//	}
//}
//	return button_output;
//
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
////
////
////#define CHECK_MSEC 5 		// Read hardware every 5 msec
////#define PRESS_MSEC 10 		// Stable time before registering pressed
////#define RELEASE_MSEC 100 	// Stable time before registering released
////
////// This function reads the key state from the hardware.
////extern bool_t RawKeyPressed();
////// This holds the debounced state of the key.
////bool_t DebouncedKeyPress = false;
////// Service routine called every CHECK_MSEC to
////// debounce both edges		0             0
////void DebounceSwitch1(bool_t *Key_changed, bool_t *Key_pressed){
////	static uint8_t Count = RELEASE_MSEC / CHECK_MSEC;
////	bool_t RawState;
////	*Key_changed = false; /* 0 */
////	*Key_pressed = DebouncedKeyPress; /* 0 */
////	RawState = RawKeyPressed(); /* 0 */
////	if (RawState == DebouncedKeyPress) { /* 0 */
////		// Set the timer which allows a change from current state.
////		if (DebouncedKeyPress /*0*/) Count = RELEASE_MSEC / CHECK_MSEC;
////		else Count = PRESS_MSEC / CHECK_MSEC;
////	}
////	else {
////		// Key has changed - wait for new state to become stable.
////		if (--Count == 0) {
////			// Timer expired - accept the change.
////			DebouncedKeyPress = RawState;
////			*Key_changed=true;
////			*Key_pressed=DebouncedKeyPress;
////			// And reset the timer.
////			if (DebouncedKeyPress) Count = RELEASE_MSEC / CHECK_MSEC; /*20*/
////			else Count = PRESS_MSEC / CHECK_MSEC;					  /*  2*/
////		}
////	}
////}
////
//#endif /* TKLIBRARIES_VKBUTTON5MS_C_ */
