/*
 * tkFreq.c
 *  Created on: 27 груд. 2018 р.
 *      Author: KozakVF
 */
#include "tkConfig.h"
#include "math.h"
#if defined GRD_FURIE || defined FURIE_TEST
#include <tkGrid.h>
#include "stdint.h"
#include "gpio.h"
#include "tkSGL.h"
#include "adc.h"
#include "tim.h"
#include "float.h"
#include "tkFurie.h"
#include "wk_atime64.h"
#include "string.h"
#include "tk_wh1602b3.h"

#include <vkButtonsADC.h>
#ifdef 	DIFFERNCIAL_CURRENT_TRANSFORMER
#endif	//DIFFERNCIAL_CURRENT_TRANSFORMER



//float ph[100]={0};
//float phd[100] = {0};
//uint16_t i_phd=0;

#define U_KOEFF0  0.17065 /* 50Гц, амплітуда 2048 получаем 100.0v*/

/**********************************************************************************************
 * ГЛОБАЛЬНІ ЗМІННІ. ВХІДНІ ДЛЯ МОДУЛЯ
 **********************************************************************************************/
volatile uint16_t peru16 = 2000;
/* Відліки АЦП для Фур'є */
volatile adcf_t uFurieC[N_FURIE]={0};
volatile adcf_t iFurieA[N_FURIE]={0};
volatile adcf_t iFurieB[N_FURIE]={0};
volatile adcf_t iFurieC[N_FURIE]={0};
volatile adcf_t iFurieLeak[N_FURIE]={0};

/* Оголошені масиви для зберігання відліків струмів та напруги
 * Вводяться у файлі stm32f1_it.c (при відлагодженні - mdGRD_test.с
 * використовуються лише у цьому файлі */
volatile adcu16_t iA[240]={0};
volatile adcu16_t iB[240]={0};
volatile adcu16_t iC[240]={0};
volatile adcf_t   uC[240]={0};

volatile adcu16_t iLeak[240]={0};



/**********************************************************************************************
 * ГЛОБАЛЬНІ ЗМІННІ. ВИХІДНІ ДЛЯ МОДУЛЯ
 **********************************************************************************************/


/* Зглажена частота в герцах 	  */
volatile hzf_t 		Frequency 	= 50.0;
/* Зглажена напруга в вольтах*/
volatile voltf_t 	Voltage		=220.0;
volatile amperf_t 	aAmperage	=0.0;
volatile amperf_t 	bAmperage	=0.0;
volatile amperf_t 	cAmperage	=0.0;

/* Зглажені основні коєфіцієнти ефективності мережі  */
procentf_t PowerFactor =100.0;
procentf_t UnbalanceDeratingFactor=100.0;
procentf_t HarmonicsDeratingFactor=100.0;

//procentf_t LeakDeratingFactor=100.0;
//procentf_t GrundDeratingFactor=100.0;
//procentf_t NeutralDeratingFactor=100.0;

/* Зглажені інтегральні показники ефективності мережі */
procentf_t HmUnbDerating =100.0;
procentf_t fullDerating =100.0;
/* Показники енергоспоживання                               */
vatt_t	ActivePower 		= 	0;
vatt_t	ReactivePower 		=   0;
vatt_t	UnbalanceLostPower 	=	0;
vatt_t	HarmonicLostPower 	= 	0;
vatt_t	NeutralHarmonicLostPower 	= 	0;
vatt_t	NegativeHarmonicLostPower 	= 	0;
/**********************************************************************************************
 * ЛОКАЛЬНІ ЗМІННІ
 **********************************************************************************************/
/*Зглажені параметри ефективності мережі 					*/
/*Коефіцієнт нелінійних спотворень по струму у кожній фазі  */
procentf_t aTotalHarmonicDistortion = 0;
procentf_t bTotalHarmonicDistortion = 0;
procentf_t cTotalHarmonicDistortion = 0;

/************************************************************************
Коефіцієнт нелінійних спотворень по струму узагальнений    				*/
static procentf_t THDi_HarmonicAmperageDistortion = 0;
/*THDі вказує на ступінь спотворення форми синусоїди струму.
THDi менше 10% вважається нормальним,
ризик несправностей обладнання відсутній, перегрівання до 6%.

THDi між 10% і 50% вказує на значне гармонійне спотворення.
Падіння коефіцієнта потужності елекродвигунів, підвищення температури,
що означає, що кабелі та установок повинні бути понадгабаритними.

THDi, що перевищує 50%, вказує на високі гармонічні спотворення.
Високий ризик виходу обладнання з ладу. Необхідний поглиблений аналіз причин
 і заходи з їх усунення, зокрема рекомендується
 система придушення несинусоїдальнусті струму.

Скорочення терміну служби обладнання:
(Дані Канадської асоціації електриків).
Коли спотворення напруги живлення знаходиться в діапазоні 10%,
термін служби обладнання значно скорочується.
Залежно від типу пристрою, скорочення терміну служби:
32,5% для однофазних електродвигунів
18% для трифазних електродвигунів
5% для трансформаторів.
***************************************************************************
3, 5, 7, 11 and 13 - найбільше небезпечні для електродвигунів
*/


/* Коефiцієнт перекосу фаз 									*/
volatile procentf_t AmperageUnbalance = 0;

/* Процент струму, що витікає на землю, ключаючи як фазу на землю,
 * так і нульову послідовність гармонік */
volatile static procentf_t LeakAmperageDistortion 		= 0;

/* Процент струму, що витікає на землю
 * через замикання на                   */
volatile static procentf_t GrundAmperageDistortion 		= 0;

/* Процент струму, що витікає на землю
 * через нульову послідовність гармонік */
volatile static procentf_t NeutralAmperageDistortion 	= 0;
volatile static procentf_t NegativeAmperageDistortion 	= 0;
/**/




/*Струм у попугаях та фаза в градусах (незглажені)
 * обчислені у кожному циклі контролера */
volatile static ifurie_out_t iAfurie_out = {0};
volatile static ifurie_out_t iBfurie_out = {0};
volatile static ifurie_out_t iCfurie_out = {0};
volatile ufurie_out_t uCfurie_out = {0};

