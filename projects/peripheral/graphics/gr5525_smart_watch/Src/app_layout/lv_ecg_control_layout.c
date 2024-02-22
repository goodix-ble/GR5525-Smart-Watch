#include "app_key.h"
#include "lvgl.h"

#include <stdio.h>


#define LV_STATE_ECG_STARTED LV_STATE_USER_1

lv_obj_t *lv_ecg_control_layout_create(lv_obj_t * parent_tv_obj);


static lv_anim_t s_logo_anim;
static bool s_ecg_started = false;

static void set_logo_zoom(void *var, int32_t v)
{
    lv_img_set_zoom((lv_obj_t *)var, v);
}

static void lv_ecg_control_btn_clicked_cb(lv_event_t *evt)
{

    lv_obj_t *obj = lv_event_get_current_target(evt);
    if (s_ecg_started)
    {
        // ECG Started
        lv_obj_clear_state(obj, LV_STATE_ECG_STARTED);
        // Stop Logo Animation
        lv_anim_set_repeat_count(lv_anim_get(s_logo_anim.var, NULL), 0);
        s_ecg_started = false;
    }
    else
    {
        // ECG Stopped
        lv_obj_add_state(obj, LV_STATE_ECG_STARTED);
        // Start Logo Animation
        lv_anim_set_repeat_count(&s_logo_anim, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&s_logo_anim);

        s_ecg_started = true;
    }
}


lv_obj_t * lv_ecg_control_layout_create(lv_obj_t * parent_tv_obj)
{
    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_clear_flag(p_window, LV_OBJ_FLAG_SCROLLABLE);

    // ECG Logo
    lv_obj_t *p_logo = lv_img_create(p_window);
    lv_obj_align(p_logo, LV_ALIGN_TOP_MID, 0, 80);
    lv_obj_set_size(p_logo, 54, 49);
    lv_img_set_pivot(p_logo, 27, 24);
    // Workaround: Add a little rotation to force LVGL zoom from pivot instead of left-top corner
    lv_img_set_angle(p_logo, 10);

    // Logo Animation
    lv_anim_init(&s_logo_anim);
    lv_anim_set_var(&s_logo_anim, p_logo);
    lv_anim_set_time(&s_logo_anim, 150);
    lv_anim_set_playback_delay(&s_logo_anim, 0);
    lv_anim_set_playback_time(&s_logo_anim, 150);
    lv_anim_set_repeat_delay(&s_logo_anim, 300);
    lv_anim_set_values(&s_logo_anim, 256, 384);
    lv_anim_set_exec_cb(&s_logo_anim, set_logo_zoom);
    lv_anim_set_path_cb(&s_logo_anim, lv_anim_path_ease_in);

    // ECG Title
    lv_obj_t *p_title = lv_label_create(p_window);
    lv_label_set_text(p_title, "ABCD");
    lv_obj_set_style_text_font(p_title, &lv_font_montserrat_40, LV_STATE_DEFAULT);
    lv_obj_align(p_title, LV_ALIGN_TOP_MID, 0, 150);

    // Control Button
    lv_obj_t *p_btn = lv_btn_create(p_window);
    lv_obj_set_style_radius(p_btn, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(p_btn, lv_ecg_control_btn_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_size(p_btn, 60, 60);
    lv_obj_remove_style(p_btn, NULL, LV_STATE_PRESSED);
    lv_obj_align(p_btn, LV_ALIGN_TOP_MID, 0, 240);

    

    return p_window;
}
