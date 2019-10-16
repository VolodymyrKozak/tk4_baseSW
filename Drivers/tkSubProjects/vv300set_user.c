/*
 * vv300set_user.c
 *
 *  Created on: 9 ���. 2019 �.
 *      Author: KozakVF
 */

#include "vv300set.h"


///* ����������� ����� ������� � ������� ���  */
//volatile static adcf_t ratedAdcAmperage =  362.0;
///* ̳�������� �����, % �� ������������, ��� ����� ���������� ������ */
//volatile static procentf_t lLimitRms =   7.0;

	 uint32_t us32set[N_USET / 2 +2] = {0};
	 uint16_t *uset = 0;



 /* 2) ��������� ����� �������� u16menu_orn_t, �� �������� ��� ����� �������:*/
     u16menu_orn_t u[N_USET]={
    				 { "���.�����", 	"I���",		"�",	1000, 10, 1},
					 { "CalcLeak%", 	"���%",		"%",	100,  1, 1 }
     };
     /*  ���������� �������� ���������*/
void f_UsetInit(void){
	 uint16_t *uset =(uint16_t*)(us32set);

    uset[0]  = 100; /* ���������� ����� */
    uset[1]  = 10;  /* % �� �����������, ��� ����� ������ ���� */
}
flash_status_t f_VV300_UserSets_Read(void){
	return f_VV_Sets_Read(   us32set,
						     sizeof(us32set),
							 FLASH_PAGE_USER_SETS_A,
							 FLASH_PAGE_USER_SETS_B
							);
}
flash_status_t f_VV300_UserSets_Write(void){
	return f_VV_Sets_Write(   us32set,
						     sizeof(us32set),
							 FLASH_PAGE_USER_SETS_A,
							 FLASH_PAGE_USER_SETS_B
							);
}

