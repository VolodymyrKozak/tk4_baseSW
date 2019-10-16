/*
 * zFF.c
 *
 *  Created on: 24 лют. 2019 р.
 *      Author: Volodymyr.Kozak
 */
#include "zFF.h"


int8_t strHigh_menu[17]={0};
int8_t strLow_menu[17]={0};
int8_t strLow0_menu[17]={0};

int8_t strHigh_prev[17]={0};
int8_t strLow_prev[17]={0};

dsp_status_t tkDisplay_Init(void){

	memset (strHigh_menu,0,sizeof(strHigh_menu));
	memset (strLow_menu,0,sizeof(strHigh_menu));
	memset (strLow0_menu,0,sizeof(strHigh_menu));

	memset (strHigh_prev,0,sizeof(strHigh_prev));
	memset (strLow_prev,0,sizeof(strLow_prev));
	return dsp_OK;
}
dsp_status_t tkDisplay_ClearScreen(void){
	dsp_status_t dsp_status=dsp_UNKNOWN_Err;
#ifdef WINSTAR_Display_WO1602G
	wo1602g_status_t wo_status = WO1602G_UNKNOWN_Err;
	wo_status = WO1602_ClearScreen();
	if (wo_status == WO1602G_OK){
		dsp_status = dsp_OK;
	}
	else if(wo_status == WO1602G_I2C_Err){
		dsp_status = dsp_I2C_Err;
	}
	else {
		dsp_status =dsp_wo1602gErr;
	}
#endif
	return dsp_status;
}


dsp_status_t tkDisplay_PutHighStr(int8_t *hstr){
	dsp_status_t dsp_status = dsp_UNKNOWN_Err;
	if (memcmp (hstr, strHigh_prev, 16) !=0){
#ifdef WINSTAR_Display_WH1602B3
		WH1602b3_WriteHighString(hstr);
#endif
#ifdef WINSTAR_Display_WO1602G


		wo1602g_status_t wo_status = WO1602G_UNKNOWN_Err;
		wo_status = WO1602_WriteHighString (hstr);
		if (wo_status == WO1602G_OK){
			dsp_status = dsp_OK;
		}
		else if(wo_status == WO1602G_I2C_Err){
			dsp_status = dsp_I2C_Err;
		}
		else {
			dsp_status =dsp_wo1602gErr;
		}

#endif
		memcpy (strHigh_prev, hstr, 16);
	}
	return dsp_status;
}
dsp_status_t tkDisplay_PutLowStr(int8_t *hstr){
	dsp_status_t dsp_status = dsp_UNKNOWN_Err;
	if (memcmp (hstr, strLow_prev, 16) !=0){

#ifdef WINSTAR_Display_WH1602B3
		WH1602b3_WriteLowString(hstr);
#endif
#ifdef WINSTAR_Display_WO1602G
		wo1602g_status_t wo_status = WO1602G_UNKNOWN_Err;
		wo_status=WO1602_WriteLowString (hstr);
		if (wo_status == WO1602G_OK){
			dsp_status = dsp_OK;
		}
		else if(wo_status == WO1602G_I2C_Err){
			dsp_status = dsp_I2C_Err;
		}
		else {
			dsp_status =dsp_wo1602gErr;
		}
#endif
		memcpy (strLow_prev, hstr, 16);
	}
	return dsp_status;
}

