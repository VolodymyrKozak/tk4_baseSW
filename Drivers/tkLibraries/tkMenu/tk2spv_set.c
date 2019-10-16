/*
 * tk2spv_set.c
 *
 *  Created on: 25 ���. 2019 �.
 *      Author: Volodymyr.Kozak
 */
#include "tk2spv_set.h"
#include "tkDTset.h"
#include "zFF.h"
#include "wk_atime64.h"
#include  "vv300set.h"

uiset_state_t UIset_state = UIset_main_state;
uint32_t spv_Tick=0;
void f_1UIset_Spv_Init(void){
	UIset_state = UIset_main_state;
	spv_Tick=0;
}



uiset_state_t f_1UIset_Spv(void){


	/*****************************************************************************
	 * ����.���.����.
	 *****************************************************************************/
	static struct tm setTM={0};

	if( UIset_state== UIset_main_state){
		snprintf((char*)strHigh_menu, 17,"���.���� �� ���");
	 	tkDisplay_PutHighStr(strHigh_menu);
		  /*0123456789012345*/
		  /*2019/02/15 11:55*/
	 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
	 			p_wk64_tm->tm_year+1900,
				p_wk64_tm->tm_mon+1,
				p_wk64_tm->tm_mday,
				p_wk64_tm->tm_hour,
				p_wk64_tm->tm_min);
	 	tkDisplay_PutLowStr(strLow_menu);
	 	UIset_state=UIset_mainBTN_state;
	}
	/*****************************************************************************
	 * ����.���.���������� ������.
	 *****************************************************************************/
	else if (UIset_state==UIset_mainBTN_state){
// 		if  (BTN==DOWN_btn){
// 			UIset_state= UIset_Powers_state;
// 		}
// 		else
 		if  (BTN==ESC_btn){
 			UIset_state = UIset_exit_state;
 		}
 		else if (BTN==ENTER_btn){
 			memcpy (&setTM, p_wk64_tm, sizeof (struct tm));
 			f_TMmenuInit();
 			UIset_state=UIset_time_dymamic;
 			BTN_Delay=900;
 		}
 		else if (BTN==DOWN_btn){
 			f_ARRu16menu_Init();
 			UIset_state=UIset_userArray;
 		}
 		else{}
	}
	/*****************************************************************************
	 * ����.���.�������.
	 *****************************************************************************/
	else if (UIset_state==UIset_time_dymamic){

		tm_menu_state_t uis = f_TMmenu(&setTM);
		if (uis==TMmenu_valuePreSet){
			UIset_state=UIset_datatime_SET;
		}
		else if (uis==TMmenu_escape){
			UIset_state=UIset_main_state;
		}
		else{}
	}
	/*****************************************************************************
	 * ����.���.Գ������ � ���'��.
	 *****************************************************************************/
	else if (UIset_state == UIset_datatime_SET){
		if (BTN==ENTER_btn){
			setDataAndTime(&setTM);
			snprintf((char*)strLow_menu, 17, "���� ���������");
			tkDisplay_PutLowStr(strLow_menu);
			UIset_state=UIset_main_state;
			BTN_Delay=900;
			f_ARRu16menu_Init();
			UIset_state=UIset_userArray;
		}
	}
	/*****************************************************************************
	 * ���� ������� ����������� (��� ������ � ����)
	 *****************************************************************************/
	else if (UIset_state==UIset_userArray){
		ARRu16_state_t arru16 = f_ARRu16menu (u, uset, 2);
		if (arru16 == ARRu16cmplt){
			/* ���� ������� ����������� ����������. �� ������ ���?*/
			UIset_state = UIset_userArray_Exit;
		}
	}
	/*****************************************************************************
	 * ���� �� ������ ���
	 *****************************************************************************/
	else if (UIset_state==UIset_userArray_Exit){
		 if  (BTN==ESC_btn){
			 UIset_state = UIset_exit_state;
		 }
		 else if (BTN==ENTER_btn){
			  /* ����� ������� ����������� � ���� */
			 f_VV300_UserSets_Write();
			 snprintf((char*)strLow_menu, 17, "��� ���������");
			 tkDisplay_PutLowStr(strLow_menu);
		 }
		 else if (BTN==DOWN_btn){
		 	/* � ��� - ��� �� ������ � ��������� ���������,���� �����*/
			f_u16menu_Init();
		 	UIset_state=UIset_PSW2_Dynamic;
		 }
		 else{}
	}

//	UIset_userArray_Exit,
//
//	UIset_PSW2_Entry,
//	UIset_PSW2_BTN,
//	UIset_PSW2_Dynamic,
//	void f_u16menu_Init(void){
//		u16menu_state = u16menu_input;
//	}
//
//	u16menu_state_t f_u16menu(		u16menu_orn_t *u16g,
//									uint16_t start_value,
//									uint16_t *p_out_value){
	/*****************************************************************************
	 * ������ ���������.����.
	 *****************************************************************************/
	else if (UIset_state==UIset_PSW2_Dynamic){
		static	u16menu_orn_t u16psw2_orn =
			{"������ ������.","----", "----",	10000, 1111, 1};

		static uint16_t psw2 = 0;
		u16menu_state_t psw2_state =f_u16menu(&u16psw2_orn,
											  5555,
											  &psw2);
		if (psw2_state==u16menu_valuePreSet){
			if (psw2==5558){
				f_ARRu16menu_Init();
				UIset_state=UIset_FacilityArr;
			}
			else {
				f_u16menu_Init();
			}
		}
		 if  (BTN==ESC_btn){
			 UIset_state = UIset_exit_state;
		 }
	}
	/*****************************************************************************
	 * ���� ������� ��������� (��� ������ � ����)
	 *****************************************************************************/
	else if (UIset_state==UIset_FacilityArr){
		ARRu16_state_t arru16 = f_ARRu16menu (fc, fcset, 4);
		if (arru16 == ARRu16cmplt){
			/* ���� ������� ����������� ����������. �� ������ ���?*/
			UIset_state = UIset_FacilityCplt;
			snprintf((char*)strLow_menu, 17, "�������� ? ");
			tkDisplay_PutLowStr(strLow_menu);
		}
	}
	else if (UIset_state==UIset_FacilityCplt){
		 if  (BTN==ESC_btn){
			 UIset_state = UIset_exit_state;
		 }
		 else if (BTN==ENTER_btn){
			 f_VV300_UserSets_Write();
			 snprintf((char*)strLow_menu, 17, "��� ���������");
			 tkDisplay_PutLowStr(strLow_menu);
			 UIset_state=UIset_FacilitySvd;
		 }
	}
	else if (UIset_state==UIset_FacilitySvd){
		 if  (BTN==ESC_btn){
			 UIset_state = UIset_exit_state;
		 }
		 else if (BTN==ENTER_btn){
			 UIset_state = UIset_exit_state;
		 }
	}

//	else if ((BTN==ESC_btn)){
//		UIset_state=UIset_main_state;
//		BTN_Delay=900;
//		}
//	}


//	else if (UIset_state == UIset_exit_state){
//		prvUIset_state = UIset_main_state;
//	}
	else{}
	return UIset_state;
}
