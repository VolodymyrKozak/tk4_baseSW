/*
 * mb_master.c
 *
 *  Created on: 27 жовт. 2017 р.
 *      Author: Volodymyr.Kozak
 */
//#include "stm32f4xx_hal_uart.h"
//#include "stm32f4xx_it.h"
#include "mb_master.h"

#include "usart.h"


#include "tkModbus/mb.h"

//struct modbus_master_tx_msg_t modbus_master_tx = {0x00};
volatile modbus_master_rx_msg_t modbus_master_rx = {0x00};
enum master_state_5_t master_state_5 = Idle;
static volatile uint8_t master_rx_iddle_msg_length =0;




//struct mb_t {
//	uint8_t tx_m_msg_length;
//	uint8_t tx_m_msg [125];
//}
modbus_master_tx_msg_t m_tx[50]=

	{
		/*******************************************************************************************************************************************/
		/* 0х08 Д І А Г Н О С Т И К А */
		/*******************************************************************************************************************************************/
		{8, { 0x01, 0x08, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00 }},
		/*******************************************************************************************************************************************
		*  П О М И Л К О В И Й      К О Д     Ф У Н К Ц І Ї      М о д б у с
		*******************************************************************************************************************************************/
		{8, { 0x01, 0x08, 0x01, 0x00, 0xFF, 0xFF, 0x00, 0x00 }},        /* помилковий код підфункції											  */
		{7, { 0x01, 0x08, 0x00, 0x00, 0xFF, 0x00, 0x00 }},				/* помилкова довжина повідомлення										  */


		/*******************************************************************************************************************************************/
		/* 0х03 	R E A D    H O L D I N G    R E G I S T E R S  																				   */
		/*******************************************************************************************************************************************/
		{8, { 0x01, 0x03, 0xAA, 0xAA, 0x00, 0x07, 0x00, 0x00 }},		/* 0xAA, 0xAA - адреса, з якої читати регістриб 0х07 - кількість регістрів */
		{8, { 0x01, 0x03, 0xAA, 0xAA, 0x00, 0x01, 0x00, 0x00 }},      	/* 0xAA, 0xAA - адреса, з якої читати регістриб 0х01 - кількість регістрів */
		/*******************************************************************************************************************************************
		*  П О М И Л К О В И Й      К О Д     Ф У Н К Ц І Ї      М о д б у с
    	********************************************************************************************************************************************/
		{8, { 0x01, 0x03, 0xAA, 0x00, 0xF7, 0x00, 0x00, 0x00 }},     	/* надто велика кількість регістрів										   */
		{7, { 0x01, 0x03, 0x00, 0x07, 0x00, 0x00}},      				/* помилкова довжина повідомлення 										   */

		/*******************************************************************************************************************************************/
		/* 0х06     W R I T E    S I N G L E    R E G I S T E R 																				   */
		/*******************************************************************************************************************************************/
		{8, { 0x01, 0x06, 0x0F, 0x00, 0x01, 0x27, 0x00, 0x00 }},		/* 0x0F, 0x00 - адреса, куди писати, 0х01, 0х27 - що писати				   */
		/*******************************************************************************************************************************************
		*  П О М И Л К О В И Й      К О Д     Ф У Н К Ц І Ї      М о д б у с
		********************************************************************************************************************************************/
		{7, { 0x01, 0x06, 0x0F, 0xAA, 0x00,  0x00, 0x00 }}      		/* помилкова довжина повыдомлення 										   */


	};


						/* 0x01, 0x03, 0xII, 0xII, 0x00, 0x07 0xCRCry 0xCRCrh*/
