/*
 * tkGridEvents.c
 *
 *  Created on: 15 ���. 2019 �.
 *      Author: KozakVF
 */

#include "tkGrid.h"
#include "wk_atime64.h"
#include "vv300Flash.h"


union gr gr={0};

static uint32_t prev_gstatus = 0;

void f_GRD_Events(void){
	gr.gr_rec.gstatus = f_GRD_Calc();
	if (gr.gr_rec.gstatus != prev_gstatus){
		gr.gr_rec.elipsed_time = wk_real64_value;

		flash_status_t fsst = f_write2page_record
										(	GRID_STATUS_REC_A,	/* ����� ������ �������  */
											gr.grfl,			/* �������� �� ����� � RAM*/
											sizeof (gr.grfl)	/* ����� ������ � ������ */
										);

		if (fsst != FLASH_OK){
			;
		}

	}
	prev_gstatus = gr.gr_rec.gstatus;
}
