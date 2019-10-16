/*
 * mb_slave.h
 *
 *  Created on: 10 жовт. 2017 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef MB_SLAVE_H_
#define MB_SLAVE_H_

#include "tkConfig.h"
#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)

#include <stdint.h>
#include "usart.h"

#include "mb.h"
/**************************************************************************************************
 *              ПУБЛІЧНІ ОГОЛОШЕННЯ                                        						 *
 *************************************************************************************************/

extern uint8_t volatile msg_received[RX_MAX_SLAVE_MSG_LENGTH];
extern uint8_t volatile msg_received_length5;
extern uint8_t volatile msg_transmitted[TX_MAX_SLAVE_MSG_LENGTH];
extern uint8_t volatile msg_transmitted_length;

extern void MB_Slave_Selective_Handler(
			uint8_t *rx_msg,        /* отримане повідомлення 				*/
			uint8_t rx_msg_length,  /* кількість байт в отр.повідомл.		*/
			uint8_t *tx_msg,	    /* повідомлення для відповіді    		*/
			uint8_t *tx_msg_length  /*довжина повідомлення для відповіді	*/
			);

extern void MB_Slave_Broadcast_Handler(
			uint8_t *rx_msg,		/* отримане повідомлення 				*/
			uint8_t rx_msg_length   /* кількість байт в отр.повідомл.		*/
			);
/**************************************************************************************************
 *                                     ПРИВАТНІ ОГОЛОШЕННЯ                                        *
 *************************************************************************************************/
//typedef unsigned char name[30];
/* MODBUS_SLAVE може знаходись водному з станів  modbus_slave_state: */

typedef enum {
	unknown_slave_state,
	no_message_for_us,   	/* у каналі не виявлено повідомлень, які підлягають обробці */
	message_discovered,		/* у каналі виявлено повідомлення */
	msg_regognition_1,
	msg_recognized,
	answer_transmitting,	/* здійснюється передача відповіді										*/
	our_answer_is_sent,		/* завершено передачу відповіді, slave готовий приймати нове повідомлення*/
	debug					/* пустушка																*/
} modbus_slave_state_t;
extern modbus_slave_state_t modbus_slave_state;
typedef enum {
	unknown_message,         /* виявлено поки-що невідоме повідомлення*/
	wrong_message,			/* виявлено хибне повідомлення (wrong CRC, wrong adress     */

						/* виявлено повідомлення, що підлягає обробці, здійснюється обробка:	*/
	new_message_for_us,  	/*     =адресного повідомлення для нашого slave-контролера 				*/
	broadcast_message,		/*     =циркулярного повідомлення для всіх slaves-контролерів			*/
						/* підготовка відповіді, згідно з логікою протокола Модбас здійснюється
						   функцією MB_Slave_Handling (void) файлу mb_slave5	*/
} msg_t;
extern msg_t msg_status; /* що за повідомлення	*/

/* Виявлення на вході RTU-приймача повідомлень, які підлягають обробці здійснюються в режимі переривань
 * Slave_RxIdleCallback <- HAL_UART_RxIdleCallback(&SLAVE_HUART), після виявлення
 * відсутності на вході сигналу протягом терміну передачі понад одного символа.
 * Для активізації такого переривання, відсутнього в стандарних функціях бібліотеки HAL,
 * необхідно у файлі usart.с у функції
 * void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
 * після   HAL_NVIC_EnableIRQ(UART5_IRQn);
 добавити рядок
           __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
  ****
  Виявлення на вході RTU-приймача повідомлень здійснюється лише у стані no_message_for_us.
  Обробка виявленого вхідного повідомлення здійснюється в основному циклі, при виявленні стану
  new_message_for_us або
  broadcast_message
  Передача відповіді у стані answer_transmitting.
  Після отримання переиривання по завершенні передачі, система повертається у стан no_message_for_us
  і готова до приймання наступних повідомлень..

  Варто мати на увазі, що ми не можемо обробити більше одного повідомлення протягом одного основного циклу.

 */

/**************************************************************************************************
 *                                     D E B U G                                       *
 *************************************************************************************************/
typedef enum {
	no_idle,
	yes_idle
} rx_idle_state_t;
extern rx_idle_state_t rx_idle_state;

typedef enum {
	no_rx_err_call_back,
	yes_rx_err_call_back
} rx_err_call_back_t;
extern rx_err_call_back_t rx_err_call_back;

/************************************************************************************************
 *                                  ПУБЛІЧНІ ОГОЛОШЕННЯ
 ***********************************************************************************************/
//void MB_Master_Tx_Imitation (void);
void MB_Slave_Receive_Init (void);
void Slave_RxIdleCpltCallback(void);
void MB_Slave_Transmit (void);
void Slave_TxCpltCallback(void);



#endif defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)




#endif /* MB_SLAVE_H_ */
