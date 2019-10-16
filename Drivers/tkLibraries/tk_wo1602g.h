/*
 * tk_wo1602g.h

 *  Created on: 14 ����. 2017 �.
 *  Author: Alexander Olenyev (alexandes.zp@gmail.com) & Volodymyr.Kozak (volodymyr@kozak@gmail.com)
 ******************************************************************************************************************/
/* ���˲����� ����ֲ� ��� ������� WO1602G
 *
 * ���������� �� ��, �����������  WO1602G, ������ ������ � ���������� �� I2C.
 * ���������� ����� ������:
 * 	������������ ��� 3� ����� (���.STM32),
 * 	������� ����������� �� ������ � ������� 5�8,
 * 	����� ����������� �� ��� �����,
 * 	��������� ������ ��������� ������ ���� ����� ��������� �����
 * ������������� ���� ���������� ������� �������� � ��������� ������, ��� ����� "��" (���� �� ��������� ��)
 * �������� (���.���������) �� ������� I2C �� ������ I2C slave ������ ��������� ��� ������������.
 * �������, ������ I2C, ������ ���������� - 0�3�, ��� ���������� ������ - �������������� ������� I2C_ScanBus();
 * �� ��������� ������ � ��������� �������� ���������.
 * */
#include "tkConfig.h"
#ifdef WINSTAR_Display_WO1602G

#ifndef TK_WO1602G_H_
#define TK_WO1602G_H_

#include "stdint.h"
#include "i2c.h"

// LCD Commands
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define WO1602G_CGRAM_ADDR      0x40
#define WO1602G_DDRAM_ADDR      0x80

// flags for display on/off and cursor control
// ---------------------------------------------------------------------------
#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00


/*******************************************************************************************************************
 *******************************************************************************************************************
 *     P U B L I C    V A R I A B L E S
 *******************************************************************************************************************
*/

typedef enum {                   	/* ������, ���� ���������� ������� �������� 									*/
	WO1602G_OK,						/* OK, ��� ���������															*/
	WO1602G_UNKNOWN_Err,			/* ����������� ������� 															*/
	WO1602G_I2C_Err,				/* ������� ��������������� ����� �� I2C 										*/
	WO1602G_BUSY,
	WO1602G_WRONG_Arg,				/* �� ���� ������� ���������� ���������, ���� � ������� ���� ������ ���������� */
	WO1602G_NULL_Ptr,				/* �������� �������� (���. - ���������), ������ ���������� ������ 				*/
	WO1602G_Err						/* ������� ��� ��������� �������, ���� �� ���.. 								*/
} wo1602g_status_t;


/*******************************************************************************************************************
 *******************************************************************************************************************
 *     P U B L I C   F U N C T I O N S
 *******************************************************************************************************************
*/
/*******************************************************************************************************************
 *******************************************************************************************************************
 *     � � � � � � � � � � � � �    WO1602G
 *******************************************************************************************************************
������������ wo1602g - ������������ ���������� ������ � ���������� �� ������������
https://www.google.com.ua/url?sa=t&rct=j&q=&esrc=s&source=web&cd=2&ved=0ahUKEwjJybbOqMLXAhXDVxoKHaV5ByMQFggtMAE&url=http%3A%2F%2Fbiakom.com%2Fpdf%2Fwo1602g-yyh-ct_Winstar.pdf&usg=AOvVaw331bZtHQjKDcXgZc0cMr9E
����.12
����������� ���������� ������������ �������� ��� 3� �����. ��� 5� ����� - ���. �������� ����
������� ����������� �� ������ � ������� 5�8,
����� ����������� �� ��� �����,
��������� ������ ��������� ������ ���� ����� ��������� �����
*/

wo1602g_status_t WO1602_Init(I2C_HandleTypeDef *hi2c, uint8_t devAddr);


/*******************************************************************************************************************
 *******************************************************************************************************************
 *     ���� ��������� �����    WO1602G
 *******************************************************************************************************************
��������� ���� ��������� ����� ������� 16 �������
���� �� ���� ������� ����� ������ ���� ����������� � ����� 16 �������
���� ������� ����� 16 - ���������� ���� 16, ������� �� ����� ������� ���������� �� 16 �������, �� �������
���� ������� ����� 16 - �������� �� �������, ��� ����� �� ������ ����������� ������ ����������� �������
����������� �������� ��������� ����������� ����� 16 �������.
*/

