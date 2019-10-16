/*
 * prg_MBRTU_SLAVE.c
 *
 *  Created on: 26 груд. 2018 р.
 *      Author: KozakVF
 */
#include "tkConfig.h"
#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
#include "mb_slave.h"
#include "mb_slave5.h"

void MB_Slave_Selective_Handler(uint8_t *rx_msg,
								uint8_t rx_msg_length,
								uint8_t *tx_msg,
								uint8_t *tx_msg_length){

	/*******************************************************************************************************************************************/
	/* 0х08 Д І А Г Н О С Т И К А */
	/*******************************************************************************************************************************************/
	if ((rx_msg[1]==DIAGNOSTIC ) && (rx_msg_length>=4)){
		/* Якщо отримана Modbus-функція 0х08 Діагностика
		 * - повтрюємо отримане повідомлення            */
		for (uint8_t i_dgn=0;i_dgn<rx_msg_length;++i_dgn){
			tx_msg[i_dgn]=msg_received[i_dgn];
		}
		(*tx_msg_length)=rx_msg_length;
	}
	else{/* Якщо відповідь не передається */
		(*tx_msg_length)=0;
	}

}
void MB_Slave_Broadcast_Handler(uint8_t *rx_msg,
								uint8_t rx_msg_length){

}
#endif (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
