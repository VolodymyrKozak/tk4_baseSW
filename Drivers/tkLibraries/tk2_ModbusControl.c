/*
 * tk2_ModbusControl.c
 *
 *  Created on: 25 черв. 2018 р.
 *      Author: KozakVF
 */
#include <tkConfig.h>
#ifdef DRIVES_REMOTE_CONTROL
#include "../tkLibraries/tk2_ModbusControl.h"
#include "../tkLibraries/tkModbus/mb.h"
//typedef struct {
//	drive_regime_t drive_regime;
//	drive_op_t task_operational;
//	drive_op_t drive_current_state;
//	uint8_t motor_protected_code;
//	sensor_has_just_worked_t high_level;
//	sensor_has_just_worked_t low_level;
//	sensor_has_just_worked_t middle_level;
//	uint16_t amperage_1;
//	uint16_t amperage_2;
//	uint16_t amperage_3;
//	uint16_t TimeCode;
//} drive_op_state_t;
//
//typedef struct {
//	uint8_t type_size;
//	uint8_t motor_start_delay;
//	uint8_t motor_stop_delay;
//	uint8_t protection_reset_delay;
//	uint8_t reserve6;
//	setpoint_t self_start;
//	uint8_t numbers_of_starts_per_4mins;
//	uint16_t izolation;
//	uint16_t amperage_high_limit;
//	uint16_t amperage_low_limit;
//	uint16_t sensor_level_threshold;
//	uint16_t TimeCode;
//} drive_set_state_t;



uint8_t remove_drive_number=0;
static uint8_t request_timeout = 3000;
//remote_drives_t remote_drive[3]={0};
//
//tk2_session_status_t f_drive_op_state_Init (remote_drives_t *p_remote_drive){
//
//	remote_drive[0].drive_modbus_address=0x01;
//	remote_drive[1].drive_modbus_address=0x01;
//	remote_drive[2].drive_modbus_address=0x01;
//
//	return tk2_OK;
//}

