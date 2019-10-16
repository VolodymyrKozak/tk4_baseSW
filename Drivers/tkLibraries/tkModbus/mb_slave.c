/*
 * mb_slave.c
 *
 *  Created on: 10 жовт. 2017 р.
 *      Author: Volodymyr.Kozak
 */


/*
 * tkModbus_slave7.c
 *
 *  Created on: 27 вер. 2017 р.
 *      Author: Volodymyr.Kozak
 */
#include <tkConfig.h>


#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)

#include "mb_slave.h"
#include <stdint.h>
#include "gpio.h"

#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_it.h"
#include "usart.h"

#include "tkDebugThings.h"
#include "mb.h"
#include "tkCRC.h"

/* ОБРОБКА ВХІДНОГО ПОВІДОМЛЕННЯ MODBUS_RTU
 *
// * Функція  RX_Init_modbus_slave (void) викликається при ініціалізації slave-контролера
 *  постійно сканує вхід з мережі modbus_rtu RS-485->uart,
 *  для чого приймає та перевіряє всі повідомлення в мережі.
 *  Якщо повідомлення
 *  	адресовано цьому slave
 *  	або це циркулярне повідомлення для всіх slaves у мережі
 *  і якщо співпадає CRC у повідомленні з обчисленим  CRC
 *  повідомлення передається на обробку, всі інші повідомлення автоматично відбраковуються.
 *  Логіка повідомлення Modbus у цьому модулі не перевіряється.
 *
 *  На час підготовки відповіді вхід modbus_rtu RS-485->uart блокується.
 *  Підготовлена для Modbus-master відповідь подається в цей модуль,
 *  де укомплектовується байтом адреси Modbus-master та двома байтами СRC
 *  і надсилається в мережу uart ->RS-485->modbus_rtu
 *  Після відправки вихідного повідомлення автоматично запускаєстья функція RX_Init_modbus_slave (void)
 *  для продовження сканування входу мережі modbus_rtu RS-485->uart з метою виявлення повідомлень,
 *  що призначені для цього slave
 *
 ***********************************************************************************************************************************
 *                                   ПУБЛІЧНІ ОГОЛОШЕННЯ ТА ПОЧАТКОВІ ПРИЗНАЧЕННЯ
 */
uint8_t volatile msg_received[RX_MAX_SLAVE_MSG_LENGTH] = {0};   /* буфер вхідного повідомлення 							*/
uint8_t volatile msg_received_length=0;						    /* поточна довжина прийнятого повідомлення в байтах	для обробки переривання Idle	*/
uint8_t volatile msg_received_length5=0;						/* поточна довжина прийнятого повідомлення в байтах	для обробки повідомлення Modbus	*/
uint16_t volatile msg16_transmitted [TX_MAX_SLAVE_MSG_LENGTH/2+1]={0};
uint8_t volatile msg_transmitted[TX_MAX_SLAVE_MSG_LENGTH]={0};
uint8_t volatile msg_transmitted_length=0;


msg_t msg_status=unknown_message; /* що за повідомлення	*/
modbus_slave_state_t modbus_slave_state = unknown_slave_state; /* що за стан */
uint8_t debug_message_discarded_n=0;
const uint8_t Zero252Byte  = {0};

uint8_t crc_last_byte =0;
uint8_t crc_pre_last_byte =0;

rx_idle_state_t rx_idle_state = no_idle;
rx_err_call_back_t rx_err_call_back=no_rx_err_call_back;
/**********************************************************************************************************************
 * *********************************************************************************************************************
 * *******************************************************************************************************************
 *
 */
HAL_StatusTypeDef RxIt=0;


void MB_Slave_Receive_Init (void){

/* The board receives the message */
  /*##-1- Put UART peripheral in reception process ###########################*/

  RxIt = HAL_UART_Receive_IT( &SLAVE_HUART,
		  	  	  	  	  	  &msg_received, /*tx_length*/
							  RX_MAX_SLAVE_MSG_LENGTH
							  );
  if(  RxIt!= HAL_OK) {
	  Error_Handler();
  }
  modbus_slave_state = no_message_for_us;
} // end of Init 2
/**********************************************************************************************************************
 **********************************************************************************************************************
 **********************************************************************************************************************
 **********************************************************************************************************************/

