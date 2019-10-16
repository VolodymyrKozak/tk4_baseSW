/*
 * vv300Flash.c
 *
 *  Created on: 11 лют. 2019 р.
 *      Author: KozakVF
 */


#include "vv300Flash.h"
#include "tk0flash_base.h"
#include "wk_atime64.h"
#include "rtc.h"

/*
 * vd_flashTest.c
 *
 *  Created on: 14 янв. 2019 г.
 *      Author: VDShulyak
 */
#include "stdint.h"


/* ЧИТАТЬ МАССИВ УСТАВОК
 * Массив уставок считывается в ОП в массив pSettingD [set_arr_size/4] из
 * FlashPage_A, или FlashPage_B, если основная страница не читается.
 Возвращаемое значение:
 	 FLASH_OK 				- массив уставок считался хотя бы из одной страницы
 	 VD_MENIAT_USTAVKU_Err  - массив уставок не считался ни из одной страницы
 */
flash_status_t f_VV_Sets_Read  ( uint32_t *pSettingD,
								 uint32_t  set_arr_size,
								 uint32_t  FlashPage_A,
								 uint32_t  FlashPage_B
								){
	volatile flash_status_t sr_status = FLASH_UNKNOWN_Err;

	while (sr_status != FLASH_OK){
		/* считываем массив из основной страницы флеш в ОП в Settings_TK_fl*/
		sr_status= f_read_from_flash_rec (
											 FlashPage_A,
											 pSettingD,
											 set_arr_size
										 );
		/* если массив уставок не прочитался с основной страницы, читаем из запасной        */
		if (sr_status != FLASH_OK){
			sr_status= f_read_from_flash_rec (
											FlashPage_B,
											pSettingD,
											set_arr_size
											  );
		}
		/* если массив не считался ни из основной, ни из запасной страницы - фиксируем ошибку
		 * и прекращаем выполнение функции */
		if (sr_status != FLASH_OK){
			sr_status=VD_MENIAT_USTAVKU_Err;
			break;
		}
		sr_status = FLASH_OK;
	}
	return sr_status;
}

/* СОХРАНИТЬ МАССИВ УСТАВОК НА ФЛЕШ
 * Массив уставок pSettingD [set_arr_size/4] из ОП записывается в ФЛЕШ на страницу
 * FlashPage_A и дублируется на страницу флеш  FlashPage_B,
  Возвращаемое значение:
 	 FLASH_OK 				- уставка записалалась в флеш успешно на обе страницы
 	 VD_PRIMARY_PAGE_Err	- основная страница битая
 	 VD_SECONDARY_PAGE_Err	- дополнительная страница битая
 	 VD_MENIAT_USTAVKU_Err  - обе страницы битые, контроллер неисправен
 */
