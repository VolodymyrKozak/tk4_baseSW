/*
 * uList_set.h
 *
 *  Created on: 3 бер. 2019 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKLIBRARIES_TKMENU_ULIST_SET_H_
#define TKLIBRARIES_TKMENU_ULIST_SET_H_
#include "stdint.h"
typedef enum{
	LISTmenu_unknown_state,
	LISTmenu_input,
	LISTmenu_inputBTN,
	LISTmenu_Next,
	LISTmenu_Prev,
	LISTmenu_setting,
	LISTmenu_escape,
	LISTmenu_valuePreSet,
	LISTmenu_valueSet,
	LISTmenu_WrongData
}LISTmenu_state_t;

void f_listmenu_Init(void);

LISTmenu_state_t f_listmenu(    int8_t *uName,            /* показчик на назву параметру*/
								int8_t *uList,            /* показчик на список рядків*/
								uint8_t number_of_records,/* кількість елементів у списку*/
								uint8_t start_number,	  /* початковий елемент*/
								uint8_t *p_out_number);	  /* показчик на вибраний елекмент */






#endif /* TKLIBRARIES_TKMENU_ULIST_SET_H_ */
