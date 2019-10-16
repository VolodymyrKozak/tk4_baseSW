/*
 * tkFlash.c
 *
 *  Created on: 27 лют. 2018 р.
 *      Author: Volodymyr.Kozak
 */
#include "tk0flash_base.h"

#include "stdint.h"
#include <stdio.h>
#include <stdio.h>
#include <tkConfig.h>
#include "crc.h"
//#include "stm32f10x_flash.h"
#include "stm32f1xx_hal.h"


//#include "tkSystemSet.h"

#include "tkErrLib.h"
uint32_t PAGEError = 0;
static	HAL_StatusTypeDef	flash_ok = HAL_ERROR;
static FLASH_EraseInitTypeDef EraseInitStruct;
//BUFFER_SIZE (sizeof (&user_setting)-4);
//(uint32_t *)aDataBuffer=(uint32_t*)(&user_settings),
//BUFFER_SIZE (sizeof (&user_setting)-4);

CRC_HandleTypeDef   CrcHandle;

/*****************************************************************************************************************************
 * ЗАПИС масиву 32-розрядних слівЗ ОПЕРАТИВНОЇ ПАМ'ЯТІ ДО ФЛЕШ (без відмикання/замикання флеш, без перевірки правильності записів)
 *****************************************************************************************************************************/

static flash_status_t f_write_to_flash_array (uint32_t *u_from_RAM,
							     	 	 	  uint32_t  u_to_FLASH,
											  size_t    size_of_array
								 );
/******************************************************************************************************************
 * ПЕРЕВІРКА СЛОВО В СЛОВО ЗАПИСІВ У ФЛЕШ ТА В ОПЕРАТИВНІЙ ПАМЯТІ
 ******************************************************************************************************************/
//	  /* Check if the programmed data is OK
//	      MemoryProgramStatus = 0: data programmed correctly
//	      MemoryProgramStatus != 0: number of words not programmed correctly ******/
static flash_status_t f_check_flash_record(uint32_t a_in_FLASH,
										   uint32_t *u_in_RAM,
										   uint32_t *u_MemoryRecordStatus,
	 	 	 	 	 	 	 	 	 	   size_t size_of_array);

/**************************************************************************************************************
 * ФОРМУВАННЯ CRC
 *************************************************************************************************************/

static void f_u32CRC_creation (uint32_t *u_array, size_t size_of_array);

/**************************************************************************************************************
 * ПЕРЕВІРКА CRC
 *************************************************************************************************************/
static flash_status_t f_u32CRC_examenation (uint32_t a_array, size_t size_of_array);

static flash_status_t f_read_flash_array(	uint32_t address_from_FLASH,
									uint32_t *address_to_RAM,
	 	 	 	 	 	 	 	 	size_t size_of_record
									);

/*****************************************************************************************************************************
 * ОБГОРТКА ДЛЯ ВИКЛИКУ ФУНКЦІЇ ЗАПИСУ ДАНИХ
 *****************************************************************************************************************************/

//void f_save (void){
//	uint32_t *p_DataBuffer=NULL;
//	p_DataBuffer=(uint32_t*)(&user_settings);
//
////	f_setting_saving (p_DataBuffer,
////					  FLASH_SECTOR_7,
////					  sizeof (user_settings_t)
////					 );
//}

#ifdef NUCLEO
/**************************************************************************************************************
 * СТИРАННЯ СЕКТОРА ФЛЕШ
 *************************************************************************************************************/

