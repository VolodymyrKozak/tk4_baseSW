/*
 * tk1flash2page.h
 *
 *  Created on: 18 бер. 2019 р.
 *      Author: KozakVF
 */

#ifndef TKLIBRARIES_TK1FLASH2PAGE_H_
#define TKLIBRARIES_TK1FLASH2PAGE_H_
#include "stdint.h"
#include "string.h"
#include "tk0flash_base.h"
/*
 * 1. Виділяємо для зберігання конкретного типу записів по 2 сторінки флеш,
 *  починаючи з парної (кожна сторінка - 2кб/2048байт/0х800байт)
 * size запису - RecSize, обов'язково ступiнь двiйки (1024, 512, 256, 128, 64, 32, 16..
 * Кількість записів на сторінці - 0х800 / RecSize;*/

 flash_status_t f_write2page_record (uint32_t page_addr, /* парна адреса сторінки  */
									 uint32_t *pRecord,  /* показчик на запис в RAM*/
									 size_t RecSize);    /* розмір запису в байтах */
 /* 2. При збереженні записів для пошуку адреси куди записувати
 * 2 якщо останній пустий -йдемо до першого, поки не здибаємо зайнятий
 * пишемо в наступний після занятого, а або в нульовый, якщо зайнятого не знайшли
 * 3 якщо останній - зайнятий, перевіряємо зайнятість останнього запису першої сторінки
 * 3.1. якщо він зайнятий - стираємо першу сторінку, пишемо в 0
 * 3.2. якщо пустий - йдемо до першого, поки не здибаємо зайнятий,
 * пишемо в наступний після занятого або в нульовый, якщо зайнятого не знайшли
 * 4. Якщо поточний==3 стираэмо наступну сторінку..
 * */
void f_look_next_flash_to_read_Init(void);
uint32_t f_look_start_flash_to_read(uint32_t page_addr, size_t RecSize);
uint32_t f_look_next_flash_to_read(uint32_t page_addr, size_t RecSize);

#endif /* TKLIBRARIES_TK1FLASH2PAGE_H_ */
