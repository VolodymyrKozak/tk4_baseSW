/*
 * tkErrHandler.c



 *
 *  Created on: 27 лют. 2018 р.
 *      Author: Volodymyr.Kozak
 */
#include "../tkLibraries/tkErrLib.h"

#include "stdint.h"
#include <stdio.h>
#include <tkConfig.h>
#include "stm32f1xx_hal.h"

uint8_t errCode=0;
uint8_t errStr[128]={0};
uint8_t low_errStr[17];
void woErrPrint (void){
#ifdef	DEBUGGING_ONLY
//	WO1602_ClearScreen ();
//	WO1602_WriteHighString  ("tkSOFT_ERROR");
    snprintf(errStr, 128,"%s %s %s %s %s\n",__FILE__, __LINE__,__func__,__DATE__,__TIME__);
	for (uint8_t i_ep =0; i_ep<122;i_ep++){
		for (uint8_t ii_ep =0; ii_ep<16;ii_ep++){
			low_errStr[ii_ep]=errStr[i_ep+ii_ep];
		}
//		WO1602_WriteLowString  (low_errStr);
		HAL_Delay(500);
	}
#endif
#ifndef	DEBUGGING_ONLY
//	static FLASH_EraseInitTypeDef EraseInitStructErrArchive;
//	static uint32_t PAGEErrorErrArchive = 0;
//			while(HAL_FLASH_Unlock()!= HAL_OK){
//			}
//			    /* Erase the user Flash area
//			    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
//			    /* Fill EraseInit structure*/
//			EraseInitStructErrArchive.TypeErase   = FLASH_TYPEERASE_PAGES;
//			EraseInitStructErrArchive.PageAddress = 0x0803D000;
//			EraseInitStructErrArchive.NbPages     = 1;
//
//			if (HAL_FLASHEx_Erase(&EraseInitStructErrArchive, &PAGEErrorErrArchive) != HAL_OK){
//			}
//			while(HAL_FLASH_Lock() != HAL_OK){
//			}
#endif
}