flash_status_t f_VV_Sets_Write ( uint32_t *pSettingD,
								 uint32_t  set_arr_size,
								 uint32_t  FlashPage_A,
								 uint32_t  FlashPage_B
								){

	volatile flash_status_t sw_status = FLASH_UNKNOWN_Err;

	uint32_t vdMemoryRecordStatus=0;

	while (!((sw_status == FLASH_OK)||
			(sw_status == VD_PRIMARY_PAGE_Err)||
			(sw_status == VD_SECONDARY_PAGE_Err))){

		/* Стираем основную страницу*/
		sw_status = f_flash_page_erase(FlashPage_A);

		/* Записываем откорректированый массив уставок на основную страницу флеш
		 * при этом запись в последнее слово массива CRC и пословная проверка
		 * записанного на флеш и хранящегося в ОП - внутри функции записи    */
		if (sw_status == FLASH_OK){
			sw_status= f_write_to_flash_record (
									 pSettingD,
								     FlashPage_A,
									 set_arr_size
									 );
		}
		/* eсли ошибка записи или ошибка сверки массивов во флеш и ОП */
		if (sw_status != FLASH_OK) {
			/* стереть основную страницу и попытаться записать заново */
			sw_status=f_flash_page_erase(FlashPage_A);
			if (sw_status != FLASH_OK){
				sw_status= f_write_to_flash_record (
									 pSettingD,
									 FlashPage_A,
									 set_arr_size
									);
			}
			if ((sw_status != FLASH_OK) || (vdMemoryRecordStatus!=0)){
				/* Если и второй раз не получилось записать */
				sw_status = VD_PRIMARY_PAGE_Err;
			}
		}
		/* Записываем откорректированый массив уставок на дополнительную страницу флеш
		* при этом запись в последнее слово массива CRC и пословная проверка
		* записанного на флеш и хранящегося в ОП - внутри функции записи    		*/
		flash_status_t sw_status_SEC = FLASH_UNKNOWN_Err;
		/* Стираем дополнительную страницу*/
		sw_status_SEC = f_flash_page_erase(FlashPage_B);
		if (sw_status_SEC == FLASH_OK){
			sw_status= f_write_to_flash_record (
										pSettingD,
										FlashPage_B,
										set_arr_size
										);
		}
		/* eсли ошибка записи или ошибка сверки массивов во флеш и ОП */
		if ((sw_status_SEC != FLASH_OK) || (vdMemoryRecordStatus!=0)){
		/* стереть ополнительную страницу и попытаться записать заново */
			sw_status=f_flash_page_erase(FlashPage_B);
			if (sw_status_SEC == FLASH_OK){
				sw_status_SEC= f_write_to_flash_record (
										pSettingD,
										FlashPage_B,
										set_arr_size
										);
			}
		}
		if ((sw_status_SEC != FLASH_OK) || (vdMemoryRecordStatus!=0)){
		/* Если и второй раз не получилось записать */
			sw_status_SEC = VD_SECONDARY_PAGE_Err;
		}
		if ((sw_status==VD_PRIMARY_PAGE_Err) && (sw_status_SEC == VD_SECONDARY_PAGE_Err)){
			sw_status=VD_MENIAT_USTAVKU_Err;
		}
		else if ((sw_status==VD_PRIMARY_PAGE_Err)&& (sw_status_SEC == FLASH_OK)){
			sw_status=VD_PRIMARY_PAGE_Err;
		}
		else if ((sw_status==FLASH_OK)	&& (sw_status_SEC == VD_SECONDARY_PAGE_Err)){
			sw_status=VD_SECONDARY_PAGE_Err;
		}
		else{}
	}
	return sw_status;
}
//void f_vkFlashTest(void){
//	volatile flash_status_t vfFlashOpStatus = FLASH_UNKNOWN_Err;
///*1) Нехай у нас є масив 200 чисел */
//int32_t ArraySettings[200]={0};
//int32_t ArrayRx[200]={0};
//uint32_t* u32 = NULL;
//u32=(uint32_t*)ArrayRx;
//	/*4) Зчитаємо по одному слову без перевірки CRC				*/
//			for (uint8_t i=0;i<128;i++){
//				u32[i]=f_read_from_flash_word(FLASH_PAGE_A_START_ADDR+4*i);;
//			}
//	/* 2) Стираємо сторінку 120 */
//	/**************************************************************************************************************
//	* СТИРАННЯ СТОРІНКИ ФЛЕШ stm32F105
//	*************************************************************************************************************/
//