flash_status_t f_flash_sector_erase (uint32_t sector_addr){
	flash_status_t fsa_status = FLASH_UNKNOWN_Err;


	flash_ok = HAL_ERROR;
		while(flash_ok != HAL_OK){
			flash_ok = HAL_FLASH_Unlock();
		}
	LCD_Printf ("HAL_FLASH_Unlock() ok\n\n");
	//Стираем память
	FLASH_Erase_Sector(sector_addr, VOLTAGE_RANGE_3);
//	HAL_Delay (2000);
	flash_ok = HAL_ERROR;
	LCD_Printf ("FLASH_Erase_Sector ok\n\n");

//	  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
//	     you have to make sure that these data are rewritten before they are accessed during code
//	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
//	     DCRST and ICRST bits in the FLASH_CR register. */
	  __HAL_FLASH_DATA_CACHE_DISABLE();
	  __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

	  __HAL_FLASH_DATA_CACHE_RESET();
	  __HAL_FLASH_INSTRUCTION_CACHE_RESET();

	  __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	  __HAL_FLASH_DATA_CACHE_ENABLE();
//
//  /* Lock the Flash to disable the flash control register access (recommended
//      to protect the FLASH memory against possible unwanted operation) *********/
//   HAL_FLASH_Lock();
	while(flash_ok != HAL_OK){
		flash_ok = HAL_FLASH_Lock();
	}
	LCD_Printf ("HAL_FLASH_Lock()\n");
   fsa_status=FLASH_OK;
   return fsa_status;
}
#endif

/**************************************************************************************************************
 * СТИРАННЯ СТОРІНКИ ФЛЕШ stm32F105
 ************************************************************************************************************
https://www.cs.indiana.edu/~bhimebau/f3lib/html/group__FLASH__Group2.html
*/
flash_status_t f_flash_page_erase (uint32_t addr_page){
	flash_status_t fsa_status = FLASH_UNKNOWN_Err;
	while (fsa_status != FLASH_OK){
	flash_ok = HAL_ERROR;
		while(flash_ok != HAL_OK){
			flash_ok = HAL_FLASH_Unlock();
		}
		 /* Erase the user Flash area
		    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

		  /* Fill EraseInit structure*/
		  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		  EraseInitStruct.PageAddress = addr_page;
		  EraseInitStruct.NbPages     = 1;

		  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){
		    /*
		      Error occurred while page erase.
		      User can add here some code to deal with this error.
		      PAGEError will contain the faulty page and then to know the code error on this page,
		      user can call function 'HAL_FLASH_GetError()'
		    */
			  fsa_status=FLASH_ERASE_Err;  break;
		  }

		flash_ok = HAL_ERROR;
		while(flash_ok != HAL_OK){
			flash_ok = HAL_FLASH_Lock();
		}
	 fsa_status=FLASH_OK;
	}
   fsa_status=FLASH_OK;
   return fsa_status;
}



/*****************************************************************************************************************************
 * ЗАПИС МАСИВУ 32-розрядних слів З ОПЕРАТИВНОЇ ПАМ'ЯТІ ДО ФЛЕШ (варіант 1 - лише структуру)
 * включає послідовну перевірку записаного після завершення процесу запису
 *****************************************************************************************************************************/

flash_status_t f_write_to_flash_record (uint32_t *u_from_RAM,
							     uint32_t u_to_FLASH,
							     size_t size_of_array
								 ){
	flash_status_t uss_status = FLASH_UNKNOWN_Err;
    /* Відмикаємо флеш */
	while (uss_status != FLASH_OK){
		flash_ok = HAL_ERROR;
		while(flash_ok != HAL_OK){
			flash_ok = HAL_FLASH_Unlock();
		}
		/* записуємо масив як послідовність слів */

		if (f_write_to_flash_array (u_from_RAM,
								    u_to_FLASH,
								    size_of_array
								) != FLASH_OK){
			uss_status = FLASH_Err; break;
		}
		/* замикаємо флеш */
		flash_ok = HAL_ERROR;
		while(flash_ok != HAL_OK){
			flash_ok = HAL_FLASH_Lock();
		}
		uint32_t MemoryRecordStatus =0;
		/* звіряємо щойно записане з вихідним масивом */
		if (f_check_flash_record(u_to_FLASH,
								u_from_RAM,
								&MemoryRecordStatus,
								size_of_array
								) != FLASH_OK){
			uss_status = FLASH_Err; break;
		}
		if (MemoryRecordStatus !=0){
			uss_status=FLASH_RAM_COMPARISON_Err;
		}
		else{
			uss_status=FLASH_OK;
		}
	}
	return uss_status;
}