/* Викликається функцією void HAL_UART_RxIdleCallback(UART_HandleTypeDef *UartHandle) файлу mb.c,
 * cпільною при функціонуванні slave та/або master
 * This procedure could be used for aborting any ongoing transfer started in Interrupt or DMA mode.
 *  This procedure performs following operations :
		Disable PPP Interrupts
		Disable the DMA transfer in the peripheral register (if enabled)
		Abort DMA transfer by calling HAL_DMA_Abort_IT (in case of transfer in DMA mode)
		Set handle State to READY
		At abort completion, call user abort complete callback
		This procedure is executed in Interrupt mode, meaning that abort procedure could be considered as completed only
		when user abort complete callback is executed (not when exiting function).
		Прототип функції HAL_UART_RxIdleCallback(UART_HandleTypeDef *UartHandle) оголошено в файлі stm32f1xx_hal_uart.h
*/
uint32_t RxTxSlaveClick=0;
volatile uint32_t SlaveDuration =0;
volatile uint32_t IdleHandlDuration =0;
void Slave_RxIdleCpltCallback(void){
	RxTxSlaveClick=HAL_GetTick();
	if (modbus_slave_state != no_message_for_us){
		return;
	}
    /* довжина повідомлення RTU */
	msg_received_length=SLAVE_HUART.RxXferSize - SLAVE_HUART.RxXferCount;
    /* Є повідомлення якоїсь прийнятної довжини*/
	if ((msg_received_length!=0)&&(msg_received_length<=RX_MAX_SLAVE_MSG_LENGTH)){
		modbus_slave_state = message_discovered;
		msg_received_length5=msg_received_length;
		/* припиняємо подальший прийом повідомлення 		*/
		if (HAL_UART_AbortReceive_IT(&SLAVE_HUART)!=HAL_OK){
			Error_Handler();
		}
	}
	while (modbus_slave_state != msg_recognized){
		 /* якщо повідомлення коротше ніж 4 байти, 			*/
		 /* відбраковка, надто коротке повідомлення			*/
		 if (msg_received_length<4) {
			 msg_status=wrong_message;
			 modbus_slave_state = msg_recognized;
			 break;
		 }
		 /* Якщо повідомлення не для нас і не циркулярне
		  *відбраковка	*/
		 if (!((msg_received[0]==SLAVE_BROADCAST_ADDRESS)||

					(msg_received[0]==SLAVE_NODE_ADDRESS))){
			 msg_status=wrong_message;
			 modbus_slave_state = msg_recognized;
			 break;
		 }
		 /*    перевіряємо CRC                           */
		 uint16_t crc_calc_rx = CRC_16x (msg_received, msg_received_length -2*(sizeof(uint8_t)));
		 crc_last_byte=crc_calc_rx / 0x100;
		 crc_pre_last_byte=crc_calc_rx % 0x100;
		 /* старший байт CRC - останній */
		 if (crc_calc_rx / 0x100 != msg_received[msg_received_length-1]){
				/* відбраковуємо повідомлення, якщо не співпадає старший байт */
			    msg_status = wrong_message;
				modbus_slave_state = msg_recognized;
				break;
		 }
		 /* 	молодший байт CRC - передостанній*/
		 if (crc_calc_rx % 0x100 != msg_received[msg_received_length-2]) {
				/*відбраковуємо повідомлення, якщо не співпадає молодший байт*/
			    msg_status = wrong_message;
				modbus_slave_state = msg_recognized;
				break;
		 }
		 if (msg_received[0]==SLAVE_BROADCAST_ADDRESS){
			 	/* виставляємо прапор на обробку циркулярного повідомлення*/
			 	msg_status = broadcast_message;
			 	/* тут ще, по ідеї, потрібно виставити ногу на передачу,
			 	 * заблокувавши прийом повідомлень до відправки відповіді */
		 }
		 else if (msg_received[0]==SLAVE_NODE_ADDRESS){
			/* виставляємо прапор на обробку вибіркового повідомлення */
			 	 msg_status= new_message_for_us;
		 }
		 else {
			 	 msg_status= wrong_message;
		 }
		 modbus_slave_state = msg_recognized;
	} /*(msg_received_length>=4)*/
/*********************************************************************************************/
	if (msg_status== wrong_message){
		/* обнуляємо хибне повідомлення у буфері	*/
		for (uint32_t i_del=0; i_del<msg_received_length; ++i_del){
				msg_received[i_del]=0x00;
		}
		/* запускаємо прийом наступного повідомлення */
		if(HAL_UART_Receive_IT (&SLAVE_HUART,
								&(msg_received),
								RX_MAX_SLAVE_MSG_LENGTH
								) != HAL_OK){																  /*перезапускаємо прийом наступного повідомленя */
			Error_Handler();
		}
		modbus_slave_state = no_message_for_us;
	}
	IdleHandlDuration = HAL_GetTick()-RxTxSlaveClick;
}//else if (msg_received_length!=0){


