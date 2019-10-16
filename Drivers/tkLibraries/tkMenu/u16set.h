/*
 * u16set.h
 *
 *  Created on: 20 лют. 2019 р.
 *      Author: KozakVF
 */

#ifndef TKLIBRARIES_TKMENU_U16SET_H_
#define TKLIBRARIES_TKMENU_U16SET_H_
#include "stdint.h"



typedef enum{
	u16menu_unknown_state,
	u16menu_input,
	u16menu_inputBTN,
	u16menu_Next,
	u16menu_Prev,
	u16menu_setting,
	u16menu_escape,
	u16menu_valuePreSet,
	u16menu_valueSet,
}u16menu_state_t;

typedef struct {
	uint8_t value_name[17];
	uint8_t left_sign[4];
	uint8_t unit_name [4];
	int16_t MAX_value;
	int16_t MIN_value;
	int8_t  flag16;

	uint32_t blinkTick;
}u16menu_orn_t;

void f_u16menu_Init(void);
u16menu_state_t f_u16menu(		u16menu_orn_t *u16g,
								uint16_t start_value,
								uint16_t *p_out_value
								);
typedef	enum {
		ARRu16dynamic,
		ARRu16waiting,
		ARRu16,
		ARRu16exit,
		ARRu16cmplt
	} ARRu16_state_t;
void f_ARRu16menu_Init(void);
ARRu16_state_t f_ARRu16menu (u16menu_orn_t *pu, uint16_t *puset, uint16_t nArray);




#endif /* TKLIBRARIES_TKMENU_U16SET_H_ */