/*****************************************************************************************************************************
 * ЧИТАННЯ масиву 32-розрядних слів З ФЛЕШ ДО ОПЕРАТИВНОЇ ПАМЯТІ
 * (включає перевірку CRC)
 *****************************************************************************************************************************/
flash_status_t f_read_from_flash_rec (uint32_t a_rec_from_FLASH,
							     	  uint32_t *u_rec_to_RAM,
								      size_t   size_of_array
								 ){
	flash_status_t rffr_status=FLASH_UNKNOWN_Err;
	if ((a_rec_from_FLASH <8000000) || (u_rec_to_RAM==NULL)){
		return FLASH_ZERO_Pointer;
	}
	if (size_of_array<8){
		return FLASH_WRONG_Arg;
	}

	uint32_t a_current_from_FLASH		= 0;
	uint32_t *u_current_to_RAM          = NULL;
	uint32_t a_stop				    	= 0;
	volatile uint32_t current_RAM_word;

	a_current_from_FLASH		= a_rec_from_FLASH;
	u_current_to_RAM            = u_rec_to_RAM;
	a_stop = a_rec_from_FLASH +size_of_array;
	while (rffr_status != FLASH_OK){
		if (f_u32CRC_examenation (a_rec_from_FLASH, size_of_array) !=FLASH_OK){
			rffr_status =FLASH_READ_CRC_Err;break;
		}
		while (a_current_from_FLASH < a_stop){
			current_RAM_word = f_read_flash_word(a_current_from_FLASH);
			(*u_current_to_RAM)=current_RAM_word;
			u_current_to_RAM++;
			a_current_from_FLASH+=4;
		}
		rffr_status=FLASH_OK;
	}
	return  rffr_status;
}


/*****************************************************************************************************************************
 * ЗАПИС масиву 32-розрядних слів З ОПЕРАТИВНОЇ ПАМ'ЯТІ ДО ФЛЕШ
 * (без відмикання/замикання флеш, без перевірки правильності записів)
 * але з присвоєнням останньому елементу масива значення CRC
 *****************************************************************************************************************************/

static flash_status_t f_write_to_flash_array (uint32_t *u_from_RAM,
							     	 	 	  uint32_t  a_to_FLASH,
											  size_t    size_of_array
								 ){
	flash_status_t wfa_status = FLASH_UNKNOWN_Err;
	/* обчислюємо CRC і записуємо його останнім елементом масиву */


	f_u32CRC_creation (u_from_RAM, size_of_array);

	uint32_t *u_current_RAM = NULL;
	uint32_t a_current_FLASH = 0;
	uint32_t *u_stop = 0;
	volatile uint32_t current_word =0;

	u_current_RAM=u_from_RAM;
	a_current_FLASH = a_to_FLASH;
	u_stop = u_from_RAM +size_of_array/4;
	/* переписуємо слово в слово масив з оперативної памяті в флеш */
	while (u_current_RAM < u_stop){
		current_word = *(u_current_RAM);
		flash_ok = HAL_ERROR;
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
							  a_current_FLASH,
							  current_word
							  ) == HAL_OK){
			u_current_RAM ++;  			    /* ++адреса - означає збільшення адреси на sizeof(uint32_t),  	*/
		    a_current_FLASH += 4;			/* тобто не на одиницю, а на чотири 							*/
		}
		else{
			/* Error occurred while writing data in Flash memory.
		       User can add here some code to deal with this error */
			wfa_status = FLASH_WRITE_Err; break;
		}
		wfa_status = FLASH_OK;
	}
	return wfa_status;
}
/******************************************************************************************************************
 * ПЕРЕВІРКА СЛОВО В СЛОВО ЗАПИСІВ У ФЛЕШ ТА В ОПЕРАТИВНІЙ ПАМЯТІ
 ******************************************************************************************************************/
