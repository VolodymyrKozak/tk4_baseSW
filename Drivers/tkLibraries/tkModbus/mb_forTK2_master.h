/*
 * tkModbusMaster1.h
 *
 *  Created on: 18 трав. 2018 р.
 *      Author: KozakVF
 */

#ifndef TKMODBUSMASTER1_H_
#define TKMODBUSMASTER1_H_

#include <tkConfig.h>

#ifdef  MODBUS_MASTER_for_TK2
#include "usart.h"
#include "../../tkLibraries/tkModbus/mb.h"
#include "../../tkLibraries/tkModbus/mb_master.h"
#include "../../tkLibraries/tkModbus/mb_master5.h"


typedef enum {
	MBtk2_Master_Idle,
	MBtk2_Master_Waiting_for_reply,
	MBtk2_Master_Processing_reply
} master_state_t;
typedef enum {
	ManualSTOP,
	ManualSTART,
	AutomatSTOP,
	AutomatSTART,
	ProtectionCastOFF_AutomatSTART,
	ProtectionCastOFF_ManualSTART,
	Protection_LIMIT_CastOFF_AutomatSTART,
	Protection_LIMIT_CastOFF_ManualSTART
} task110_t;


typedef enum {
	tk2_OK,
	tk2_UnknownERR,
	tk2_Modbus_ERR,
	tk2_WrongDevice,
	tk2_WrongeFunction,
	tk2_WrongeRegime,
	tk2_WrongeTask,
	tk2_AnswerLOST,
} tk2_session_status_t;


    tk2_session_status_t f_tk2_TASKS_110 (uint8_t tk2_ADDRESS, task110_t task110);
	modbus_status_t f_MB_PC_DiagnosticFE (uint8_t slave_address,uint8_t msg_counter_x,uint8_t msg_counter_y);
	modbus_status_t f_MBtk2_Diagnostic_108 (uint8_t slave_address,uint8_t msg_counter_x,uint8_t msg_counter_y);
	void WO1602_Write_HEX (modbus_master_tx_msg_t ww);

#endif  /*MODBUS_MASTER_for_TK2*/
#endif /* TKMODBUSMASTER1_H_ */