volatile iheat_out_t  iAheat_out = {0};
volatile iheat_out_t  iBheat_out = {0};
volatile iheat_out_t  iCheat_out = {0};


volatile static ifurie_out_t iLEAKfurie_out = {0};
volatile static iheat_out_t  iLEAKheat_out = {0};


/* Зглажені значення нулів АЦП для всіх струмів */
volatile static adcf_t aAdc0Amperage=2048;
volatile static adcf_t bAdc0Amperage=2048;
volatile static adcf_t cAdc0Amperage=2048;
volatile static adcf_t leakAdc0Amperage=2048;

/* Зглажені теплові середньоквадратичні струми фаз в попугаях */
volatile static adcf_t aAdcAmperage=0;  /* 362.0*/
volatile static adcf_t bAdcAmperage=0;
volatile static adcf_t cAdcAmperage=0;
volatile static adcf_t leakAdcAmperage=0;
volatile static adcf_t grundAdcAmperage=0;
volatile static adcf_t neutralAdcAmperage=0;
volatile static adcf_t negativeAdcAmperage=0;

volatile static adcf_t cAdcVoltage =100.0;  /* */

/* Зглажені середньоквадратичні струми основної (першої) гармоніки фаз в попугаях */
volatile static adcf_t aAdc1Amperage=0;  /* 362.0*/
volatile static adcf_t bAdc1Amperage=0;
volatile static adcf_t cAdc1Amperage=0;
volatile static adcf_t leakAdc1Amperage=0;
/*****************************************************************************/
/* зглажені кути в градусах                                                  */
static gradf_t u_i_Phi =0;
static gradf_t a_b_Phi =120.0;
static gradf_t b_c_Phi =120.0;
static gradf_t c_a_Phi =120.0;
/* УСТАВКИ - ЗАВОДСЬКІ КАЛІБРОВКИ*/
/* При амплітуді основної гармоніки напруги 2048 папуг АЦП струм унормовано до 100.0
 * ця калібровка потрібна, для врахування особливостей заліза						*/
static float u_clbr = 2.20f;
static float iA_clbr = 1.0f;
static float iB_clbr = 1.0f;
static float iC_clbr = 1.0f;
/* УСТАВКИ, ПРИВЕДЕНІ ДО папуг АЦП */
/* Номінативний струм системи в папугах АЦП  */
volatile static adcf_t ratedAdcAmperage =  362.0;
/* Мінімальний струм, % до номінативного, для якого обчислюємо витоки */
volatile static procentf_t lLimitRms =   7.0;
/* Допустимый процент тока утечки по отношению к среднеквадратическому в данный момент току
В разі використання диференційного трансформатора (УЗО)*/
#ifdef DIFFERNCIAL_CURRENT_TRANSFORMER
volatile static procentf_t hLimitLeak =  5.0;
#endif
#ifndef DIFFERNCIAL_CURRENT_TRANSFORMER
volatile static procentf_t hLimitLeak =  15.0;
#endif
/* Повідомлення про граничну напругу */
volatile static adcf_t lLimitVoltage =  60.0;
volatile static adcf_t hLimitVoltage =  150.0;
/* Повідомлення про витік струму на землю
 * 1) В разі використання диференційного трансформатора (УЗО)*/


/*********************************************************************************************
 * Змінні допоміжні для організації обчислень
 *********************************************************************************************/

/* допоміжні змінні для обчислень */

static uint32_t fr_counter=0;
static float oldPhase = 0;

static float PhaseDiff = 0;
volatile static float currentPhaseDiff =FLT_MIN;
/* Змінна для фіксації зміни частоти мережі, а отже і частоти ТІМ7*/
 static uint16_t old_peru16=2000;
/* Змінні для відлагоджування/тестування вимірювання частоти */
static volatile float phd[100]={0};
static volatile uint32_t i_phd=0;
static volatile float ph[100]={0};
static volatile uint32_t j_phd=0;
/* Змінні для засікання часу/тривалості при відлагоджуванні */
static uint32_t iclk=0;
static volatile uint32_t iDuration =0;

/* змінні для оцінки тривалості процедур при відлагоджуванні */
uint32_t grd_tick =0;
uint32_t grd_ti =0;
volatile uint32_t grd_duration=0;
volatile uint32_t grd_mins=0;
volatile uint32_t phd_duration=0;
uint32_t grd_phd = 0;
//static uint32_t uclk=0;
//static uint32_t uDuration =0;
//volatile static irms[200]={0};
//volatile static iphase[200]={0};

//uint16_t i200 =0;
/******************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 * ****************************************************************************************/
static grid_status_t f_GRID_Frequency_Calc(void);
static grid_status_t f_GRID_Voltage_Calc(void);
static void f_GRID_Amperage_Calc(void);
static grid_status_t f_GRID_PowerFactor_Calc(void);
static void f_GRID_HarmonicsFactor_Calc(void);
static grid_status_t f_GRID_UnBalanceFactor_Calc(adcf_t iAsgl, adcf_t iBsgl, adcf_t iCsgl);
static grid_status_t f_GRID_Leak_Calc(void);

/******************************************************************************************
 * GLOBAL FUNCTIONS DEFINITIONS
 * ****************************************************************************************/
void f_GRD_Init(void){
	memset ((void*)&uFurieC,0,sizeof(uFurieC));
	memset ((void*)&iFurieA,0,sizeof(iFurieA));
	memset ((void*)&iFurieB,0,sizeof(iFurieB));
	memset ((void*)&iFurieC,0,sizeof(iFurieC));
	memset ((void*)&iFurieLeak,0,sizeof(iFurieLeak));

	memset ((void*)&uC,0,sizeof(uC));
	memset ((void*)&iA,0,sizeof(iA));
	memset ((void*)&iB,0,sizeof(iB));
	memset ((void*)&iC,0,sizeof(iC));
	memset ((void*)&iLeak,0,sizeof(iLeak));

	MX_TIM7_Init();
	/* При ініціалізації генеруємо масив cинусів для прискорення обчислень */
	iclk=HAL_GetTick();
	f_uiFurie_Init();
	f_GRID_Power_Init();
}


