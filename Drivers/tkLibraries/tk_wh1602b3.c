/*
 * lcd_wh1602b3.c
 *
 *  Created on: 7 лист. 2017 р.
 *      Author: KozakVF
 */
#include "tkConfig.h"
#ifdef WINSTAR_Display_WH1602B3
#include "tk_wh1602b3.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"


/*******************************************************************************************************************
 *******************************************************************************************************************
 *     P R I V A T E     D E F I N E S
 *******************************************************************************************************************
*/

#define WH1602b3_ADDR  				0x3C    /* i2c адреса дивплея wh1602b3								*/


#define WH1602B3_CMD				0x00     /* внутрішня адреса пам'яті WO1602G для запису команд 		*/
#define WH1602B3_DATA				0x40	 /* внутрішня адреса пам'яті WO1602G для запису даних  		*/
#define WH1602B3_SCREEN_ADDRESS 	0x80     /* внутрішня адреса початку знакомісць екрана 				*/


#define INIT_8BIT_MODE 				0x30
#define INIT_2x16_5x8_MODE 			0x38
#define TURN_OFF_LCD 				0x08
#define CLEAN_LCD 					0x01

#define TURN_ON_CURSOR_BLINKS 		0x0F
#define TURN_ON_NO_CURSOR			0x0C

#define CURSOR_SHIFT_RIGHT			0x14
#define DISPLAY_SHIFT_LEFT			0x18

static uint8_t WH1602b3_chCyrillic(uint8_t ch_Cyrillic);
static wh1602b3_status_t WH1602b3_WriteCmd(uint8_t cmd);
static wh1602b3_status_t WH1602b3_WriteData(uint8_t data);
//
//	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, GPIO_PIN_SET);				/* 	AVARIA_ZACHYST	 */
//	  HAL_Delay(1000);
//	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, GPIO_PIN_RESET);				/* 	AVARIA_ZACHYST	 */
//	  HAL_Delay(1000);
//	  return (0);
//}

