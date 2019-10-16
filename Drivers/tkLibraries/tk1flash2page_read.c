/*
 * tk1flash2page_read.c
 *
 *  Created on: 18 бер. 2019 р.
 *      Author: KozakVF
 */

#include "tk1flash2page.h"

static struct {
	uint32_t 	page_addr;
	size_t 		RecSize	 ;
    uint8_t 	N 		 ;
    uint8_t 	l_start  ;
    uint8_t     l_i		 ;
} l = {0};


void f_look_next_flash_to_read_Init(void){
	memset (&l, 0, sizeof(l));
}

uint32_t f_look_next_flash_to_read(uint32_t page_addr, size_t RecSize){
	if (page_addr != l.page_addr){ return  0xFFFFFFFF;}
	/* Якщо розмір запису більше ніж пів-сторінки */
	if ((RecSize > 0x400) || (RecSize < 0x8)){return 0xFFFFFFFF;}
	/* Якщо розмір запису не кратний розміру сторінки  */
	//if ( __builtin_popcount (RecSize) != 1 ){return 0xFFFFFFFF;};
	if ((0x800 % RecSize) !=0) {return 0xFFFFFFFF;}
	/* якщо стартували з другої сторінки */
	if (l.l_start >= l.N){
		/* з останього запису - перша сторінка стерта*/
		if (l.l_start == (l.N*2-1)){
			/* проходимо лише другу сторінку*/
			if (l.l_i >l.N){l.l_i--;}
		}
		else{
			/* зменшуємо номер запису до нуля, проходячи другу і першу сторінки */
			if (l.l_i >0){l.l_i--;}
		}
	}
	/* якщо стартували з першої сторінки */
	else if ((l.l_start < l.N)){
		/* якщо у попередньому циклі першу сторінку уже пройшли до нуля*/
		if ((l.l_i == 0)){
			/* і якщо ми при останньому запису не стерли другу сторінку,
			 * бо запис на першій був останнім */
			if (l.l_start!=(l.N-1)){
				 /* починаємо з найбільшої адреси запису на другій сторінці,
				 * адже, вся сторінка заповнена.. */
				l.l_i = l.N*2-1;
			}
		}
		/* якщо це не нульовий запис */
		else {
			/* проходимо сторінку до нуля */
			if ((l.l_i <l.N) &&(l.l_i >0)){l.l_i--;
			/* після чого переходимо на другу сторінку */
			}
			/* i проходимо всю другу сторінку */
			else if (l.l_i >l.N) {
				l.l_i--;
			}
			else{}
		}
	}
	else {}
	return  (page_addr+l.l_i*RecSize);
}


uint32_t f_look_start_flash_to_read (uint32_t page_addr, size_t RecSize){
	/* Якщо це не адреса першої з двої сторінок */
	if ((page_addr % 0x1000) != 0) {return 0xFFFFFFFF;}
	/* Якщо розмір запису більше ніж пів-сторінки */
	if ((RecSize > 0x400) || (RecSize < 0x8)){return 0xFFFFFFFF;}
	/* Якщо розмір запису не кратний розміру сторінки  */
	//if ( __builtin_popcount (RecSize) != 1 ){return 0xFFFFFFFF;};
	if ((0x800 % RecSize) !=0) {return 0xFFFFFFFF;}

	uint32_t record_addr = 0xFFFFFFFF;
	uint32_t Nrecords = 0x1000 / RecSize;
	f_look_next_flash_to_read_Init();
	l.page_addr			=page_addr;
	l.RecSize			=RecSize;
    l.N 				=0x800 / RecSize;

	while (record_addr == 0xFFFFFFFF){
		/* Читаємо перше слово останнього запису другої сторінки */
		uint32_t wordtest = f_read_flash_word((page_addr+(Nrecords-1)*RecSize));
		if (wordtest==0xFFFFFFFF){
			/* якщо там нічого не записано */
			for (uint8_t i = Nrecords-1; i>0;i--){
				/* перевіряємо перше слово кожного запису починаючи з останнього до першого */
//				uint32_t addr = page_addr+(i-1)*RecSize;
				uint32_t wordtest1=f_read_flash_word(page_addr+(i-1)*RecSize);
				/* якщо виявили наявність записаних даних (i-1) - виходимо 	*/
				if (wordtest1 != 0xFFFFFFFF){
					/* отже (i-1) - номер останнього запису					*/
					record_addr=page_addr+(i-1)*RecSize;
					/* фіксуємо номер останнього запису для обчислення номерів/адрес усіх інших записів */
					l.l_start = i-1;
					l.l_i = l.l_start;
					break;

				}
			}
		}
		/* якщо виявлено наявний запис у першому слові останнього запису другої сторінки*/
		else if (wordtest!=0xFFFFFFFF){
			/* перевіряємо перше слово останнього запису першої сторінки */
			uint32_t wordtest0 = f_read_flash_word(page_addr+(Nrecords/2-1)*RecSize);
			/* якщо там нічого не записано */
			if (wordtest0==0xFFFFFFFF){
				/* перевіряємо усі записи першої сторінки, починаючи з останнього */
				for (int8_t i = Nrecords/2-1; i>0;i--){
					wordtest0=f_read_flash_word(page_addr+(i-1)*RecSize);
					if (wordtest0 != 0xFFFFFFFF){
						/* якщо знайшли запис - видаємо адресу останнього запису */
						record_addr=page_addr+(i-1)*RecSize;
						l.l_start = i-1;
						l.l_i = l.l_start;
						break;
					}
					else if (wordtest0 == 0xFFFFFFFF){
						if (i==1){
							/* якщо на першій сторінці запису не знайшли -
							 * початковим записом є останній запис другої сторінки */
							l.l_start = l.N*2-1;
							l.l_i = l.l_start;
							record_addr=page_addr+(l.l_start)*RecSize;
						}
					}
					else{}
				}
			}
		}
		else{}
	}


	return record_addr;
}
