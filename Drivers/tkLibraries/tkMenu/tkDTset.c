/*
 * tkDTset.c
 *
 *  Created on: 24 ëþò. 2019 ð.
 *      Author: Volodymyr.Kozak
 */
#include "tkDTset.h"
#include "zFF.h"
#include "wk_atime64.h"

tm_menu_state_t tm_state = TMmenu_input;
static	uint32_t tmmenuTick;
void f_TMmenuInit(void){
	tm_state = TMmenu_input;

}


tm_menu_state_t f_TMmenu(struct tm *outTM){

//static struct tm TMm = {0};
//	TMm =(*p_wk64_tm); /* Öå íå ïîìèëêà. ×àñ îíîâëþýòüñÿ..*/
	// 	/*************************************************************
	// 	 * ²Í²Ö²ÀË²ÇÀÖ²ß
	// 	 * ************************************************************/
	if (tm_state ==  TMmenu_input){
	                             /*1234567890123456*/
		snprintf((char*)strHigh_menu, 17,"äàòà/÷àñ");
	 	tkDisplay_PutHighStr(strHigh_menu);
		  /*0123456789012345*/
		  /*2019/02/15 11:55*/
	 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
	 			outTM->tm_year+1900,
				outTM->tm_mon+1,
				outTM->tm_mday,
				outTM->tm_hour,
				outTM->tm_min);
	 	tkDisplay_PutLowStr(strLow_menu);
	 	snprintf((char*)strLow0_menu, 17, "    /%02d/%02d %02d:%02d",

	 				outTM->tm_mon+1,
					outTM->tm_mday,
					outTM->tm_hour,
					outTM->tm_min);
	 	BTN_Delay=900;
	 	tmmenuTick = HAL_GetTick();
	 	tm_state = TMmenu_settingYYYY;
	}
 	/*************************************************************
 	 * ÂÈÁ²Ð ÇÍÀ×ÅÍÍß ÐÎÊÓ
 	 * ************************************************************/
    else if (tm_state == TMmenu_settingYYYY){
 		if (BTN==ESC_btn){
 			tm_state=TMmenu_escape;
 		}
 		else if (BTN==DOWN_btn){
// 			outTM->tm_year+1900,
 			outTM->tm_year--;
 		    if  ((outTM->tm_year+1900) < 2019){
 		    	outTM->tm_year=2019-1900;
 		    }
// 		 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
// 		 			outTM->tm_year+1900,
// 					outTM->tm_mon+1,
// 					outTM->tm_mday,
// 					outTM->tm_hour,
// 					outTM->tm_min);
 		}
 		else if (BTN==UP_btn){
 			outTM->tm_year++;
 		    if  (outTM->tm_year+1900 > 2099){
 		    	outTM->tm_year = 2099-1900;
 		    }
// 		 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
// 		 			outTM->tm_year+1900,
// 					outTM->tm_mon+1,
// 					outTM->tm_mday,
// 					outTM->tm_hour,
// 					outTM->tm_min);
 		}
 		else if (BTN==ENTER_btn){
 			tm_state=TMmenu_settingMM;
	 		snprintf((char*)strLow0_menu, 17, "%4d/  /%02d %02d:%02d",
	 				outTM->tm_year+1900,
//	 				outTM->tm_mon+1,
					outTM->tm_mday,
					outTM->tm_hour,
					outTM->tm_min);
 		}
		else{}
 	}
 	/*************************************************************
 	 * ÂÈÁ²Ð ÇÍÀ×ÅÍÍß Ì²ÑßÖß
 	 * ************************************************************/
	else if (tm_state == TMmenu_settingMM){
 		if (BTN==ESC_btn){
 			tm_state=TMmenu_escape;
 		}
 		else if (BTN==DOWN_btn){
 			outTM->tm_mon--;
 		    if  (outTM->tm_mon ==-1){
 //		    	outTM->tm_mon =11;
 		    }
// 		 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
// 		 			outTM->tm_year+1900,
// 					outTM->tm_mon+1,
// 					outTM->tm_mday,
// 					outTM->tm_hour,
// 					outTM->tm_min);
 		}
 		else if (BTN==UP_btn){
 			outTM->tm_mon++;
 		    if  (outTM->tm_mon ==12){
 		    	outTM->tm_mon =0;
 		    }
// 		 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
// 		 			outTM->tm_year+1900,
// 					outTM->tm_mon+1,
// 					outTM->tm_mday,
// 					outTM->tm_hour,
// 					outTM->tm_min);
 		}
 		else if (BTN==ENTER_btn){
 			tm_state=TMmenu_settingDD;
	 		snprintf((char*)strLow0_menu, 17, "%4d/%02d/   %02d:%02d",
	 				outTM->tm_year+1900,
	 				outTM->tm_mon+1,
//					outTM->tm_mday,
					outTM->tm_hour,
					outTM->tm_min);
 		}
 		else if (BTN==ESC_btn){
 			tm_state=TMmenu_escape;
 		}
		else{}
 	}
 	/*************************************************************
 	 * ÂÈÁ²Ð ÇÍÀ×ÅÍÍß ÄÍß Ì²ÑßÖß
 	 * ************************************************************/
	else if (tm_state == TMmenu_settingDD){
		const uint8_t days_in_mon[2][12] = {
				{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
				{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
		};

 		if (BTN==ESC_btn){
 			tm_state=TMmenu_escape;
 		}
 		else if ((BTN==DOWN_btn)||(BTN==DOWNLong_btn)){
 			if (outTM->tm_mday>1){
 				outTM->tm_mday--;
 			}
// 		 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
// 		 			outTM->tm_year+1900,
// 					outTM->tm_mon+1,
// 					outTM->tm_mday,
// 					outTM->tm_hour,
// 					outTM->tm_min);
 		}
 		else if ((BTN==UP_btn)||(BTN==UPLong_btn)){
 			uint8_t NdaysInThisMonth =0;
 			if ((outTM->tm_year % 4) == 0){
 				NdaysInThisMonth=days_in_mon[0][outTM->tm_mon];
 			}
 			else {
 				NdaysInThisMonth=days_in_mon[1][outTM->tm_mon];
 			}
 			if (outTM->tm_mon<NdaysInThisMonth){
 				outTM->tm_mday++;
 			}
// 		 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
// 		 			outTM->tm_year+1900,
// 					outTM->tm_mon+1,
// 					outTM->tm_mday,
// 					outTM->tm_hour,
// 					outTM->tm_min);
 		}
 		else if (BTN==ENTER_btn){
 			tm_state=TMmenu_settingHH;
	 		snprintf((char*)strLow0_menu, 17, "%4d/%02d/%02d   :%02d",
	 				outTM->tm_year+1900,
	 				outTM->tm_mon+1,
					outTM->tm_mday,
//					outTM->tm_hour,
					outTM->tm_min);
 		}
		else{}
 	}
 	/*************************************************************
 	 * ÂÈÁ²Ð ÇÍÀ×ÅÍÍß ÃÎÄÈÍ
 	 * ************************************************************/
	else if (tm_state == TMmenu_settingHH){
 		if (BTN==ESC_btn){
 			tm_state=TMmenu_escape;
 		}
 		else if ((BTN==DOWN_btn)||(BTN==DOWNLong_btn)){
 			outTM->tm_hour--;
 			if (outTM->tm_hour==-1){
 				outTM->tm_hour=23;
 			}
// 		 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
// 		 			outTM->tm_year+1900,
// 					outTM->tm_mon+1,
// 					outTM->tm_mday,
// 					outTM->tm_hour,
// 					outTM->tm_min);
 		}
 		else if ((BTN==UP_btn)||(BTN==UPLong_btn)){

 			outTM->tm_hour++;
 			if (outTM->tm_hour==24){
 				outTM->tm_hour=0;
 			}
// 		 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
// 		 			outTM->tm_year+1900,
// 					outTM->tm_mon+1,
// 					outTM->tm_mday,
// 					outTM->tm_hour,
// 					outTM->tm_min);
 		}
 		else if (BTN==ENTER_btn){
 			tm_state=TMmenu_settingMinute;
	 		snprintf((char*)strLow0_menu, 17, "%4d/%02d/%02d %02d:  ",
	 				outTM->tm_year+1900,
	 				outTM->tm_mon+1,
					outTM->tm_mday,
					outTM->tm_hour
//					outTM->tm_min
					);
 		}
		else{}
 	}
 	/*************************************************************
 	 * ÂÈÁ²Ð ÇÍÀ×ÅÍÍß ÕÂÈËÈÍ
 	 * ************************************************************/
	else if (tm_state == TMmenu_settingMinute){
 		if (BTN==ESC_btn){
 			tm_state=TMmenu_escape;
 		}
 		else if (BTN==DOWN_btn){
 			outTM->tm_min--;
 			if (outTM->tm_min==-1){
 				outTM->tm_min=59;
 			}
 		}
 		else if (BTN==UP_btn){

 			outTM->tm_min++;
 			if (outTM->tm_min==60){
 				outTM->tm_min=0;
 			}
 		}
 		else if (BTN==UPLong_btn){
 			outTM->tm_min+=5;
 			if (outTM->tm_min>59){
 				outTM->tm_min=59;
 			}
 		}
 		else if (BTN==DOWNLong_btn){
 			outTM->tm_min-=5;
 			if (outTM->tm_min<0){
 				outTM->tm_min=0;
 			}
 		}
 		else if (BTN==ENTER_btn){
 			tm_state=TMmenu_valuePreSet;
 			/* Ïðèïèíèëè áëèìàòè*/
 			memcpy (strLow0_menu, strLow_menu, 16);
 		}
		else{}
 	}
	else{}
	if (/* ïðè íàòèñêàíí³ áóäü ÿêî¿ êëàâ³ø³ */
		(BTN==DOWNLong_btn)||
		(BTN==UPLong_btn)  ||
		(BTN==UP_btn)      ||
		(BTN==DOWN_btn)
		){
		/* ï³äãîòóâàëè äî âèâîäó íîâå çíà÷åííÿ ÐÐÐÐ.ÌÌ.ÄÄ ÃÃ.ÕÕ*/
		 	snprintf((char*)strLow_menu, 17, "%4d/%02d/%02d %02d:%02d",
		 			outTM->tm_year+1900,
					outTM->tm_mon+1,
					outTM->tm_mday,
					outTM->tm_hour,
					outTM->tm_min);
	}


	if (
			(tm_state==TMmenu_settingYYYY) 		||
			(tm_state==TMmenu_settingMM)   		||
			(tm_state==TMmenu_settingDD)   		||
			(tm_state==TMmenu_settingHH)   		||
			(tm_state==TMmenu_settingMinute)	||
			(tm_state==TMmenu_valuePreSet)
			){
 		/* blinking low string value                    */
 		uint32_t blinkDur = HAL_GetTick()-tmmenuTick;
 		if (blinkDur<500){
 			tkDisplay_PutLowStr(strLow0_menu);
 		}
 		else if ((blinkDur>=500)&&(blinkDur<1000)){
 			tkDisplay_PutLowStr(strLow_menu);
 		}
 		else {
 			tmmenuTick=HAL_GetTick();
 		}
	}
	return tm_state;
}

