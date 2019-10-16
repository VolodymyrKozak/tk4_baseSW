/*
 * mb_slave5a.c
 *
 *  Created on: 29 ñ³÷. 2019 ð.
 *      Author: Volodymyr.Kozak
 */

#include "tkConfig.h"
#include "mb_slave5a.h"
#include "mb.h"
#include "stdint.h"
#include <stddef.h>

modbus_status_t f_READ_HOLDING_REGISTERS_Handler (
									uint16_t iStarting_Address,
									uint16_t iQuantity_of_Registers,
									uint8_t* u8_answer,
									modbus_exception_t *exc
									){
	if ((u8_answer == NULL)|| (exc==NULL)){return MODBUS_ZERO_POINTER;}
	modbus_status_t rhrh_status=MODBUS_ERR_UNKNOWN;
	while (rhrh_status != MODBUS_SUCCESS){
		if ((iStarting_Address < 0x100) || (iStarting_Address >= 0x110)){
			(*exc)=MODBUS_EXT_ILLEGAL_DATA_ADDRESS;
			rhrh_status = MODBUS_SUCCESS; break;

		}
		if (iQuantity_of_Registers>0x10){
			(*exc)=MODBUS_EXC_ILLEGAL_DATA_VALUE;
			rhrh_status = MODBUS_SUCCESS; break;
		}
		if (iStarting_Address + iQuantity_of_Registers > 0x110){
			(*exc)=MODBUS_EXC_ILLEGAL_DATA_VALUE;
			rhrh_status = MODBUS_SUCCESS; break;
		}
		uint16_t grid16y[0x10] = {0};
		f_GRID_RaportHR(&grid16y);
		for (uint8_t i;i<iQuantity_of_Registers;i++){
			u8_answer[2*i]  =grid16y[iStarting_Address-0x100] / 0x100;
			u8_answer[2*i+1]=grid16y[iStarting_Address-0x100] % 0x100;
		}
		(*exc)=MODBUS_F_SUCCESS;
		rhrh_status = MODBUS_SUCCESS;
	}
	return rhrh_status;
}
