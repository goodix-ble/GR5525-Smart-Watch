#include "app_key.h"
#include "lvgl.h"

#include <stdio.h>

lv_obj_t *lv_layout_tv_mid_create(lv_obj_t * parent_tv_obj);

lv_obj_t * lv_layout_tv_mid_create(lv_obj_t * parent_tv_obj)
{
    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_clear_flag(p_window, LV_OBJ_FLAG_SCROLLABLE);

    // ECG Title
    lv_obj_t *p_title = lv_label_create(p_window);
    lv_label_set_text(p_title, "MID");
    lv_obj_set_style_text_font(p_title, &lv_font_montserrat_40, LV_STATE_DEFAULT);
    lv_obj_center(p_title);


    return p_window;
}
