/*
 * uList_set.c
 *
 *  Created on: 3 бер. 2019 р.
 *      Author: Volodymyr.Kozak
 */
#include "uList_set.h"
#include "zFF.h"
#include "string.h"

LISTmenu_state_t LISTmenu_state = LISTmenu_unknown_state;
static uint8_t current_number=0;
static uint32_t ListTick=0;
void f_listmenu_Init(void){
	LISTmenu_state = LISTmenu_input;
	current_number=0;
	ListTick=0;
}

LISTmenu_state_t f_listmenu(    int8_t *uName,            /* показчик на назву параметру*/
								int8_t *uList,            /* показчик на список рядків*/
								uint8_t number_of_records,/* кількість елементів у списку*/
								uint8_t start_number,	  /* початковий елемент*/
								uint8_t *p_out_number){	  /* показчик на вибраний елекмент */


// 	/*************************************************************
// 	 * ІНІЦІАЛІЗАЦІЯ
// 	 * ************************************************************/
 	if (LISTmenu_state ==  LISTmenu_input){
 		if ((uList==NULL)||(p_out_number==NULL)){
 			LISTmenu_state=LISTmenu_WrongData;
 		}
 		else if ((sizeof(uList)/number_of_records)!=16){
 			LISTmenu_state=LISTmenu_WrongData;
 		}
 		else{
 			snprintf((char*)strHigh_menu, 16,"%s", uName);
 			tkDisplay_PutHighStr(strHigh_menu);
 			current_number=start_number;
 			snprintf((char*)strLow_menu, 16,"%s", uList+16*start_number);
 			tkDisplay_PutLowStr(strLow_menu);
 			LISTmenu_state = LISTmenu_inputBTN;
 		}
 	}
 	/********************************************************************
 	 * користувач вибирає, чи потрібно корегувати цю уставку
 	 * ******************************************************************/
 	else if (LISTmenu_state == LISTmenu_inputBTN){
 		if  (BTN==ENTER_btn){
 			LISTmenu_state = LISTmenu_setting;
 			/* для мигання */
 			memset (strLow_menu, 0, sizeof(strLow_menu));
 			ListTick = HAL_GetTick();
 		}
 		else if (BTN==UP_btn){
 			LISTmenu_state = LISTmenu_Next;
 		}
 		else if (BTN==DOWN_btn){
 			LISTmenu_state = LISTmenu_Prev;
 		}
 		else if (BTN==ESC_btn) {
 			LISTmenu_state=LISTmenu_escape;
 		}
 		else{}
 	}
 	/*************************************************************
 	 * ВИБІР ЗНАЧЕННЯ
 	 * ************************************************************/
 	else if (LISTmenu_state == LISTmenu_setting){
 		if (BTN==ESC_btn){
 			LISTmenu_state=LISTmenu_escape;
 		}
 		else if (BTN==DOWN_btn){
 			if (current_number>0){
 				current_number--;
 			}
 			else{
 				current_number = number_of_records-1;
 			}
 			snprintf((char*)strLow_menu, 16,"%s", uList+16*current_number);
 		}
 		else if (BTN==UP_btn){
 			current_number++;
 			if (current_number==number_of_records){
 				current_number=0;
 			}
 			snprintf((char*)strLow_menu, 16,"%s", uList+16*current_number);
 		}

 	 	/*************************************************************
 	 	 * ПОВЕРНЕННЯ ВИБРАНОГО ЗНАЧЕННЯ
 	 	 * ************************************************************/
 		else if (BTN==ENTER_btn){
 			(*p_out_number)=current_number;
 			LISTmenu_state=LISTmenu_valuePreSet;
 		}
 	 	/*************************************************************
 	 	 * ВИХІД ПО ВІДМІНІ
 	 	 * ************************************************************/
 		else if (BTN==ESC_btn){
 			LISTmenu_state=LISTmenu_escape;
 		}
		else{}
 	 	/*************************************************************
 	 	 *  blinking low string value
 	 	 * ************************************************************/
 		uint32_t blinkDur = HAL_GetTick()-ListTick;
 		if (blinkDur<500){
 			tkDisplay_PutLowStr(strLow0_menu);
 		}
 		else if ((blinkDur>=500)&&(blinkDur<1000)){
 			tkDisplay_PutLowStr(strLow_menu);
 		}
 		else {
 			ListTick=HAL_GetTick();
 		}
 	}
	 /*************************************************************
	 *  STOP blinking low string value
	 * ************************************************************/
 	else if (LISTmenu_state==LISTmenu_valuePreSet){
 		tkDisplay_PutLowStr(strLow_menu);
 		if (BTN==ESC_btn){
 			LISTmenu_state=LISTmenu_escape;
 		}
 		if (BTN==ENTER_btn){
 			LISTmenu_state=LISTmenu_valueSet;
 		}
 	}
// 	else if (u16menu_state==u16menu_valueSet){
// 		(*p_out_value)=current_value_u16;
// 	}
// 	else if (u16menu_state==u16menu_escape){
// 		;
// 	}
 	else{}
	return LISTmenu_state;
}


