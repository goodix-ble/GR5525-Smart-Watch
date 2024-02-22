#include "lvgl.h"
#include "lv_font.h"
#include "lv_img_dsc_list.h"
#include "lv_imgdigits.h"
#include "lv_img_rgb565.h"
#include "lv_img_gximg.h"
#include "watchface_manager.h"

#include "app_rtc.h"

#include <stdio.h>

#define USE_GX_IMG 1 // reduce 6.8ms
#define USE_IMG_RGB565 1 // reduce 2.8ms

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
typedef struct
{
    lv_timer_t *refr_timer;
    lv_obj_t *date_label;
    lv_obj_t *step_label;
    lv_obj_t *temp_label;
    lv_obj_t *temp_icon;
    lv_obj_t *hand_hour;
    lv_obj_t *hand_minute;
    lv_obj_t *hand_second;
} watchface_black_ctx_t;

DECLARE_WF_REFR_CTX_FUNC(watchface_black_ctx_t);

static const lv_style_const_prop_t STEP_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x33, 0x33, 0x33)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t OTHER_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x1A, 0x1A, 0x1A)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(STEP_LABEL_STYLE, STEP_LABEL_STYLE_PROPS);
LV_STYLE_CONST_INIT(OTHER_LABEL_STYLE, OTHER_LABEL_STYLE_PROPS);


/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static void refr_timer_cb(lv_timer_t *p_timer)
{
    app_rtc_time_t time;
    app_rtc_get_time(&time);
    watchface_black_ctx_t *ctx = wf_mngr_get_refr_ctx(p_timer);

#if USE_GX_IMG
    lv_img_gximg_set_angle(ctx->hand_second, time.sec * 60 + 2700);
    lv_img_gximg_set_angle(ctx->hand_minute, time.min * 60 + 2700);
    lv_img_gximg_set_angle(ctx->hand_hour, time.hour * 300 + time.min * 5 + 2700);
#else
    lv_img_set_angle(ctx->hand_second, time.sec * 60 + 2700);
    lv_img_set_angle(ctx->hand_minute, time.min * 60 + 2700);
    lv_img_set_angle(ctx->hand_hour, time.hour * 300 + time.min * 5 + 2700);
#endif
}

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_watchface_black_clock_layout_create(lv_obj_t *parent)
{
    lv_gximg_init();

    // Background
    #if USE_IMG_RGB565
    lv_obj_t *p_window = lv_img_rgb565_create(parent);
    #else
    lv_obj_t *p_window = lv_obj_create(parent);
    #endif

    watchface_black_ctx_t* ctx = wf_mngr_create_refr_ctx(p_window, 500, refr_timer_cb);

    lv_obj_remove_style_all(p_window);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_set_style_bg_img_src(p_window, &wd_img_black_clock_face, 0);
    //lv_obj_center(p_window);

    // Steps
    #if USE_IMG_RGB565
    lv_obj_t *p_step_icon = lv_img_rgb565_create(p_window);
    lv_img_rgb565_set_src(p_step_icon, &wd_img_black_clock_step);
    #else
    lv_obj_t *p_step_icon = lv_img_create(p_window);
    lv_img_set_src(p_step_icon, &wd_img_black_clock_step);
    #endif
    lv_obj_set_pos(p_step_icon, 104 - wd_img_black_clock_step.header.w / 2, 177 - wd_img_black_clock_step.header.h);

    ctx->step_label = lv_label_create(p_window);
    lv_obj_set_size(ctx->step_label, 40, 20);
    lv_obj_add_style(ctx->step_label, (lv_style_t *)&STEP_LABEL_STYLE, 0);
    lv_obj_set_pos(ctx->step_label, 104 - 20, 177 + 5);
    lv_label_set_text_fmt(ctx->step_label, "%d", 8989);

    // Date
    ctx->date_label = lv_label_create(p_window);
    lv_obj_set_size(ctx->date_label, 60, 20);
    lv_obj_add_style(ctx->date_label, (lv_style_t *)&OTHER_LABEL_STYLE, 0);
    lv_obj_set_pos(ctx->date_label, 254 - 30, 177 - 10);
    lv_label_set_text_fmt(ctx->date_label, "%02d-%02d", 12, 24);

    // temperature
    #if USE_IMG_RGB565
    ctx->temp_icon = lv_img_rgb565_create(p_window);
    lv_img_rgb565_set_src(ctx->temp_icon, &wd_img_black_clock_sun);
    #else
    ctx->temp_icon = lv_img_create(p_window);
    lv_img_set_src(ctx->temp_icon, &wd_img_black_clock_sun);
    #endif
    lv_obj_set_pos(ctx->temp_icon, 179 - wd_img_black_clock_sun.header.w / 2, 266 - wd_img_black_clock_sun.header.h);

    ctx->temp_label = lv_label_create(p_window);
    lv_obj_set_size(ctx->temp_label, 80, 20);
    lv_obj_add_style(ctx->temp_label, (lv_style_t *)&OTHER_LABEL_STYLE, 0);
    lv_obj_set_pos(ctx->temp_label, 179 - 40, 266 + 4);
    lv_label_set_text_fmt(ctx->temp_label, "%d", 26);

#if USE_GX_IMG
    ctx->hand_minute = lv_img_gximg_create(p_window);
    lv_obj_set_pos(ctx->hand_minute, DISP_HOR_RES / 2, DISP_VER_RES / 2);
    lv_img_gximg_set_pivot_and_src(ctx->hand_minute, 0, 0, &wd_gximg_watchface_hand_min_bundle);

    ctx->hand_hour = lv_img_gximg_create(p_window);
    lv_obj_set_pos(ctx->hand_hour, DISP_HOR_RES / 2, DISP_VER_RES / 2);
    lv_img_gximg_set_pivot_and_src(ctx->hand_hour, 0, 0, &wd_gximg_watchface_hand_hour_bundle);

    ctx->hand_second = lv_img_gximg_create(p_window);
    lv_obj_set_pos(ctx->hand_second, DISP_HOR_RES / 2, DISP_VER_RES / 2);
    lv_img_gximg_set_pivot_and_src(ctx->hand_second, 0, 0, &wd_gximg_watchface_hand_second_bundle);
#else
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
#endif

    // Center
    lv_obj_t *center_point = lv_img_create(p_window);
    lv_img_set_src(center_point, &wd_img_watchface_center_point);
    lv_obj_center(center_point);

    return p_window;
}
