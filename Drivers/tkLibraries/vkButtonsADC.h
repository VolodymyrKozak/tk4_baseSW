/*
 * 	tkButtonsADC.h
 *  Created on: 1 лют. 2018 р.
 *  Author: Volodymyr.Kozak
    ОПИТУВАННЯ КНОПОК МІКРОКОНТРОЛЕРА (ДИСПЛЕЯ)
 *  (зчитування стану кнопок здійснюється через один вхід АЦП, для кожної кнопки - на вході свій рівень АЦП сигналу)
 *  При утримуванні кнопки фіксується послідовність натискань
 *  Максимальна швидкість - 2 натискання в секунду
 */
#ifndef TKBUTTONSADC_H_
#define TKBUTTONSADC_H_

#include "stdint.h"

/********************************************************************************************************************
 *  ПУБЛІЧНІ ОГОЛОШЕННЯ
 ********************************************************************************************************************/

typedef enum {            /*перелічуваний набір всіх можливих цілочисельних значень результату роботи публічної функції button_pool() */
	NONE_btn,
	DELAY_btn,

	ESC_btn,
	UP_btn,
	DOWN_btn,
	ENTER_btn,

	ESCLong_btn,
	UPLong_btn,
	DOWNLong_btn,
	ENTERLong_btn

} button_output_t;
extern button_output_t BTN;
extern uint32_t BTN_DelayTick;
extern uint32_t BTN_Delay;

extern uint16_t iBTNdiv;
typedef enum {
	pool_btw_btn_state,
	pool5s_before_start_state,
	pool_started_state,
	pool_Btn_state,
	pool5s_before_LongPressed_state,
	pool_LongPressed_state,

	pool_calm_down,
	pool_calm_down_wait,

	pool_exit_state
} btn_pool_state_t;

typedef struct {
	uint8_t  iReset;
	uint8_t  iYes;
	uint32_t iBtw;
	uint8_t  iNo;
	uint8_t  xLongBP;
	btn_pool_state_t s;
	button_output_t sPool;
} i_btn_t;

/* Теорія: фіксація коротокого натискання кнопки - 100 мсек,
 *         фіксація довгого натискання кнопки    - 500мсек
 * Кнопки опитуюються через кожні приблизно 2..7 мсек мс, за 100мсек - десь 17-19 опитувань кнопок
 * Після попереднього натискання кнопки повинно пройти */
typedef struct {
	uint32_t LBtw;  		/* мервий час після попереднього натискання кнопки, не варто змінювати */
							/* В разі необхідності мертвого часу після конкретного натискання кнопки меню
							 * використовувати BTN_Delay*/
	uint8_t  LBeforeStart;  /* кількість так до початку фіксації так;*/
	uint8_t  LYes; 			/* кількість так до рішення про короткий PRESSED */
	uint8_t  LNo;			/* кількість ні після так.. */
	uint8_t  LReset;		/* кількість ні для фіксації помилки короткого натисння           */
    uint8_t  xLYes;         /* кратність LYes для підтягування підрахунків до PRESSED чи LONGPRESSED*/
    						/* LYes x xLYyes - кількість Yes до прийняття рішення PRESSED чи LONGPRESSED */
    						/* 100мсек - це 20 тактів, 500мсек - це 100 тактів*/
    						/* якщо  xLYes=5 LYes=10, - це 50 Yes, тобто приблизно 250мсек*/
    						/* якщо  xLYes=10 - це 250 мсек
    						 * xLYes вибирати в залежнотсі від застосування */
} btn_limit_t;




 /* */
/********************************************************************************************************************
 *  ПРИВАТНі ОГОЛОШЕННЯ
 ********************************************************************************************************************/


/********************************************************************************************************************
 *  ПУБЛІЧНА ФУНКЦІЯ ОПИТУВАННЯ КНОПОК
 ********************************************************************************************************************/
int f_BTNs_Pool_Init(void);
button_output_t f_BTN_Pool (uint32_t adc_buttons_pool);
button_output_t buttonsADCpool(uint32_t adc_buttons_pool);
void f_btn_debug (button_output_t btn);

int f_BTNs_Pool5ms_Init(void);
button_output_t f_BTN_Pool5ms (uint32_t adc_buttons_pool);


int f_BTNs_PoolSmooth_Init(void);
void f_BTN_PoolSmooth (uint32_t adc_buttons_pool);

#endif /* TKBUTTONSADC_H_ */
