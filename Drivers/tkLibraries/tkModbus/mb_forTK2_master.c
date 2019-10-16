/*
 * tkModbusMaster1.c
 *
 *  Created on: 18 трав. 2018 р.
 *      Author: KozakVF
 */

#include <tkConfig.h>

#ifdef  MODBUS_MASTER_for_TK2
/*
 * tkModbusMaster.c
 *
 *  Created on: 14 трав. 2018 р.
 *      Author: KozakVF
 */
#include <mb_forTK2_master.h>
#include "mb_master.h"
#include "mb_master5.h"
#include "mb.h"



static modbus_status_t MBtk2_master_err = MODBUS_ERR_UNKNOWN;
static master_state_t MBtk2_master_state = MBtk2_Master_Idle;


//файл mb_master5.h
//typedef struct  {
//	uint32_t timeout;                         	/* час очікування клієнтом відповіді сервера													*/
//	uint8_t  waited_normal_response_length;		/* очікувана довжина в байтах нормальної віповіді сервера на запит клієнта						*/
//	uint8_t  waited_error_response_length;		/* очікувана довжина в байтах відповіді сервера про помилку при обробці повідомлення 			*/
//	uint8_t  waited_slave_response_address;		/* адреса сервера: 0x00 - циркулярний, для функцій запису даних в сервер, 0х00..0хFF-вибірковий */
//	uint8_t  waited_slave_response_function;	/* код функції 0x03,0x05,0x06,0x08															    */
//	uint32_t transmitting_began_at;				/* початок відліку часу виконання функції Modbus												*/
//	modbus_exception_t *ptr_exception;			/* показчик на адресу пам'яті для повернення коду помилки, виявленої сервером					*/
//	modbus_master_tx_msg_t *ptr_tx;				/* показчик на адресу пам'яті буфера повідомлення, що передається								*/
//}sprv_t;
static volatile sprv_t sprvFE = {0};
static volatile sprv_t sprv108 = {0};

static volatile sprv_t sprv106 = {500, 16, 0, 0, 106, 0, NULL, NULL};
static volatile sprv_t sprv110 = {500, 13, 0, 0, 110, 0, NULL, NULL};
static volatile sprv_t sprv112 = {500, 20, 0, 0, 112, 0, NULL, NULL};



