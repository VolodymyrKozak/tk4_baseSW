/*
 * tk_oven_smi2.c
 *
 *  Created on: 23 трав. 2018 р.
 *      Author: KozakVF
 */
#include <tkConfig.h>

#include "tk_oven_smi2.h"
#include <stdio.h>
#include "mb.h"

#ifdef   MODBUS_MASTER_for_OVEN_INDICATOR

typedef struct {
	uint8_t  symbol[8];
}oven_smi2_string_t;

/*********************************************************************************************************************
 *  ОГОЛОШЕННЯ СТАТИЧНИХ ФУНКЦІЙ
 *********************************************************************************************************************/

static oven_session_status_t f_ovenSMI2_FloatToString (float value,
													   oven_smi2_string_t *ps
													   );



//typedef struct {
//	float value[31];
//}vector_of_float_t;

/*********************************************************************************************************************
 *  Ф У Н К Ц І Ї
 *********************************************************************************************************************/
oven_session_status_t f_ovenSMI2_Init (UART_HandleTypeDef *phuart,
									   uint8_t ovenSMI_ADDRESS){


	f_ovenSMI2_TASKS_06(phuart, ovenSMI_ADDRESS, 8, 48); 				/* задержка ответа - 48 мс		*/
	HAL_Delay (20);
	f_ovenSMI2_TASKS_06(phuart, ovenSMI_ADDRESS, 17, 3); 				/* тип переменной - "строка" 	*/
	HAL_Delay (20);
	f_ovenSMI2_TASKS_06(phuart, ovenSMI_ADDRESS, 0x0023, 0x0000); 		/* выключить мигание			*/
	HAL_Delay (20);
}
/**********************************************************************************************************************
 * ШИРОКОВЕЩАТЕЛЬНАЯ ЗАПИСЬ ОПЕРАТИВНЫХ ПАРАМЕТРОВ
 * 1. Модбас-адреса нескольких (до 31, по моему, индикаторов СМИ2 должны быть подряд,
 * например 0х20, 0х21, ... , 0х51. Указывается лишь адрес первого регистра и количчество регистров.
 * 2. Массив плавающих чисел должен быть объявлен и определен.
 *
 **********************************************************************************************************************/