//	  /* Check if the programmed data is OK
//	      MemoryProgramStatus = 0: data programmed correctly
//	      MemoryProgramStatus != 0: number of words not programmed correctly ******/
static flash_status_t f_check_flash_record(uint32_t a_in_FLASH,
										   uint32_t *u_in_RAM,
										   uint32_t *u_MemoryRecordStatus,
	 	 	 	 	 	 	 	 	 	   size_t size_of_array){
	uint32_t in_MemoryRecordStatus = 0x0;
	uint32_t *u_current_RAM   = NULL;
	uint32_t a_current_FLASH = 0;
	uint32_t a_stop           =0;
	volatile uint32_t current_FLASH_word =0;

	volatile uint32_t current_RAM_word =0;
	u_current_RAM		= u_in_RAM;
	a_current_FLASH     = a_in_FLASH;
	a_stop = a_in_FLASH +size_of_array;

	  while (a_current_FLASH < a_stop){
	    current_FLASH_word = f_read_flash_word(a_current_FLASH);
	    current_RAM_word   = (*u_current_RAM);
	    if (current_FLASH_word != current_RAM_word){
	    	in_MemoryRecordStatus++;
	    }
	    ++u_current_RAM;
	    a_current_FLASH+=4;
	  }
	  (*u_MemoryRecordStatus)=in_MemoryRecordStatus;
	  return FLASH_OK;
}

/**************************************************************************************************************
 * ФОРМУВАННЯ CRC
 *************************************************************************************************************/
static void f_u32CRC_creation (uint32_t *u_array, size_t size_of_array){
	volatile uint32_t usCRCValue = 0;

//	uint32_t aa[17]={0};
//		for (uint8_t i=0; i<17;i++){aa[i]=i;}
//		usCRCValue = HAL_CRC_Calculate(&hcrc,
//										aa,
//										17
//										);

	usCRCValue = HAL_CRC_Calculate(&hcrc,
									u_array,
									size_of_array/sizeof(uint32_t)-1
									);

	uint32_t *u_CRC=NULL;
    u_CRC=u_array+size_of_array/sizeof(uint32_t)-1;
    (*u_CRC)=usCRCValue;
}

/**************************************************************************************************************
 * ПЕРЕВІРКА CRC при читанні з ФЛЕШ:
 * обчислюється СКС зчитуваного масиву (без значення СRC) і порівнюється
 * з записаний у останньому слові масиву на ФЛЕШ СRC
 *************************************************************************************************************/
static flash_status_t f_u32CRC_examenation (uint32_t a_array, size_t size_of_array){
//	CrcHandle.Instance = CRC;
	uint32_t read_CRCValue = 0;
	uint32_t calculated_CRCValue = 0;
	uint32_t a_read_CRCValue = 0;
	/* адреса CRC на флеш */
	a_read_CRCValue=a_array+size_of_array-4;
	read_CRCValue=f_read_flash_word(a_read_CRCValue);
	calculated_CRCValue = HAL_CRC_Calculate(&hcrc,
									(uint32_t*)a_array,
									size_of_array/sizeof(uint32_t)-1
									);

    if (calculated_CRCValue != read_CRCValue){
    	return FLASH_READ_CRC_Err;
    }
    else {
    	return FLASH_OK;
    }
}

