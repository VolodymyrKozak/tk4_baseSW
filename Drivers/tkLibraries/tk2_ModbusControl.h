/*
 * tk2_ModbusControl.h
 *
 *  Created on: 25 черв. 2018 р.
 *      Author: KozakVF
 */

#ifndef TK2_MODBUSCONTROL_H_
#define TK2_MODBUSCONTROL_H_
#include <tkConfig.h>
#ifdef DRIVES_REMOTE_CONTROL

#include<stdint.h>
typedef enum {
	tk2_OK,
	tk2_UnknownERR,
	tk2_Modbus_ERR,
	tk2_WrongDevice,
	tk2_WrongeFunction,
	tk2_WrongeRegime,
	tk2_WrongeTask,
	tk2_NULL_POINTER,
	tk2_AnswerLOST
} tk2_session_status_t;

typedef enum {
	Error 			= 0x00,
	Manual 			= 0x01,
	Automatical 	= 0x02,
	Remote_Manual 	= 0x04,
	Remote_Auto 	= 0x08
}drive_regime_t;

typedef enum{
	Disable,
	Enable
} setpoint_t;


typedef enum {
	Start 			= 0x01,
	Stop 			= 0x02
} drive_task_t;

typedef enum {
	Motor_is_On 	= 0x01,
	Motor_is_Off 	= 0x00
} drive_op_t;

typedef enum {
	Motor_OK		= 0x00,
	Unbalance 		= 0x01,
	Overload 		= 0x02,
	Underload		= 0x03,
	Isolation		= 0x04,
	Ground_Fault	= 0x05,
	Dry_Move		= 0x06,
	Process_Sencor_Foult		= 0x07,
	Jogging			= 0x08,
	On4Number		= 0x09,
	unknown10		= 0x0A,
	FlashFoult		= 0x0B
} motor_protect_t;

typedef enum{
	No				= 0x00,
	Yes				= 0x01
}sensor_has_just_worked_t;

typedef struct {
	drive_regime_t drive_regime;
	drive_task_t task_operational;
	drive_op_t drive_current_state;
	motor_protect_t motor_protected_code;
	sensor_has_just_worked_t high_level;
	sensor_has_just_worked_t low_level;
	sensor_has_just_worked_t middle_level;
	uint16_t amperage_1;
	uint16_t amperage_2;
	uint16_t amperage_3;
	uint16_t TimeCode;
} drive_op_state_t;

typedef enum{
	A2_10 		= 0x00,
	A6_25 		= 0x01,
	A10_55 		= 0x02,
	A20_65 		= 0x03,
	A30_90 		= 0x04,
	A50_220 	= 0x05,
	A159_360 	= 0x06,
	A220_500 	= 0x07,
	A500plus 	= 0x08
}typesize_t;

typedef struct {
	typesize_t typesize;
	uint8_t motor_start_delay; 				/*  количестве основных циклов контроллера, т.е. х0.2 сек*/
	uint8_t motor_stop_delay;				/*  количестве основных циклов контроллера, т.е. х0.2 сек*/
	uint8_t protection_reset_delay;
	uint8_t amperage_sensor_type;
	uint8_t reserve6;
	uint8_t CX_recovery_delay;
	setpoint_t self_start;
	uint8_t numbers_of_starts_per_4mins;
	uint16_t izolation;
	uint16_t amperage_high_limit;
	uint16_t amperage_low_limit;
	uint16_t el_sensor_level_threshold;
	uint16_t izol_soft_start;
	uint16_t heating_constant;
	uint16_t project_version;
	uint16_t TimeCode;
} drive_set_state_t;

typedef struct {
	uint8_t drive_modbus_address;
	drive_op_state_t drive_op_state;
	drive_set_state_t drive_set_state;
} remote_drives_t;

tk2_session_status_t f_drive_op_state_request105 (uint8_t in_modbus_adress, drive_op_state_t *p_drive_op_state);
#endif /* DRIVES_REMOTE_CONTROL */
#endif /* TK2_MODBUSCONTROL_H_ */
