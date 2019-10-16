/*
 * vkButtonSmooth.c
 *
 *  Created on: 28 лют. 2019 р.
 *      Author: Volodymyr.Kozak
 */


#include <vkButtonsADC.h>
#include "string.h"
#include "tkConfig.h"
#include "stm32f1xx_hal.h"
#include "tkSGL.h"


button_output_t BTN = NONE_btn;
uint32_t BTN_DelayTick = 0;
uint32_t BTN_Delay =0;
uint16_t iBTNdiv = 1;



//	iSmooth
static btn_pool_state_t Sth = pool_btw_btn_state;
static uint32_t btnADCsmoothed = 0;
static uint32_t prev_btnADCsmoothed = 0;
static uint32_t ibtnCount = 0;
static volatile button_output_t short_btn = NONE_btn;
static volatile button_output_t long_btn = NONE_btn;


static volatile uint16_t adcArr[200]={0};
static uint16_t iADCh = 0;

#ifdef OFFICEBUTTONS
static button_output_t f_ADCbtn_office (uint32_t adc_buttons_pool);
#endif //OFFICEBUTTONS
#ifdef HOMEBUTTONS
static button_output_t f_ADCbtn_home (uint32_t adc_buttons_pool);
#endif //HOMEBUTTONS

int f_BTNs_PoolSmooth_Init(void){
	btnADCsmoothed = 0;
	ibtnCount = 0;
	prev_btnADCsmoothed = 0;
	Sth = pool_btw_btn_state;
	short_btn = NONE_btn;
	long_btn = NONE_btn;
	return 0;
}
void f_BTN_PoolSmooth (uint32_t adc_buttons_pool) {
	/* ОПИТУВАННЯ КНОПОК ДИСПЛЕЯ */
	/* Постійно щопять мсек опитуємо ацк від кнопок і зглажуємо значення, щоб
	 * ігнорувати випадкові стрибки значення ацп*/
	btnADCsmoothed=f_btnExpSmoothing(
		adc_buttons_pool,            	 /* поточне значення вимірювання, од */
		prev_btnADCsmoothed, 			 /* попереднє зглажене значення, од  */
		3				                 /* ПОСТІЙНА ЗГЛАЖУВАННЯ В кількості відліків*/
		);
	prev_btnADCsmoothed=btnADCsmoothed;
	/* ацп < 400, стан - тишина, не зафіксованого жодного натискування жодної кнопки */
	if (Sth==pool_btw_btn_state){
		if ((HAL_GetTick() - BTN_DelayTick)<BTN_Delay){
			Sth=pool_calm_down;
		}
		/* якщо виявили зростання АЦП - починаємо відлік */
		else if (btnADCsmoothed>400){
			Sth=pool_started_state;
			ibtnCount=0;
			/* запис в пам'ять відліків ацп для відлагоджування */
			int8_t *adcArr_r = 0;
			adcArr_r=(int8_t*)adcArr;
			memset (adcArr_r, 0, sizeof(adcArr));
			iADCh = 0;
		}
	}
	else if (Sth==pool_started_state){
		/* запис в пам'ять відліків ацп для відлагоджування */
		if (iADCh<200){
			adcArr[iADCh]=adc_buttons_pool;
			iADCh++;
		}
#ifdef HOMEBUTTONS
		short_btn = f_ADCbtn_home (adc_buttons_pool);
#endif 	//HOMEBUTTONS
#ifdef OFFICEBUTTONS
		short_btn = f_ADCbtn_office (adc_buttons_pool);
#endif 	//HOMEBUTTONS
		ibtnCount++;
		if (ibtnCount==40){
			if (short_btn !=NONE_btn){
				BTN=short_btn;
				Sth=pool5s_before_LongPressed_state;
			}
			else {
				Sth=pool_calm_down;
			}
			ibtnCount=0;
		}
	}
	else if (Sth==pool5s_before_LongPressed_state){
		/* запис в пам'ять відліків ацп для відлагоджування */
		if (iADCh<200){
			adcArr[iADCh]=adc_buttons_pool;
			iADCh++;
		}




		ibtnCount++;
#ifdef HOMEBUTTONS
		long_btn = f_ADCbtn_home (adc_buttons_pool);
#endif 	//HOMEBUTTONS
#ifdef OFFICEBUTTONS
		long_btn = f_ADCbtn_office (adc_buttons_pool);
#endif 	//HOMEBUTTONS
		if (ibtnCount == 21){
		    if	(long_btn != short_btn){
				ibtnCount=0;
				Sth=pool_calm_down;
			}
		}
		if (ibtnCount == 100){
			if (long_btn == short_btn){
				uint32_t tmpsh=(uint32_t)short_btn;
				BTN=(button_output_t)(tmpsh+4);
			}
			else {
				Sth=pool_calm_down;
			}
			ibtnCount=0;
		}
	}
	else if (Sth==pool_calm_down){
		long_btn = NONE_btn;
		short_btn = NONE_btn;

		iADCh=0;

		ibtnCount=0;
		if (adc_buttons_pool<=400){
			Sth=pool_btw_btn_state;
		}
		else {
			Sth==pool_calm_down_wait;
		}
	}
	else if (Sth==pool_calm_down_wait){
		ibtnCount++;
		if (ibtnCount>20){
			if (adc_buttons_pool<=400){
				ibtnCount=0;
				Sth=pool_btw_btn_state;
			}
		}
	}
	else {}

}


#ifdef OFFICEBUTTONS
static button_output_t f_ADCbtn_office (uint32_t adc_buttons_pool){
	button_output_t out_btn = NONE_btn;
		if (adc_buttons_pool<=700){
			out_btn =NONE_btn;
		}
		else if ((adc_buttons_pool > 700)  && (adc_buttons_pool <= 1700)) {
			out_btn=ESC_btn;;
		}
		else if ((adc_buttons_pool > 1700) && (adc_buttons_pool <= 2300)) {
			out_btn=UP_btn;
		}
		else if ((adc_buttons_pool > 2300) && (adc_buttons_pool <= 3100)) {
			out_btn=DOWN_btn;
			}
		else if ((adc_buttons_pool > 3100) && (adc_buttons_pool <= 4095)) {
			out_btn=ENTER_btn;
			}
		else {}
		return out_btn;
}
#endif //OFFICEBUTTONS

#ifdef HOMEBUTTONS
static button_output_t f_ADCbtn_home (uint32_t adc_buttons_pool){
	button_output_t out_btn = NONE_btn;
	if (adc_buttons_pool<=400){
		out_btn =NONE_btn;
	}
	else if ((adc_buttons_pool > 400)  && (adc_buttons_pool <= 1200)) {
		out_btn=ESC_btn;;
	}
	else if ((adc_buttons_pool > 1200) && (adc_buttons_pool <= 2000)) {
		out_btn=UP_btn;
	}
	else if ((adc_buttons_pool > 2000) && (adc_buttons_pool <= 2900)) {
		out_btn=DOWN_btn;
		}
	else if ((adc_buttons_pool > 2900) && (adc_buttons_pool <= 4095)) {
		out_btn=ENTER_btn;
		}
	else {}
	return out_btn;
}
#endif //HOMEBUTTONS
