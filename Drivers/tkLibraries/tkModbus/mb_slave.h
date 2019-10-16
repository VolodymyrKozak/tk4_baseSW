/*
 * mb_slave.h
 *
 *  Created on: 10 ����. 2017 �.
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
 *              ���˲�Ͳ ����������                                        						 *
 *************************************************************************************************/

extern uint8_t volatile msg_received[RX_MAX_SLAVE_MSG_LENGTH];
extern uint8_t volatile msg_received_length5;
extern uint8_t volatile msg_transmitted[TX_MAX_SLAVE_MSG_LENGTH];
extern uint8_t volatile msg_transmitted_length;

extern void MB_Slave_Selective_Handler(
			uint8_t *rx_msg,        /* �������� ����������� 				*/
			uint8_t rx_msg_length,  /* ������� ���� � ���.�������.		*/
			uint8_t *tx_msg,	    /* ����������� ��� ������    		*/
			uint8_t *tx_msg_length  /*������� ����������� ��� ������	*/
			);

extern void MB_Slave_Broadcast_Handler(
			uint8_t *rx_msg,		/* �������� ����������� 				*/
			uint8_t rx_msg_length   /* ������� ���� � ���.�������.		*/
			);
/**************************************************************************************************
 *                                     ������Ͳ ����������                                        *
 *************************************************************************************************/
//typedef unsigned char name[30];
/* MODBUS_SLAVE ���� ��������� ������� � �����  modbus_slave_state: */

typedef enum {
	unknown_slave_state,
	no_message_for_us,   	/* � ����� �� �������� ����������, �� ��������� ������� */
	message_discovered,		/* � ����� �������� ����������� */
	msg_regognition_1,
	msg_recognized,
	answer_transmitting,	/* ����������� �������� ������										*/
	our_answer_is_sent,		/* ��������� �������� ������, slave ������� �������� ���� �����������*/
	debug					/* ��������																*/
} modbus_slave_state_t;
extern modbus_slave_state_t modbus_slave_state;
typedef enum {
	unknown_message,         /* �������� ����-�� ������� �����������*/
	wrong_message,			/* �������� ����� ����������� (wrong CRC, wrong adress     */

						/* �������� �����������, �� ������ �������, ����������� �������:	*/
	new_message_for_us,  	/*     =��������� ����������� ��� ������ slave-���������� 				*/
	broadcast_message,		/*     =������������ ����������� ��� ��� slaves-����������			*/
						/* ��������� ������, ����� � ������ ��������� ������ �����������
						   �������� MB_Slave_Handling (void) ����� mb_slave5	*/
} msg_t;
extern msg_t msg_status; /* �� �� �����������	*/

/* ��������� �� ���� RTU-�������� ����������, �� ��������� ������� ����������� � ����� ����������
 * Slave_RxIdleCallback <- HAL_UART_RxIdleCallback(&SLAVE_HUART), ���� ���������
 * ��������� �� ���� ������� �������� ������ �������� ����� ������ �������.
 * ��� ���������� ������ �����������, ���������� � ���������� �������� �������� HAL,
 * ��������� � ���� usart.� � �������
 * void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
 * ����   HAL_NVIC_EnableIRQ(UART5_IRQn);
 �������� �����
           __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
  ****
  ��������� �� ���� RTU-�������� ���������� ����������� ���� � ���� no_message_for_us.
  ������� ���������� �������� ����������� ����������� � ��������� ����, ��� �������� �����
  new_message_for_us ���
  broadcast_message
  �������� ������ � ���� answer_transmitting.
  ϳ��� ��������� ������������ �� ��������� ��������, ������� ����������� � ���� no_message_for_us
  � ������ �� ��������� ��������� ����������..

  ����� ���� �� ����, �� �� �� ������ �������� ����� ������ ����������� �������� ������ ��������� �����.

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
 *                                  ���˲�Ͳ ����������
 ***********************************************************************************************/
//void MB_Master_Tx_Imitation (void);
void MB_Slave_Receive_Init (void);
void Slave_RxIdleCpltCallback(void);
void MB_Slave_Transmit (void);
void Slave_TxCpltCallback(void);



#endif defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)




#endif /* MB_SLAVE_H_ */