/* Читання масиву без перевірки CRC */
static flash_status_t f_read_flash_array(
									uint32_t address_from_FLASH,
									uint32_t *u32_to_RAM,
	 	 	 	 	 	 	 	 	size_t size_of_record
									){

	uint32_t *current_to_RAM_pointer = NULL;
	uint32_t current_from_FLASH_address = 0;
	uint32_t stop_FLASH_address=0;
	uint32_t current_from_FLASH_word =0;

	current_to_RAM_pointer		= u32_to_RAM;
	current_from_FLASH_address   = address_from_FLASH;
	stop_FLASH_address = address_from_FLASH +size_of_record;

	  while (current_from_FLASH_address < stop_FLASH_address){
	    current_from_FLASH_word = f_read_flash_word(current_from_FLASH_address);
	    (*current_to_RAM_pointer)= current_from_FLASH_word;
	    /* показчик на слово - добаляється 4*/
	    ++current_to_RAM_pointer;
	    /* адреса флеш - добавляємо 4*/
	    current_from_FLASH_address+=4;
	  }
	  return FLASH_OK;
}
/*****************************************************************************************************************/
uint32_t f_read_flash_word (uint32_t address) {
	return (*(__IO uint32_t*) address);
}


uint32_t f_flash_address(uint8_t in_flashpage_number){
	if ((in_flashpage_number<64) || (in_flashpage_number<64)){ return 0x0802000;}
	return (0x0802000 + (in_flashpage_number - 64)* 0x800);
}
/* Private variables ---------------------------------------------------------*/
/* ПРиклад для перевірки роботи індексації масиву*/
//typedef struct {
//	uint8_t a0;
//	uint8_t a1;
//	uint8_t a2;
//	uint8_t a3;
//	uint8_t a4;
//	uint8_t a5;
//	uint8_t a6;
//	uint8_t a7;
//	uint32_t A8_11;
//	uint32_t A12_15;
//}

//volatile str_t my_str = { 0, 1, 2, 3, 4, 5, 6, 7, 0x890A, 0xBCDE};


//	uint32_t b0_3;
//	uint32_t b4_7;
//	uint32_t b8_b11;
//	uint32_t b14_15;
//}

//volatile arr_t my_arr = {0};

//void f_cheks (void){
//	volatile uint32_t *pp_DataBuffer=NULL;
//	pp_DataBuffer=(uint32_t*)(&my_str);
//
//	f_check_write_record0 (
//			&my_arr,
//			pp_DataBuffer,
//			4
//	);
//}
//flash_status_t f_check_write_record0 (	uint32_t *address_in_FLASH,
//										uint32_t *address_in_RAM,
//	 	 	 	 	 	 	 	 	 	uint32_t size_of_array){
//
//	uint32_t *current_RAM_address = NULL;
//	uint32_t *current_FLASH_address = NULL;
//	uint32_t *stop_address=NULL;
//	volatile uint32_t current_RAM_word =0;
///***********************************************************************************/



//flash_status_t f_flash_page_erase (uint32_t addr_page){
///* Unlock the Flash to enable the flash control register access *************/
// HAL_FLASH_Unlock();
//
// /* Erase the user Flash area
//   (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
// void    FLASH_PageErase(uint32_t PageAddress);
// /* Fill EraseInit structure*/
// EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
// EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
// EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;
//
// if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
// {
//   /*
//     Error occurred while page erase.
//     User can add here some code to deal with this error.
//     PAGEError will contain the faulty page and then to know the code error on this page,
//     user can call function 'HAL_FLASH_GetError()'
//   */
//   /* Infinite loop */
//   while (1)
//   {
//     /* Make LED2 blink (100ms on, 2s off) to indicate error in Erase operation */
//     BSP_LED_On(LED2);
//     HAL_Delay(100);
//     BSP_LED_Off(LED2);
//     HAL_Delay(2000);
//   }
// }
//
// /* Program the user Flash area word by word
//   (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
//
// Address = FLASH_USER_START_ADDR;
//
// while (Address < FLASH_USER_END_ADDR)
// {
//   if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
//   {
//     Address = Address + 4;
//   }
//   else
//   {
//     /* Error occurred while writing data in Flash memory.
//        User can add here some code to deal with this error */
//
//   }
// }
//
// /* Lock the Flash to disable the flash control register access (recommended
//    to protect the FLASH memory against possible unwanted operation) *********/
// HAL_FLASH_Lock();
//
//}
