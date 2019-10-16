/*
 * tkErr_Handler.h
 *
 *  Created on: 27 лют. 2018 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKERR_HANDLER_H_
#define TKERR_HANDLER_H_

#include "stdint.h"


#define ERR  (int8_t*)__FILE__,(int8_t*)__FUNCTION__,__LINE__,(uint32_t)



/************************************************************************************
 * ERROR HANDLING POLICY
 * *********************************************************************************
 If a subsistem/file/library operates with error statuses
 the fist four error code should be:
 typedef enum {
 	 WC_OK,
 	 WC_UNKNOWN_ERR,
 	 WC_ZERO_POINTER,
 	 WC_WRONG_ARGUMENT
} wc_error_status_t

*/
//extern volatile uint32_t u32_ErrorRecords[512];

/*	To call function use
 *  short form f_PutErrRecord( ERR, CodeErr) or
 *  full  form f_PutErrRecord( __FILE__,__FUNCTION__, __LINE__, 0xFE);
 *  return the length of the Record*/









/* Вставляти функцію для обробки помилок
 * f_PutErrRecord(ERR tkI);
 * */


uint32_t f_PutErrRecord (
					 int8_t const *pErrFile,
					 int8_t const *pErrFunc,
					 uint32_t const errLine,
					 uint32_t errCode);


void tk_Stop (void);


#endif /* TKERR_HANDLER_H_ */
