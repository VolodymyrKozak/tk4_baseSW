/*
 * tkMain.h
 *
 *  Created on: 27 лют. 2018 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKMAIN_H_
#define TKMAIN_H_

//#include "tkTimeDate.h"

typedef enum {                   	/* Статус, коди завершення функцій бібліотеки 									*/
	tkMAIN_OK,						/* OK, без коментарів															*/
	tkMAIN_UNKNOWN_Err,			/* невизначена помилка*/
	tkMAIN_wo1602gErr,
	tkMAIN_wh1602b3Err,
	tkMAIN_MenuSPV_Err,
	tkMAIN_FLASHErr,
	tkMAIN_I2C_Err,				/* помилка транспортування даних по I2C 										*/
	tkMAIN_BUSY,
	tkMAIN_WRONG_Arg,				/* на вході функції некоректні аргументи, отже і функції щось робила некоректно */
	tkMAIN_NULL_Ptr,				/* нульовий показчик (рос. - указатель), аврійне припинення роботи 				*/
	tkMAIN_Err						/* помилка при виконання функції, щось не так.. 								*/
} tkMAIN_status_t;




tkMAIN_status_t tkInit(void);
tkMAIN_status_t tkLoop(void);
void tkErrHandler(void);






#endif /* TKMAIN_H_ */