uint32_t f_GRD_Calc(void){
	volatile uint32_t grid_event_status = 0x0000;
	volatile grid_status_t grid_calc_status = GRID_Unknown_Err;

	grd_tick=HAL_GetTick();
	/* частота відліків для TIM7 АЦП напруги/струму для Фур'є":*/
	float TIM7_Freq=N_FURIE*Frequency;
	/* де Ne 		- кількість відліків за період, файл 'tkConfig.h'
	 *    Frequency - поточна виміряна частота мережі, файл 'tkGrid.c' */
	float perf=1800000/TIM7_Freq;
	peru16=(uint16_t)perf;
	if (peru16 != old_peru16){
		MX_TIM7_Init();
	}

	grid_calc_status =f_GRID_Frequency_Calc();
	grid_event_status = (uint32_t)grid_calc_status;
	grid_calc_status =f_GRID_Voltage_Calc();
	grid_event_status |= (uint32_t)grid_calc_status;

//	Voltage=u_clbr*cAdcVoltage;

	f_GRID_Amperage_Calc();

	grid_calc_status=f_GRID_PowerFactor_Calc();
	grid_event_status |= (uint32_t)grid_calc_status;

	f_GRID_HarmonicsFactor_Calc();
	grid_calc_status = f_GRID_UnBalanceFactor_Calc (
							aAdcAmperage,
							bAdcAmperage,
							cAdcAmperage);
	grid_event_status |= (uint32_t)grid_calc_status;

	grid_calc_status = f_GRID_Leak_Calc();
	grid_event_status |= (uint32_t)grid_calc_status;

	fullDerating  = (HarmonicsDeratingFactor*UnbalanceDeratingFactor/100.0)*PowerFactor/100.0;

	grd_duration = HAL_GetTick()- grd_tick;

	grd_duration = HAL_GetTick()- grd_tick;


	memset ((void*)&uFurieC,0,sizeof(uFurieC));
	memset ((void*)&iFurieA,0,sizeof(iFurieA));
	memset ((void*)&iFurieB,0,sizeof(iFurieB));
	memset ((void*)&iFurieC,0,sizeof(iFurieC));
	memset ((void*)&iFurieLeak,0,sizeof(iFurieLeak));

	memset ((void*)&uC,0,sizeof(uC));
	memset ((void*)&iA,0,sizeof(iA));
	memset ((void*)&iB,0,sizeof(iB));
	memset ((void*)&iC,0,sizeof(iC));
	memset ((void*)&iLeak,0,sizeof(iLeak));

	return grid_event_status;
}

/******************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 * ****************************************************************************************/
/* За результатами Фурьє-перетворення для масиву відліків напруги (20..200 відліків),
 * яким є
 * .rms - умовне середньоквадратичне значення напруги  та
 * .phase - фаза у градусах основної гармоніки напруги
 * уточнюємо частоту мережі енергопостачання
 * та обчислюєму напругу в вольтах */

static grid_status_t f_GRID_Frequency_Calc(void){
//	ufurie_out_t ufurie_out = {FLT_MIN};
//	iclk = HAL_GetTick();
	grid_status_t fc_status = GRID_Ok;
	memset ((void*)&uCfurie_out,0,sizeof(uCfurie_out));

	f_ufurie(uFurieC, &uCfurie_out);
	iDuration=HAL_GetTick()-iclk;
	if (fr_counter>20){/* починаємо уточнювати частоту після певного часу */
		/* Форматуємо значення фази 0..360 */
		if (uCfurie_out.u_phase<0){uCfurie_out.u_phase = uCfurie_out.u_phase +360.0;}
		ph[i_phd]=uCfurie_out.u_phase;
		/*різниця фази поточної та попередньої за п'ять періодів напруги   */
		currentPhaseDiff=uCfurie_out.u_phase-oldPhase;
		/*Форматуємо різницю фази -180..+180   */
		if (currentPhaseDiff>180.0f){currentPhaseDiff-=360.0f;}
		if (currentPhaseDiff<-180.0f){currentPhaseDiff+=360.0f;}
			phd[i_phd]=currentPhaseDiff;
			/* Середньоквадратичне відхилення */

			i_phd++;
			j_phd++;
			if ((i_phd%100)==0){
				i_phd=0;
				if ((j_phd%200)==0){
					j_phd=0;
//					HAL_Delay(10);
					/* Розрахунок середньоквадратичного відхилення */
					float sum_phdxphd=0;
					for (uint8_t i=0;i<100;i++){
						float dDiff=PhaseDiff-phd[i_phd];
						sum_phdxphd+=dDiff*dDiff;
					} /* 0.5 ... 0.8 град.*/
					volatile float diffStandardDeviation = sqrtf(sum_phdxphd/100);
					/**/
					phd_duration=HAL_GetTick()-grd_phd;
					grd_phd = HAL_GetTick();
				}
				memset ((void*)&ph,0,sizeof(ph));
				memset ((void*)&phd,0,sizeof(phd));
			}
			/*(100.0+newPhaseDiff*100)/360.0) тривалість у мілісекундах п'яти періодів напруги */

			float newPhaseDiff =0;
			f_ExponentialSmoothing  (
							currentPhaseDiff,
							PhaseDiff,
							20.0,
						   	10.0,
							&newPhaseDiff
						   	);
			PhaseDiff=newPhaseDiff;


			if (fabsf(PhaseDiff) < 144.0){
					float oneperiodPhaseDiff = PhaseDiff/5.0;
					float jhy = (360.0+oneperiodPhaseDiff)/360.0;
					Frequency = 50.0 * jhy;
					fc_status=GRID_Ok;
			}
			else if (PhaseDiff>144){/*46hz=144grad*/
				/* Аварія частоти   */
				fc_status=GRID_UnderFrequency46;
			}
			else if (PhaseDiff<-144){/*54hz = 144grad*/
				fc_status=GRID_OverFrequency54;
			}
			else{}
	}
	fr_counter++;
//	if ((fr_counter % 200)==0){
//		HAL_Delay(10);
//	}
	oldPhase=uCfurie_out.u_phase;
	iDuration=HAL_GetTick()-iclk;

	return fc_status;
}
/* Кінець вимірювання частоти											*/
static grid_status_t f_GRID_Voltage_Calc(void){
	adcf_t newAdcVoltage=FLT_MIN;
	uCfurie_out.u1harm_ampl=uCfurie_out.u1harm_ampl*U_KOEFF0;
	f_ExponentialSmoothing  (
							uCfurie_out.u1harm_ampl,
							cAdcVoltage,
			    			20.0,
			   				10.0,
							&newAdcVoltage
			   				);
	/* cAdcVoltage = 100v якщо амплітудуда півперіоду напруги 2048 */
	cAdcVoltage=newAdcVoltage;
	Voltage = cAdcVoltage * u_clbr;
	if (cAdcVoltage>hLimitVoltage){return GRID_OverVoltage;}
	else if (cAdcVoltage<lLimitVoltage){return GRID_UnderVoltage;}
	else{return GRID_Ok;}

}


