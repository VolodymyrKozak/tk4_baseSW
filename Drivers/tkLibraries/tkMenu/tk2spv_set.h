/*
 * tk2spv_set.h
 *
 *  Created on: 25 лют. 2019 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKLIBRARIES_TKMENU_TK2SPV_SET_H_
#define TKLIBRARIES_TKMENU_TK2SPV_SET_H_

typedef	enum {
		UIset_unknown_state,
		UIset_main_state,
		UIset_mainBTN_state,
		UIset_time_dymamic,
		UIset_datatime_SET,
		UIset_userArray,

		UIset_userArray_Exit,

		UIset_PSW2_Entry,
		UIset_PSW2_BTN,
		UIset_PSW2_Dynamic,

		UIset_FacilityArr,
		UIset_FacilityCplt,
		UIset_FacilitySvd,
		UIset_Powers_state,
		UIset_Derating_state,

		UIset_exit_state
} uiset_state_t;
void f_1UIset_Spv_Init(void);
uiset_state_t f_1UIset_Spv(void);

#endif /* TKLIBRARIES_TKMENU_TK2SPV_SET_H_ */
