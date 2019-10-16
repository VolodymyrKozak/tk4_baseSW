/*
 * vv300factory_set.c
 *
 *  Created on: 9 лют. 2019 р.
 *      Author: KozakVF
 */

#include "vv300set.h"


uint32_t fc32set[N_FCSET / 2 +2] = {0};
uint16_t *fcset =(uint16_t*)(fc32set);
/* 2) визначаємо масив структур u16menu_orn_t, по елементу для кожної уставки:*/

u16menu_orn_t fc[N_FCSET]={
        				 { "koef uC", 	"k_uC",		"",	1000,  1, 1},
    					 { "koef iA", 	"k_iA",		"",	1000,  1, 1},
						 { "koef iB", 	"k_iB",		"",	1000,  1, 1},
						 { "koef iC", 	"k_iC",		"",	1000,  1, 1}
};
     /*  присвоюємо значення елементам*/


void f_FSsetInit(void){
    fcset[0] = 127;
    fcset[1] = 127;
    fcset[2] = 127;
    fcset[3] = 127;
}
flash_status_t f_VV300_FactorySets_Read(void){
	return f_VV_Sets_Read(   fc32set,
						     sizeof(fc32set),
							 FLASH_PAGE_FACILILY_SETS_A,
							 FLASH_PAGE_FACILILY_SETS_B
							);
}
flash_status_t f_VV300_FactorySets_Write(void){
	return f_VV_Sets_Write(   fc32set,
						     sizeof(fc32set),
							 FLASH_PAGE_FACILILY_SETS_A,
							 FLASH_PAGE_FACILILY_SETS_B
							);
}