oven_session_status_t f_ovenSMI2_DisplayBroadCast (	UART_HandleTypeDef *phuart,
													uint8_t fist_ovenSMI_ADDRESS,
													uint8_t number_of_values,
													vector_of_float_t *pv){
	oven_session_status_t oven_dysplay_status = oven_UnknownERR;
	if (number_of_values>31){
		oven_dysplay_status = oven_WrongeTask; return oven_dysplay_status;
	}
	if (pv==NULL){
		oven_dysplay_status = oven_NULL_POINTER; return oven_dysplay_status;
	}
	volatile modbus_master_tx_msg_t w_fl = {0};

		w_fl.msg [0]= 0x00; /* modbuss broudcactaddress */
		w_fl.msg [1]= 0x10; /* function code*/
		uint16_t ADAD_ADDR = 0x03e8 + fist_ovenSMI_ADDRESS;

		w_fl.msg [2]= ADAD_ADDR/0x100; /* register address to begin writing, fist byte of uint16_t */
		w_fl.msg [3]= ADAD_ADDR%0x100; /* register address to begin writing, last byte of uint16_t*/
		uint16_t number_of_16bit_words = number_of_values*4;
		w_fl.msg [4]= number_of_16bit_words/0x100; /* number of 16bit words, fist byte of uint16_t */
		w_fl.msg [5]= number_of_16bit_words%0x100; /* number of 16bit words, last byte of uint16_t */
		w_fl.msg [6]= number_of_values*8; /* number of bytes*/
		w_fl.length=7+w_fl.msg [6]+2;

		for (uint8_t i=0;i<number_of_values;i++){
			oven_smi2_string_t tmp_str={0};
			float f_tmp=pv->value[i];
			f_ovenSMI2_FloatToString (f_tmp, &tmp_str);

			w_fl.msg[7+i*8+0]=tmp_str.symbol[0];
			w_fl.msg[7+i*8+1]=tmp_str.symbol[1];
			w_fl.msg[7+i*8+2]=tmp_str.symbol[2];
			w_fl.msg[7+i*8+3]=tmp_str.symbol[3];
			w_fl.msg[7+i*8+4]=tmp_str.symbol[4];
			w_fl.msg[7+i*8+5]=tmp_str.symbol[5];
			w_fl.msg[7+i*8+6]=tmp_str.symbol[6];
			w_fl.msg[7+i*8+7]=tmp_str.symbol[7];
		}

	uint16_t crc_calc_tx16 = 0;
	crc_calc_tx16 = CRC_16x (&(w_fl.msg), 			    			/* 2. Обчислюємо 16-бітний CRC   									*/
			        w_fl.length -2*(sizeof(uint8_t)));
		w_fl.msg[w_fl.length-1]	= crc_calc_tx16 / 0x100;			/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
		w_fl.msg[w_fl.length-2]	= crc_calc_tx16 % 0x100;			/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

																	/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
		modbus_master_rx_msg_t w16responce = {0};
		w16responce.length=8;

		while (oven_dysplay_status != oven_OK) {
			if (tk4_Modbus_Session (phuart, &w_fl, &w16responce, 400) != HAL_OK){
				oven_dysplay_status=oven_Modbus_ERR;break;
			}
			uint16_t crc_calc_rx=0;
			crc_calc_rx = CRC_16x (&(w16responce.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
					w16responce.length -2*(sizeof(uint8_t)));

			if (
				 (w16responce.msg[w16responce.length-1] != crc_calc_rx / 0x100) ||/* якщо CRC у повідомленні правильний*/
				 (w16responce.msg[w16responce.length-2] != crc_calc_rx % 0x100)
			   ){
				oven_dysplay_status=oven_AnswerLOST;break;
			}
			if (w16responce.msg[0] != w_fl.msg[0]){
				oven_dysplay_status=oven_WrongDevice;break; 		/* address ok*/
			}
			if (w16responce.msg[1] != w_fl.msg[1]){
				oven_dysplay_status=oven_WrongeFunction;break;		/* function ok*/
			}

			oven_dysplay_status=oven_OK;
		}
		return oven_dysplay_status;

}

oven_session_status_t f_ovenSMI2_DisplayFloat (	UART_HandleTypeDef *phuart,
												uint8_t ovenSMI_ADDRESS,
												float value
												){
	oven_session_status_t oven_dysplay_status = oven_UnknownERR;
	volatile modbus_master_tx_msg_t w_fl = {0};

		w_fl.msg [0]= ovenSMI_ADDRESS;
		w_fl.msg [1]= 0x10; /* function */

		w_fl.msg [2]= 0x00; /* register address to begin writing, fist byte of uint16_t */
		w_fl.msg [3]= 0x1D; /* register address to begin writing, last byte of uint16_t*/

		w_fl.msg [4]= 0x00; /* number of 16bit words, fist byte of uint16_t */
		w_fl.msg [5]= 0x04; /* number of 16bit words, last byte of uint16_t */


		w_fl.msg [6]= 0x08; /* number of bytes*/

		oven_smi2_string_t st = {0};
		f_ovenSMI2_FloatToString (value, &st);

		w_fl.length=16;
		/* байти 7..14 - рядок на дисплей */
		w_fl.msg [7]  = st.symbol[0];
		w_fl.msg [8]  = st.symbol[1];
		w_fl.msg [9]  = st.symbol[2];
		w_fl.msg [10] = st.symbol[3];
		w_fl.msg [11] = st.symbol[4];
		w_fl.msg [12] = st.symbol[5];
		w_fl.msg [13] = st.symbol[6];
		w_fl.msg [14]=  st.symbol[7];

	uint16_t crc_calc_tx16 = 0;
	crc_calc_tx16 = CRC_16x (&(w_fl.msg), 			    			/* 2. Обчислюємо 16-бітний CRC   									*/
			        w_fl.length -2*(sizeof(uint8_t)));
		w_fl.msg[w_fl.length-1]	= crc_calc_tx16 / 0x100;			/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
		w_fl.msg[w_fl.length-2]	= crc_calc_tx16 % 0x100;			/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

																	/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
		modbus_master_rx_msg_t w16responce = {0};
		w16responce.length=8;

		while (oven_dysplay_status != oven_OK) {
			if (tk4_Modbus_Session (phuart, &w_fl, &w16responce, 400) != HAL_OK){
				oven_dysplay_status=oven_Modbus_ERR;break;
			}
			uint16_t crc_calc_rx=0;
			crc_calc_rx = CRC_16x (&(w16responce.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
					w16responce.length -2*(sizeof(uint8_t)));

			if (
				 (w16responce.msg[w16responce.length-1] != crc_calc_rx / 0x100) ||/* якщо CRC у повідомленні правильний*/
				 (w16responce.msg[w16responce.length-2] != crc_calc_rx % 0x100)
			   ){
				oven_dysplay_status=oven_AnswerLOST;break;
			}
			if (w16responce.msg[0] != w_fl.msg[0]){
				oven_dysplay_status=oven_WrongDevice;break; 		/* address ok*/
			}
			if (w16responce.msg[1] != w_fl.msg[1]){
				oven_dysplay_status=oven_WrongeFunction;break;		/* function ok*/
			}

			oven_dysplay_status=oven_OK;
		}
		return oven_dysplay_status;



}
/* ВКЛЮЧИТЬ МИГАНИЕ */
oven_session_status_t f_ovenSMI2_DisplayBlinkingON (UART_HandleTypeDef *phuart,
													uint8_t ovenSMI_ADDRESS){
	volatile uint16_t inDm=0xFF;
	f_ovenSMI2_TASKS_06(phuart, ovenSMI_ADDRESS, 0x23, 0xBB); 		/* выключить мигание			*/
	HAL_Delay (20);
}
/* ВЫКЛЮЧИТЬ МИГАНИЕ */
oven_session_status_t f_ovenSMI2_DisplayBlinkingOFF (UART_HandleTypeDef *phuart,
													 uint8_t ovenSMI_ADDRESS){

	uint16_t inDm=0xFE;
	f_ovenSMI2_TASKS_03 ( phuart,ovenSMI_ADDRESS,0x23,&inDm);
		HAL_Delay (20);

	f_ovenSMI2_TASKS_06(phuart, ovenSMI_ADDRESS, 0x23, 0x00); 		/* выключить мигание			*/
	HAL_Delay (20);

	f_ovenSMI2_TASKS_03 ( phuart,ovenSMI_ADDRESS,0x23,&inDm);
		HAL_Delay (20);
}



static oven_session_status_t f_ovenSMI2_FloatToString (float value,
													   oven_smi2_string_t *ps
													   ){
	oven_session_status_t oven_dysplay_status = oven_UnknownERR;
	uint32_t d_int = 0;
	uint8_t  dig   =0;
	if (value >9999.0) {

				ps->symbol [0]   = 0x45; /*    E            */
				ps->symbol [1]   = 0x72; /*    r            */
				ps->symbol [2]   = 0x72; /*    r            */
				ps->symbol [3]   = 0x20; /*                 */

				ps->symbol [4]   = 0x20;  /*                 */
				ps->symbol [5]   = 0x20;  /*                 */
				ps->symbol [6]   = 0x20;  /*                 */
				ps->symbol [7]   = 0x20;  /*                 */
	}
	else if ((value >=1000.0)&&(value<10000.0)) {
		d_int=(uint32_t)value;
		dig=d_int/1000;
					ps->symbol [0]   = 0x30+dig;
		d_int=d_int-dig*1000;
		dig=d_int/100;
					ps->symbol [1]   = 0x30+dig;
		d_int=d_int-dig*100;
		dig=d_int/10;
					ps->symbol [2]   = 0x30+dig;
		d_int=d_int-dig*10;
		dig=d_int;
					ps->symbol [3]   = 0x30+dig;

					ps->symbol [4]   = 0x20;
					ps->symbol [5]   = 0x20;
					ps->symbol [6]   = 0x20;
					ps->symbol [7]   = 0x20;
		}
	else if ((value >=100.0)&&(value<1000.0)) {
		d_int=(uint32_t)(value*10.0);
		dig=d_int/1000;
					ps->symbol [0]   = 0x30+dig;
		d_int=d_int-dig*1000;
		dig=d_int/100;
					ps->symbol [1]   = 0x30+dig;
		d_int=d_int-dig*100;
		dig=d_int/10;
					ps->symbol [2]   = 0x30+dig;

					ps->symbol [3]   = 0x2E;
		d_int=d_int-dig*10;
		dig=d_int;
					ps->symbol [4]   = 0x30+dig;


					ps->symbol [5]   = 0x20;
					ps->symbol [6]   = 0x20;
					ps->symbol [7]   = 0x20;
	}
	else if ((value >=10.0)&&(value<100.0)) {
		d_int=(uint32_t)(value*100.0);
		dig=d_int/1000;
					ps->symbol [0]   = 0x30+dig;
		d_int=d_int-dig*1000;
		dig=d_int/100;
					ps->symbol [1]   = 0x30+dig;
					ps->symbol [2]   = 0x2E;

		d_int=d_int-dig*100;
		dig=d_int/10;
					ps->symbol [3]   = 0x30+dig;
		d_int=d_int-dig*10;
		dig=d_int;
					ps->symbol [4]   = 0x30+dig;
					ps->symbol [5]   = 0x20;
					ps->symbol [6]   = 0x20;
					ps->symbol [7]   = 0x20;
	}
	else if ((value >=0.001)&&(value<10.0)) {
		d_int=(uint32_t)(value*1000.0);
		dig=d_int/1000;
					ps->symbol [0]   = 0x30+dig;
					ps->symbol [1]   = 0x2E;
		d_int=d_int-dig*1000;
		dig=d_int/100;
					ps->symbol [2]   = 0x30+dig;


		d_int=d_int-dig*100;
		dig=d_int/10;
					ps->symbol [3]   = 0x30+dig;
		d_int=d_int-dig*10;
		dig=d_int;
					ps->symbol [4]   = 0x30+dig;
					ps->symbol [5]   = 0x20;
					ps->symbol [6]   = 0x20;
					ps->symbol [7]   = 0x20;
	}
	else if ((value >-0.01)&&(value<0.001)) {

					ps->symbol [0]   = 0x30;
					ps->symbol [1]   = 0x30;
					ps->symbol [2]   = 0x30;
					ps->symbol [3]   = 0x30;
					ps->symbol [4]   = 0x20;
					ps->symbol [5]   = 0x20;
					ps->symbol [6]   = 0x20;
					ps->symbol [7]   = 0x20;
	}
	else if ((value >-10.0)&&(value<=-0.01)) {

		    ps->symbol [0]   = 0x2D;

			d_int=(uint32_t)(value*100.0*(-1.0));
			dig=d_int/100;
					ps->symbol [1]   = 0x30+dig;
					ps->symbol [2]   = 0x2E;
			d_int=d_int-dig*100;
			dig=d_int/10;

					ps->symbol [3]   = 0x30+dig;

			d_int=d_int-dig*10;
			dig=d_int;
						ps->symbol [4]   = 0x30+dig;

						ps->symbol [5]   = 0x20;
						ps->symbol [6]   = 0x20;
						ps->symbol [7]   = 0x20;

		}

	else if ((value >-100.0)&&(value<=-10.0)) {

		    ps->symbol [0]   = 0x2D;

			d_int=(uint32_t)(value*10.0*(-1.0));
			dig=d_int/100;
					ps->symbol [1]   = 0x30+dig;

			d_int=d_int-dig*100;
			dig=d_int/10;

					ps->symbol [2]   = 0x30+dig;
					ps->symbol [3]   = 0x2E;
			d_int=d_int-dig*10;
			dig=d_int;
						ps->symbol [4]   = 0x30+dig;

						ps->symbol [5]   = 0x20;
						ps->symbol [6]   = 0x20;
						ps->symbol [7]   = 0x20;
		}
	else if ((value >-1000.0)&&(value<=-100.0)) {

			    ps->symbol [0]   = 0x2D;

				d_int=(uint32_t)(value*(-1.0));
				dig=d_int/100;
						ps->symbol [1]   = 0x30+dig;

				d_int=d_int-dig*100;
				dig=d_int/10;

						ps->symbol [2]   = 0x30+dig;

				d_int=d_int-dig*10;
				dig=d_int;
				ps->symbol [3]   = 0x30+dig;

				ps->symbol [4]   = 0x20;
				ps->symbol [5]   = 0x20;
				ps->symbol [6]   = 0x20;
				ps->symbol [7]   = 0x20;
			}
	if (value <-999.0) {

					ps->symbol [0]   = 0x2D; /*    -             */
					ps->symbol [1]   = 0x45; /*    E            */
					ps->symbol [2]   = 0x72; /*    r            */
					ps->symbol [3]   = 0x72; /*    r            */


					ps->symbol [4]   = 0x20;  /*                 */
					ps->symbol [5]   = 0x20;  /*                 */
					ps->symbol [6]   = 0x20;  /*                 */
					ps->symbol [7]   = 0x20;  /*                 */
		}
  else {}
	oven_dysplay_status=oven_OK;
	return oven_dysplay_status;
}
oven_session_status_t f_ovenSMI2_Init_String (	UART_HandleTypeDef *phuart,
												uint8_t ovenSMI_ADDRESS){
	oven_session_status_t oven_init_status = oven_UnknownERR;

	f_ovenSMI2_TASKS_06(phuart, ovenSMI_ADDRESS, 8, 48);
	f_ovenSMI2_TASKS_06(phuart, ovenSMI_ADDRESS, 17, 3);

	uint8_t test_str[4]={'0','2','3','4'};
	f_ovenSMI2_TASKS_16 (phuart,
						 0x20, /* Адресс прибора 		*/
						 0x1D, /* Номер первого регистра*/
						 0x02, /* Количество регистров	*/
						 0x04, /* Количество байт		*/
						 &test_str
						);

}
/****************************************************************************************************************************
* PRESET_MULTIPLE_REGISTERS *
* ***************************************************************************************************************************
*/
oven_session_status_t f_ovenSMI2_TASKS_16 (UART_HandleTypeDef *phuart,
										   uint8_t ovenSMI_ADDRESS,
									       uint8_t register_to_BEGIN,
										   uint8_t N_of_registers,
										   uint8_t N2_of_bytes,
										   uint8_t *str_to_oven){
	volatile oven_session_status_t session_status16 = oven_UnknownERR;
	volatile modbus_master_tx_msg_t w16 = {0};
	volatile sprv_t sprv16 = {0};
	uint16_t crc_calc_tx16 = 0;
	w16.length = 7+N2_of_bytes;
	w16.msg[0] = ovenSMI_ADDRESS;
	w16.msg[1] = 0x10;  /* function */
	w16.msg[2] = register_to_BEGIN;
	w16.msg[3] = N_of_registers;
	w16.msg[4] = N2_of_bytes;

	w16.msg[5] = str_to_oven[0];
	w16.msg[6] = str_to_oven[1];
	w16.msg[7] = str_to_oven[2];
	w16.msg[8] = str_to_oven[3];

	crc_calc_tx16 = CRC_16x (&(w16.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
		        w16.length -2*(sizeof(uint8_t)));
	w16.msg[w16.length-1]	= crc_calc_tx16 / 0x100;			/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
	w16.msg[w16.length-2]	= crc_calc_tx16 % 0x100;			/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

																/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
	modbus_master_rx_msg_t w16responce = {0};
	w16responce.length=8;

	while (session_status16 != oven_OK) {
		if (tk4_Modbuss_Session (&w16, &w16responce, 400) != HAL_OK){
			session_status16=oven_Modbus_ERR;break;
		}
		uint16_t crc_calc_rx=0;
		crc_calc_rx = CRC_16x (&(w16responce.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
				w16responce.length -2*(sizeof(uint8_t)));

		if (
			 (w16responce.msg[w16responce.length-1] != crc_calc_rx / 0x100) ||/* якщо CRC у повідомленні правильний*/
			 (w16responce.msg[w16responce.length-2] != crc_calc_rx % 0x100)
		   ){
			session_status16=oven_AnswerLOST;break;
		}
		if (w16responce.msg[0] != w16.msg[0]){
			session_status16=oven_WrongDevice;break; 		/* address ok*/
		}
		if (w16responce.msg[1] != w16.msg[1]){
			session_status16=oven_WrongeFunction;break;		/* function ok*/
		}

		session_status16=oven_OK;
	}
	return session_status16;
}





/****************************************************************************************************************************
* PRESET_SINGLE_REGISTER *
* ***************************************************************************************************************************
*/
oven_session_status_t f_ovenSMI2_TASKS_06 (UART_HandleTypeDef *phuart,
										   uint8_t ovenSMI_ADDRESS,
										   uint16_t register_to_PRESET,
										   uint16_t register_value){
	volatile oven_session_status_t session_status06 = oven_UnknownERR;
	volatile modbus_master_tx_msg_t w06 = {0};
	volatile sprv_t sprv06 = {0};
	uint16_t crc_calc_tx06 = 0;
	w06.length = 8;
	w06.msg[0] = ovenSMI_ADDRESS;
	w06.msg[1] = 0x06;  /* function */
	w06.msg[2] = register_to_PRESET / 0x100;
	w06.msg[3] = register_to_PRESET % 0x100;

	w06.msg[4] = register_value / 0x100; /* quantity of registers */
	w06.msg[5] = register_value % 0x100; /* quantity of registers */

	crc_calc_tx06 = CRC_16x (&(w06.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
		        w06.length -2*(sizeof(uint8_t)));
	w06.msg[w06.length-1]	= crc_calc_tx06 / 0x100;			/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
	w06.msg[w06.length-2]	= crc_calc_tx06 % 0x100;			/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

																/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
	modbus_master_rx_msg_t w06responce = {0};
	w06responce.length=8;

	while (session_status06 != oven_OK) {
		if (tk4_Modbus_Session (phuart,&w06, &w06responce, 400) != HAL_OK){
			session_status06=oven_Modbus_ERR;break;
		}
		uint16_t crc_calc_rx=0;
		crc_calc_rx = CRC_16x (&(w06responce.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
				w06responce.length -2*(sizeof(uint8_t)));
		if (
			 (w06responce.msg[w06responce.length-1] != crc_calc_rx / 0x100) ||/* якщо CRC у повідомленні правильний*/
			 (w06responce.msg[w06responce.length-2] != crc_calc_rx % 0x100)
		   ){
			session_status06=oven_AnswerLOST;break;
		}
		if (w06responce.msg[0] != w06.msg[0]){
			session_status06=oven_WrongDevice;break; 		/* address ok*/
		}
		if (w06responce.msg[1] != w06.msg[1]){
			session_status06=oven_WrongeFunction;break;		/* function ok*/
		}

		session_status06=oven_OK;
	}
	return session_status06;
}
/****************************************************************************************************************************
* read_SINGLE_REGISTER * Не працює. ОВЕН у відповіді перший байт забиває випадковим числом..
	* Щоб правильно прочитати - треба ігнорувати перший отриманий байт і CRC і читати решту.
	* Треба буде - зробимо, поки що - не треба..
* ***************************************************************************************************************************
*/
#ifdef MODBUS_MASTER_for_OVEN_INDICATOR_TESTING
oven_session_status_t f_ovenSMI2_TASKS_03 ( UART_HandleTypeDef *phuart,
											uint8_t ovenSMI_ADDRESS,
											uint16_t register_to_READ,
											uint16_t *ptr_received_values){
	volatile oven_session_status_t session_status03 = oven_UnknownERR;
	volatile modbus_master_tx_msg_t w03 = {0};
	volatile sprv_t sprv03 = {0};
	uint16_t crc_calc_tx03 = 0;
	w03.length = 8;
	w03.msg[0] = ovenSMI_ADDRESS;
	w03.msg[1] = 0x03;  /* function */
	w03.msg[2] = register_to_READ / 0x100;
	w03.msg[3] = register_to_READ % 0x100;

	w03.msg[4] = 0; /* quantity of registers */
	w03.msg[5] = 1; /* quantity of registers */

	crc_calc_tx03 = CRC_16x (&(w03.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
		        w03.length -2*(sizeof(uint8_t)));
	w03.msg[w03.length-1]	= crc_calc_tx03 / 0x100;			/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
	w03.msg[w03.length-2]	= crc_calc_tx03 % 0x100;			/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

																/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
	modbus_master_rx_msg_t w03responce = {0};
	w03responce.length=7;

	while (session_status03 != oven_OK) {

		if (tk4_Modbus_Session(phuart, &w03, &w03responce, 400) != HAL_OK){
			session_status03=oven_Modbus_ERR;break;
		}
		uint16_t crc_calc_rx=0;
		crc_calc_rx = CRC_16x (&(w03responce.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
				w03responce.length -2*(sizeof(uint8_t)));

		if (
			 (w03responce.msg[w03responce.length-1] != crc_calc_rx / 0x100) ||/* якщо CRC у повідомленні правильний*/
			 (w03responce.msg[w03responce.length-2] != crc_calc_rx % 0x100)
		   ){
			session_status03=oven_AnswerLOST;break;
		}
		if (w03responce.msg[0] != w03.msg[0]){
			session_status03=oven_WrongDevice;break; 		/* address ok*/
		}
		if (w03responce.msg[1] != w03.msg[1]){
			session_status03=oven_WrongeFunction;break;		/* function ok*/
		}
		(*ptr_received_values) = w03.msg[3]*0x100 + w03.msg[4];
		session_status03=oven_OK;
	}
	return session_status03;
}
#endif // MODBUS_MASTER_for_OVEN_INDICATOR_TESTING





/*report slave ID*/
oven_session_status_t f_ovenSMI2_TASKS_17 (	UART_HandleTypeDef *phuart,
											uint8_t ovenSMI_ADDRESS){
	volatile oven_session_status_t session_status17 = oven_UnknownERR;
	volatile modbus_master_tx_msg_t w17 = {0};
	volatile sprv_t sprv17 = {0};
	uint16_t crc_calc_tx17 = 0;
	w17.length = 4;
	w17.msg[0] = ovenSMI_ADDRESS;
	w17.msg[1] = 17;

	crc_calc_tx17 = CRC_16x (&(w17.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
		        w17.length -2*(sizeof(uint8_t)));
	w17.msg[w17.length-1]	= crc_calc_tx17 / 0x100;			/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
	w17.msg[w17.length-2]	= crc_calc_tx17 % 0x100;			/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

																/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
	modbus_master_rx_msg_t w17responce = {0};
	w17responce.length=19;

	while (session_status17 != oven_OK) {
		if (tk4_Modbus_Session (phuart, &w17, &w17responce, 400) != HAL_OK){
			session_status17=oven_Modbus_ERR;break;
		}
		uint16_t crc_calc_rx=0;
		crc_calc_rx = CRC_16x (&(w17responce.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
				w17responce.length -2*(sizeof(uint8_t)));

		if (
			 (w17responce.msg[w17responce.length-1] != crc_calc_rx / 0x100) ||/* якщо CRC у повідомленні правильний*/
			 (w17responce.msg[w17responce.length-2] != crc_calc_rx % 0x100)
		   ){
			session_status17=oven_AnswerLOST;break;
		}
		if (w17responce.msg[0] != w17.msg[0]){
			session_status17=oven_WrongDevice;break; 		/* address ok*/
		}
		if (w17responce.msg[1] != w17.msg[1]){
			session_status17=oven_WrongeFunction;break;		/* function ok*/
		}
		session_status17=oven_OK;
	}
	return session_status17;
}
#endif   /*MODBUS_MASTER_for_OVEN_INDICATOR*/
