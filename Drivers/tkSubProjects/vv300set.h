/*
 * vv300_set.h
 *
 *  Created on: 9 лют. 2019 р.
 *      Author: KozakVF
 */

#ifndef TKSUBPROJECTS_VV300SET_H_
#define TKSUBPROJECTS_VV300SET_H_

#include "tkConfig.h"
#include "u16set.h"
#include "vv300Flash.h"

#define N_USET  2
#define N_FCSET 4

//extern uint32_t us32set[N_USET / 2 +2];
extern uint16_t *uset;
extern uint32_t fc32set[N_FCSET / 2 +2];
extern uint16_t *fcset;

 /* 2) визначаємо масив структур u16menu_orn_t, по елементу для кожної уставки:*/
extern u16menu_orn_t u[N_USET];
extern u16menu_orn_t fc[N_FCSET];
void f_FSsetInit(void);
void f_UsetInit(void);

flash_status_t f_VV300_FactorySets_Read(void);
flash_status_t f_VV300_FactorySets_Write(void);
flash_status_t f_VV300_UserSets_Read(void);
flash_status_t f_VV300_UserSets_Write(void);

#endif /* TKSUBPROJECTS_VV300SET_H_ */