wh1602b3_status_t WH1602b3_Init( void ){

//	 volatile uint8_t i2c_addr =0;
//	 i2c_addr=f_I2C_ScanBus();
	 volatile wh1602b3_status_t Err=0;
	 HAL_Delay (100);
	 Err = WH1602b3_WriteCmd (INIT_8BIT_MODE);
	 HAL_Delay (100);
	 Err = WH1602b3_WriteCmd (INIT_2x16_5x8_MODE);
	 HAL_Delay (100);
	 Err = WH1602b3_WriteCmd (TURN_OFF_LCD);
	 HAL_Delay (100);
	 Err = WH1602b3_WriteCmd (CLEAN_LCD);
	 HAL_Delay (100);
	 Err = WH1602b3_WriteCmd (TURN_ON_NO_CURSOR);
	 HAL_Delay (100);
	 Err = WH1602b3_WriteCmd (CURSOR_SHIFT_RIGHT);
//	 Err = WH1602b3_WriteCmd (DISPLAY_SHIFT_LEFT);

	 HAL_Delay (100);
	 return 0;
 }

 wh1602b3_status_t WH1602b3_WriteHighString(uint8_t *str){
// wh1602b3_status_t  WН1602b3_WriteHighString (uint8_t *str){

	 if (str==NULL) { return WH1602B3_NULL_Ptr;}
	 uint8_t sSize = strlen (str);
	 if (sSize>17) {
		 sSize=17;
		 return (WH1602b3_WriteSyllable(0,0,sSize,str));
	 }
	 else {
		 uint8_t strTemp[17]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
		 snprintf(strTemp, 17, "%s", str );
		 return (WH1602b3_WriteSyllable(0,0,17,strTemp));
	 }

 }

 wh1602b3_status_t WH1602b3_WriteLowString(uint8_t *str){

	 if (str==NULL) { return WH1602B3_NULL_Ptr;}
	 uint32_t sSize = strlen (str);
	 if (sSize>17) {
		 sSize=17;
		 return (WH1602b3_WriteSyllable(0,1,sSize,str));
	 }
	 else {
		 uint8_t strTemp[17]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
		 snprintf(strTemp, 17, "%s",str );
		 return (WH1602b3_WriteSyllable(0,1,17,strTemp));
	 }
 }


 wh1602b3_status_t WH1602b3_WriteSyllable(	uint8_t x,       	/* знакомісце, з якого починається друк, 0..15 	*/
		 	 	 	 	 	 	 	 	 	uint8_t y,  		/* рядок, 0 - верхній, 1 - нижній				*/
											uint8_t size, 		/* кількисть симолів для друку					*/
											uint8_t *syllable	/* показчик на рядок байт, які треба роздрукува */
											){
	 if (syllable == NULL){
		 return WH1602B3_NULL_Ptr;
	 }
	 if (x>15) {
		 return WH1602B3_WRONG_Arg;
	 }
	 if ((x+size)>16){
		 size=17-x;
	 }


	 WH1602b3_SetCursor(x, y);
	 for (uint8_t i=0;i<size;i++){
		 if ((syllable[i] != 0)
//				 ||	(syllable[i] != '\n')
				 ){
				 WH1602b3_WriteChar(syllable[i]);
		 }
	 }
	 return WH1602B3_OK;
 }
 /*******************************************************************************************************************
  *******************************************************************************************************************
  *     ДРУК ОДНОГО СИМВОЛА   WO1602G
  *******************************************************************************************************************
 Друкується один заданий символ на позиції, заданій функцією WO1602_SetCursor, або на позиції, справа від попередньо
  надрукованого символа
 Рекомендуємо використовувати  при необхідносты друкувати не весь рядок, а лише
 окремі позиції заданого рядка
 Якщо номер рядка і номер знакомісця задані некорекно, функція працює некоректно і повертає сигнал про помилку
 */
 wh1602b3_status_t WH1602b3_WriteChar(uint8_t ch) {
 	wh1602b3_status_t wh1602b3ch = WH1602B3_UNKNOWN_Err;
 	uint8_t char_wh1602b3 = WH1602b3_chCyrillic (ch);
 	wh1602b3ch=WH1602b3_WriteData(char_wh1602b3);
 	return (wh1602b3ch);
 }
 /*******************************************************************************************************************
  *******************************************************************************************************************
  *     ПЕРЕМІЩЕННЯ КУРСОРА НА ЗАДАНУ ПОЗИЦІЮ   WO1602G
  *******************************************************************************************************************
 Курсор переміщується на позицію у = 0 - верхнй рядок, х=0 - крайнє ліве знакомісце
 Екран не стирається
 Рекомендуємо використовувати спільно з функцією WO1602_WriteChar при необхыдносты друкувати не весь рядок, а лише
 окремі позиції заданого рядка
 Якщо номер рядка і номер знакомісця задані некорекно, функція працює некоректно і повертає сигнал про помилку
 */
 wh1602b3_status_t WH1602b3_SetCursor(uint8_t x, uint8_t y) {
 	wh1602b3_status_t wh1602sc = WH1602B3_UNKNOWN_Err;
 	if (y > 1) {
 		y = 1;
 		wh1602sc = WH1602B3_WRONG_Arg;

 	}
 	else if (x > 0x0F) {
 		x = 0x0F;/* можно до 63 (0х3F, але ж це не має практичного смислу, в рядку лише 16 символів 			*/
 		wh1602sc = WH1602B3_WRONG_Arg;
 	}
 	else {
 		wh1602sc = WH1602B3_OK;
 	}
 	wh1602b3_status_t wh1602sc1 = WH1602b3_WriteCmd(WH1602B3_SCREEN_ADDRESS | (y * 0x40) | x);
 	if ( wh1602sc1 != WH1602B3_OK){
 		wh1602sc= wh1602sc1;
 	}
 	return (wh1602sc);
 }
 /*******************************************************************************************************************
  *******************************************************************************************************************
  *     ОЧИСТИТИ ЕКРАН
  ******************************************************************************************************************/
 wh1602b3_status_t WH1602b3_ClearScreen (void) {
 	return (WH1602b3_WriteCmd(0x01));
 	HAL_Delay(1);
 }
 /*******************************************************************************************************************
  *******************************************************************************************************************
  *     ПЕРЕКОДУВАННЯ КИРИЛИЧНИХ СИМВОЛІВ
  *******************************************************************************************************************
  * Функція заміни кодів windows для кирилицы внутрішніми кодами  дисплея
  * Ніби є усі необхідні літери російські, з українськими складніше,
  * відсутнє внутрішнє кодування для  літери єЄ, поки що буде друкуватися еЕ,
  * є можливість створвати власні символи, але це - на майбутнє для бажаючих
 */

 static uint8_t WH1602b3_chCyrillic(uint8_t ch_Cyrillic){
 	uint8_t ch_wo1602g = 0x00;
   switch(ch_Cyrillic)
    {
     case 'А': ch_wo1602g = 'A'; break;
     case 'Б': ch_wo1602g = '\240';break;
     case 'В': ch_wo1602g = 'B';break;
     case 'Г': ch_wo1602g = '\241';break;
     case 'Д': ch_wo1602g = '\340';break;
     case 'Е': ch_wo1602g = 'E';break;
     case 'Ё': ch_wo1602g = '\242';break;
     case 'Ж': ch_wo1602g = '\243';break;
     case 'З': ch_wo1602g = '\244';break;
     case 'И': ch_wo1602g = '\245';break;
     case 'Й': ch_wo1602g = '\246';break;
     case 'К': ch_wo1602g = 'K';break;
     case 'Л': ch_wo1602g = '\247';break;
     case 'М': ch_wo1602g = 'M';break;
     case 'Н': ch_wo1602g = 'H';break;
     case 'О': ch_wo1602g = 'O';break;
     case 'П': ch_wo1602g = '\250';break;
     case 'Р': ch_wo1602g = 'P';break;
     case 'С': ch_wo1602g = 'C';break;
     case 'Т': ch_wo1602g = 'T';break;
     case 'У': ch_wo1602g = '\251';break;
     case 'Ф': ch_wo1602g = '\252';break;
     case 'Ч': ch_wo1602g = '\253';break;
     case 'Х': ch_wo1602g = 'X';break;
     case 'Ш': ch_wo1602g = '\254';break;//
     case 'Ц': ch_wo1602g = '\341';break;//
     case 'Щ': ch_wo1602g = '\342';break;//
     case 'Ы': ch_wo1602g = '\256';break;//
     case 'Ь': ch_wo1602g = 'b';break;
     case 'Ъ': ch_wo1602g = 0xAD;break;
     case 'Э': ch_wo1602g = 0xAF;break;
     case 'Ю': ch_wo1602g = '\260';break;
     case 'Я': ch_wo1602g = '\261';break;
     case 'а': ch_wo1602g = 'a';break;
     case 'б': ch_wo1602g = '\262';break;
     case 'в': ch_wo1602g = '\263';break;
     case 'г': ch_wo1602g = '\264';break;
     case 'д': ch_wo1602g = '\343';break;
     case 'е': ch_wo1602g = 'e';break;
     case 'ё': ch_wo1602g = '\265';break;
     case 'ж': ch_wo1602g = '\266';break;
     case 'з': ch_wo1602g = '\267';break;
     case 'и': ch_wo1602g = '\270';break;
     case 'й': ch_wo1602g = '\271';break;
     case 'к': ch_wo1602g = '\272';break;
     case 'л': ch_wo1602g = '\273';break;
     case 'м': ch_wo1602g = '\274';break;
     case 'н': ch_wo1602g = '\275';break;
     case 'о': ch_wo1602g = 'o';break;
     case 'п': ch_wo1602g = '\276';break;
     case 'р': ch_wo1602g = 'p';break;
     case 'с': ch_wo1602g = 'c';break;
     case 'т': ch_wo1602g = '\277';break;
     case 'у': ch_wo1602g = 'y';break;
     case 'ч': ch_wo1602g = '\300';break;
     case 'ф': ch_wo1602g = '\344';break;
     case 'ш': ch_wo1602g = '\301';break;
     case 'щ': ch_wo1602g = '\346';break;
     case 'х': ch_wo1602g = 'x';break;
     case 'ц': ch_wo1602g = '\345';break;
     case 'ъ': ch_wo1602g = '\302';break;
     case 'ы': ch_wo1602g = '\303';break;
     case 'ь': ch_wo1602g = '\304';break;
     case 'э': ch_wo1602g = '\305';break;
     case 'ю': ch_wo1602g = '\306';break;
     case 'я': ch_wo1602g = '\307';break;
     case 'і': ch_wo1602g = 'i';break;
     case 'І': ch_wo1602g = 'I';break;
     case 'ї': ch_wo1602g = 0x9F;break;
     case 'Ї': ch_wo1602g = 0x1F;break;
     case 'є': ch_wo1602g = 'e';break;
     case 'Є': ch_wo1602g = 'E';break;
     case '\n':ch_wo1602g = ' ';break;
//     case ' ': ch_wo1602g = 0x02;break;
     default : ch_wo1602g = ch_Cyrillic;
    }
   return (ch_wo1602g);
 }


 static wh1602b3_status_t WH1602b3_WriteCmd(uint8_t cmd) {
 	wh1602b3_status_t wh1602b3wc = WH1602B3_UNKNOWN_Err;
 	uint8_t writing_attempts = 0;
 	uint8_t busytest_attemts =0;
 	HAL_StatusTypeDef i2cmw = HAL_ERROR;
 	while (wh1602b3wc == WH1602B3_UNKNOWN_Err){

 		while  ((writing_attempts<10) && (i2cmw!= HAL_OK)){
 			i2cmw=HAL_I2C_IsDeviceReady(&hi2c1, WH1602b3_ADDR << 1, 1, 10);
 			if (i2cmw==HAL_BUSY){
 				i2c1BUSY_Doctor ();
 			}
 		}
 		if (i2cmw != HAL_OK){
 			wh1602b3wc=WH1602B3_BUSY; break;
 		}

 		do {
 		 i2cmw = HAL_I2C_Mem_Write(	&hi2c1,					/* показчик на хендлер I2c										*/
 				 	 	 	 	 	WH1602b3_ADDR << 1,		/* адреса i2c дисплея, зсунута на 1 вліво
 									 	 	 	 	 	 	 bit, що звільнився, заповнюєть HAL для команди читати/писати	*/
 									WH1602B3_CMD,			/* дефайн, адреса для запису команд 0х00						*/
 									I2C_MEMADD_SIZE_8BIT,	/* довжина команди в байтах = 1									*/
 									&cmd,					/* вхідний параметр, власне код інструкції, один байт			*/
 									1,						/* кількість байтів 											*/
   									100);					/* таймаут 														*/
 		 ++writing_attempts;

 		}
 		while  ((writing_attempts<10) && (i2cmw!= HAL_OK));
 		if (i2cmw == HAL_OK) {
 			wh1602b3wc = WH1602B3_OK;}
 		else {
 			wh1602b3wc = WH1602B3_Err;
 		}
 	}
 	delayUs(24);
 	return (wh1602b3wc);
 }
 /*******************************************************************************************************************
  *******************************************************************************************************************
  *     З А П И С    Д А Н И Х  У   WO1602G
  *******************************************************************************************************************

 */
 static wh1602b3_status_t WH1602b3_WriteData(uint8_t data) {
 	wh1602b3_status_t wh1602b3rd = WH1602B3_UNKNOWN_Err;
 	uint8_t wh_Data = data;
 	uint8_t writing_attempts = 0;
 	uint8_t busytest_attemts =0;
 	HAL_StatusTypeDef i2cmw = HAL_ERROR;
 	while (wh1602b3rd == WH1602B3_UNKNOWN_Err){

 		while  ((writing_attempts<10) && (i2cmw!= HAL_OK)){
 			i2cmw=HAL_I2C_IsDeviceReady(&hi2c1, WH1602b3_ADDR << 1, 1, 10);
 			if (i2cmw==HAL_BUSY){
 				i2c1BUSY_Doctor ();
 			}
 		}
 		if (i2cmw != HAL_OK){
 			wh1602b3rd=WH1602B3_BUSY; break;
 		}

 		do {
 		 i2cmw = HAL_I2C_Mem_Write(	&hi2c1,					/* показчик на хендлер I2c										*/
 				 	 	 	 	 	WH1602b3_ADDR << 1,		/* адреса i2c дисплея, зсунута на 1 вліво
 									 	 	 	 	 	 	 bit, що звільнився, заповнюєть HAL для команди читати/писати	*/
									WH1602B3_DATA,			/* дефайн, адреса для запису команд 0х00						*/
 									I2C_MEMADD_SIZE_8BIT,	/* довжина команди в байтах = 1									*/
 									&wh_Data,					/* вхідний параметр, власне код інструкції, один байт			*/
 									1,						/* кількість байтів 											*/
   									100);					/* таймаут 														*/
 		 ++writing_attempts;

 		}
 		while  ((writing_attempts<10) && (i2cmw!= HAL_OK));
 		if (i2cmw == HAL_OK) {
 			wh1602b3rd = WH1602B3_OK;}
 		else {
 			wh1602b3rd = WH1602B3_Err;
 		}
 	}
 	delayUs(24);
 	return (wh1602b3rd);
 }









 /**************************************************************************************************/
#endif //WINSTAR_Display_WH1602B3