static void f_GRID_Amperage_Calc(void){
	/* Обробляємо масиви відліків 18 */
	memset ((void*)(&iAfurie_out),0,sizeof(iAfurie_out));
	f_ifurie(	iFurieA,   &iAfurie_out);

	memset ((void*)(&iBfurie_out),0,sizeof(iBfurie_out));
	f_ifurie(	iFurieB,   &iBfurie_out);

	memset ((void*)(&iCfurie_out),0,sizeof(iCfurie_out));
	f_ifurie(	iFurieC,   &iCfurie_out);

	memset ((void*)(&iFurieLeak),0,sizeof(iFurieLeak));

#ifndef	DIFFERNCIAL_CURRENT_TRANSFORMER
	for (uint8_t i=0;i<N_FURIE;i++){
		iFurieLeak[i]=  (iFurieA[i]-aAdc0Amperage)
				       +(iFurieB[i]-bAdc0Amperage)
					   +(iFurieC[i]-cAdc0Amperage);
	}
#endif //DIFFERNCIAL_CURRENT_TRANSFORMER
	/* находимо середньоквадратичне значення струму витоку
	 * першої гармоніки */
	f_ifurie(	iFurieLeak,&iLEAKfurie_out);

	/* Обробляємо масиви відліків 200 */
	float Nh = 	(uint16_t)(1.0/(Frequency*0.1*0.001));
	memset ((void*)(&iAheat_out),0,sizeof(iAheat_out));
	f_iheat(	iA,
				aAdc0Amperage,
				Nh,
				&iAheat_out);

	memset ((void*)(&iBheat_out),0,sizeof(iBheat_out));
	f_iheat(	iB,
			    bAdc0Amperage,
				Nh,
				&iBheat_out);

	memset ((void*)(&iCheat_out),0,sizeof(iCheat_out));
	f_iheat(	iC,
				cAdc0Amperage,
				Nh,
				&iCheat_out);

	memset ((void*)(&iLEAKheat_out),0,sizeof(iLEAKheat_out));
	/* Обчислюємо миттєві значення теплового струму, що витікає на землю,
	 *  як суму миттєвих значень лінійних струмів фаз*/
	/* Обчислюємо миттєві значення теплового струму, що витікає на землю,
	 *  як суму миттєвих значень лінійних струмів фаз*/
	memset ((void*)(&iLeak),0,sizeof(iLeak));

	for (uint8_t i=0;i<240;i++){
		iLeak[i]=    (iA[i]-(uint16_t)aAdc0Amperage)
				    +(iB[i]-(uint16_t)bAdc0Amperage)
					+(iC[i]-(uint16_t)cAdc0Amperage)+
					+((aAdc0Amperage	+bAdc0Amperage +cAdc0Amperage)/3);
	}
	f_iheat(
			    iLeak,
				leakAdc0Amperage,
				Nh,
				&iLEAKheat_out);


	/* Зглажуємо нулі струму в попугаях для всіх АЦП  */
	float newSglPermComp=FLT_MIN;
	f_ExponentialSmoothing  (
								iAheat_out.permanent_component,
					   			aAdc0Amperage,
					    		20.0,
					   			10.0,
								&newSglPermComp
					   			);
	aAdc0Amperage=newSglPermComp;
	newSglPermComp=0;
	f_ExponentialSmoothing  (
								iBheat_out.permanent_component,
					   			bAdc0Amperage,
					    		20.0,
					   			10.0,
								&newSglPermComp
					   			);
	bAdc0Amperage=newSglPermComp;
	newSglPermComp=0;
	f_ExponentialSmoothing  (
								iCheat_out.permanent_component,
					   			cAdc0Amperage,
					    		20.0,
					   			10.0,
								&newSglPermComp
					   			);
	cAdc0Amperage=newSglPermComp;



#ifdef DIFFERNCIAL_CURRENT_TRANSFORMER
	newSglPermComp=0;
	f_ExponentialSmoothing  (
								iLEAKheat_out.permanent_component,
					   			leakAdc0Amperage,
					    		20.0,
					   			10.0,
								&newSglPermComp
					   			);
	leakAdc0Amperage=newSglPermComp;
#endif //DIFFERNCIAL_CURRENT_TRANSFORMER
	/* Обчислюємо зглажені теплові струми в попугаях */
	float newAmperage=FLT_MIN;
	f_ExponentialSmoothing  (
								iAheat_out.rms_heat,
					   			aAdcAmperage,
					    		20.0,
					   			10.0,
								&newAmperage
					   			);
	aAdcAmperage=newAmperage;

	newAmperage=0;
	f_ExponentialSmoothing  (
								iBheat_out.rms_heat,
					   			bAdcAmperage,
					    		20.0,
					   			10.0,
								&newAmperage
					   			);
	bAdcAmperage=newAmperage;

	newAmperage=0;
	f_ExponentialSmoothing  (
								iCheat_out.rms_heat,
					   			cAdcAmperage,
					    		20.0,
					   			10.0,
								&newAmperage
					   			);
	cAdcAmperage=newAmperage;


	newAmperage=0;
	f_ExponentialSmoothing  (
								iLEAKheat_out.rms_heat,
					   			leakAdcAmperage,
					    		20.0,
					   			10.0,
								&newAmperage
					   			);
	leakAdcAmperage=newAmperage;

	aAmperage	= iA_clbr*aAdcAmperage;
	bAmperage	= iB_clbr*bAdcAmperage;
	cAmperage	= iC_clbr*bAdcAmperage;

}


