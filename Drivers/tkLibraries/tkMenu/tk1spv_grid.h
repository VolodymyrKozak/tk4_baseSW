/*
 * tk1spv_grid.h
 *
 *  Created on: 25 лют. 2019 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKLIBRARIES_TKMENU_TK1SPV_GRID_H_
#define TKLIBRARIES_TKMENU_TK1SPV_GRID_H_
typedef	enum {
		UIgrid_unknown_state,
		UIgrid_main_state,
		UIgrid_main_state_btn,
		UIgrid_Powers_state,
		UIgrid_Powers_state_btn,
		UIgrid_Derating_state,
		UIgrid_Derating_state_btn,
		UIgrid_phasesTHDs,
		UIgrid_phasesTHDs_btn,
		UIgrid_grid_param_state,
		UIgrid_grid_param_state_btn,
		UIgrid_EventsArchive,
		UIgrid_EventsArchiveEnt,
		UIgrid_EventsArchiveRead,
		UIgrid_exit_state
} uigrid_state_t;
void f_1UIgrid_Spv_Init(void);
uigrid_state_t f_1UIgrid_Spv(void);

#endif /* TKLIBRARIES_TKMENU_TK1SPV_GRID_H_ */
