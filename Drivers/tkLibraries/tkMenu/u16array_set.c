/*
 * u16array_set.c
 *
 *  Created on: 1 бер. 2019 р.
 *      Author: Volodymyr.Kozak
 */


#include "u16set.h"





static ARRu16_state_t ARRu16_state=ARRu16dynamic;
static uint8_t iARR =0;


void f_ARRu16menu_Init(void){
	iARR=0;
	ARRu16_state=ARRu16dynamic;
	f_u16menu_Init();

}

ARRu16_state_t f_ARRu16menu (u16menu_orn_t *pu, uint16_t *puset, uint16_t nArray){
	if (iARR==nArray){
		ARRu16_state = ARRu16cmplt;
	}
	else if (ARRu16_state == ARRu16dynamic) {
		uint16_t tmp16 = (*(puset+2*iARR));
		u16menu_state_t um1=u16menu_unknown_state;
        um1 = f_u16menu(
				   &pu[iARR],
				   tmp16,
				   puset+2*iARR
				);
        if (um1==u16menu_escape){
        	f_u16menu_Init();
        	ARRu16_state = ARRu16cmplt;
        }
        else if (um1==u16menu_valueSet){
        	iARR++;f_u16menu_Init();
        }
        else if (um1==u16menu_Next){
        	iARR++;f_u16menu_Init();
        }
        else {};
	}
	return ARRu16_state;
}