wo1602g_status_t  WO1602_WriteHighString (uint8_t *str);

/*******************************************************************************************************************
 *******************************************************************************************************************
 *     ���� �������� �����    WO1602G
 *******************************************************************************************************************
��������� ���� �������� ����� ������� 16 �������
���� �� ���� ������� ����� ������ ���� ����������� � ����� 16 �������
���� ������� ����� 16 - ���������� ���� 16, ������� �� ����� ������� ���������� �� 16 �������, �� �������
���� ������� ����� 16 - �������� �� �������, ��� ����� �� ������ ����������� ������ ����������� �������
����������� �������� ��������� ����������� ����� 16 �������.
*/
wo1602g_status_t  WO1602_WriteLowString (uint8_t *str);

/*******************************************************************************************************************
 *******************************************************************************************************************
 *     �������� ������    WO1602G
 *******************************************************************************************************************
����� ���������. ������ (���������) - � ��������� ����� ���� ������
*/
wo1602g_status_t WO1602_ClearScreen (void);

/*******************************************************************************************************************
 *******************************************************************************************************************
 *     ����̲����� ������� �� ������ ����ֲ�   WO1602G
 *******************************************************************************************************************
������ ����������� �� ������� � = 0 - ������ �����, �=0 - ������ ��� ���������
����� �� ���������
����������� ��������������� ������ � �������� WO1602_WriteChar ��� ������������ ��������� �� ���� �����, � ����
����� ������� �������� �����
���� ����� ����� � ����� ��������� ������ ���������, ������� ������ ���������� � ������� ������ ��� �������
*/
wo1602g_status_t WO1602_SetCursor(uint8_t x, uint8_t y);

/*******************************************************************************************************************
 *******************************************************************************************************************
 *     ���� ������ �������   WO1602G
 *******************************************************************************************************************
��������� ���� ������� ������ �� �������, ������� �������� WO1602_SetCursor, ��� �� �������, ����� �� ����������
 ������������� �������
����������� ���������������  ��� ������������ ��������� �� ���� �����, � ����
����� ������� �������� �����
���� ����� ����� � ����� ��������� ������ ���������, ������� ������ ���������� � ������� ������ ��� �������
*/
wo1602g_status_t WO1602_WriteChar(uint8_t ch);




wo1602g_status_t  WO1602_CreateCustomCharacter(
											   uint8_t *pucPattern, /* ����� � 8 ���� ��� ������� �����������
											   	   	   	   	   	   	   ���������, ��� ��������� '�' {
											    						***01110							0x0E
											    						***10001							0x11
											    						***10000							0x10
											    						***11100							0x1C
											    						***10000							0x10
											    						***10001							0x11
											    						***01110							0x0E
											    						***00000							0x00 }*/
											   uint8_t  cLocation     /*0..7 - ���������� ����� ������ � ������ ������� ����������� */
											   );













/*******************************************************************************************************************
 *******************************************************************************************************************
 *     P R I V A T E     F U N C T I O N S
 *******************************************************************************************************************
*/


#endif /* TK_WO1602G_H_ */