//float iph[200]={0};
static grid_status_t f_GRID_PowerFactor_Calc(void){
//	grid_status_t pfc_status=GRID_Unknown_Err;
	/* Аналіз фаз можливий, коли амплітуди напруги і струмів нижче заданих */
	if ((uCfurie_out.u1harm_ampl<lLimitRms) ||
			(iCfurie_out.rms_fundamental<lLimitRms*ratedAdcAmperage/100)){
		return GRID_Calc_Impossible;
	}

	/* Фази -180..+180 град перетворюємо в фази 0..360 град*/
	if (uCfurie_out.u_phase<0){uCfurie_out.u_phase=uCfurie_out.u_phase+360.0;}
	if (iAfurie_out.phase_fundamental<0){iAfurie_out.phase_fundamental=iAfurie_out.phase_fundamental+360.0;}
	if (iBfurie_out.phase_fundamental<0){iBfurie_out.phase_fundamental=iBfurie_out.phase_fundamental+360.0;}
	if (iCfurie_out.phase_fundamental<0){iCfurie_out.phase_fundamental=iCfurie_out.phase_fundamental+360.0;}

	gradf_t deltaPHI_ab = 0;
	deltaPHI_ab=iAfurie_out.phase_fundamental-iBfurie_out.phase_fundamental;
	if (deltaPHI_ab< -180.0){deltaPHI_ab=deltaPHI_ab+360.0;}
	if (deltaPHI_ab>  180.0){deltaPHI_ab=deltaPHI_ab-360.0;}

	gradf_t new_deltaPHI=FLT_MIN;
	new_deltaPHI=0;
	f_ExponentialSmoothing  (
								deltaPHI_ab,
								a_b_Phi,
				    			2.0,
				   				10.0,
								&new_deltaPHI
				   				);
	a_b_Phi=new_deltaPHI;


	gradf_t deltaPHI_bc = 0;
	deltaPHI_bc=iBfurie_out.phase_fundamental-iCfurie_out.phase_fundamental;
	if (deltaPHI_bc< -180.0){deltaPHI_bc=deltaPHI_bc+360.0;}
	if (deltaPHI_bc>  180.0){deltaPHI_bc=deltaPHI_bc-360.0;}

	new_deltaPHI=0;
	f_ExponentialSmoothing  (
								deltaPHI_bc,
								b_c_Phi,
				    			2.0,
				   				10.0,
								&new_deltaPHI
				   				);
	b_c_Phi=new_deltaPHI;


	gradf_t deltaPHI_ca = 0;
	deltaPHI_ca=iCfurie_out.phase_fundamental-iAfurie_out.phase_fundamental;
	if (deltaPHI_ca< -180.0){deltaPHI_ca=deltaPHI_ca+360.0;}
	if (deltaPHI_ca>  180.0){deltaPHI_ca=deltaPHI_ca-360.0;}

	new_deltaPHI=0;
	f_ExponentialSmoothing  (
								deltaPHI_ca,
								c_a_Phi,
				    			2.0,
				   				10.0,
								&new_deltaPHI
				   				);
	b_c_Phi=new_deltaPHI;


	if ((a_b_Phi<0)||(b_c_Phi<0)||(c_a_Phi<0)){
		return GRID_WrongPhaseRotation;
	}


	gradf_t deltaPHI_iCuC=0;
	deltaPHI_iCuC=uCfurie_out.u_phase-iCfurie_out.phase_fundamental;
	if (deltaPHI_iCuC< -180.0){deltaPHI_iCuC=deltaPHI_iCuC+360.0;}
	if (deltaPHI_iCuC>  180.0){deltaPHI_iCuC=deltaPHI_iCuC-360.0;}
//
//				ph[i_phd]=uCfurie_out.u_phase;
//				iph[i_phd] = iCfurie_out.phase_fundamental;
//				phd[i_phd]=delta_PHI;
//				i_phd++;
//
//
//				if ((i_phd % 100)==0){
//					HAL_Delay(10);
//					i_phd=0;
//				}
	procentf_t new_u_i_Phi=FLT_MIN;
	f_ExponentialSmoothing  (
								deltaPHI_iCuC,
								u_i_Phi,
				    			10.0,
				   				10.0,
								&new_u_i_Phi
				   				);
	u_i_Phi=new_u_i_Phi;
	if (u_i_Phi<0){u_i_Phi=0;}
	PowerFactor = 100.0*cosf(M_PI * u_i_Phi/180.0);

	return GRID_Ok;

}

static grid_status_t f_GRID_UnBalanceFactor_Calc(adcf_t iAsgl, adcf_t iBsgl, adcf_t iCsgl){
	grid_status_t ubf_status = GRID_Unknown_Err;
	float out_factor=0;
	/*NEMA (National Electric Manufacturers Associations of the
	USA) Std. (1993) - визначення коефіцієнта асиметрії:
	найбільше відхилення від середнього значення
	https://wp.kntu.ac.ir/tavakoli/pdf/Journal/j.ijepes.2010.12.003.pdf*/
	float iAv = (iAsgl+iBsgl+iCsgl)/3.0;
	float idmax= 0;
	idmax=fabsf(iAsgl-iAv);
	if (fabsf(iBsgl-iAv)>idmax){
		idmax=fabsf(iBsgl-iAv);
	}
	if (fabsf(iCsgl-iAv)>idmax){
		idmax=fabsf(iCsgl-iAv);
	}
	AmperageUnbalance=(idmax/iAv)*100.0;

	procentf_t voltage_unbalance=AmperageUnbalance/7.0;
	if (voltage_unbalance<=10.0){
		if ((voltage_unbalance >= 0) && (voltage_unbalance <= 0.5)){
			out_factor=1.0;
		}
		else if ((voltage_unbalance > 0.5) && (voltage_unbalance <= 1.0)){
			out_factor=1.0-0.02*(voltage_unbalance-0.5);
		}
		else if ((voltage_unbalance > 1.0) && (voltage_unbalance <= 2.0)){
			out_factor=0.99-0.03*(voltage_unbalance-1.0);
		}
		else if ((voltage_unbalance > 2.0) && (voltage_unbalance <=10.0)){
			out_factor=0.96-0.07*(voltage_unbalance-2.0);
		}
		else{out_factor=0;}
		UnbalanceDeratingFactor=100.0*out_factor;
		ubf_status = GRID_Ok;
	}
	else {
		UnbalanceDeratingFactor = 0;
		ubf_status = GRID_Phase_Loss;
	}

	return ubf_status;
}

