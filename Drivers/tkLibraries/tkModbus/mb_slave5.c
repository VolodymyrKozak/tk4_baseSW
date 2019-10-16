/*
 * mb_slave5.c
 *
 *  Created on: 17 жовт. 2017 р.
 *      Author: Volodymyr.Kozak
 */
#include <tkConfig.h>
#if defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)
#include "mb_slave5.h"
#include "mb.h"
#include "mb_slave.h"


void MB_Slave_Handling (void) {
//LCD_Printf ("Slave_Handling\n");
	if (msg_status == new_message_for_us ){

#ifdef MODBUS_APPLICATION_SLAVE_asTK2
		MB_Slave_Selective_Handler (&msg_received,
									msg_received_length5,
									&msg_transmitted,
									&msg_transmitted_length);
#endif //#define MODBUS_APPLICATION_SLAVE_asTK2
#ifdef	MODBUS_APPLICATION_SLAVE_CLASSIC
		/*******************************************************************************************************************************************/
		/* 0х03 	R E A D    H O L D I N G    R E G I S T E R S  																				   */
		/*******************************************************************************************************************************************/
		if (msg_received[1]==READ_HOLDING_REGISTERS){
			if (msg_received_length5 == 8){
				uint16_t Starting_Address = msg_received[2] * 0x100 + msg_received[3];
				uint16_t Quantity_of_Registers=msg_received[4] * 0x100 + msg_received[5];
				if (Quantity_of_Registers < 125){							/* 0x01, 0x03, 0xII, 0xII, 0x00, 0x07 0xCRCry 0xCRCrh*/
					msg_transmitted [1]=READ_HOLDING_REGISTERS;             /* формуємо код функції як отриманий, засилаємо в байт 1*/
					uint8_t ByteCount=Quantity_of_Registers * 2;			/* обчислюємо кількість байт, що вимагає Master */
					msg_transmitted [2] = ByteCount % 0x100;				/* засилаємо кількість інформаційних байт, засилаємо в байт 2 */
					/* Обробка прикладоно частини запиту */
					modbus_status_t rhr_status = MODBUS_ERR_UNKNOWN;
					modbus_exception_t ex03 =    MODBUS_EXC_NOT_DEFINED;
					rhr_status =
							f_READ_HOLDING_REGISTERS_Handler (
									Starting_Address,
									Quantity_of_Registers,
									&msg_transmitted[3],
									&ex03
									);
					if (ex03==MODBUS_F_SUCCESS){
						msg_transmitted_length=ByteCount+5;
					}
					else {
						msg_transmitted[1] = READ_HOLDING_REGISTERS | 0x80;		/* формуємо код-функції exception-function_code як отриманий + 0x80  */
						msg_transmitted[2] = (uint8_t)ex03; 							/* засилаємо в наступний байт код помилки згідно зі стандартом Modbus*/
																				/* повертаємо exception-function_code,  якщо довжина неправильна */
						msg_transmitted_length=5;								/* встановлюємо довжину повідомлення про помилку		*/
					}
					/*Кінець обрбки прикладної частини запиту */
					/* обчислюємо кількість байт у повідомленні-відповіді */
				}															/* 0x01, 0x03, 0x0e, 0x03, 0x04, ..., 0x0d, 0x0e, 0x0f, 0x10, 0x11 0xCRC, 0xCRC */
				else { /* Quantity of Registers is WRONG */
					msg_transmitted[1] = READ_HOLDING_REGISTERS | 0x80;		/* формуємо код-функції exception-function_code як отриманий + 0x80  */
					msg_transmitted[2] = (uint8_t)MODBUS_EXC_ILLEGAL_DATA_VALUE; 	/* засилаємо в наступний байт код помилки згідно зі стандартом Modbus*/
																			/* повертаємо exception-function_code,  якщо довжина неправильна */
					msg_transmitted_length=5;								/* встановлюємо довжину повідомлення про помилку		*/
				}
			}
			else {/* msg_received_length5 is WRONG */
				msg_transmitted[1] = READ_HOLDING_REGISTERS | 0x80;		/* формуємо код-функції exception-function_code як отриманий + 0x80  */
				msg_transmitted[2]	=  (uint8_t) MODBUS_EXC_ILLEGAL_DATA_VALUE; 	/* засилаємо в наступний байт код помилки згідно зі стандартом Modbus*/
																		/* повертаємо exception-function_code,  якщо довжина неправильна */
				msg_transmitted_length=5;								/* встановлюємо довжину повідомлення про помилку		*/
			}
		} /* end of READ_HOLDING_REGISTERS*/
		/*******************************************************************************************************************************************/
		/* 0х06     W R I T E    S I N G L E    C O I L 																				   */
		/*******************************************************************************************************************************************/
		else if (msg_received[1]==WRITE_SINGLE_COIL){
			if (msg_received_length5 == 8){
				uint16_t sOutput_Address = msg_received[2] * 0x100 + msg_received[3];
				uint16_t sOutput_Value=msg_received[4] * 0x100 + msg_received[5];
				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

				if ((sOutput_Value==0x0000) || (sOutput_Value==0xFF00)) {
							msg_transmitted [1]=WRITE_SINGLE_COIL;             /* формуємо код функції як отриманий, засилаємо в байт 1*/
							msg_transmitted [2] = sOutput_Address / 0x100;			/* засилаємо в регіст 2 старший байт  										*/
							msg_transmitted [3] = sOutput_Address % 0x100;			/* засилаємо в регіст 3 молодший байт  										*/
							msg_transmitted [4] = sOutput_Value / 0x100;			/* засилаємо в регіст 4 старший байт  										*/
							msg_transmitted [5] = sOutput_Value % 0x100;			/* засилаємо в регіст 5 молодший байт  										*/

							msg_transmitted_length=8;						/* обчислюємо кількість байт у повідомленні-відповіді 									*/
				}
				else {/* Modbus command received is WRONG */
					msg_transmitted[1] = WRITE_SINGLE_COIL | 0x80;			/* формуємо код-функції exception-function_code як отриманий + 0x80  		*/
					msg_transmitted[2]	=   (uint8_t)MODBUS_EXC_ILLEGAL_DATA_VALUE; 	/* засилаємо в наступний байт код помилки згідно зі стандартом Modbus		*/
																			/* повертаємо exception-function_code,  якщо довжина неправильна 			*/
					msg_transmitted_length=5;								/* встановлюємо довжину повідомлення про помилку							*/
				}
			}
			else {/* msg_received_length5 is WRONG */
				msg_transmitted[1] = WRITE_SINGLE_COIL | 0x80;				/* формуємо код-функції exception-function_code як отриманий + 0x80  		*/
				msg_transmitted[2]	=   (uint8_t)MODBUS_EXC_ILLEGAL_DATA_VALUE; 		/* засилаємо в наступний байт код помилки згідно зі стандартом Modbus		*/
																			/* повертаємо exception-function_code,  якщо довжина неправильна 			*/
				msg_transmitted_length=5;									/* встановлюємо довжину повідомлення про помилку							*/
			}
		}

		/*******************************************************************************************************************************************/
		/* 0х06     W R I T E    S I N G L E    R E G I S T E R 																				   */
		/*******************************************************************************************************************************************/
		else if (msg_received[1]==WRITE_SINGLE_REGISTER){
			if (msg_received_length5 == 8){
				uint16_t Register_Address = msg_received[2] * 0x100 + msg_received[3];
				uint16_t Register_Value=msg_received[4] * 0x100 + msg_received[5];
				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
				msg_transmitted [1]=WRITE_SINGLE_REGISTER;             /* формуємо код функції як отриманий, засилаємо в байт 1*/

				msg_transmitted [2] = Register_Address / 0x100;			/* засилаємо в регіст 2 старший байт  										*/
				msg_transmitted [3] = Register_Address % 0x100;			/* засилаємо в регіст 3 молодший байт  										*/
				msg_transmitted [4] = Register_Value / 0x100;			/* засилаємо в регіст 4 старший байт  										*/
				msg_transmitted [5] = Register_Value % 0x100;			/* засилаємо в регіст 5 молодший байт  										*/

			msg_transmitted_length=8;						/* обчислюємо кількість байт у повідомленні-відповіді 									*/
			}
			else {/* msg_received_length5 is WRONG */
				msg_transmitted[1] = WRITE_SINGLE_REGISTER | 0x80;		/* формуємо код-функції exception-function_code як отриманий + 0x80  		*/
				msg_transmitted[2]	=   (uint8_t)MODBUS_EXC_ILLEGAL_DATA_VALUE; 	/* засилаємо в наступний байт код помилки згідно зі стандартом Modbus		*/
																		/* повертаємо exception-function_code,  якщо довжина неправильна 			*/
				msg_transmitted_length=5;								/* встановлюємо довжину повідомлення про помилку							*/
			}
		}
		/*******************************************************************************************************************************************/
		/* 0х08 Д І А Г Н О С Т И К А */
		/*******************************************************************************************************************************************/
		else if (msg_received[1]==DIAGNOSTIC ){                       					/* Якщо отримана Modbus-функція 0х08 Діагностика 					*/
			if 		(
					((msg_received_length5>=6) && (msg_received_length5<125)) &&
					(msg_received_length5 % 2 == 0) &&									/*   {0x01, 0x08, 0x00, 0x00, 0xCRCty, 0xCRCth}*/
					((msg_received [2]==0x00) && (msg_received[3]==0x00))
					) { 	/* і якщо отримана підфункція 0х0000 - Return Query Data  			*/																										/*   {0x01, 0x08, 0x00, 0x00, 0xCRCry, 0xCRCrh}*/
						uint8_t N_TestData=(msg_received_length5-6)/2;
						msg_transmitted_length=6+(N_TestData*2);
						msg_transmitted[1]=DIAGNOSTIC;									/*  ОК,    повертаємо отримане повідомлення, якщо правильна довжина  */
						msg_transmitted[2]=0x00;										/*   {0x01, 0x08, 0x00, 0x00, 0xCRCty, 0xCRCth}*/
						msg_transmitted[3]=0x00;
						msg_transmitted_length=4+N_TestData*2+2;
						if (N_TestData !=0){
							for (uint8_t i_dgn=0;i_dgn<N_TestData*2;++i_dgn){
								msg_transmitted[4+i_dgn]=msg_received[4+i_dgn];
							}
						}
			}
			else{																/* якщо кiлькість байт у повыдомленны менше шести						*/
																				/* або якщо підфункція не 0000, і у нас вона не реалізована				*/
																				/* або неправильно задана кількість даних                  				*/
						msg_transmitted[1] = DIAGNOSTIC | 0x80;		    		/* формуємо код-команди exception-function_code як отриманий + 0ч80  	*/
						msg_transmitted[2]	=   (uint8_t)MODBUS_EXC_ILLEGAL_DATA_VALUE; 	/* засилаємо в наступний байт код помилки згідно зі стандартом Modbus	*/
																				/* повертаємо exception-function_code,  якщо довжина неправильна 		*/
						msg_transmitted_length=5;								/* встановлюємо довжину повідомлення про помилку						*/
			}

		}
		/*******************************************************************************************************************************************/
		/* 0хFE ВІДЛАГОДЖЕННЯ КАНАЛУ З КОМПЮТЕРОМ */
		/*******************************************************************************************************************************************/
		else if (msg_received[1]==tk4forPC_Master ){                       					/* Якщо отримана Modbus-функція 0хFE Відлагодження */
				if (
					((msg_received_length5>=6) && (msg_received_length5<125)) &&
					(msg_received_length5 % 2 == 0) &&									/*   {0x01, 0x77, 0x00, 0x00, 0xCRCty, 0xCRCth}*/
					((msg_received [2]==0x00) && (msg_received[3]==0x00))
					) { 	/* і якщо отримана підфункція 0х0000 - Return Query Data  			*/																										/*   {0x01, 0x08, 0x00, 0x00, 0xCRCry, 0xCRCrh}*/
					uint8_t N_TestData=(msg_received_length5-6)/2;
					msg_transmitted_length=6+(N_TestData*2);
					msg_transmitted[1]=tk4forPC_Master;									/*  ОК,    повертаємо отримане повідомлення, якщо правильна довжина  */
					msg_transmitted[2]=0x00;										/*   {0x01, 0x08, 0x00, 0x00, 0xCRCty, 0xCRCth}*/
					msg_transmitted[3]=0x00;
					msg_transmitted_length=4+N_TestData*2+2;
					if (N_TestData !=0){
						for (uint8_t i_dgn=0;i_dgn<N_TestData*2;++i_dgn){
							msg_transmitted[4+i_dgn]=msg_received[4+i_dgn];
						}
					}
				}
				else{																/* якщо кiлькість байт у повыдомленны менше шести						*/
																						/* або якщо підфункція не 0000, і у нас вона не реалізована				*/
																						/* або неправильно задана кількість даних                  				*/
					msg_transmitted[1] = tk4forPC_Master | 0x80;		    		/* формуємо код-команди exception-function_code як отриманий + 0ч80  	*/
					msg_transmitted[2]	=   (uint8_t)MODBUS_EXC_ILLEGAL_DATA_VALUE; 	/* засилаємо в наступний байт код помилки згідно зі стандартом Modbus	*/
																						/* повертаємо exception-function_code,  якщо довжина неправильна 		*/
					msg_transmitted_length=5;								/* встановлюємо довжину повідомлення про помилку						*/
				}

		}
		/*******************************************************************************************************************************************
		 *  П О М И Л К О В И Й      К О Д     Ф У Н К Ц І Ї      М о д б у с
		 ******************************************************************************************************************************************/
		else {   /* помилка кода команди */                            /* Якщо ми отримали помилковий код команди Modbus: 					*/
			msg_transmitted[1]=msg_received[1] | 0x80;				   /* формуємо код-команди exception-function_code як отриманий + 0ч80  */
			msg_transmitted[2]	=  (uint8_t)MODBUS_EXC_ILLEGAL_FUNCTION;		   /* засилаємо в наступний байт код помилки згідно зі стандартом Modbus*/
			msg_transmitted_length=5;								   /* встановлюємо довжину повідомлення про помилку						*/

		}



		/************************************************************************************************************************************
		 *  Ф О Р М У В А Н Н Я     В І Д П О В І Д І  НА  повідомлення, адресоване конкретно нам
		 ************************************************************************************************************************************/
#endif //MODBUS_APPLICATION_SLAVE_CLASSIC
		if (msg_transmitted_length != 0){
//																		/* Формуємо повідомлення - відповідь для Master   					*/
//
			msg_transmitted[0]=SLAVE_NODE_ADDRESS;                          /* 1. В перший/нульвий байт - адресу Slave - 0x01 					*/

			uint16_t crc_calc_tx = CRC_16x (msg_transmitted, 			    /* 2. Обчислюэмо 16-бітний CRC   									*/
				        msg_transmitted_length -2*(sizeof(uint8_t)));
			msg_transmitted[msg_transmitted_length-1]	= crc_calc_tx / 0x100;	/* 3. Старший байт CRC засилаємо в останній байт повідомлення 		*/
			msg_transmitted[msg_transmitted_length-2]	= crc_calc_tx % 0x100;	/* 4. Молодший байт СRC засилаэмо в передостанный байт повыдомлення */
			modbus_slave_state=answer_transmitting;
			HAL_Delay(5);
			MB_Slave_Transmit ();											/* 5. Відправляємо повідомлення                                     */
			for (uint8_t izb=0;izb<msg_received_length5;++izb){				/* Обнуляэмо буфер вхідного повідомлення */
				msg_received[izb]=0x00;
			}
//		/************************************************************************************************************************************
//		 * **********************************************************************************************************************************
//		 */
		}
	}
	/*  Р Е А К Ц І Я       Н А     Ц И Р К У Л Я Р Н Е  П О В І Д О М Л Е Н Я    */
	/*****************************************************************************************************************************************/
	else if (msg_status == broadcast_message ) {
#ifdef MODBUS_APPLICATION_SLAVE_asTK2

		MB_Slave_Broadcast_Handler(&msg_received,
								   msg_received_length5
								   );
		for (uint8_t izb=0;izb<msg_received_length5;++izb){				/* Обнуляэмо буфер вхідного повідомлення */
			msg_received[izb]=0x00;
		}
		 if(HAL_UART_Receive_IT (&SLAVE_HUART,
								&(msg_received),
								RX_MAX_SLAVE_MSG_LENGTH
								) != HAL_OK){																  /*перезапускаємо прийом наступного повідомленя */
			Error_Handler();
		 }
		 modbus_slave_state=no_message_for_us;

#endif //#define MODBUS_APPLICATION_SLAVE_asTK2
#ifdef	MODBUS_APPLICATION_SLAVE_CLASSIC

//		/**************************************************************************************************************************************
//		 * 0х05  W R I T E _  S I N G L E _  C O I L
//		 *************************************************************************************************************************************/
		if (msg_received[1]==WRITE_SINGLE_COIL){
		}
		/*************************************************************************************************************************************/
		else { 	/* помилка кода команди */                             /* скидаємо кількісь байт у вхідному повідомленні, чим уможливлюємо   */
																	   /* роботу переривань по відсутності сигнала в мережі					 */
			msg_received_length5=0;

		}

//	/****************************************************************************************************************************************/
#endif//	MODBUS_APPLICATION_SLAVE_CLASSIC
	}
	else {}//	 modbus_slave_state=no_message_for_us;


}







#endif //defined (SLAVERTU_HUART_4) || defined (SLAVERTU_HUART_5)



