/*
 * stm32_rtc.h
 *
 *  Created on: 12 жовт. 2018 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef WC_DRIVERS_TIME64_STM32_RTC_H_
#define WC_DRIVERS_TIME64_STM32_RTC_H_
#include "stdbool.h"
#include "time64.h"

typedef struct {
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} date_time_t;
int f_ZeroTest(void);
void f_ZeroLabelWrite(int Label);
void HALRTC_ZeroInit(void);
void HALRTC_AverageInit(void);
/* This function is to set User Setting Time
 * Time64_T in_Time64 is user set time in GNU C Library elapsed time representation*/
void HALRTC_UserTimeSet(Time64_T in_Time64);
/* Функція для установки часу з меню дисплея..*/
void setDataAndTime(struct tm *p_wk64_tm);
/* This function is to get time value from PCF85263A
 * into Time64_T GNU C Library elapsed time representation*/
void HALRTC_TimeGet(Time64_T *p_in_Time64);

bool f_checkNewDay(uint32_t bkp_reg1);
void writeDateToBackup(uint32_t bkp_reg1, uint32_t bkp_reg2);
#endif /* WC_DRIVERS_TIME64_STM32_RTC_H_ */