//			f_flash_page_erase(FLASH_PAGE_A_START_ADDR); /* аргумент не показчик, а ціле число                */
//	/*****************************************************************************************************************************
//*/
//			for (uint8_t i=0;i<128;i++){
//				ArrayRx[i]=(uint32_t)i;
//			}
//
//
///*****************************************************************************************************************************
// * ЗАПИС масиву 32-розрядних слів  З ОПЕРАТИВНОЇ ПАМ'ЯТІ ДО ФЛЕШ (варіант 1 - лише структуру)
// *****************************************************************************************************************************
//включає обчислення і запис останнім елементом масиву CRC
//включає превірку після запису слово в слово записаного масиву в флеш і масиву, що записувався в оперативній памяті
//*/
//		uint32_t vdMemoryRecordStatus=0;
//		uint32_t* u32ArrayRx = (uint32_t*)ArrayRx;
//
//		vfFlashOpStatus= f_write_to_flash_record (
//								 u32ArrayRx,
//							     (uint32_t*)FLASH_PAGE_A_START_ADDR,
//							     sizeof ArrayRx,
//								 &vdMemoryRecordStatus
//								 );
///* 5) Обнулимо тестовий масив 									*/
//		memset (ArrayRx, 0, sizeof(ArrayRx));
///* 6) Прочитаємо з флеш в масив в памяті весь масив з перевірокою CRC*/
///*****************************************************************************************************************************
//* ЧИТАННЯ масиву 32-розрядних слів З ФЛЕШ ДО ОПЕРАТИВНОЇ ПАМЯТІ (варіант 1 - лише масиву слів)
//* (включає перевірку CRC)
//*****************************************************************************************************************************/
////		vfFlashOpStatus= f_read_from_flash_rec (
////												 (uint32_t *)FLASH_PAGE_A_START_ADDR,
////												  ArrayRx,
////											      sizeof (ArrayRx)
////											   );
//
//		uint32_t* pSettingD =NULL;
//		pSettingD=(uint32_t*)Settings_TK_fl;
//		vfFlashOpStatus= f_read_from_flash_rec (
//												  (uint32_t *)FLASH_PAGE_A_START_ADDR,
//												  pSettingD,
//												  sizeof (Settings_TK_fl)
//												);
//
//		/* ПОМЕНЯТЬ УСТАВКУ НОМЕР USTAVKA НОМЕР I_UST*/
//
//
//
////
////	/*1) Заповнимо частину масиву числами підряд:*/
////		for (uint8_t i=0;i<128;i++){
////			ArraySettings[i]=i;
////		}
////	/* 2) Стираємо сторінку 120 */
////	/**************************************************************************************************************
////	 * СТИРАННЯ СТОРІНКИ ФЛЕШ stm32F105
////	 *************************************************************************************************************/
////
////		f_flash_page_erase(FLASH_PAGE_63); /* аргумент не показчик, а ціле число                */
////	/*****************************************************************************************************************************
////	3) запишемо цей масив на флеш
/////*****************************************************************************************************************************
//// * ЗАПИС масиву 32-розрядних слів  З ОПЕРАТИВНОЇ ПАМ'ЯТІ ДО ФЛЕШ (варіант 1 - лише структуру)
//// *****************************************************************************************************************************
////включає обчислення і запис останнім елементом масиву CRC
////включає превірку після запису слово в слово записаного масиву в флеш і масиву, що записувався в оперативній памяті
////*/
////		uint32_t vdMemoryRecordStatus=0;
////		vfFlashOpStatus= f_write_to_flash_record (
////								 &ArraySettings,
////							     (uint32_t*)FLASH_PAGE_63,
////							     sizeof ArraySettings,
////								 &vdMemoryRecordStatus
////								 );
////	/*4) Зчитаємо по одному слову без перевірки CRC				*/
////		for (uint8_t i=0;i<128;i++){
////			ArrayRx[i]=f_read_from_flash_word(FLASH_PAGE_63+4*i);;
////		}
////	/* 5) Обнулимо тестовий масив 									*/
////		memset (ArrayRx, 0, sizeof(ArrayRx));
////	/* 6) Прочитаємо з флеш в масив в памяті весь масив з перевірокою CRC*/
////		/*****************************************************************************************************************************
////		 * ЧИТАННЯ масиву 32-розрядних слів З ФЛЕШ ДО ОПЕРАТИВНОЇ ПАМЯТІ (варіант 1 - лише масиву слів)
////		 * (включає перевірку CRC)
////		*****************************************************************************************************************************/
////		vfFlashOpStatus= f_read_from_flash_rec (
////											 (uint32_t *)FLASH_PAGE_63,
////											  ArrayRx,
////										      sizeof (ArrayRx)
////										 );
////
//		f_VD_Meniat_USTAVKU (8, -999);
//
//}
//
//flash_status_t f_vkFlasOldInit(void){
//	flash_status_t vfFlashOpStatus = FLASH_UNKNOWN_Err;
//	uint32_t* pSettingD =NULL;
//	pSettingD=(uint32_t*)Settings_TK_fl;
//
//	/* считываем массив из основной страницы флеш в ОП в Settings_TK_fl
// 	 Зчитаємо по одному слову без перевірки CRC							*/
//	for (uint8_t i=0;i<128;i++){
//		pSettingD[i]=f_read_from_flash_word(FLASH_PAGE_A_START_ADDR+4*i);
//	}
//	vfFlashOpStatus = f_VD_Sets_Write ();
//	return vfFlashOpStatus;
//}
//
//
