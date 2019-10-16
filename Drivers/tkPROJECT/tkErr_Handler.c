/*
 * tkTest1Err_Handler.c
 *
 *  Created on: 27 лют. 2018 р.
 *      Author: Volodymyr.Kozak
 */
#include "tkErr_Handler.h"
#include <tkConfig.h>
#include "stdio.h"
#include <string.h>
#include "wk_atime64.h"
#include "vv300Flash.h"

/* If an software error  is happened anywhere in the program,
 * this function should be called to fix all possible error information
 * to string sErrorHandler[2048] with function 'f_PutErrRecord'
 * On the Release Stage - this string should be written to EEPROM,
 * to be analyzed later.
 * On the Debug Stage - this string should be printed to display
 * if from 'wc_main.c' file function WC_Error_Handler will be called
 * to be analyzed later.
 *
 * */





uint32_t f_PutErrRecord (
					 int8_t const *pErrFile,
					 int8_t const *pErrFunc,
					 uint32_t const errLine,
					 uint32_t errCode){

//	int8_t *sErrorHandler=NULL;
	uint32_t s32ErrorHandler[0x20]={0};
	int8_t *sErrorHandler=NULL;
	sErrorHandler=(int8_t*)(&s32ErrorHandler);
	/*
	 * 0x00 DataTime 		0x10
	 * 0x10 _FILE_ 			0x30
	 * 0x40 _FUNC_			0x30
	 * 0x70 _LINE_			0x04
	 * 0x74 ErrCode			0x04
	 * 0x78 Reserve         0x04
	 * 0x7C CRC         	0x04
							----
							0x80
	*/
 	snprintf((char*)sErrorHandler, 17, "%4d/%02d/%02d %02d:%02d",

				p_wk64_tm->tm_year+1900,
				p_wk64_tm->tm_mon+1,
				p_wk64_tm->tm_mday,
				p_wk64_tm->tm_hour,
				p_wk64_tm->tm_min
		);
// 	void * memcpy (void *restrict to, const void *restrict from, size_t size)
	uint32_t  sizeStr = 0;
	sizeStr=strlen((char*)pErrFile);
	if (sizeStr<=0x30){
		memcpy (sErrorHandler+0x10, pErrFile, sizeStr);
	}
	else if (sizeStr>0x30){
		memcpy (sErrorHandler+0x10, pErrFile+sizeStr-0x30, 0x30);
	}
	else{}
	sizeStr=strlen((char*)pErrFunc);
	if (sizeStr<=0x30){
		memcpy (sErrorHandler+0x40, pErrFunc, sizeStr);
	}
	else if (sizeStr>0x30){
		memcpy (sErrorHandler+0x40, pErrFunc+sizeStr-0x30, 0x30);
	}
	else{}

	snprintf ((char*)(sErrorHandler+0x70),5,"%4d",(int)errLine);
	snprintf ((char*)(sErrorHandler+0x74),5,"%4d",(int)errCode);
//				"%4d-%02d-%02d %02d:%02d\n",
//				p_wk64_tm->tm_year+1900,
//				p_wk64_tm->tm_mon+1,
//				p_wk64_tm->tm_mday,
//
	 f_write2page_record (	FLASH_PAGE_ERR_ARCHIVE1,
					 	 	sErrorHandler,
							0x80);
	return 0;
}
