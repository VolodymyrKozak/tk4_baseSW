/*
 * pr_OVEN_MCD200.c
 *
 *  Created on: 15 лют. 2019 р.
 *      Author: KozakVF
 */

#include "pr_OVEN_MCD200.h"
#ifdef MODBUS_MASTER_for_OVEN_ARCHIVE_MCD200
/*
Прибор получает данные от мастера сети и сохраняет их в архиве.
В режиме «Slave» прибор поддерживает работу только с протоколом ModBus RTU,
используя функцию протокола ModBus 0x10 (запись нескольких регистров).
В режиме «Slave» отсутствует возможность архивировать данные встроенных измерителей тока.
Для работы прибора в режиме «Slave» в группе общих параметров архивирования
должен быть задан конфигурационный параметр «Адрес»
определяющий адрес прибора МСД-200 в сети и
параметр «Задержка ответа» определяющий задержку ответа прибора МСД-200
на команды мастера сети.
В группе индивидуальных параметров каналов архивирования необходимо задать
=следующие параметры:
=архивирование,
=имя канала,
=тип данных,
=количество знаков дробной части,
=адрес регистра.
*/



#endif //MODBUS_MASTER_for_OVEN_ARCHIVE_MCD200
/*
16 (0x10) Write Multiple registers
Function code 1 Byte 0x10
Starting Address 2 Bytes 0x0000 to 0xFFFF
Quantity of Registers 2 Bytes 0x0001 to 0x007B
Byte Count 1 Byte 2 x N*
Registers Value N* x 2 Bytes valu
*/

//void nname (void){
//if(ADR==4)
// {if(Fun==16)
//   { SBUFtx[1]=Fun;  /* Function code 1 Byte 0x10 */
//     SBUFtx[2]=0x10; /*Starting Address 2 Bytes 0x0000 to 0xFFFF*/
//     SBUFtx[3]=0x00;
//     SBUFtx[4]=0x00; /*Quantity of Registers 2 Bytes 0x0001 to 0x007B*/
//     SBUFtx[5]=tmp_fm=(((Buf_P_MB[0]>>8)&0x0F)+1);
//
//     if(float_DST==0){
//     SBUFtx[6]= /* Byte Count 1 Byte 2 x N*   */
//     	 SBUFtx[5]*2;}
//     if(float_DST==1){SBUFtx[6]= (2 + ((tmp_fm-1)*4));
//                      SBUFtx[5]=(tmp_fm-1)*2 +1;
//
//
//                       }
//
//    if(float_DST==0)
//     {for(i_fm=0;i_fm <=tmp_fm; i_fm++)
//      {tmp_fm1= Buf_P_MB[N_Buf_interm*17+i_fm];
//
//       if(i_fm==0)
//        {tmp_fm2= ((tmp_fm1&0x000F)+ (((tmp_fm1>>4)&0x000F)*10)+ (((tmp_fm1>>8)&0x000F)*100));
//         tmp_fm1=tmp_fm2;
//        }
//
//        SBUFtx[7+(i_fm*2)]= (tmp_fm1&0x00FF);
//        SBUFtx[7+(i_fm*2)+1]=(tmp_fm1>>8);
//      }
//      i2=(7+ (tmp_fm*2));goto frb2;
//     }
//      if(float_DST==1)
//       {
//         kk=0;
//         tmp_fm1= Buf_P_MB[N_Buf_interm*17];
//
//         tmp_fm2= ((tmp_fm1&0x000F)+ (((tmp_fm1>>4)&0x000F)*10)+ (((tmp_fm1>>8)&0x000F)*100));
//         tmp_fm1=tmp_fm2;
//           // tmp_fm4=tmp_fm1;
//
//         SBUFtx[7]= (tmp_fm1&0x00FF);
//         SBUFtx[8]=(tmp_fm1>>8);
//
//         for(i_fm=1;i_fm <= tmp_fm; i_fm++)
//         {tmp_fm1=(int)Buf_P_MB[N_Buf_interm*17+i_fm];
//          Tmp_fm3= (float)(tmp_fm1)/10.0;
//          point =(char *)&Tmp_fm3;
//          byte_1 = point[3]; SBUFtx[7+kk+(i_fm)*2]=byte_1;
//          byte_1 = point[2]; SBUFtx[7+kk+(i_fm*2)+1]=byte_1;
//          byte_1 = point[1]; SBUFtx[7+kk+(i_fm*2)+2]=byte_1;
//          byte_1 = point[0]; SBUFtx[7+kk+(i_fm*2)+3]=byte_1;
//          kk=kk+2;
//        }
//       i2=(9+ ((tmp_fm-1)*4));goto frb2;
//       }
//     }
//  }
//
//frb2:
//  Temp2=CRC_16((unsigned char*)&SBUFtx[0],i2);
//  SBUFtx[i2]=Temp2%256;SBUFtx[i2+1]=Temp2/256;i2=i2+2;
//  return;
//}
//
//}
