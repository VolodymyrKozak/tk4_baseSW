/*
 * tkConfig.c
 *
 *  Created on: 23 лют. 2019 р.
 *      Author: KozakVF
 *  Created on: 23 лют. 2019 р.
 *      Author: Volodymyr.Kozak
 */

#include "tkConfig.h"

#ifdef WINSTAR_Display_WH1602B3
#include "tk_wh1602b3.h"
#endif
#ifdef WINSTAR_Display_WO1602G
#include "tk_wo1602g.h"
#endif
#include "string.h"
#include "zFF.h"