#endif //WINSTAR_Display_WO1602G
/*Hex value to create the pattern (Arrow)*/
//	uint8_t aucPattern1[8] = {0x00,0x04,0x08,0x1f,0x08,0x04,0x00,0x00};
//	/*Hex value to create the pattern (betel leaf)*/
//	uint8_t aucPattern2[8]=  {0x00,0x1b,0x15,0x11,0x0a,0x04,0x00,0x00};
//	uint8_t aucPattern3[8]=  {0x0E,0x11,0x10,0x1C,0x10,0x11,0x0E,0x00};
//
//
//
//	wo1602g_status = WO1602_Init(&hi2c1, 0x3E);
//
//
//	WO1602_CreateCustomCharacter(aucPattern1,0); /*Create arrow at 0th location of CGRAM*/
//	WO1602_CreateCustomCharacter(aucPattern2,1);
//	WO1602_CreateCustomCharacter(aucPattern3,2);
//
//
//wo1602g_status = WO1602_Init(&hi2c1, 0x3E);
//wo1602g_status = WO1602_ClearScreen ();
//
//	wo1602g_status = WO1602_SetCursor(0, 0);
//	wo1602g_status = WO1602_WriteChar('P');
//	wo1602g_status = WO1602_WriteChar(0x30);
//	wo1602g_status = WO1602_WriteChar(0x31);
//	wo1602g_status = WO1602_WriteChar(0x32);
//	wo1602g_status = WO1602_WriteChar(0x33);
//	wo1602g_status = WO1602_WriteChar(0x00);
//	wo1602g_status = WO1602_WriteChar(0x01);
//	wo1602g_status = WO1602_WriteChar(0x02);
//	wo1602g_status = WO1602_WriteChar(0x03);
//	wo1602g_status = WO1602_WriteChar(0x20);
//	wo1602g_status = WO1602_WriteChar(0x21);
//	wo1602g_status = WO1602_WriteChar(0x22);
//	wo1602g_status = WO1602_WriteChar(0x33);
//
//	WO1602_noDisplay();
//
//	WO1602_display();
//
//
//	// Turns the underline cursor on/off
//	WO1602_noCursor();
//
//	WO1602_cursor();
//
//
//	// Turns on/off the blinking cursor
//	WO1602_noBlink();
//
//
//	WO1602_blink();
//
//
//
//
//
//
//
//	wo1602g_status = WO1602_WriteChar('P');
//	wo1602g_status = WO1602_WriteChar('P');
//
//
//
//	wo1602g_status = WO1602_Init(&hi2c1, 0x3E);
//
//	wo1602g_status = WO1602_SetCursor(4, 0);
//	wo1602g_status = WO1602_WriteChar('P');
//	wo1602g_status = WO1602_WriteChar(0x00);
//	wo1602g_status = WO1602_WriteChar(0x01);
//	wo1602g_status = WO1602_WriteChar(0x02);
//	wo1602g_status = WO1602_WriteChar(0x03);
//	wo1602g_status = WO1602_WriteChar('E');
//wo1602g_status=WO1602_WriteHighString   ("����������������");
//	if (wo1602g_status != WO1602G_OK){
//		Error_Handler();
//	}
////		HAL_Delay (2000);
//	wo1602g_status=	WO1602_WriteLowString  	("����������������");
//	if (wo1602g_status != WO1602G_OK){
//			Error_Handler();
//	}
//	HAL_Delay (2000);
//	wo1602g_status = WO1602_ClearScreen ();
//	HAL_Delay (2000);
//	wo1602g_status=	WO1602_WriteHighString 	("����������������");
//	if (wo1602g_status != WO1602G_OK){
//			Error_Handler();
//	}
////		HAL_Delay (2000);
//	wo1602g_status=	WO1602_WriteLowString  	("����������������");
//	if (wo1602g_status != WO1602G_OK){
//				Error_Handler();
//	}
//	HAL_Delay (2000);
//	wo1602g_status = WO1602_ClearScreen ();
//	HAL_Delay (2000);
//	wo1602g_status=	WO1602_WriteHighString 	("�����Ȫ�");
//	if (wo1602g_status != WO1602G_OK){
//			Error_Handler();
//	}
////		HAL_Delay (2000);
//	wo1602g_status = WO1602_ClearScreen ();
//	wo1602g_status=WO1602_WriteHighString   ("������� 220�");
//	if (wo1602g_status != WO1602G_OK){
//		Error_Handler();
//	}
//	wo1602g_status=	WO1602_WriteLowString  	("����� 134.5�");
//	if (wo1602g_status != WO1602G_OK){
//		Error_Handler();
//	}
////		HAL_Delay (2000);
//	wo1602g_status = WO1602_ClearScreen ();
////		HAL_Delay (2000);
//
//	//The C library function int sprintf(char *str, const char *format, ...)
//	uint8_t printed_str [8]={0};
//
////		uint32_t secs=0; uint32_t mins=0; uint32_t hours=0;
////		for (uint32_t i_out=0; i_out<1000000;++i_out) {
////			HAL_Delay (200);
////			++secs;
////			if (secs==60){secs=0;++mins;};
////			if (mins==60){mins=0;++hours;};
////			if (hours==24){hours=0;};
////			wo1602g_status = WO1602_SetCursor(4, 0);
////			sprintf (&printed_str, "%2d.%2d.%2d", hours,mins,secs);
////			for (uint8_t j=0;j<8;++j) {
////				wo1602g_status = WO1602_WriteChar(printed_str[j]);
////			}
////		}
