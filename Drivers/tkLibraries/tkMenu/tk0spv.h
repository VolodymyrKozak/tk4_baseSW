/*
 * tk0spv.h
 *
 *  Created on: 25 лют. 2019 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKLIBRARIES_TKMENU_TK0SPV_H_
#define TKLIBRARIES_TKMENU_TK0SPV_H_
typedef enum {
	spv_OK,						/* OK, без коментарів															*/
	spv_UNKNOWN_Err,			/* невизначена помилка*/
	spv_wo1602gErr,
	spv_FLASHErr,
	spv_I2C_Err,				/* помилка транспортування даних по I2C 										*/
	spv_BUSY,
	spv_WRONG_Arg,				/* на вході функції некоректні аргументи, отже і функції щось робила некоректно */
	spv_NULL_Ptr,				/* нульовий показчик (рос. - указатель), аврійне припинення роботи 				*/
	spv_Err						/* помилка при виконання функції, щось не так..*/
}spv_status_t;


spv_status_t f_0UI_Init(void);
spv_status_t f_0UI_Spv(void);

#endif /* TKLIBRARIES_TKMENU_TK0SPV_H_ */
