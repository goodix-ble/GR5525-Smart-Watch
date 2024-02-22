#include "lvgl.h"
#include "lv_font.h"
#include "lv_img_dsc_list.h"
#include "lv_imgdigits.h"
#include "watchface_manager.h"

#include "app_rtc.h"

#include <stdio.h>

#define SHOW_ROTATED_DIGITS 1

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
typedef struct
{
    lv_obj_t *date_label;
    lv_obj_t *hand_hour;
    lv_obj_t *hand_minute;
    lv_obj_t *hand_second;
#if SHOW_ROTATED_DIGITS
    lv_obj_t *value_hr;
    lv_obj_t *value_steps;
    lv_obj_t *value_battery;
    lv_obj_t *value_calories;
#endif // SHOW_ROTATED_DIGITS
} watchface_vivid_ctx_t;

DECLARE_WF_REFR_CTX_FUNC(watchface_vivid_ctx_t);

#if SHOW_ROTATED_DIGITS
static const lv_img_dsc_t *const MISC_IMGDIGITS_LIST[] = {
    &wd_img_wf_num_0,
    &wd_img_wf_num_1,
    &wd_img_wf_num_2,
    &wd_img_wf_num_3,
    &wd_img_wf_num_4,
    &wd_img_wf_num_5,
    &wd_img_wf_num_6,
    &wd_img_wf_num_7,
    &wd_img_wf_num_8,
    &wd_img_wf_num_9,
    NULL,
    NULL,
    NULL,
};
#endif // SHOW_ROTATED_DIGITS

static const lv_style_const_prop_t DATE_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0x99, 0x99, 0x99)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x20, 0x20, 0x20)),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(VIVID_DATE_LABEL_STYLE, DATE_LABEL_STYLE_PROPS);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static void refr_timer_cb(lv_timer_t *p_timer)
{
    app_rtc_time_t time;
    app_rtc_get_time(&time);
    watchface_vivid_ctx_t *ctx = wf_mngr_get_refr_ctx(p_timer);
    lv_img_set_angle(ctx->hand_second, time.sec * 60 + 2700);
    lv_img_set_angle(ctx->hand_minute, time.min * 60 + 2700);
    lv_img_set_angle(ctx->hand_hour, time.hour * 300 + time.min * 5 + 2700);
}

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_watchface_vivid_clock_layout_create(lv_obj_t *parent)
{
    lv_gximg_init();

    // Background
    lv_obj_t *p_window = lv_obj_create(parent);
    lv_obj_remove_style_all(p_window);
    lv_obj_clear_flag(p_window, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_set_style_bg_img_src(p_window, &wd_img_watchface_bg, 0);
    lv_obj_set_scrollbar_mode(p_window, LV_SCROLLBAR_MODE_OFF);
    lv_obj_center(p_window);

    watchface_vivid_ctx_t *ctx = wf_mngr_create_refr_ctx(p_window, 500, refr_timer_cb);

    // Date
    ctx->date_label = lv_label_create(p_window);
    lv_obj_set_size(ctx->date_label, 120, 60);
    lv_obj_add_style(ctx->date_label, (lv_style_t *)&VIVID_DATE_LABEL_STYLE, 0);
    lv_obj_set_pos(ctx->date_label, 180 - 60, 100 - 11);
    lv_label_set_text_fmt(ctx->date_label, "%02d-%02d", 12, 24);

    // Buttons
    lv_obj_t *btn;
    btn = lv_img_create(p_window);
    lv_img_set_src(btn, &wd_img_app_04_stopwatch);
    lv_obj_align(btn, LV_ALIGN_CENTER, -80, 0);

    btn = lv_img_create(p_window);
    lv_img_set_src(btn, &wd_img_app_02_activity);
    lv_obj_align(btn, LV_ALIGN_CENTER, 80, 0);

    btn = lv_img_create(p_window);
    lv_img_set_src(btn, &wd_img_app_06_events);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 80);

#if SHOW_ROTATED_DIGITS
    // HR Number
    ctx->value_hr = lv_imgdigits_create(p_window);
    lv_imgdigits_set_digits_img(ctx->value_hr, (const lv_img_dsc_t **)MISC_IMGDIGITS_LIST);
    lv_obj_set_pos(ctx->value_hr, 70, 85);
    lv_imgdigits_set_value(ctx->value_hr, 87);
    lv_imgdigits_set_text_align(ctx->value_hr, LV_TEXT_ALIGN_CENTER);
    lv_imgdigits_set_angle(ctx->value_hr, 3150);

    // Step Number
    ctx->value_steps = lv_imgdigits_create(p_window);
    lv_imgdigits_set_digits_img(ctx->value_steps, (const lv_img_dsc_t **)MISC_IMGDIGITS_LIST);
    lv_obj_set_pos(ctx->value_steps, 270, 60);
    lv_imgdigits_set_value(ctx->value_steps, 5678);
    lv_imgdigits_set_text_align(ctx->value_steps, LV_TEXT_ALIGN_CENTER);
    lv_imgdigits_set_angle(ctx->value_steps, 450);

    // Battery Number
    ctx->value_battery = lv_imgdigits_create(p_window);
    lv_imgdigits_set_digits_img(ctx->value_battery, (const lv_img_dsc_t **)MISC_IMGDIGITS_LIST);
    lv_obj_set_pos(ctx->value_battery, 85, 295);
    lv_imgdigits_set_value(ctx->value_battery, 87);
    lv_imgdigits_set_text_align(ctx->value_battery, LV_TEXT_ALIGN_CENTER);
    lv_imgdigits_set_angle(ctx->value_battery, 2250);

    // Calories Number
    ctx->value_calories = lv_imgdigits_create(p_window);
    lv_imgdigits_set_digits_img(ctx->value_calories, (const lv_img_dsc_t **)MISC_IMGDIGITS_LIST);
    lv_obj_set_pos(ctx->value_calories, 300, 270);
    lv_imgdigits_set_value(ctx->value_calories, 1123);
    lv_imgdigits_set_text_align(ctx->value_calories, LV_TEXT_ALIGN_CENTER);
    lv_imgdigits_set_angle(ctx->value_calories, 1350);
#endif // SHOW_ROTATED_DIGITS

    // Clock hands
    ctx->hand_minute = lv_img_create(p_window);
    lv_img_set_src(ctx->hand_minute, &wd_gximg_watchface_hand_min_bundle);
    lv_port_gximg_set_img_pos(ctx->hand_minute, DISP_HOR_RES / 2, DISP_VER_RES / 2);

    ctx->hand_hour = lv_img_create(p_window);
    lv_img_set_src(ctx->hand_hour, &wd_gximg_watchface_hand_hour_bundle);
    lv_port_gximg_set_img_pos(ctx->hand_hour, DISP_HOR_RES / 2, DISP_VER_RES / 2);

    ctx->hand_second = lv_img_create(p_window);
    lv_img_set_src(ctx->hand_second, &wd_gximg_watchface_hand_second_bundle);
    lv_port_gximg_set_img_pos(ctx->hand_second, DISP_HOR_RES / 2, DISP_VER_RES / 2);

    // Center
    lv_obj_t *center_point = lv_img_create(p_window);
    lv_img_set_src(center_point, &wd_img_watchface_center_point);
    lv_obj_center(center_point);

    return p_window;
}