static void f_GRID_HarmonicsFactor_Calc(void){
	/* Обчислюємо зглажені струми основної/першої гармоніки в в папугах */
	float new1Amperage=FLT_MIN;
	f_ExponentialSmoothing  (
								iAfurie_out.rms_fundamental,
					   			aAdc1Amperage,
					    		20.0,
					   			10.0,
								&new1Amperage
					   			);
	aAdc1Amperage=new1Amperage;
	f_ExponentialSmoothing  (
								iBfurie_out.rms_fundamental,
					   			bAdc1Amperage,
					    		20.0,
					   			10.0,
								&new1Amperage
					   			);
	bAdc1Amperage=new1Amperage;
	f_ExponentialSmoothing  (
								iCfurie_out.rms_fundamental,
					   			cAdc1Amperage,
					    		20.0,
					   			10.0,
								&new1Amperage
					   			);
	cAdc1Amperage=new1Amperage;

	/* зглажений струм витоку першої гармоніки */
	f_ExponentialSmoothing  (
								iLEAKfurie_out.rms_fundamental,
								leakAdc1Amperage,
						    	20.0,
						   		10.0,
								&new1Amperage
						   		);
	leakAdc1Amperage=new1Amperage;



	   /* Квадрат теплових струмів усіх гармонік як різниця між
	    * сумою квадратів реальних елементарних струмів і
	    * квадрат теплового струму основної гармоніки*/
//	   volatile float summa_i2harm = summa_Qnf-iD2;
	   /* THD_I на основі теплових струмів основної гармоніки і сумарного теплового струму */
	   float aD2 =   (aAdcAmperage*aAdcAmperage)
								   -(aAdc1Amperage*aAdc1Amperage);
	   /*THD -  безрозмірних величинах */
	   if (aD2<0){aD2=0;}
	   aTotalHarmonicDistortion=
			   M_SQRT1_2*sqrtf(aD2)/aAdc1Amperage;
	   /*********************************************************/
	   float bD2 =   (bAdcAmperage*bAdcAmperage)
								   -(bAdc1Amperage*bAdc1Amperage);
	   if (bD2<0){bD2=0;}
	   bTotalHarmonicDistortion=
			   M_SQRT1_2*sqrtf(bD2)/bAdc1Amperage;
	   /*********************************************************/
	   float cD2 =   (cAdcAmperage*cAdcAmperage)
								   -(cAdc1Amperage*cAdc1Amperage);
	   if (cD2<=0){cD2=0;}
	   cTotalHarmonicDistortion=
			   M_SQRT1_2*sqrtf(cD2)/cAdc1Amperage;
	   /*********************************************************/
	   float THDi = aTotalHarmonicDistortion;
	   if (THDi < bTotalHarmonicDistortion){THDi = bTotalHarmonicDistortion;}
	   if (THDi < cTotalHarmonicDistortion){THDi = cTotalHarmonicDistortion;}
	   /*The THD for the 25 lowest harmonic components of a
	   rectangular current is: 29% */

	   HarmonicsDeratingFactor = 100.0f/(1.0f+THDi*THDi);
	   THDi_HarmonicAmperageDistortion = THDi*100.0f;

//	   iLEAKfurie_out.rms_3
//	   return GRID_Ok;
}