volatile uint8_t TEST[8]={0x01,0x08,0x00,0x00,0xFF,0xFF,0x00,0x00};
uint8_t Sent_Slave_Unicast_Address = 0x00;
HAL_StatusTypeDef TxIt=0;
#if defined (MASTERRTU_HUART_4) || defined (MASTERRTU_HUART_5)
void MB_Master_Tx_Imitation (void){
	 /*******************************************************************************************************************************************/

	  HAL_Delay(2000);
	  uint8_t i=0;
	  Sent_Slave_Unicast_Address=m_tx[i].msg[0];
	  uint16_t crc_calc_tx = CRC_16x (&m_tx[i].msg, m_tx[i].length-2);
	  m_tx[i].msg[m_tx[i].length-1]=crc_calc_tx / 0x100;
	  m_tx[i].msg[m_tx[i].length-2]=crc_calc_tx % 0x100;

	  HAL_StatusTypeDef TxIt =HAL_UART_Transmit_IT(&huart4, &(m_tx[i].msg), m_tx[i].length/* RX_MAX_SLAVE_MSG_LENGTH*/);
	  	if (TxIt != HAL_OK) {
	  	//	mb_f_status=MODBUS_ERR_TX;
	  	}
	  LCD_Printf ("START\n");
	  for (uint8_t i_rt=0; i_rt<m_tx[i].length; i_rt++){
		  LCD_Printf ("0x%X%X ", m_tx[i].msg[i_rt] / 0x10, m_tx[i].msg[i_rt]  %0x10);
	  }
	  LCD_Printf ("\n");
//	  LCD_Printf ("0x%X%X\n", m_tx[i].msg[m_tx[i].length-2]/0x10, m_tx[i].msg[m_tx[i].length-2]%0x10);
//	  LCD_Printf ("0x%X%X\n", m_tx[i].msg[m_tx[i].length-1]/0x10, m_tx[i].msg[m_tx[i].length-1]%0x10);

	/*********************************************************************************************************************************************/
}
/* Викликається функцією void HAL_UART_RxIdleCallback(UART_HandleTypeDef *UartHandle) файлу mb.c,
 * cпільною при функціонуванні slave та/або master
 * */
void Master_RxIdleCallback(UART_HandleTypeDef huart) { /* Обробка переривання IDLE / протягом часу приймання байта лінія була вільна */

	if (master_state_5==Waiting_for_reply) {

		master_rx_iddle_msg_length=huart.RxXferSize - huart.RxXferCount; /* довжина повідомлення RTU */

		if  (master_rx_iddle_msg_length==0)  {										  /* якщо повідомлення власне не було, то нічого не робимо, */
			/* do nothiong */												  /* чекаємо повідомлення */
		}
		if (master_rx_iddle_msg_length==RX_MAX_MASTER_MSG_LENGTH){
			/* do nothing */
		}
		if ((master_rx_iddle_msg_length !=0 ) && (master_rx_iddle_msg_length!=RX_MAX_MASTER_MSG_LENGTH)){

			modbus_master_rx.length = master_rx_iddle_msg_length;
			/* якщо це повідомлення якоїсь, відмінної від нуля довжини */
			if (HAL_UART_AbortReceive_IT(&huart)!=HAL_OK){ 			  /* припиняємо подальший прийом повідомлення */
//				LCD_Printf ("Fuck you, ABORT !\n");
//				Error_Handler();
			}
			else{}
			master_state_5=m_msg_recived_RxIdle;
		}


	}
}

/********************************************************************************************************************************************/



/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   This example shows a simple way to report end of IT Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void Master_TxIdleCallback(void) {
  /* Set transmission flag: transfer complete */
	if (master_state_5 == m_msg_transmitting){
			master_state_5 = m_msg_transmitting_Cplt;
	}
}


/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  *
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle) {

	if ((UartHandle == &huart4)||(master_state_5 == m_msg_transmitting)){
		master_state_5 = m_msg_transmitting_ERROR;
	}
	else if ((UartHandle == &huart4)||(master_state_5 == m_msg_receiving)){
		master_state_5 = m_msg_receiving_ERROR;
	}
	else {}
}
#endif defined (MASTERRTU_HUART_4) || defined (MASTERRTU_HUART_5)


