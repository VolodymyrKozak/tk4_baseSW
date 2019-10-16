/*
 * tk1flash2page_write.c
 *
 *  Created on: 18 бер. 2019 р.
 *      Author: KozakVF
 */

#include "tk1flash2page.h"



static uint32_t f_look_for_flash_address (uint32_t page_addr, size_t RecSize);





flash_status_t f_write2page_record (uint32_t page_addr, /* парна адреса сторінки  */
									 uint32_t *pRecord,  /* показчик на запис в RAM*/
									 size_t RecSize){    /* розмір запису в байтах */
	flash_status_t w300_status = FLASH_UNKNOWN_Err;
	uint32_t flashAddr = f_look_for_flash_address (page_addr, RecSize);
	if (f_write_to_flash_record (	pRecord,
									flashAddr,
									RecSize
								 ) != FLASH_OK){
		if ((flashAddr - page_addr) < 0x800){
			w300_status=VD_PRIMARY_PAGE_Err;
		}
		else {
			w300_status=VD_SECONDARY_PAGE_Err;
		}
	}
	else {
		w300_status=FLASH_OK;
	}
//	uint32_t volatile ghg[8] = {0};
//	for (uint8_t i=0;i<8;i++){
//		ghg[i] = f_read_flash_word(0x08033000+512*i);
//	}
//	volatile uint32_t flashReadAddr = f_look_start_flash_to_read(page_addr, RecSize);
//	uint32_t volatile ljmkl[11]={0};
//
//	for (uint8_t i=0;i<11;i++){
//		ljmkl[i]=f_look_next_flash_to_read(page_addr, RecSize);
//	}
//	HAL_Delay(1);

	return w300_status;
}
/*
 * 1. Виділяємо для зберігання конкретного типу протоколу роботи ПО 2 сторінки
 * флеш, починаючи з парної
 * size запису - RecSize, обов'язково ступiнь двiйки (1024, 512, 256, 128, 64, 32, 16..
 * кількість записів на сторінці - 0х800 / RecSize;
 * 2 якщо останній пустий -йдемо до першого, поки не здибаємо зайнятий
 * пишемо в наступний після занятого, а або в нульовый, якщо зайнятого не знайшли
 * 3 якщо останній - зайнятий, перевіряємо зайнятість останнього запису першої сторінки
 * 3.1. якщо він зайнятий - стираємо першу сторінку, пишемо в 0
 * 3.2. якщо пустий - йдемо до першого, поки не здибаємо зайнятий,
 * пишемо в наступний після занятого або в нульовый, якщо зайнятого не знайшли
 * 4. Якщо поточний==3 стираємо наступну сторінку..
 * */


static uint32_t f_look_for_flash_address (uint32_t page_addr, size_t RecSize){
	/* Якщо це не адреса першої з двої сторінок */
	if ((page_addr % 0x1000) != 0) {return 0xFFFFFFFF;}
	/* Якщо розмір запису більше ніж пів-сторінки */
	if ((RecSize > 0x400) || (RecSize < 0x8)){return 0xFFFFFFFF;}
	/* Якщо розмір запису не кратний розміру сторінки  */
	//if ( __builtin_popcount (RecSize) != 1 ){return 0xFFFFFFFF;};
	if ((4096 % RecSize) !=0) {return 0xFFFFFFFF;}
	uint32_t record_addr = 0xFFFFFFFF;
	uint32_t Nrecords = 0x1000 / RecSize;
	while (record_addr == 0xFFFFFFFF){
		/* Читаємо перше слово останнього запису другої сторінки */
		uint32_t wordtest = f_read_flash_word((page_addr+(Nrecords-1)*RecSize));
		if (wordtest==0xFFFFFFFF){
			/* якщо там нічого не записано */
			for (uint8_t i = Nrecords-1; i>0;i--){
				/* перевіряємо перше слово кожного запису починаючи з останнього до першого */
//				uint32_t addr = page_addr+(i-1)*RecSize;
				uint32_t wordtest1=f_read_flash_word(page_addr+(i-1)*RecSize);


				/* якщо виявили наявність записаних даних (i-1) - виходимо */
				if (wordtest1 != 0xFFFFFFFF){
					/* отже (i) - номер запису, куди можна писати */
					record_addr=page_addr+i*RecSize;
					break;

				}
				/* якщо дійшли до нульового запису і він пустий -  */
				else if (wordtest1 == 0xFFFFFFFF){
					if(i==1){
						f_flash_page_erase (page_addr);
						record_addr=page_addr;
					}
				}
				else{}
			}
		}
		/* якщо виявлено наявний запис у першому слові останнього запису другої стрінки*/
		else if (wordtest!=0xFFFFFFFF){
			/* перевіряємо перше слово останнього запису першої сторінки */
			uint32_t wordtest0 = f_read_flash_word(page_addr+(Nrecords/2-1)*RecSize);
			/* якщо там нічого не записано */
			if (wordtest0==0xFFFFFFFF){
				/* перевіряємо усі записи першої сторінки, починаючи з останньої */
				for (int8_t i = Nrecords/2-1; i>0;i--){
					wordtest0=f_read_flash_word(page_addr+(i-1)*RecSize);
					if (wordtest0 != 0xFFFFFFFF){
						/* якщо знайшли запис - видаємо адресу наступного запису */
						record_addr=page_addr+i*RecSize;break;
					}
					else if(wordtest0 == 0xFFFFFFFF){
						/* якщо дійшли до кінця і вільного місця не виявили,
						 * хоча такого бути не повинно   */
						if (i==1){
							f_flash_page_erase (page_addr);
							record_addr=page_addr;
						}
					}
					else{}
				}
				/* Якщо знайшли вільне місце */
			}
			/* якщо й перша сторінка заповнена,
			 * хоча такого буди не повинно */
			else if (wordtest0 != 0xFFFFFFFF){
				f_flash_page_erase (page_addr);
				record_addr=page_addr;
			}
			else{}
		}
		else{}
	}
		/* Якщо записуємо останній запис першої сторінки */
		if (record_addr == page_addr+(Nrecords/2-1)*RecSize){
			/* Стираємо наступну стрінку */
			f_flash_page_erase (page_addr+0x800);
		}
		/* Якщо записуємо останній запис другої сторінки */
		if (record_addr == page_addr+(Nrecords-1)*RecSize){
			/* Стираємо першу стрінку */
			f_flash_page_erase (page_addr);
		}

	return record_addr;
}