static grid_status_t f_GRID_Leak_Calc(void){

	/* Витік струму перевіряємо лише тоді, коли робочий струм
	 * більший за деякий заданий мінімальний*/
	adcf_t iPHv = (aAdcAmperage+bAdcAmperage+cAdcAmperage)/3.0;
	if (iPHv < (ratedAdcAmperage*lLimitRms/100)){
		LeakAmperageDistortion 		= 0;
		GrundAmperageDistortion 	= 0;
		NeutralAmperageDistortion 	= 0;
		return GRID_GroundCalc_Impossible;
	}
	grid_status_t lc_status=GRID_Unknown_Err;

	/* Треті гармоніки усіх лінійних струмів сумуються,
	 * це струм яких витікає на нейтральний провідник */
	adcf_t iNeutral_out = iAfurie_out.rmsZero + iBfurie_out.rmsZero + iCfurie_out.rmsZero;
	/* Зглажений струм, що витікає на нейтраль */
	adcf_t newiNeutral=0;
	f_ExponentialSmoothing  (
							iNeutral_out,
							neutralAdcAmperage,
							20.0,
							10.0,
							&newiNeutral
							);
	neutralAdcAmperage=newiNeutral;

	/* Гармоніки негативної послідовності (3*к+2, тобто 2, 5, 8, 11, 14..
	 * лінійних струмів середнюється,
		 * це струм яких довбає електродвигуни          */
		adcf_t iNegative_out = 	  (iAfurie_out.rmsNegative
								+ iBfurie_out.rmsNegative
								+ iCfurie_out.rmsNegative)
								/ 3.0f;
		/* Зглажений струм, що довбає двигуни  */
		adcf_t newiNegative=0;
		f_ExponentialSmoothing  (
								iNegative_out,
								negativeAdcAmperage,
								20.0,
								10.0,
								&newiNegative
								);
		negativeAdcAmperage=newiNegative;

	/* leakAdcAmperage - середньоквадратичний тепловий струм витоку, обчислений
	 * суму квадратів 200 відліків струму  */
	LeakAmperageDistortion   =100.0*leakAdcAmperage/iPHv;

	/* neutralAdcAmperage сума у трьох фазах коренів із суми квадратів усіх гармонік, кратних трьом  */
	NeutralAmperageDistortion =100.0*neutralAdcAmperage/iPHv;
	NegativeAmperageDistortion=100.0*neutralAdcAmperage/iPHv;
/*  Струм на землю це весь струм витоку мінус струм на нейтраль
	leakAdcAmperage    - тепловий струм витоку, через суму квадратів сум 200 відліків
	neutralAdcAmperage - середньквадратичний струм суми кратних трьом гармонік
						 (сума сум третіх гармонік по кожній фазі рівна
					      сумі третіх гармонік фур'є масиву суми елементарних відліків)

	leakAdc1Amperage   - середньоквадратичний струм 1 гармоніки витоку фур'є масиву суми елементарних відліків
*/
	float D2 = leakAdcAmperage*leakAdcAmperage-neutralAdcAmperage*neutralAdcAmperage;
	if (D2>0){
		grundAdcAmperage=sqrtf (D2);
	}
	else {
		grundAdcAmperage=0;
	}

//	volatile static procentf_t LeakAmperageDistortion = 0;
//	volatile static procentf_t GrundAmperageDistortion = 0;
//	volatile static procentf_t NeutralAmperageDistortion = 0;
	if (grundAdcAmperage<leakAdc1Amperage){grundAdcAmperage=leakAdc1Amperage;}
	GrundAmperageDistortion = 100.0*grundAdcAmperage/iPHv;
	if ((GrundAmperageDistortion) > hLimitLeak){
		lc_status = GRID_Ground_Fault;
	}

	/* factors calculations */
//	if (LeakAmperageDistortion > 1.0f){
//	LeakDeratingFactor    =100.0/(1.0f+(LeakAmperageDistortion/100.0f)   *(LeakAmperageDistortion/100.0f));
//	}
//	else {
//		LeakDeratingFactor =100.0f;
//	}

//	if (NeutralAmperageDistortion > 1.0f){
//	NeutralDeratingFactor =100.0f/(1.0f+(NeutralAmperageDistortion/100.0f)*(NeutralAmperageDistortion/100.0f));
//	}
//	else {
//		NeutralDeratingFactor =100.0f;
//	}
//	if (GrundAmperageDistortion > 1.0f){
//	GrundDeratingFactor   =100.0f/(1.0f+(GrundAmperageDistortion/100.0f)  *(GrundAmperageDistortion/100.0f));
//	}
//	else {
//		GrundDeratingFactor =100.0f;
//	}

	return lc_status;
}

power_t WattSec_ElectricityConsumption = {0};
power_t kWh_ElectricityConsumption ={0};


uint32_t daylim = 0;
uint32_t daydur   = 0;
uint32_t daytick  = 0;
void f_GRID_Power_Init(void){
	daylim=24*60*60*1000;
	daytick=HAL_GetTick();

}
void f_GRID_Power(void){

	  /* Тимчасово для роботи з витратами електоренергії ввдедемо коефіцієнти,
	   * які пізніше поправимо для обчислення миттєвих, в сенсі протягом 100мс
	   * значень rms струмів та фазної напруги*/


	  amperf_t   Aamper = 		iA_clbr*iAheat_out.rms_heat;
	  amperf_t   Bamper = 		iB_clbr*iBheat_out.rms_heat;
	  amperf_t   Camper = 		iC_clbr*iCheat_out.rms_heat;
	  voltf_t    C_PHASEvolt =  u_clbr *uCfurie_out.u1harm_ampl;

	  /* Повна енергія, по суті, нікому не цікава,
	   * оскільки платять за використану активну,
	   * а подекуда - за використану реактивну   */
	  vatt_t	full_power =
			  (Aamper + Bamper + Camper) *C_PHASEvolt;

	  ActivePower = 			full_power*PowerFactor/100;
	  ReactivePower = 		full_power*sqrtf
			  	  	  	  	  	(1.0-(PowerFactor/100.0)*(PowerFactor/100.0));
	  UnbalanceLostPower =	ActivePower*((100.0-UnbalanceDeratingFactor)/100.0);

	  HarmonicLostPower = 	ActivePower*sqrtf
			                    (1.0-(HarmonicsDeratingFactor/100.0)
			  	  	  	  	  	  	*(HarmonicsDeratingFactor/100.0)
								);
	  float nAD=NeutralAmperageDistortion/100.0f;
	  float nhlp = 1.0/(1.0+nAD*nAD);
	  NeutralHarmonicLostPower=ActivePower*sqrtf(1.0-nhlp*nhlp);

	  nAD=NegativeAmperageDistortion/100.0f;
	  nhlp = 1.0/(1.0+nAD*nAD);
	  NegativeHarmonicLostPower=ActivePower*sqrtf(1.0-nhlp*nhlp);


	  /* Cчитаем ват-секунды */
	  WattSec_ElectricityConsumption.Active 	+=ActivePower 		* 0.1;
	  WattSec_ElectricityConsumption.Reactive 	+=ReactivePower 	* 0.1;
	  WattSec_ElectricityConsumption.Unbalance 	+=UnbalanceLostPower* 0.1;
	  WattSec_ElectricityConsumption.Harmonic 	+=HarmonicLostPower * 0.1;
	  WattSec_ElectricityConsumption.Neutral 	+=NeutralHarmonicLostPower* 0.1;
	  WattSec_ElectricityConsumption.Negative 	+=NegativeHarmonicLostPower* 0.1;

	  /* Раз на добу Cчитаем kWh */
	  if((HAL_GetTick()-daytick)==daylim){
		  kWh_ElectricityConsumption.Active  	+=WattSec_ElectricityConsumption.Active		/360000.0;
		  kWh_ElectricityConsumption.Reactive	+=WattSec_ElectricityConsumption.Reactive	/360000.0;
		  kWh_ElectricityConsumption.Unbalance	+=WattSec_ElectricityConsumption.Unbalance	/360000.0;
		  kWh_ElectricityConsumption.Harmonic	+=WattSec_ElectricityConsumption.Harmonic	/360000.0;
		  kWh_ElectricityConsumption.Neutral 	+=WattSec_ElectricityConsumption.Neutral	/360000.0;
		  kWh_ElectricityConsumption.Negative 	+=WattSec_ElectricityConsumption.Negative	/360000.0;

		  WattSec_ElectricityConsumption.Active   =0;
		  WattSec_ElectricityConsumption.Reactive =0;
		  WattSec_ElectricityConsumption.Unbalance=0;
		  WattSec_ElectricityConsumption.Harmonic =0;
		  WattSec_ElectricityConsumption.Neutral  =0;
		  WattSec_ElectricityConsumption.Negative =0;
	  }
	  /* Якщо кінець мысяця записуємо в архів..*/

 }
	/* сутки это 24*60*60 */
