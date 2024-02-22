#ifndef __DISPLAY_CRTL_DRV_H__
#define __DISPLAY_CRTL_DRV_H__

#include "lv_hal_disp.h"


#define SCREEN_TYPE         1       // 0 - XSJ-QSPI-454; 1 - FLS-QSPI-360
#if SCREEN_TYPE == 1
#define TE_SIGNAL_ENABLED   1
#else
#define TE_SIGNAL_ENABLED   0
#endif // SCREEN_TYPE == 1

void disp_crtl_init(void);
void disp_crtl_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void disp_crtl_set_show(void);

#if LV_GDX_PATCH_SET_CLIP_AREA_ONCE
void disp_crtl_set_clip_area(const lv_area_t *clip_area);
#else
#define disp_crtl_set_clip_area(...)
#endif // LV_GDX_PATCH_SET_CLIP_AREA_ONCE

#endif /*__DISPLAY_CRTL_DRV_H__*/
