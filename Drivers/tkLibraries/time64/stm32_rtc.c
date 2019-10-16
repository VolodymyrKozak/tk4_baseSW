/*
 * stm32_rtc.c
 *
 *  Created on: 12 жовт. 2018 р.
 *      Author: Volodymyr.Kozak
 */
#include "stm32_rtc.h"
#include "tkConfig.h"
#include "rtc.h"


#include "time64.h"

static RTC_TimeTypeDef sTime ={0};
static RTC_DateTypeDef sDate ={0};




static void readBackupToDate(uint32_t bkp_reg1, uint32_t bkp_reg2);

int f_ZeroTest(void){
	return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
}
void f_ZeroLabelWrite(int Label){
	/* Записуємо в регістр мітку про готовність до відліку часу
	 * при вимкнутому електроживленні 								*/
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, Label);
}


/* This function is to set the stm32 rtc clock with
 * pre_compilator _DATA_ and _TIME */
void HALRTC_ZeroInit(void){
 /* Якщо контролер вмикається перший раз 								*/

	uint8_t datestr[] = __DATE__;
	uint8_t timestr[] = __TIME__;
	RTC_TimeTypeDef dtTime = {0};
	RTC_DateTypeDef dtDate = {0};
	         if ( memcmp (datestr, "Jan", 2)==0){dtDate.Month=1;}
	    else if ( memcmp (datestr, "Feb", 2)==0){dtDate.Month=2;}
	    else if ( memcmp (datestr, "Mar", 2)==0){dtDate.Month=3;}
	    else if ( memcmp (datestr, "Apr", 2)==0){dtDate.Month=4;}
	    else if ( memcmp (datestr, "May", 2)==0){dtDate.Month=5;}
	    else if ( memcmp (datestr, "Jun", 2)==0){dtDate.Month=6;}
	    else if ( memcmp (datestr, "Jul", 2)==0){dtDate.Month=7;}
	    else if ( memcmp (datestr, "Aug", 2)==0){dtDate.Month=8;}
	    else if ( memcmp (datestr, "Sep", 2)==0){dtDate.Month=9;}
	    else if ( memcmp (datestr, "Oct", 2)==0){dtDate.Month=10;}
	    else if ( memcmp (datestr, "Nov", 2)==0){dtDate.Month=11;}
	    else if ( memcmp (datestr, "Dec", 2)==0){dtDate.Month=12;}
	    else{dtDate.Month=0;}
	    uint32_t intbuf=0;
	//        strcpy( dtm, "Saturday March 25 1989" );
	//        sscanf( dtm, "%s %s %d  %d", weekday, month, &day, &year );
	    sscanf (datestr+4, "%2d", &intbuf); dtDate.Date=intbuf;
	    sscanf (datestr+7, "%4d", &intbuf); dtDate.Year=intbuf-2000;

	    sscanf (timestr,   "%2d", &intbuf); dtTime.Hours=intbuf;
	    sscanf (timestr+3, "%2d", &intbuf); dtTime.Minutes=intbuf;
	    sscanf (timestr+6, "%2d", &intbuf); dtTime.Seconds=intbuf;
	    /* Запускаємо RTC становивши початковий час -
	     * час компіляції програми
	     * 									*/
//	    MX_RTC_Init();
	    HAL_RTC_SetDate(&hrtc, &dtDate, RTC_FORMAT_BIN);
	    HAL_RTC_SetTime(&hrtc, &dtTime, RTC_FORMAT_BIN);
	    writeDateToBackup(RTC_BKP_DR2,RTC_BKP_DR3);
	    /****************************************************************/

  }
 void HALRTC_AverageInit(void){
	 /*Алгоритм буде приблизно такий:
	  * https://stm32withoutfear.blogspot.com/2016/12/RTC-stm32f1xx.html
 	 Запам'ятовуємо значення RTC лічильника в тимчасову змінну,
	 тому що виклик функції запису збереженої дати зменшить значення RTC лічильника в межах доби.
	 І час, коли MCU знаходився у відключеному стані - втратиться.
	 Структуру дати заповнюємо даними з backup регістрів де зберігається дата перед вимкненням пристрою.
	 Відновлюємо значення RTC лічильника з тимчасової змінної та викликаємо функцію
	 HAL_RTC_GetTime або HAL_RTC_GetDate які приведуть час до стану "в межах доби",
	 а якщо MCU був відключений більше ніж добу, то "пропущений" час додасця до дати автоматично;
	 */
	 	 // Після увімкнення, пробудження, скидання, потрібно викликати цю функцію
	 	 HAL_RTC_WaitForSynchro(&hrtc);
	    // якщо вже годинник встановлено, то...
	     // запам'ятовуємо кількість секунд з лічильника RTC
	     uint32_t timeCounter = (RTC->CNTH << 16) | RTC->CNTL;
	     // відновимо з backup регістрів дату. Від лічильника RTC відніметься час що виходить за межі доби
	     readBackupToDate(RTC_BKP_DR2,RTC_BKP_DR3);
	     // відновимо попереднє, збережене значення лічильника RTC
	     RTC->CNTH = timeCounter>>16;
	     RTC->CNTL = timeCounter;
	     // Після запису до регістру CNT потрібно викликати цю функцію
	     HAL_RTC_WaitForSynchro(&hrtc);
	     // виклик цієї функції врахує пропущені дні що був MCU у вимкненому стані
	     HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	     HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

/* This function is to set User Setting Time
 * Time64_T in_Time64 is user set time in GNU C Library elapsed time representation*/
void HALRTC_UserTimeSet(Time64_T in_Time64){
	struct tm *p_wk64_tm;
	p_wk64_tm = localtime64 (&in_Time64);
	setDataAndTime(p_wk64_tm);

}
/* Функція для установки часу з меню дисплея..*/
void setDataAndTime(struct tm *p_wk64_tm){
	sDate.Year		= (uint8_t)  p_wk64_tm->tm_year-100;
	sDate.Month 	= (uint8_t)  p_wk64_tm->tm_mon+1;
	sDate.Date  	= (uint8_t)  p_wk64_tm->tm_mday;
	sTime.Hours 	= (uint8_t)  p_wk64_tm->tm_hour;
	sTime.Minutes	= (uint8_t)  p_wk64_tm->tm_min;
	sTime.Seconds	= (uint8_t)  p_wk64_tm->tm_sec;
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}



/* This function is to get time value from PCF85263A
 * into Time64_T GNU C Library elapsed time representation*/
void HALRTC_TimeGet(Time64_T *p_in_Time64){

	HAL_RTC_GetDate (&hrtc, &sDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime (&hrtc, &sTime, RTC_FORMAT_BIN);
	struct tm wk64_tm;

	wk64_tm.tm_year		=	(uint32_t)sDate.Year	+100;
	wk64_tm.tm_mon		=   (uint32_t)sDate.Month 	-1;
	wk64_tm.tm_mday		=	(uint32_t)sDate.Date;
	wk64_tm.tm_hour		= 	(uint32_t)sTime.Hours;
	wk64_tm.tm_min 		= 	(uint32_t)sTime.Minutes;
	wk64_tm.tm_sec		=	(uint32_t)sTime.Seconds;

	(*p_in_Time64)  = mktime64 (&wk64_tm);
}

struct TM *localtime64   (const Time64_T *);
Time64_T   mktime64      (struct TM *);



/*Функція перевірки нового дня
Ця функція потрібна щоб перевірити чи настав новий день.
Як настав, то повертає "false", як той самий день, то повертає "true".
*/
bool f_checkNewDay(uint32_t bkp_reg1){
 HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
 if(sDate.Date != HAL_RTCEx_BKUPRead(&hrtc,bkp_reg1) >> 8){
   return false;
 }
 else {
   return true;
 }
}
/*Функція збереження дати до backup регістрів
Так як для збереження дати потрібно 32 біти, а backup регістри мають по 16 біт,
то нам потрібно для збереження дати аж два backup регістри.
На вхід функція отримує в які саме регістри потрібно зберегти дату. Нічого не повертає:
*/
void writeDateToBackup(uint32_t bkp_reg1, uint32_t bkp_reg2){

  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  volatile uint16_t weekdayyear =0;
  weekdayyear=(sDate.WeekDay << 8) | (sDate.Year);
  volatile uint16_t datemonth = 0;
  datemonth = (sDate.Date << 8) | (sDate.Month);
  HAL_RTCEx_BKUPWrite(&hrtc, bkp_reg1, weekdayyear);
  HAL_RTCEx_BKUPWrite(&hrtc, bkp_reg2, datemonth);
}

/*Функція відновлення дати з backup регістрів
І ще нам потрібна функція яка буде читати записану дату з backup регістрів,
заповнювати структуру дати і встановлювати її як поточну.
На вхід функція отримує з яких саме регістрів потрібно відновити дату.
Нічого не повертає:
*/
static void readBackupToDate(uint32_t bkp_reg1, uint32_t bkp_reg2){
	 volatile uint16_t  weekdayyear= 0;
	 weekdayyear = HAL_RTCEx_BKUPRead(&hrtc,bkp_reg1);
	 sDate.WeekDay =  weekdayyear / 0x100;
	 sDate.Year    =  weekdayyear % 0x100;

	 volatile uint16_t datemonth = 0;

	 datemonth = HAL_RTCEx_BKUPRead(&hrtc,bkp_reg2);
	 sDate.Date = datemonth / 0x100;
	 sDate.Month = datemonth % 0x100;

 HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

