/*
 * tkFurie.h
 *
 *  Created on: 10 ����. 2018 �.
 *      Author: KozakVF
 */


#ifndef TKLIBRARIES_TKFURIE_H_
#define TKLIBRARIES_TKFURIE_H_
#include "tkConfig.h"
#ifdef FURIE_LIB




#include <stdbool.h>

/* ���������� ��������� ��� ���������� ���������� rms ����� ���� �������� ������������ ������*/
typedef struct {
	adcf_t 	permanent_component; /* ������� �������� ������, ���� �� ���� - ����� ���(uint16),
							      	  	  �� ��������, ������� �� ����������� �����, ����� ���		*/
	adcf_t 	rms_heat;            /* root_mean-square, ����� ����� ��������, ����� ���			*/
} iheat_out_t;
/* ���������� ��������� ��� ���������� ���������� rms ����� ������������ ���'� ��� ������� ��������� */
typedef struct {
	adcf_t 	rms_fundamental;     /* root_mean-square, ����� ����� ������� ���������, ����� ��� 	*/
	gradf_t phase_fundamental;   /* ���� ������� ���������,  �������*/
	adcf_t	rmsZero;			 /* ����� ����� ���� ��������, ������� �����				*/
	adcf_t  rmsNegative;         /* ����� ����� ��������, ������� 3*�+2                    */
} ifurie_out_t;


typedef struct {
	adcf_t  u1harm_ampl;	/*�������� ����� ��������� �������, ������ 	*/
	gradf_t u_phase;		/*���� �������, ������� 				*/
} ufurie_out_t;

/* ���������� �� ����-��� ������� �����������, ���� �� �������� ������/�������� ����� ���
 * ��� ������������ � ����� ���������� ������� ������ 								*/
void f_uiFurie_Init(void);

/* ������������ ����� ��� �������,
 * ������� �������� - ����������, ������������:
 * �������� ������� ( � �������� ��� ��� ���������� �������������
 * ���� - � ��������, ��� ������������
 * �� ���� -
 * �������� �� ����� ����� ��� 18 �����,
 * ������� ������� ����� ����������������, 40..60��
 * ������� �� ������� ������������ ����� �������,
 * */
int	f_ufurie(
		volatile adcf_t *pf_array,		/* �������� �� ����� ����� ��� 0..4096, ������������ � ������ float 		*/
		volatile ufurie_out_t *p_ufr	/*u_rms   -�������� ������� ( � �������� ��� ��� ���������� �������������	*/
		);



/* ������� ������ ��������� ������������ �����
 * ��� ���������� ������� � ����� ������� ������ ����� ������� 46..54��
 * ������� ����� - ������ 18 */
int f_ifurie(	volatile adcf_t *pf_array,     // �������� �� ����� ����� ���, ������� ����� = 18
				volatile ifurie_out_t *p_ifr); // ���������

/* ������� ���������� rms ����� ���� �������� ������������ ������*/
int f_iheat(
		volatile adcu16_t *p200_array, /* �������� �� ����� �����, �� 240 */
		float zero_line,			   /* ������ �����, ���� ��� ��������� � ��������� ������ */
		uint16_t Nn,				   /* ��������� ������� �����, ���������� ����� ������� ������� */
		volatile iheat_out_t  *p_ih);  /* ���������											*/











#endif //FURIE_LIB
#endif /* TKLIBRARIES_TKFURIE_H_ */
