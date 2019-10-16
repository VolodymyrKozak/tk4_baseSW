/*
 * mb.c
 *
 *  Created on: 30 æîâò. 2017 ð.
 *      Author: Volodymyr.Kozak
 */
#include "mb.h"




#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
extern void Slave_RxIdleCpltCallback(void);
extern void Slave_TxCpltCallback(void);
#endif //defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)

#if defined MASTERRTU_HUART_4 || defined MASTERRTU_HUART_5
extern void Master_RxIdleCallback(void);
extern void Master_TxIdleCallback(void);
#endif //defined (MASTERRTU_HUART_4) || defined (MASTERRTU_HUART_5)

#ifdef DEBUGGING_ONLY
	volatile uint32_t IdleCounter =0;
	volatile uint32_t CpltCounter =0;
#endif

/* Îáðîáêà ïåðåðèâàííÿ IDLE / ïðîòÿãîì ÷àñó ïðèéìàííÿ áàéòà ë³í³ÿ áóëà â³ëüíà */
void HAL_UART_RxIdleCallback(UART_HandleTypeDef *UartHandle) {
#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
	if (UartHandle == &SLAVE_HUART){
#ifdef DEBUGGING_ONLY
	IdleCounter++;
#endif
		Slave_RxIdleCpltCallback();
	}
#endif //defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
}
//efined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) {
#if defined MASTERRTU_HUART_4 || defined MASTERRTU_HUART_5
		Master_TxIdleCallback();
#endif defined MASTERRTU_HUART_4 || defined MASTERRTU_HUART_5

#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
		Slave_TxCpltCallback();
#endif // (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
}
/********************************************************************************************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle){

#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
	if (UartHandle == &SLAVE_HUART){
#ifdef DEBUGGING_ONLY
	CpltCounter++;
#endif
		Slave_RxIdleCpltCallback();
	};
#endif //defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
}


#if defined (MASTERRTU_HUART_4) || defined (MASTERRTU_HUART_5)
HAL_StatusTypeDef tk4_Modbus_Session (	UART_HandleTypeDef *phuart,
										modbus_master_tx_msg_t *p_tx_msg,
										modbus_master_rx_msg_t *p_rx_msg,
										uint32_t msg_timeout
										){
	volatile HAL_StatusTypeDef mb_session_status = HAL_ERROR;
	while (mb_session_status != HAL_OK) {
	/* Â²ÄÏÐÀÂÊÀ ÏÎÂ²ÄÎÌËÅÍÍß */
//#define RS485_UART4_SWITCH_PORT GPIOA
//#define RS485_UART4_SWITCH_PIN  GPIO_PIN_15
//#define RS485_UART5_SWITCH_PORT
//#define RS485_UART5_SWITCH_PIN  GPIO_PIN_3

		if (phuart->Instance==UART4){
			HAL_GPIO_WritePin(RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN, GPIO_PIN_SET);
//			volatile GPIO_PinState lh6j5zi= HAL_GPIO_ReadPin (RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PORT);
//			volatile GPIO_PinState lzegwi= HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3);
		}
		else if ((phuart->Instance==UART5)){
			HAL_GPIO_WritePin(RS485_UART5_SWITCH_PORT, RS485_UART5_SWITCH_PIN, GPIO_PIN_SET);
//			volatile GPIO_PinState ljji= HAL_GPIO_ReadPin (RS485_UART5_SWITCH_PORT, RS485_UART5_SWITCH_PIN);
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
//			volatile GPIO_PinState li= HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_8);
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
//			volatile GPIO_PinState lzi= HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3);
//			volatile GPIO_PinState lzegwi= HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_3);
		}
		else{}
//		volatile GPIO_PinState ljji= HAL_GPIO_ReadPin (RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN);
//		volatile GPIO_PinState lhjjji= HAL_GPIO_ReadPin (RS485_UART5_SWITCH_PORT, RS485_UART5_SWITCH_PIN);
		mb_session_status =
				HAL_UART_Transmit (phuart, p_tx_msg->msg, p_tx_msg->length , 80 );

		if (mb_session_status != HAL_OK) {
			break;
		}
		if (phuart->Instance==UART4){
			HAL_GPIO_WritePin(RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN, GPIO_PIN_RESET);
		}
		else if ((phuart->Instance==UART5)){
			HAL_GPIO_WritePin(RS485_UART5_SWITCH_PORT, RS485_UART5_SWITCH_PIN, GPIO_PIN_RESET);
//			volatile GPIO_PinState ljji= HAL_GPIO_ReadPin (RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN);
		}
		else{}
//	volatile GPIO_PinState ljuyji= HAL_GPIO_ReadPin (RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN);
//	volatile GPIO_PinState lhdfjjji= HAL_GPIO_ReadPin (RS485_UART5_SWITCH_PORT, RS485_UART5_SWITCH_PIN);

	/* Îòðèìàííÿ-î÷³êóâàííÿ â³äïîâ³ä³ */
		mb_session_status =
			HAL_UART_Receive (phuart,p_rx_msg->msg, p_rx_msg->length,  msg_timeout);
		if (mb_session_status != HAL_OK) {
			break;
		}
		mb_session_status=HAL_OK;
	}
	return mb_session_status;
}

HAL_StatusTypeDef tk4_Modbus_Tx_Session (UART_HandleTypeDef *phuart,
										 modbus_master_tx_msg_t *p_tx_msg){
	volatile HAL_StatusTypeDef mb_session_status = HAL_ERROR;
	while (mb_session_status != HAL_OK) {
	/* Â²ÄÏÐÀÂÊÀ ÏÎÂ²ÄÎÌËÅÍÍß */
		if (phuart->Instance==UART4){
			HAL_GPIO_WritePin(RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN, GPIO_PIN_SET);
		}
		else if ((phuart->Instance==UART5)){
			HAL_GPIO_WritePin(RS485_UART5_SWITCH_PORT, RS485_UART5_SWITCH_PIN, GPIO_PIN_SET);
			volatile GPIO_PinState ljji= HAL_GPIO_ReadPin (RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN);

		}
		else{}
		volatile GPIO_PinState ljji= HAL_GPIO_ReadPin (RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN);
		volatile GPIO_PinState lhjjji= HAL_GPIO_ReadPin (RS485_UART5_SWITCH_PORT, RS485_UART5_SWITCH_PIN);
		mb_session_status =
				HAL_UART_Transmit (phuart, p_tx_msg->msg, p_tx_msg->length , 80 );

		if (mb_session_status != HAL_OK) {
			break;
		}
		if (phuart->Instance==UART4){
			HAL_GPIO_WritePin(RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN, GPIO_PIN_RESET);
		}
		else if ((phuart->Instance==UART5)){
			HAL_GPIO_WritePin(RS485_UART5_SWITCH_PORT, RS485_UART5_SWITCH_PIN, GPIO_PIN_RESET);
			volatile GPIO_PinState ljji= HAL_GPIO_ReadPin (RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN);

		}
		else{}
		volatile GPIO_PinState ldghjji= HAL_GPIO_ReadPin (RS485_UART4_SWITCH_PORT, RS485_UART4_SWITCH_PIN);
		volatile GPIO_PinState lhfajjji= HAL_GPIO_ReadPin (RS485_UART5_SWITCH_PORT, RS485_UART5_SWITCH_PIN);
		mb_session_status=HAL_OK;
	}
	return mb_session_status;
}
#endif //defined (MASTERRTU_HUART_4) || defined (MASTERRTU_HUART_5)
