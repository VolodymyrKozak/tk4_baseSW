/*
 * tk0spv.h
 *
 *  Created on: 25 ���. 2019 �.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKLIBRARIES_TKMENU_TK0SPV_H_
#define TKLIBRARIES_TKMENU_TK0SPV_H_
typedef enum {
	spv_OK,						/* OK, ��� ���������															*/
	spv_UNKNOWN_Err,			/* ����������� �������*/
	spv_wo1602gErr,
	spv_FLASHErr,
	spv_I2C_Err,				/* ������� ��������������� ����� �� I2C 										*/
	spv_BUSY,
	spv_WRONG_Arg,				/* �� ���� ������� ��������� ���������, ���� � ������� ���� ������ ���������� */
	spv_NULL_Ptr,				/* �������� �������� (���. - ���������), ������ ���������� ������ 				*/
	spv_Err						/* ������� ��� ��������� �������, ���� �� ���..*/
}spv_status_t;


spv_status_t f_0UI_Init(void);
spv_status_t f_0UI_Spv(void);

#endif /* TKLIBRARIES_TKMENU_TK0SPV_H_ */