tk2_session_status_t f_tk2_TASKS_110 (uint8_t tk2_ADDRESS, task110_t task110){
	tk2_session_status_t session_status110 = tk2_UnknownERR;
	volatile modbus_master_tx_msg_t w110 = {0};
	volatile sprv_t sprv110 = {0};
	uint16_t crc_calc_tx110 = 0;
	w110.length = 9;
	w110.msg[0] = tk2_ADDRESS;
	w110.msg[1] = 110;

	if      (task110==ManualSTOP)								{w110.msg[2] = 31; w110.msg[3] = 0xFF; w110.msg[4]=0;}
	else if (task110==ManualSTART)								{w110.msg[2] = 31; w110.msg[3] = 0xFF; w110.msg[4]=1;}
	else if (task110==AutomatSTOP)								{w110.msg[2] = 32; w110.msg[3] = 0;    w110.msg[4]=0xFF;}
	else if (task110==AutomatSTART)								{w110.msg[2] = 32; w110.msg[3] = 1;    w110.msg[4]=0xFF;}

	else if (task110==ProtectionCastOFF_AutomatSTART)			{w110.msg[2] = 32; w110.msg[3] = 1;    w110.msg[4]=0xFF; w110.msg[5]=1;}
	else if (task110==ProtectionCastOFF_ManualSTART)			{w110.msg[2] = 31; w110.msg[3] = 0xFF; w110.msg[4]=1;    w110.msg[5]=1;}
	else if (task110==Protection_LIMIT_CastOFF_AutomatSTART)	{w110.msg[2] = 32; w110.msg[3] = 1;    w110.msg[4]=0xFF; w110.msg[6]=1;}
	else if (task110==Protection_LIMIT_CastOFF_ManualSTART)		{w110.msg[2] = 31; w110.msg[3] = 0xFF; w110.msg[4]=1;    w110.msg[6]=1;}
	else {}


	crc_calc_tx110 = CRC_16x (&(w110.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
		        w110.length -2*(sizeof(uint8_t)));
	w110.msg[w110.length-1]	= crc_calc_tx110 / 0x100;			/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
	w110.msg[w110.length-2]	= crc_calc_tx110 % 0x100;			/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

																/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
	modbus_master_rx_msg_t w110responce = {0};
	w110responce.length=sprv110.waited_normal_response_length;

	while (session_status110 != tk2_OK) {
		if (tk2_Modbus_Session (&w110, &w110responce, sprv110.timeout) != HAL_OK){
			session_status110=tk2_Modbus_ERR;break;
		}
		uint16_t crc_calc_rx=0;
		crc_calc_rx = CRC_16x (&(w110responce.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
				w110responce.length -2*(sizeof(uint8_t)));

		if (
			 (w110responce.msg[w110responce.length-1] != crc_calc_rx / 0x100) ||/* якщо CRC у повідомленні правильний*/
			 (w110responce.msg[w110responce.length-2] != crc_calc_rx % 0x100)
		   ){
			session_status110=tk2_AnswerLOST;break;
		}
		if (w110responce.msg[0] != w110.msg[0]){
			session_status110=tk2_WrongDevice;break;
		}
		if (w110responce.msg[1] != w110.msg[1]){
			session_status110=tk2_WrongeFunction;break;
		}
		if (w110responce.msg[3] != w110.msg[2]){
			session_status110=tk2_WrongeRegime;break;
		}
		if (w110responce.msg[5] != w110.msg[3]){
			session_status110=tk2_WrongeTask;break;
		}
		if (w110responce.msg[3] != w110.msg[2]){
			session_status110=tk2_WrongeRegime;break;
		}
		session_status110=tk2_OK;
	}
	return session_status110;
}




modbus_status_t f_MB_PC_DiagnosticFE (uint8_t slave_address,uint8_t msg_counter_x,uint8_t msg_counter_y){
		volatile modbus_status_t status_dFE = MODBUS_ERR_UNKNOWN;;

				volatile modbus_master_tx_msg_t wFE = {0};
				volatile sprv_t sprvFE = {0};
				uint16_t crc_calc_txFE = 0;
				wFE.length = 6;
				wFE.msg[0] = slave_address;
				wFE.msg[1] = 0xFE;
				wFE.msg[2] = msg_counter_x;
				wFE.msg[3] = msg_counter_y;
				crc_calc_txFE = CRC_16x (&(wFE.msg), 			    					/* 2. Обчислюємо 16-бітний CRC   									*/
					        wFE.length -2*(sizeof(uint8_t)));
				wFE.msg[wFE.length-1]	= crc_calc_txFE / 0x100;						/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
				wFE.msg[wFE.length-2]	= crc_calc_txFE % 0x100;						/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

				sprvFE.timeout = 80; 													/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */

		while (status_dFE != MODBUS_SUCCESS) {

		/* ВІДПРАВКА ПОВІДОМЛЕННЯ */
			HAL_GPIO_WritePin(MASTER_HUART_RxTx_SWITCH, GPIO_PIN_SET);
			volatile HAL_StatusTypeDef Tx =
				HAL_UART_Transmit (&MASTER_HUART, &wFE.msg, wFE.length , sprvFE.timeout );

			if (Tx != HAL_OK) {
				if (Tx == HAL_TIMEOUT) {
					status_dFE = MODBUS_ERR_TIMEOUT;
				}
				else {
					status_dFE=MODBUS_ERR_TX;
				}
				HAL_GPIO_WritePin(MASTER_HUART_RxTx_SWITCH, GPIO_PIN_RESET);

				break;
			}
		/* Очікування відповіді, отримання повідомлення */
//			HAL_UART_Receive  (&MASTER_HUART, &wFE.msg, wFE.length , sprvFE.timeout );
//			 (&huart2, &echo, 1, HAL_MAX_DELAY);

		/* ОБРОБКА ПОВІДОМЛЕННЯ */

//			WO1602_ClearScreen ();
//			WO1602_WriteHighString ("ТОВ Техноконт");
//			WO1602_WriteLowString  ("ТК412н1");



			status_dFE = MODBUS_SUCCESS;
		}
		return status_dFE;
	}


modbus_status_t f_MBtk2_Diagnostic_108 (uint8_t slave_address,uint8_t msg_counter_x,uint8_t msg_counter_y){
		modbus_status_t status_d108 = MODBUS_ERR_UNKNOWN;;

			uint8_t msg_Rx[4] = {0};
				volatile modbus_master_tx_msg_t w108 = {0};
				volatile sprv_t sprv108 = {0};
				uint16_t crc_calc_tx108 = 0;
				w108.length = 6;
				w108.msg[0] = slave_address;
				w108.msg[1] = 108;
				w108.msg[2] = msg_counter_x;
				w108.msg[3] = msg_counter_y;
				crc_calc_tx108 = CRC_16x (&(w108.msg), 			    					/* 2. Обчислюємо 16-бітний CRC   									*/
					        w108.length -2*(sizeof(uint8_t)));
				w108.msg[w108.length-1]	= crc_calc_tx108 / 0x100;						/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
				w108.msg[w108.length-2]	= crc_calc_tx108 % 0x100;						/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

				sprv108.timeout = 80; 													/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
				sprv108.waited_normal_response_length=4;  								/* 6. Задаємо довжину очікуваної успішної відповіді */
				sprv108.waited_error_response_length=4;									/* 7. Задаємо довжину очікуваної відповіді про помилку, віявлену сервером */
				sprv108.waited_slave_response_address=slave_address;
				sprv108.waited_slave_response_function=108;
				/* Засікаємо момент відправки повідомлення   */
//	    	sprv108.ptr_exception=ptr_WSR_exception;								/* задаємо посилання для отримання коду помилки від від сервера */
				sprv108.ptr_tx = &(w108.msg);
		   //	MB_Master_Transmit ();	/* 10. Відправляємо повідомлення  */


		while (status_d108 != MODBUS_SUCCESS) {
			if (MBtk2_master_state != MBtk2_Master_Idle){
				status_d108=MODBUS_MASTER_BUSY; break;
			}
			sprv108.transmitting_began_at = HAL_GetTick();
			MBtk2_master_state=MBtk2_Master_Waiting_for_reply;

			HAL_StatusTypeDef Tx =
				HAL_UART_Transmit (&MASTER_HUART, &w108.msg, w108.length , 10 );

			if (Tx != HAL_OK) {
				status_d108=MODBUS_ERR_TX; break;
			}
			HAL_StatusTypeDef Rx =
					HAL_UART_Receive (&MASTER_HUART, msg_Rx, w108.length, sprv108.timeout);
			if (Rx != HAL_OK) {
				if (Rx == HAL_TIMEOUT) {
					status_d108 = MODBUS_ERR_TIMEOUT;
				}
				else {
					status_d108=MODBUS_ERR_RX;
				}
				MBtk2_master_state=MBtk2_Master_Idle;
				break;
			}
			MBtk2_master_state=MBtk2_Master_Processing_reply;
			if (msg_Rx[0] != w108.msg[0]){
				status_d108=MODBUS_ERR_SLAVE_ADDR; break;
			}
			uint16_t crc_calc_rx108 = 0;
					crc_calc_rx108 = CRC_16x (&msg_Rx), 			    					/* 2. Обчислюємо 16-бітний CRC   									*/
								        2*(sizeof(uint8_t));
		    if (((crc_calc_rx108 / 0x100) != msg_Rx[3]) ||
		    	 	((crc_calc_rx108 % 0x100) != msg_Rx[2])){
		    	status_d108=MODBUS_ERR_CRC; break;
		    }
		    if (msg_Rx[1] != 108){
		    	status_d108=MODBUS_ERR_FC;break;
		    }
		    status_d108=MODBUS_SUCCESS;
		    MBtk2_master_state = MBtk2_Master_Idle;
		}
		return status_d108;
	}

/* вивід на екран перших десяти байт повідомлення */
void WO1602_Write_HEX (modbus_master_tx_msg_t ww){
	uint8_t s_high [17]={0};
	uint8_t s_low  [17]={0};
	uint8_t s_length = 0;
	uint8_t older    = 0;
	uint8_t younger  = 0;
	uint8_t ii=0;
	uint8_t iii=0;
	if (s_length>10){
		s_length=10;
	}
	else {
		s_length=ww.length;
	}

	if (s_length<=5){
		for (ii=0;ii<s_length;ii++){
			older   = ww.msg[ii]/0x100;
			younger = ww.msg[ii]%0x100;

			s_high[ii*3]=chHEX (older);
			s_high[ii*3+1]=chHEX (younger);
			s_high[ii*3+2]='_';

		}
	}
	else {
		for (ii=0;ii<5;ii++){
			older   = ww.msg[ii]/0x100;
			younger = ww.msg[ii]%0x100;

			s_high[ii*3]=chHEX (older);
			s_high[ii*3+1]=chHEX (younger);
			s_high[ii*3+2]='_';

		}
		for (iii=5;iii<s_length;iii++){
			older   = ww.msg[iii]/0x100;
			younger = ww.msg[iii]%0x100;

			s_low[(iii-5)*3]=chHEX (older);
			s_high[(iii-5)*3+1]=chHEX (younger);
			s_high[(iii-5)*3+2]='_';
		}
	}

	WO1602_ClearScreen ();
	WO1602_WriteHighString (s_high);
	WO1602_WriteLowString (s_low);
}

#endif  MODBUS_MASTER_for_TK2
