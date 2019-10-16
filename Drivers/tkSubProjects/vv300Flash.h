/*
 * vv300Flash.h
 *
 *  Created on: 11 лют. 2019 р.
 *      Author: KozakVF
 */

#ifndef TKSUBPROJECTS_VV300FLASH_H_
#define TKSUBPROJECTS_VV300FLASH_H_
#include "tk1flash2page.h"


#define FLASH_PAGE_ERR_ARCHIVE1 		FLASH_PAGE_100
#define PLASH_PAGE_ERR_ARCHIVE2    		FLASH_PAGE_101

#define GRID_STATUS_REC_A 				FLASH_PAGE_120
#define GRID_STATUS_REC_B 				FLASH_PAGE_121

#define FLASH_PAGE_FACILILY_SETS_A		FLASH_PAGE_122
#define FLASH_PAGE_FACILILY_SETS_B		FLASH_PAGE_123

#define FLASH_PAGE_USER_SETS_A		    FLASH_PAGE_124
#define FLASH_PAGE_USER_SETS_B			FLASH_PAGE_125



flash_status_t f_VV_Sets_Read  ( uint32_t *pSettingD,
								 uint32_t  set_arr_size,
								 uint32_t  FlashPage_A,
								 uint32_t  FlashPage_B
								);
flash_status_t f_VV_Sets_Write ( uint32_t *pSettingD,
								 uint32_t  set_arr_size,
								 uint32_t  FlashPage_A,
								 uint32_t  FlashPage_B
								);
#endif /* TKSUBPROJECTS_VV300FLASH_H_ */