//http://www.mirusinternational.com/downloads/MIRUS-TP003-A-How%20Harmonics%20have%20Contributed%20to%20Many%20Power%20Factor%20Misconceptions.pdf

void f_GRID_RaportHR(uint16_t* grid16){
	/* Вимірювання:*/
//	 * 0		uint16_t, 0.01Гц		частоти від 46Гц до 54Гц,
	 grid16[0]=(uint16_t)(Frequency*100);
//	 * 1		uint16_t  В						напруги (фаза С),
	 grid16[1]=(uint16_t)(Voltage);
//	 * 234		uint16_t  0.1A, max 6553A середньоквадратичних струмів фаз АBC (теплових),
	 grid16[2]=(uint16_t)(aAmperage);
	 grid16[3]=(uint16_t)(bAmperage);
	 grid16[4]=(uint16_t)(cAmperage);
//	 * 567		uint16_t  0.1A 		середньоквадратичних струмів першої гармоніки фаз АБС.
	 grid16[5]=(uint16_t)(aAdc1Amperage);
	 grid16[6]=(uint16_t)(bAdc1Amperage);
	 grid16[7]=(uint16_t)(cAdc1Amperage);

//	 * 	Розрахунки:
//	 * 8		uint16_t  0.1кВт    активної потужності,
	 grid16[8]=(uint16_t)(ActivePower/100.0f);
//	 * 9		uint16_t  0.1кВт 	реактивної потужності та
	 grid16[9]=(uint16_t)(ReactivePower/100.0f);
//	 * 10		int16_t   %		    коефіцієнта потужності (сos fi),
	 grid16[10]=(uint16_t)PowerFactor;

	 //	 * 	12	uint8_t	  % 		коефіцієнт падіння потужності через асиметрію струмів,
//	 *  	uint8_t	  % 		на основі коефіцієнта асиметрії.
	 grid16[11]=((uint16_t)(UnbalanceDeratingFactor))*0x100
			 	 	 	 	 	 + (uint16_t)AmperageUnbalance;

//	 * 11		uint8_t   %			коефіцієнта падіння потужності через наявність гармонік струму,
//	 * 			uint8_t	  %         на основі коефіцієнта гармонік,
	 grid16[12]=((uint16_t)(HarmonicsDeratingFactor))*0x100
			 	 	 	 	 	 + (uint16_t)THDi_HarmonicAmperageDistortion;

	 /* Процент струму, що витікає на землю, ключаючи як фазу на землю,
	  * так і нульову послідовність гармонік */
	 grid16[13]=(uint16_t)LeakAmperageDistortion * 0x100
	 /* Процент струму, що витікає на землю
	  * через замикання на                   */
			 	 	 	 	 	 + (uint16_t)GrundAmperageDistortion;
	 /* Процент струму, що витікає на землю
	  * через нульову послідовність гармонік */

	 grid16[14]=(uint16_t)NeutralAmperageDistortion * 0x100
	 /* Процент струму гармонік, що довбає електродвигуни */
			 	 	 	 	 	 + (uint16_t)NegativeAmperageDistortion;
	 grid16[15]=0xFF00;


//	 *HarmonicsDeratingFactor=100.0;
//UnbalanceDeratingFactor=100.0;
//	 * 	Відомості по днях та місяцях про:
//	 * 0x10..0x2F	uint16_t 1..31    за поточний місяць
//	 *
//	 * Зчитування з ФЛЕШ
//	 * 0x30..0x70		4 х uint32_t 0..11(1..12) за місяць року
//	 *
//	 * 		кількість спожитої електроенергії (кВт.год),
//	 * 		кількість спожитої реактивної електроенергії (кВар.год),
//	 * 		кількість електроенергії, втраченої через наявність гармонік (кВар.год),
//	 * 		кількість електроенергії, втраченої на електроприводах через асиметрію (кВт.год).
//	 *
//	 * 	Виявлення:
//	 * 		0000 0001 напруги нижче допустимої,
//	 * 		0000 0010 та вище допустимої,
//	 * 		0000 0100 частота нижче допустимої,
//	 * 		0000 1000 частота вище допустимої,
//	 * 		0001 0000 пропадання фази,
//	 * 		0010 0000 неправильної послідовності фаз,
//	 * 		0100 0000 витоку струму на землю вище допустимого
//	 * 		1000 0000 резерв
//	 *
//
//	 *		Read Holding Registers 03
//	 *		Read Exception status  07
//	 *		Read Coils 01
//	 * */
}

void f_GRD_Print(void){
	uint16_t strFr[17];
							  /*0123456789012345*/
							  /*2019/02/15 11:55*/
	snprintf((char*)strFr, 17, "%4d/%2d/%2d %2d:%2d",
			p_wk64_tm->tm_year+1900,
			p_wk64_tm->tm_mon+1,
			p_wk64_tm->tm_mday,
			p_wk64_tm->tm_hour,
			p_wk64_tm->tm_min);


	WH1602b3_WriteHighString (&strFr);

	uint16_t D=(uint16_t)Voltage;
	uint16_t F=(uint16_t)Frequency;
	uint16_t f=(uint16_t)((Frequency-(float)F)*100);
	snprintf((char*)strFr, 17, "U=%03dv  f=%02d.%02d", D, F, f);

	WH1602b3_WriteLowString (&strFr);
}

#endif //GRD_FURIE
