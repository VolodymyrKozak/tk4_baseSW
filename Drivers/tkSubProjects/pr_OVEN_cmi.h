/*
 * pr_OVEN_cmi.h
 *
 *  Created on: 24 груд. 2018 р.
 *      Author: KozakVF
 */

#ifndef TKSUBPROJECTS_PR_OVEN_CMI_H_
#define TKSUBPROJECTS_PR_OVEN_CMI_H_

#ifdef MODBUS_MASTER_for_OVEN_INDICATOR

/* тут будуть робочі функції проекту */

#ifdef MODBUS_MASTER_for_OVEN_INDICATOR_TESTING

#include "tk_oven_smi2.h"


void f_oventest_Init(void);
/* Демонстрація управління контролером роботою ОВЕН СМИ
 * Ті самі числа виводяться і на ОВЕН СМИ і на дисплей.
1. 5 хвилин виводимо випадкові числа з точкою у різних місцях,
послідовно - на один та на другий дисплей
(функція f_ovenSMI2_DisplayFloat(..)
2. 5 Хвилин виводимо числа однією циркулярною функцєю на всі
ОВЕН СМИ
(функція f_ovenSMI2_DisplayBroadCast(..)
3. Вмикаємо мигання на одному приладі
4. Вимикаємо мигання на одному приладі
*/
void f_oventest(uint32_t ov_counter);
/*1. 5 хвилин виводимо випадкові числа з точкою у різних місцях,
послідовно - на один та на другий дисплей
(функція f_ovenSMI2_DisplayFloat(..)*/

void f_oventest1(void);
/* 2. 5 Хвилин виводимо числа однією циркулярною функцєю на всі
ОВЕН СМИ */
void f_oventest2(void);
#endif// MODBUS_MASTER_for_OVEN_INDICATOR
#endif //MODBUS_MASTER_for_OVEN_INDICATOR_TESTING

#endif /* TKSUBPROJECTS_PR_OVEN_CMI_H_ */
