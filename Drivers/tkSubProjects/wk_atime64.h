/*
 * wk_time64.h
 *
 *  Created on: 5 серп. 2018 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef DISPLAY_WK_TIME64_H_
#define DISPLAY_WK_TIME64_H_
#include "../tkLibraries/time64/stm32_rtc.h"
#include "../tkLibraries/time64/time64.h"
typedef enum {
	gui_OK,
	gui_UNKNOWN_Err,
	gui_ZERO_ptr,
	gui_wrong_arg
}gui_status_t;


typedef enum {
	clock_empty,
	clock_installed_data_time,
	clock_userti_installed
}clock_installed_t;

extern volatile Time64_T wk_base64_value; /* It's base time, set into device at the facility		 */


/****************************************************************************************************
 *  Пройдений час в секундах, починаючи 1900.01.01.00.00.00
 *  В кожному циклі контролера оновлюється значення пройденого часу
 *  Два слова, всього 64 біт.
 *  Щоб користуватися в будь-якому файлі досить включити #include "wk_atime64.h"
 * **************************************************************************************************/
extern volatile Time64_T wk_real64_value; /* It's at the moment time, updatable with MCU every tick.
 	 	 	 	 	 	 	 	  It's user function to set real time at the begining 	 */



/****************************************************************************
 * Реальний час у зручному форматі, оголошений і визначений як структура:
 *  Щоб користуватися в будь-якому файлі досить включити #include "wk_atime64.h"
Ім'я	Опис
int tm_sec;		Секунди від початку хвилини (0,59)
int tm_min;		Хвилини від початку години (0,59)
int tm_hour;	Години від півночі (0,23)
int tm_mday;	Число місяця (1,31)
int tm_mon;		Місяці після січня (0,11)
int tm_year;	Роки з 1900 (в Windows), з 1970 (в Unix)
int tm_wday;	Дні з неділі (0,6)
int tm_yday;	Дні з першого січня (0,365)
int tm_isdst;	Ознака літнього часу
****************************************************************************/
extern struct tm  wk64_tm;
extern struct tm *p_wk64_tm;
/* Якщо потрібно визначити окремо - для отримання реального часу використовувати функцію:
 * struct tm MYTIME = {0};
 * struct tm *pMYTIME =NULL;
 * pMYTIME = localtime64   (&wk_real64_value);*/
gui_status_t  f_wk64_time_Init(void);
gui_status_t  f_wk64_time_Loop(void);
gui_status_t  f_wk64_time_Get(volatile Time64_T *p_Time64toGet);
gui_status_t  f_wk64_time_Set(Time64_T Time64toSet);






#endif /* DISPLAY_WK_TIME64_H_ */