tk2_session_status_t f_drive_op_state_request105 (uint8_t in_modbus_adress, drive_op_state_t *p_drive_op_state){
	tk2_session_status_t tk2_105_status = tk2_UnknownERR;

		volatile modbus_master_tx_msg_t w105 = {0};
		    w105.length=4;

			w105.msg [0]= in_modbus_adress; /* modbuss address */
			w105.msg [1]= 0x69; /* function code*/

		/* ПІДГОТОВКА ЗАПИТУ */
		uint16_t crc_calc_tx16 = 0;
		crc_calc_tx16 = CRC_16x (&(w105.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
				        w105.length -2*(sizeof(uint8_t)));
			w105.msg[w105.length-1]	= crc_calc_tx16 / 0x100;			/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
			w105.msg[w105.length-2]	= crc_calc_tx16 % 0x100;			/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

																		/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
			modbus_master_rx_msg_t w105responce = {0};
			w105responce.length=19;

			while (tk2_105_status != tk2_OK) {
		/* НАДСИЛАННЯ ЗАПИТУ і ОТРИМАННЯ ПОВІДМЛЕННЯ */
				if (tk4_Modbus_Session (&w105, &w105responce, request_timeout) != HAL_OK){
					tk2_105_status=tk2_Modbus_ERR; break;
				}

				uint16_t crc_calc_rx=0;
				crc_calc_rx = CRC_16x (&(w105responce.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
						w105responce.length -2*(sizeof(uint8_t)));

				if (
					 (w105responce.msg[w105responce.length-1] != crc_calc_rx / 0x100) ||/* якщо CRC у повідомленні правильний*/
					 (w105responce.msg[w105responce.length-2] != crc_calc_rx % 0x100)
				   ){
					tk2_105_status=tk2_AnswerLOST;break;
				}
				if (w105responce.msg[0] != w105.msg[0]){
					tk2_105_status=tk2_WrongDevice;break; 		/* address ok*/
				}
				if (w105responce.msg[1] != w105.msg[1]){
					tk2_105_status=tk2_WrongeFunction;break;		/* function ok*/
				}
			/* ОБРОБКА ОТРИМАНОГО ПОВІДОМЛЕННЯ */
				p_drive_op_state->drive_regime			=(drive_regime_t)			(w105responce.msg[2]);
				p_drive_op_state->task_operational		=(drive_task_t)				(w105responce.msg[3]);
				p_drive_op_state->drive_current_state	=(drive_op_t)				(w105responce.msg[4]);
				p_drive_op_state->motor_protected_code	=(motor_protect_t)			(w105responce.msg[5]);
				p_drive_op_state->high_level			=(sensor_has_just_worked_t)	(w105responce.msg[6]);
				p_drive_op_state->low_level				=(sensor_has_just_worked_t)	(w105responce.msg[7]);
				p_drive_op_state->middle_level			=(sensor_has_just_worked_t)	(w105responce.msg[8]);
				p_drive_op_state->amperage_1			=w105responce.msg[9]*0x100+w105responce.msg[10];
				p_drive_op_state->amperage_2			=w105responce.msg[11]*0x100+w105responce.msg[12];
				p_drive_op_state->amperage_3			=w105responce.msg[13]*0x100+w105responce.msg[14];
				p_drive_op_state->TimeCode				=w105responce.msg[15]*0x100+w105responce.msg[16];

				tk2_105_status=tk2_OK;
			}
			return tk2_105_status;
}

tk2_session_status_t f_drive_op_state_request106 (uint8_t in_modbus_adress, drive_set_state_t *p_drive_set_state){
	tk2_session_status_t tk2_106_status = tk2_UnknownERR;

		volatile modbus_master_tx_msg_t w106 = {0};
		    w106.length=4;

			w106.msg [0]= in_modbus_adress; /* modbuss address */
			w106.msg [1]= 0x6A; /* function code*/


		uint16_t crc_calc_tx16 = 0;
		crc_calc_tx16 = CRC_16x (&(w106.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
				        w106.length -2*(sizeof(uint8_t)));
			w106.msg[w106.length-1]	= crc_calc_tx16 / 0x100;			/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
			w106.msg[w106.length-2]	= crc_calc_tx16 % 0x100;			/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */

																		/* 5. Задаємо час очікування відповіді в мілісекундах згідно з параметрами прикладого алгоритму */
			modbus_master_rx_msg_t w106responce = {0};
			w106responce.length=23;

			while (tk2_106_status != tk2_OK) {
				if (tk4_Modbus_Session (&w106, &w106responce, request_timeout) != HAL_OK){
					tk2_106_status=tk2_Modbus_ERR; break;
				}

				uint16_t crc_calc_rx=0;
				crc_calc_rx = CRC_16x (&(w106responce.msg), 			    		/* 2. Обчислюємо 16-бітний CRC   									*/
						w106responce.length -2*(sizeof(uint8_t)));

				if (
					 (w106responce.msg[w106responce.length-1] != crc_calc_rx / 0x100) ||/* якщо CRC у повідомленні правильний*/
					 (w106responce.msg[w106responce.length-2] != crc_calc_rx % 0x100)
				   ){
					tk2_106_status=tk2_AnswerLOST;break;
				}
				if (w106responce.msg[0] != w106.msg[0]){
					tk2_106_status=tk2_WrongDevice;break; 		/* address ok*/
				}
				if (w106responce.msg[1] != w106.msg[1]){
					tk2_106_status=tk2_WrongeFunction;break;		/* function ok*/
				}
				p_drive_set_state->typesize=					(typesize_t)w106responce.msg[2];
				p_drive_set_state->motor_start_delay=			w106responce.msg[3];
				p_drive_set_state->motor_stop_delay=			w106responce.msg[4];
				p_drive_set_state->amperage_sensor_type=		w106responce.msg[5];
				p_drive_set_state->self_start=					(setpoint_t)w106responce.msg[6];
				p_drive_set_state->CX_recovery_delay    =		w106responce.msg[7];
				p_drive_set_state->numbers_of_starts_per_4mins=	w106responce.msg[8];

				p_drive_set_state->amperage_high_limit=			w106responce.msg[9]*0x100+w106responce.msg[10];


				p_drive_set_state->amperage_low_limit=			w106responce.msg[11]*0x100+w106responce.msg[12];
				p_drive_set_state->el_sensor_level_threshold=	w106responce.msg[13]*0x100+w106responce.msg[14];
				p_drive_set_state->izol_soft_start	= 			w106responce.msg[15]*0x100+w106responce.msg[16];
				p_drive_set_state->heating_constant = 			w106responce.msg[17]*0x100+w106responce.msg[18];
				//p_drive_set_state->project_version = 			w106responce.msg[19]*0x100+w106responce.msg[20];
				p_drive_set_state->TimeCode=					w106responce.msg[19]*0x100+w106responce.msg[20];

				tk2_106_status=tk2_OK;
			}
			return tk2_106_status;
}


#endif /* DRIVES_REMOTE_CONTROL */
