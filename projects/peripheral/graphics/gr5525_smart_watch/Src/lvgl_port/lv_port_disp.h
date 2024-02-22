#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#ifdef __cplusplusW
extern "C" {
#endif

#include "lvgl.h"


#define GX_FRAME_SIZE  (DISP_HOR_RES * DISP_VER_RES * (2))

void lv_port_disp_init(void);

void lv_refresh_enable_set(bool enable);

bool lv_refresh_enable_get(void);

void lv_display_enable_set(bool enable);

bool lv_display_enable_get(void);

void lv_port_debug_info_enable(bool enable);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

