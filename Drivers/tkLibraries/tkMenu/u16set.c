/*
 * u16set.c
 *
 *  Created on: 20 лют. 2019 р.
 *      Author: KozakVF
 */
#include "tkConfig.h"
#include "u16set.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <vkButtonsADC.h>
#include "stm32f1xx_hal.h"
#include "zFF.h"

#define u16BEFOR_STR 			(char)u16r->left_sign[0],(char)u16r->left_sign[1],(char)u16r->left_sign[2],(char)u16r->left_sign[3]
#define u16AFTER_STR 			(char)u16r->unit_name[0],(char)u16r->unit_name[1],(char)u16r->unit_name[2],(char)u16r->unit_name[3]


static uint16_t current_value_u16=0;
static uint16_t incru16=0;



volatile u16menu_state_t u16menu_state =  u16menu_input;

static void f_u16menu_LowStr(u16menu_orn_t *u16r);
static void f_u16menu_LowStr0(u16menu_orn_t *u16r);

void f_u16menu_Init(void){
	u16menu_state = u16menu_input;
}

u16menu_state_t f_u16menu(		u16menu_orn_t *u16g,
								uint16_t start_value,
								uint16_t *p_out_value){


// 	/*************************************************************
// 	 * ІНІЦІАЛІЗАЦІЯ
// 	 * ************************************************************/
 	if (u16menu_state ==  u16menu_input){

 		snprintf((char*)strHigh_menu, 17,"%s", u16g->value_name);
 		tkDisplay_PutHighStr(strHigh_menu);
 		current_value_u16=start_value;
 		f_u16menu_LowStr(u16g);
 		tkDisplay_PutLowStr(strLow_menu);
 		incru16 = (u16g->MAX_value - u16g->MIN_value)/100;
 		u16menu_state = u16menu_inputBTN;
 	}
 	/********************************************************************
 	 * користувач вибирає, чи потрібно корегувати цю уставку
 	 * ******************************************************************/
 	else if (u16menu_state == u16menu_inputBTN){
 		if  (BTN==ENTER_btn){/* потрібно коригувати */
 			u16menu_state = u16menu_setting;
 			f_u16menu_LowStr0(u16g);
 			u16g->blinkTick = HAL_GetTick();
 		}
 		else if (BTN==UP_btn){/* коригувати наступну*/
 			u16menu_state = u16menu_Next;
 		}
// 		else if (BTN==DOWN_btn){
// 			u16menu_state = u16menu_Prev;
// 		}
 		else if (BTN==ESC_btn) {/*вийти з коригування цієї уставки. КУДИ?*/
 			u16menu_state=u16menu_escape;
 		}
 		else{}
 	}
 	/*************************************************************
 	 * ВИБІР ЗНАЧЕННЯ
 	 * ************************************************************/
 	else if (u16menu_state == u16menu_setting){
 		if (BTN==ESC_btn){
 			u16menu_state=u16menu_escape;
 		}
 		else if (BTN==DOWN_btn){
 		    current_value_u16--;
 		    if  (current_value_u16 < u16g->MIN_value){
 		    	if (u16g->MIN_value==0){
 		    		current_value_u16=u16g->MIN_value;
 		    	}
 		    	else if(u16g->MIN_value==1){
 		    			current_value_u16=u16g->MAX_value;
 		    	}
 		    	else{}
 		    }
 		   f_u16menu_LowStr(u16g);
 		}
 		else if (BTN==DOWNLong_btn){
 			current_value_u16-=incru16;
 		    if  (current_value_u16 < u16g->MIN_value){
 		    	if (u16g->MIN_value==0){
 		    		current_value_u16=u16g->MIN_value;
 		    	}
 		    	else if(u16g->MIN_value==1){
 		    			current_value_u16=u16g->MAX_value;
 		    	}
 		    	else{}
 		    }
 		   f_u16menu_LowStr(u16g);
 		}
 		else if (BTN==UP_btn){
 		    current_value_u16++;
 		    if  (current_value_u16 > u16g->MAX_value){
 		    	if (u16g->flag16==0){
 		    		current_value_u16=u16g->MAX_value;
 		    	}
 		    	else if(u16g->flag16==1){
 		    			current_value_u16=u16g->MIN_value;
 		    	}
 		    	else{}
 		    }
 		   f_u16menu_LowStr(u16g);
 		}
 		else if (BTN==UPLong_btn){
 			current_value_u16+=incru16;
 		    if  (current_value_u16 > u16g->MAX_value){
 		    	if (u16g->flag16==0){
 		    		current_value_u16=u16g->MAX_value;
 		    	}
 		    	else if(u16g->flag16==1){
 		    			current_value_u16=u16g->MIN_value;
 		    	}
 		    	else{}
 		    }
 		   f_u16menu_LowStr(u16g);
 		}
 	 	/*************************************************************
 	 	 * ПОВЕРНЕННЯ ВИБРАНОГО ЗНАЧЕННЯ
 	 	 * ************************************************************/
 		else if (BTN==ENTER_btn){
 			(*p_out_value)=current_value_u16;
 			u16menu_state=u16menu_valuePreSet;
 		}
 	 	/*************************************************************
 	 	 * ВИХІД ПО ВІДМІНІ
 	 	 * ************************************************************/
 		else if (BTN==ESC_btn){
 			u16menu_state=u16menu_escape;
 		}
		else{}
 	 	/*************************************************************
 	 	 *  blinking low string value
 	 	 * ************************************************************/
 		uint32_t blinkDur = HAL_GetTick()-u16g->blinkTick;
 		if (blinkDur<500){
 			tkDisplay_PutLowStr(strLow0_menu);
 		}
 		else if ((blinkDur>=500)&&(blinkDur<1000)){
 			tkDisplay_PutLowStr(strLow_menu);
 		}
 		else {
 			u16g->blinkTick=HAL_GetTick();
 		}
 	}
	 /*************************************************************
	 *  STOP blinking low string value
	 * ************************************************************/
 	else if (u16menu_state==u16menu_valuePreSet){
 		tkDisplay_PutLowStr(strLow_menu);
 		if (BTN==ESC_btn){
 			u16menu_state=u16menu_escape;
 		}
 		if (BTN==ENTER_btn){
 			u16menu_state=u16menu_valueSet;
 		}
 	}
// 	else if (u16menu_state==u16menu_valueSet){
// 		(*p_out_value)=current_value_u16;
// 	}
// 	else if (u16menu_state==u16menu_escape){
// 		;
// 	}
 	else{}
	return u16menu_state;
}


 static void f_u16menu_LowStr(u16menu_orn_t *u16r){

 	uint8_t before=0;
 	if (current_value_u16<=0){before='-';}else {before=' ';}
 	uint16_t abs_current_value = abs(current_value_u16);

 	snprintf((char*)strLow_menu,17,  "%c%c%c%c %c%5d %c%c%c%c",
 								   u16BEFOR_STR,
 								   (char)before,
 								   (int)abs_current_value,
								   u16AFTER_STR
 								);
 }
 static void f_u16menu_LowStr0(u16menu_orn_t *u16r){
  	snprintf((char*)strLow0_menu,17,  "%c%c%c%c        %c%c%c%c",u16BEFOR_STR,u16AFTER_STR);
  }



