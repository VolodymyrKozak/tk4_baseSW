/*
 * tk1spv_grid.c
 *
 *  Created on: 25 ���. 2019 �.
 *      Author: Volodymyr.Kozak
 */
#include "tk1spv_grid.h"
#include "tkGrid.h"
#include "zFF.h"
#include "vv300Flash.h"

extern procentf_t UnbalanceDeratingFactor;
extern procentf_t HarmonicsDeratingFactor;
/*������� ��������� ����������� ����� 					*/
/*���������� �������� ���������� �� ������ � ����� ���  */
extern procentf_t aTotalHarmonicDistortion;
extern procentf_t bTotalHarmonicDistortion;
extern procentf_t cTotalHarmonicDistortion;

uigrid_state_t UIgrid_state = UIgrid_main_state;
/* ����i����� �������� ��� 									*/
extern volatile procentf_t AmperageUnbalance;
static void f_read_prot1 (void);

void f_1UIgrid_Spv_Init(void){
	UIgrid_state=UIgrid_main_state;
}



uigrid_state_t f_1UIgrid_Spv(void){
	uint16_t grd[16]={0};
	f_GRID_RaportHR(grd);
	/*****************************************************************************
	 * ������� ���� �. �������. ˳����� ����� �, �, �
	 *****************************************************************************/
	if( UIgrid_state== UIgrid_main_state){
		snprintf((char*)strHigh_menu,17,
				//	 * 0		uint16_t, 0.01��		������� �� 46�� �� 54��,
				//	 grid16[0]=(uint16_t)(Frequency*100);
				//	 * 1		uint16_t  �						������� (���� �),
				//	 grid16[1]=(uint16_t)(Voltage);
				"%3dV %2d.%02dHz",
				grd[1], grd[0]/100, grd[0]%100
		);
		snprintf((char*)strLow_menu,17,
				//	 * 234		uint16_t  0.1A, max 6553A �������������������� ������ ��� �BC (��������),
				//	 grid16[2]=(uint16_t)(aAmperage);
				//	 grid16[3]=(uint16_t)(bAmperage);
				//	 grid16[4]=(uint16_t)(cAmperage);
				"%3dA %3dA %3dA",
				grd[2]/10, grd[3]/10, grd[4]/10
		);
		tkDisplay_PutHighStr(strHigh_menu);
		tkDisplay_PutLowStr(strLow_menu);
		UIgrid_state= UIgrid_main_state_btn;
	}
	else if( UIgrid_state== UIgrid_main_state_btn){
 		if  (BTN==DOWN_btn){
 			UIgrid_state= UIgrid_Powers_state;
 		}
 		else if  (BTN==ESC_btn){
 			UIgrid_state = UIgrid_exit_state;
 		}
 		else{}
	}
	/*****************************************************************************
	 * ��������� �������. ��������� ���������. ������� ��.
	 *****************************************************************************/
	else if( UIgrid_state== UIgrid_Powers_state){
		f_GRID_RaportHR(&grd);
		snprintf((char*)strHigh_menu,17,
			   /*1234567890123456*/
				"�����Ͳ���, kWt"
		);
		snprintf((char*)strLow_menu,17,
//   * 8		uint16_t  0.1���    ������� ���������,
//					 grid16[8]=(uint16_t)(ActivePower/100.0f);
//	 * 9		uint16_t  0.1��� 	��������� ��������� ��
//					 grid16[9]=(uint16_t)(ReactivePower/100.0f);
//	 * 10		int16_t   %		    ����������� ��������� (�os fi),
//					 grid16[10]=(uint16_t)PowerFactor;
				"A%4d R%4d cs%02d",
				grd[8]/10, grd[9]/10, grd[10]
		);
		tkDisplay_PutHighStr(strHigh_menu);
		tkDisplay_PutLowStr(strLow_menu);
		UIgrid_state = UIgrid_Powers_state_btn;
	}
	else if( UIgrid_state== UIgrid_Powers_state_btn){
 		if  (BTN==UP_btn){
 			UIgrid_state= UIgrid_main_state;
 		}
 		else if  (BTN==DOWN_btn){
 			UIgrid_state= UIgrid_Derating_state;
 		}
 		else if  (BTN==ESC_btn){
 			UIgrid_state = UIgrid_exit_state;
 		}
 		else{}
	}
	/*****************************************************************************
	 * ������ ���������. 1) ����� �������� 2)����� ��������
	 *****************************************************************************/
	else if( UIgrid_state== UIgrid_Derating_state){
		snprintf((char*)strHigh_menu,17,
			   //1234567890123456
				"������ ������. "
		);
		tkDisplay_PutHighStr(strHigh_menu);
		snprintf((char*)strLow_menu,17,

			//   1234567890123456
			//	 A�� 28% ��� 12%
				"Ac� %2d%% ��� %2d%%",
				(uint16_t)(100.0f - UnbalanceDeratingFactor),
				(uint16_t)(100.0f - HarmonicsDeratingFactor)

		);
		tkDisplay_PutLowStr(strLow_menu);
		UIgrid_state = UIgrid_Derating_state_btn;
	}
	else if( UIgrid_state== UIgrid_Derating_state_btn){
 		if  (BTN==ENTER_btn){
 			UIgrid_state= UIgrid_phasesTHDs;
 		}
 		else if  (BTN==UP_btn){
 			UIgrid_state= UIgrid_Powers_state;
 		}
 		else if  (BTN==DOWN_btn){
 			UIgrid_state= UIgrid_EventsArchive;
 		}
 		else if  (BTN==ESC_btn){
 			UIgrid_state = UIgrid_exit_state;
 		}
 		else{}
	}
	/*****************************************************************************
	 * ����i����� �������� ���
	 * ���������� �������� ���������� �� ������ � ����� ���
	 *****************************************************************************/
	else if( UIgrid_state== UIgrid_phasesTHDs){
		snprintf((char*)strHigh_menu,17,
			   //1234567890123456
				"��.���� %3d%%",
				(uint16_t)(AmperageUnbalance)
		);
		tkDisplay_PutHighStr(strHigh_menu);
		snprintf((char*)strLow_menu,17,
				"%%THD %3d %3d %3d",
				(uint16_t)(aTotalHarmonicDistortion),
				(uint16_t)(bTotalHarmonicDistortion),
				(uint16_t)(cTotalHarmonicDistortion)
		);
		tkDisplay_PutLowStr(strLow_menu);
		UIgrid_state= UIgrid_phasesTHDs_btn;
	}
	else if( UIgrid_state== UIgrid_phasesTHDs_btn){
 		if  (BTN==ESC_btn){
 			UIgrid_state = UIgrid_Derating_state;
 		}
	}

	else if (UIgrid_state == UIgrid_EventsArchive){
		snprintf((char*)strHigh_menu,17,
			   //1234567890123456
				"APղ� ��Ĳ�");
		tkDisplay_PutHighStr(strHigh_menu);
		memset ((void*)&strLow_menu,0,sizeof(strLow_menu));
		tkDisplay_PutLowStr(strLow_menu);
		UIgrid_state = UIgrid_EventsArchiveEnt;
	}
	else if (UIgrid_state == UIgrid_EventsArchiveEnt){
		if  (BTN==ENTER_btn){
 			f_look_start_flash_to_read (
 					GRID_STATUS_REC_A,
					sizeof(gr.grfl)
					);
 			f_read_prot1 ();
 			UIgrid_state= UIgrid_EventsArchiveRead;
		}
		else if  (BTN==UP_btn){
		 	UIgrid_state= UIgrid_Derating_state;
		}
		else if  (BTN==ESC_btn){
 			UIgrid_state= UIgrid_exit_state;
		}
		else{}
	}
	else if (UIgrid_state == UIgrid_EventsArchiveRead){

		if  (BTN==ESC_btn){
 			UIgrid_state= UIgrid_EventsArchive;
		}
		if  (BTN==DOWN_btn){
			f_look_next_flash_to_read (
 					GRID_STATUS_REC_A,
					sizeof(gr.grfl)
					);
 			f_read_prot1 ();
		}
	}

	else if (UIgrid_state == UIgrid_exit_state){
	}
	else{}


	return  UIgrid_state;
}

static void f_read_prot1 (void){
		f_read_from_flash_rec( 	GRID_STATUS_REC_A,
								gr.grfl,
							sizeof(gr.grfl)
								);
		struct tm *pTM;
		pTM = localtime64 (&gr.gr_rec.elipsed_time);
	 	snprintf((char*)strHigh_menu, 17, "%4d/%02d/%02d %02d:%02d",
	 			pTM->tm_year+1900,
			pTM->tm_mon+1,
			pTM->tm_mday,
			pTM->tm_hour,
			pTM->tm_min);
	 	tkDisplay_PutHighStr(strHigh_menu);
	 	snprintf((char*)strLow_menu, 17, "0x%3x",(uint16_t)gr.gr_rec.gstatus);
	 	tkDisplay_PutLowStr(strLow_menu);
}