void MB_Slave_Transmit (void){

 /* The board sends the message and expects to receive it back */

 /*Configure GPIO pin Output Level to turn on RS485 TRANSMITT REGIME */
 HAL_GPIO_WritePin( SLAVE_RS485_SWITCH_PORT, SLAVE_RS485_SWITCH_PIN, GPIO_PIN_SET);

  /*##-2- Start the transmission process #####################################*/

  if(HAL_UART_Transmit_IT(&SLAVE_HUART, msg_transmitted, msg_transmitted_length)!= HAL_OK){
	    modbus_slave_state=answer_transmitting;
		Error_Handler();
  }

}

void Slave_TxCpltCallback(void) {

	/*Configure GPIO pin Output Level to turn on RS485 RECIEVE REGIME */
	 HAL_GPIO_WritePin(SLAVE_RS485_SWITCH_PORT, SLAVE_RS485_SWITCH_PIN, GPIO_PIN_RESET);
  /* Set transmission flag: transfer complete */

	for (uint8_t i_sent=0; i_sent<msg_transmitted_length;++i_sent){
		msg_transmitted [i_sent]=0x00;
	}
	if(HAL_UART_Receive_IT(&SLAVE_HUART, &(msg_received), RX_MAX_SLAVE_MSG_LENGTH) != HAL_OK) {
		/*перезапускаємо прийом наступного повідомленя */
		Error_Handler();
	}
	modbus_slave_state=no_message_for_us;
	SlaveDuration = HAL_GetTick()-RxTxSlaveClick;
	RxTxSlaveClick=0;
}

#endif //defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)



/**********************************************************************************************************************************************************
*  РЕАЛІЗАЦІЯ - варіант, якщо буде потрібно. Зараз не так.
*  Оскільки згідно з стандартом Modbus RTU slave не знає наперед кількість байт у вхідному повідомленні,
*  приймається безперервний потік байт.
*  Перший байт приймається після ініціалізації slave-контролера, в режимі переривання HAL_UART_Receive_IT
*  в разі успішного прийняття байта, по перериванню HAL_UART_RxCpltCallback, функція HAL_UART_Receive_IT запускається знову, і т.д. по колу.
*  в разі помилки прийняття байта по перериванню HAL_UART_ErrorCallback, функція HAL_UART_Receive_IT запускається знову.
*  Тобто постійне скарування входу  modbus_rtu RS-485->uart здійснюється поза основним циклом.
*  Безумовно, при цьому використовується ресурс процесора.
*
*  Кінець повідомлення, згідно з стандартом Modbus RTU, позначається паузою протягом часу, необхідного для передачі/прийому не менше 3.5 байта,
*  в разі виявлення паузи більше 1.5 байта, приймання повідомлення повинно припинятися.

*/
/* USER CODE END PFP */


/* НАЛАШТУВАННЯ ПЕРЕРИВАННЯ IDDLE:
 * див.https://community.st.com/thread/41068-cubemx-feature-request-add-usart-rx-idle-handling
 *
 * 1. файл usart.c, функція void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
 * додати після    HAL_NVIC_EnableIRQ(UART5_IRQn);
   	   	   	   	   USER CODE BEGIN UART5_MspInit 1
*******************************************************************************************
           __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
*******************************************************************************************
це те саме, що:

         	 huart5.Instance->CR1 |= USART_CR1_IDLEIE;
********************************************************************
 	 	 	 	 	 USER CODE END UART5_MspInit 1
цим ви включити перериванн IDDLE

 * 2. файл stm32f1xx_hal_uart.h додати
 * void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart)
 *
 * 3. файл stm32f1xx_hal_uart.c, функція   HAL_UART_IRQHandler(UART_HandleTypeDef *huart):
 * ***********************************************************************************
  if(((isrflags & USART_SR_IDLE) != RESET) && ((cr1its & USART_CR1_IDLEIE) != RESET))
  {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    HAL_UART_RxIdleCallback(huart);
    return;
  }
  ************************************************************************************
  *************************************************************************************/




