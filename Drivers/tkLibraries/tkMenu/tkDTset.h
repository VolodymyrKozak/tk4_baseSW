/*
 * tkDTset.h
 *
 *  Created on: 24 лют. 2019 р.
 *      Author: Volodymyr.Kozak
 */

#ifndef TKLIBRARIES_TKMENU_TKDTSET_H_
#define TKLIBRARIES_TKMENU_TKDTSET_H_
#include "wk_atime64.h"
typedef enum{
	TMmenu_unknown_state,
	TMmenu_input,

	TMmenu_settingYYYY,
	TMmenu_settingMM,
	TMmenu_settingDD,
	TMmenu_settingHH,
	TMmenu_settingMinute,

	TMmenu_escape,

	TMmenu_valuePreSet,
	TMmenu_valueSet,
}tm_menu_state_t;

void f_TMmenuInit(void);
tm_menu_state_t f_TMmenu(struct tm *outTM);

#endif /* TKLIBRARIES_TKMENU_TKDTSET_H_ */
