/*
 * tkSGL.h
 *
 *  Created on: 2 серп. 2017 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKSGL_H_
#define TKSGL_H_

#include <stdint.h> /* int32_t style anable */
#include <stddef.h> /* NULL is anable       */
#include <stdbool.h>

typedef enum  {                  /*вводиться тип даних, що повертається функціями бібліотеки sgl_error_t та перераховуються можливі помилки для error handling*/
	SGL_SUCCESS=0,
	SGL_UNKNOWN_ERROR,
	SGL_INIT_ERROR,
	SGL_NULL_ptr,
	SGL_ZERO_arg,
	SGL_WRONG_arg,
	SGL_LAST_ERROR
} sgl_err_t;

uint32_t  f_btnExpSmoothing(
				uint16_t sample_value,            /* поточне значення вимірювання, од */
				uint32_t previous_smoothed_value, /* попереднє зглажене значення, од  */
				uint32_t Ntau				      /* ПОСТІЙНА ЗГЛАЖУВАННЯ В кількості відліків
				 	 	 	 	 	 	 	 	     через 	1Tou 	67%,
				 	 	 	 	 	 	 	 	     	 	2Tou 	86,5%
				 	 	 	 	 	 	 	 	     	 	3Tou 	95,0%
				 	 	 	 	 	 	 	 	     	 	4Tou 	98,2%
				 	 	 	 	 	 	 	 	     	 	5Tou    99,3% */
				);

sgl_err_t f_ExponentialSmoothing  ( float sample_value,
									float previous_smoothed_value,
									float time_constant,
									float sampling_frequency,
									float *p_smoothed_value);
#endif /* TKSGL_H_ */
