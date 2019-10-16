/*
 * mdGRD_test.c
 *
 *  Created on: 17 ñ³÷. 2019 ð.
 *      Author: KozakVF
 */
#include <tkGrid.h>
#include "tkConfig.h"
#include "tkFurie.h"
#include "stdint.h"
#include "string.h"
#include "math.h"
#include "mdGRD_test.h"
/* */

#ifdef FURIE_TEST

#define GRID_FREQ 50.0 /* Ôèçè÷åñêàÿ ÷àñòîòà ñåòè */
#define UI_PHI    37.0 /* Cos 37ãðàä = 0.799*/

uint16_t N18 = N_FURIE;

extern adcf_t uFurieC[N_FURIE];
extern adcf_t iFurieA[N_FURIE];
extern adcf_t iFurieB[N_FURIE];
extern adcf_t iFurieC[N_FURIE];
extern adcf_t iFurieLeak[N_FURIE];

extern adcu16_t iA[240];
extern adcu16_t iB[240];
extern adcu16_t iC[240];
extern adcu16_t iLeak[240];

static float Nif = 0.0;

static adcf_t A_amlitude = 		512.0;
static adcf_t B_amlitude = 		512.0;
static adcf_t C_amlitude = 		512.0;
static adcf_t Leak_amplitude =	64.0;
static float model_phase1 = 0;



static int f_uiModel18(
					hzf_t ui_msrfreq,
					gradf_t uPhase
					);
static int f_ifurie_Model200(gradf_t phaseGrad);


void f_furie_Model(hzf_t msr_freq){
	memset ((void*)(&uFurieC),0,sizeof(uFurieC));
	memset ((void*)(&iFurieA),0,sizeof(iFurieA));
	memset ((void*)(&iFurieB),0,sizeof(iFurieB));
	memset ((void*)(&iFurieC),0,sizeof(iFurieC));
	memset ((void*)(&iFurieLeak),0,sizeof(iFurieC));


	f_uiModel18( msr_freq, model_phase1);

	Nif = 1.0/(GRID_FREQ*0.1*0.001);
	Nif=(float)((uint16_t)Nif);


	memset ((void*)(&iA),0,sizeof(iA));
	memset ((void*)(&iB),0,sizeof(iB));
	memset ((void*)(&iC),0,sizeof(iC));
	memset ((void*)(&iLeak),0,sizeof(iC));
	f_ifurie_Model200(model_phase1);

	/* Ìîäåëü çñóâó ôàçè êîæåí îñíîâíèé öèêë êîíòðîëåðà 100ìñ */
	float PhaseDiff = 0;
	float rgck = (float)(1.0-(GRID_FREQ / 50.0));
	PhaseDiff=5.0*360.0*rgck;

	model_phase1 = model_phase1+PhaseDiff;
	if (model_phase1>=360.0){model_phase1=model_phase1-360.0;}
	if (model_phase1<0){model_phase1=model_phase1+360.0;}


}

static int f_uiModel18(
					hzf_t ui_msrfreq,
					gradf_t uPhase
					){


	float N18f = (float)N_FURIE;
	for (uint8_t n=0; n<N_FURIE; n=n+1){

		float nf = (float)n;

		float ut=2048.0*cosf(
//				2.0* M_PI*((GRID_FREQ/ui_msrfreq)*(nf/N18f))
//				2.0* M_PI*(nf/N18f)
//						    );


				2.0* M_PI*((GRID_FREQ/ui_msrfreq)*(nf/N18f)+(uPhase-240.0+UI_PHI)/360.0f)
						    );

		int16_t u16t = (int16_t)ut;
		ut = (float)u16t;

    	if (ut>0) {uFurieC[n]=ut;} else {uFurieC[n]=0.0;}
/**********************************************************************************************
 *    Ôàçà À
 * ********************************************************************************************
 */
		float it=0;
		uint16_t i16t = 0;
		it=2048.0+A_amlitude*cosf(2.0* M_PI*((GRID_FREQ/ui_msrfreq)*(nf/N18f)+uPhase/360.0));
//		if (cosf(2.0* M_PI*((GRID_FREQ/ui_msrfreq)*(nf/N18f)+uPhase/360.0))>0){
//			it = 2048.0+A_amlitude;
//		}
//		else {
//			it = 2048.0-A_amlitude;
//		}
		i16t=(uint16_t)it;

		if (i16t<2048){}
		else {i16t=i16t-1;}
		it = (float)i16t;
		iFurieA[n]=it;
/**********************************************************************************************
*    Ôàçà Â
* ********************************************************************************************
*/
		it=2048.0+B_amlitude*cosf(2.0f* M_PI*((GRID_FREQ/ui_msrfreq)*(nf/N18f)+(uPhase-120.0f)/360.0f));


		i16t = (uint16_t)it;
		if (it<2048){i16t-=1;}
		it = (float)i16t;
		iFurieB[n]=it;
/**********************************************************************************************
*    Ôàçà Ñ
* ********************************************************************************************
*/
		it=2048.0+C_amlitude*cosf(2.0* M_PI*((GRID_FREQ/ui_msrfreq)*(nf/N18f)+(uPhase-240.0)/360.0));


		i16t = (uint16_t)it;
		if (it<2048){i16t-=1;}
		it = (float)i16t;
		iFurieC[n]=it;
/**********************************************************************************************
*    Ôàçà Ñ
* ********************************************************************************************
*/
		it=2048.0+Leak_amplitude *cosf(2.0* M_PI*((GRID_FREQ/ui_msrfreq)*(nf/N18f)+(uPhase-120)/360.0));
		i16t = (uint16_t)it;
		if (it<2048){i16t-=1;}
		it = (float)i16t;
		iFurieLeak[n]=it;





	}
	return 0;
}

