/*
 * tk0spv.c
 *
 *  Created on: 24 лют. 2019 р.
 *      Author: Volodymyr.Kozak
 */
#include "tk0spv.h"
#include "tk1spv_grid.h"
#include "tk2spv_set.h"
#include "zFF.h"

typedef	enum {
		UI_unknown_state,

		UI_start_state,
		UI_startBTN,

		UI_grid_param_state,
		UI_grid_paramBTN_state,
		UI_grid_param_dynamic,

		UI_device_setting_state,
		UI_device_settingBTN_state,
		UI_device_setting_dynamic

} ui_state_t;



static ui_state_t UI_state=UI_start_state;
static uint32_t uiTick=0;

volatile button_output_t BTNdebug = NONE_btn;

spv_status_t f_0UI_Init(void){
//	f_BTNs_Pool_Init();
	f_BTNs_PoolSmooth_Init();
	tkDisplay_Init();
	/* Clearning */
	UI_state = UI_start_state;
	f_1UIgrid_Spv_Init();
	return spv_OK;
}

spv_status_t f_0UI_Spv(void){
	volatile spv_status_t spv_status =  spv_UNKNOWN_Err;

	if (BTN !=NONE_btn){
		BTNdebug = BTN;
	}


	if (BTN!=NONE_btn){
		/* Старт відліку часу ля повернення до основного екрану
		 * в рызы выдсутносты активносты протягом заданого часу*/
		uiTick = HAL_GetTick();
	}

	if( UI_state== UI_start_state){
		if (tkDisplay_ClearScreen() != dsp_OK){
			spv_status =  dsp_wo1602gErr;
		}
		int8_t tk[]="ТОВ 'Tехноконт'";
		if (tkDisplay_PutHighStr(tk) != dsp_OK){
			spv_status =  dsp_wo1602gErr;
		}
		if (tkDisplay_PutLowStr((int8_t*)("ТK300 v0.v0")) != dsp_OK){
			spv_status =  dsp_wo1602gErr;
		}
		UI_state = UI_startBTN;
	}
	else if (UI_state == UI_startBTN){
 		if  (BTN==ENTER_btn){
 			UI_state = UI_grid_param_state;
 			BTN_Delay=900;
 		}
	}
	else if (UI_state == UI_grid_param_state){

		if (tkDisplay_PutHighStr((int8_t*)("ПАРАМЕТРИ")) != dsp_OK){
			spv_status =  dsp_wo1602gErr;
		}
		if (tkDisplay_PutLowStr ((int8_t*)("ЕНЕРГОСПОЖИВАННЯ")) != dsp_OK){
			spv_status =  dsp_wo1602gErr;
		}
		UI_state =UI_grid_paramBTN_state;
	}
	else if	(UI_state ==UI_grid_paramBTN_state)	{
 		if  (BTN==ENTER_btn){
 			f_1UIgrid_Spv_Init();
 			UI_state=UI_grid_param_dynamic;
 			BTN_Delay=900;
 		}
 		else if  ((BTN==DOWN_btn)||(BTN==UP_btn)){
 			UI_state = UI_device_setting_state;
 			BTN_Delay=900;
 		}
 		else if  (BTN==ESC_btn){
 			UI_state = UI_start_state;
 			BTN_Delay=900;
 		}
 		else{}
	}
	else if (UI_state==UI_grid_param_dynamic){
		uigrid_state_t fiug =f_1UIgrid_Spv();
		if (fiug == UIgrid_exit_state){
			UI_state = UI_grid_param_state;
		}
	}
	else if (UI_state == UI_device_setting_state){
		if (tkDisplay_PutHighStr((int8_t*)("НАЛАШТУВАННЯ")) != dsp_OK){
			spv_status =  dsp_wo1602gErr;
		}
		if (tkDisplay_PutLowStr ((int8_t*)(" ")) != dsp_OK){
			spv_status =  dsp_wo1602gErr;
		}
		UI_state =UI_device_settingBTN_state;
	}
	else if (UI_state == UI_device_settingBTN_state){
 		if  (BTN==ENTER_btn){
 			f_1UIset_Spv_Init();
			UI_state = UI_device_setting_dynamic;
			BTN_Delay=900;
 		}
 		else if  ((BTN==DOWN_btn)||(BTN==UP_btn)){
 			UI_state = UI_grid_param_state;
 			BTN_Delay=900;
 		}
 		else if  (BTN==ESC_btn){
 			UI_state = UI_start_state;
 			BTN_Delay=900;
 		}
 		else{}
	}
	else if (UI_state == UI_device_setting_dynamic){
		uiset_state_t uiset = f_1UIset_Spv();
		if (uiset==UIset_exit_state){
			UI_state = UI_device_setting_state;
		}
	}
	else{}




	if (BTN!=NONE_btn){
		if ((HAL_GetTick()-BTN_DelayTick)<BTN_Delay){
			BTN=DELAY_btn;
		}
		else{
			BTN=NONE_btn;
		}
		uiTick = HAL_GetTick();
	}
	/* якщо жодна клавiша не натискалася понад 20хв*/
	if((HAL_GetTick()-uiTick)>12000000){
		UI_state = UI_start_state;
		/* Clearing */
	}


	return spv_status;

}