static int f_ifurie_Model200(gradf_t phaseGrad){

	/*  âñåãî îòñ÷åòîâ çà ïåðèîä 	*/

	for (uint8_t n=0; n<240; n=n+1){
		float nf=(float)n;
//		/***************************************************************
//		 * ÏÐßÌÎÊÓÒÍÈÉ ÑÒÐÓÌ A
//		 ***************************************************************/
//		if (cosf(2* M_PI*(t/(Ni*dt)+phaseGrad/360))>=0){
//			iA[n]=2048.0+A_amlitude;
//		}
//		else{
//			iA[n]=2048.0-A_amlitude;
//		}
//		/***************************************************************
//				 * ÏÐßÌÎÊÓÒÍÈÉ ÑÒÐÓÌ B
//				 ***************************************************************/
//				if (cosf(2* M_PI*(t/(Ni*dt)+(phaseGrad-120)/360))>=0){
//					iB[n]=2048.0+B_amlitude;
//				}
//				else{
//					iB[n]=2048.0-B_amlitude;
//				}
//				/***************************************************************
//						 * ÏÐßÌÎÊÓÒÍÈÉ ÑÒÐÓÌ C
//						 ***************************************************************/
//						if (cosf(2* M_PI*(t/(Ni*dt)+(phaseGrad-240)/360))>=0){
//							iC[n]=2048.0+C_amlitude;
//						}
//						else{
//							iC[n]=2048.0-C_amlitude;
//						}

		/***************************************************************
		 * ÑÈÍÓÑÎ¯ÄÀ
		 */
		/***************************************************************/
 		float iAcosf=A_amlitude*cosf(2.0* M_PI*(nf/Nif+phaseGrad/360.0));
 		int16_t ia16 =0;
 		ia16=2048.0+(int16_t)iAcosf;
// 		if (cosf(2.0* M_PI*(nf/Nif+phaseGrad/360.0))>0){
// 			ia16 =2048.0+A_amlitude;
// 		}
// 		else {
// 			ia16 =2048.0-A_amlitude;
// 		}
		iA[n]=(uint16_t)ia16;
		/***************************************************************/
		/***************************************************************/
 		float iBcosf=B_amlitude*cosf(2.0* M_PI*(nf/Nif+(phaseGrad-120.0)/360.0));
 		int16_t ib16 =2048+(int16_t)iBcosf;
		iB[n]=(uint16_t)ib16;
		/***************************************************************/
		/***************************************************************/
 		float iCcosf=C_amlitude*cosf(2.0* M_PI*(nf/Nif+(phaseGrad-240.0)/360.0));
 		int16_t ic16 =2048+(int16_t)iCcosf;
		iC[n]=(uint16_t)ic16;
		/***************************************************************/

		/***************************************************************/
 		float iLEAKcosf=Leak_amplitude*cosf(2.0* M_PI*(nf/Nif+(phaseGrad-120.0)/360.0));
 		int16_t ileak16 =2048+(int16_t)iLEAKcosf;
 		iLeak[n]=(uint16_t)ileak16;
		/***************************************************************/
	}
	return 0;
}

#endif //FURIE_TEST
