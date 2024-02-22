#include "app_key.h"
#include "app_rtc.h"
#include "lvgl.h"
#include "lv_img_dsc_list.h"
#include "lv_port_gximg.h"

#include <stdio.h>

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static lv_obj_t   *p_time       = NULL;
// static lv_obj_t   *p_kcal_arc   = NULL;
// static lv_obj_t   *p_step_arc   = NULL;
// static lv_obj_t   *p_dis_arc    = NULL;
static lv_obj_t   *p_kcal_val   = NULL;
static lv_obj_t   *p_step_val   = NULL;
static lv_obj_t   *p_dis_val    = NULL;
static lv_timer_t *p_refr_timer = NULL;

/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */
static void time_update_cb(lv_timer_t *p_timer);
static void _set_day_kcals(int16_t val, int16_t max);
static void _set_day_steps(int16_t val, int16_t max);
static void _set_day_distance(float val, int16_t max);

/*
 * PUBLIC VARS DEFINITIONS
 *****************************************************************************************
 */
lv_obj_t *lv_activity_layout_create(lv_obj_t * parent_tv_obj);

// LV_GXIMG_DECLARE(wd_img_calories_bar_gximg_22);
// LV_GXIMG_DECLARE(wd_img_distance_bar_gximg_12);
// LV_GXIMG_DECLARE(wd_img_steps_bar_gximg_34);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static void time_update_cb(lv_timer_t *p_timer)
{
    // if (p_timer == NULL)
    // {
    //     return;
    // }

    app_rtc_time_t time;
    app_rtc_get_time(&time);
    lv_label_set_text_fmt(p_time, "%02d:%02d%s", time.hour, time.min, time.hour > 12 ? "PM" : "AM");
}

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void activity_layout_event_cb(lv_event_t * e)
{
    if (e->code == LV_EVENT_DELETE)
    {
        if (p_refr_timer)
        {
            lv_timer_del(p_refr_timer);
            p_refr_timer = NULL;
        }
    }   
    else if (e->code == LV_EVENT_READY)
    {
        if (p_refr_timer)
        {
            lv_timer_resume(p_refr_timer);
        }
        else
        {
            p_refr_timer = lv_timer_create(time_update_cb, 60000, NULL);
        }
    }   
    else if (e->code == LV_EVENT_CANCEL)
    {
        if (p_refr_timer)
        {
            lv_timer_pause(p_refr_timer);
        }
    }   
}
#endif

static void _set_day_kcals(int16_t val, int16_t max)
{
    if (val > max)
    {
        val = max;
    }
    lv_label_set_text_fmt(p_kcal_val, "%04d", val);
//    lv_arc_set_range(p_kcal_arc, 0, max);
//    lv_arc_set_value(p_kcal_arc, val);
}

static void _set_day_steps(int16_t val, int16_t max)
{
    if (val > max)
    {
        val = max;
    }
    lv_label_set_text_fmt(p_step_val, "%04d", val);
//    lv_arc_set_range(p_step_arc, 0, max);
//    lv_arc_set_value(p_step_arc, val);
}

static void _set_day_distance(float val, int16_t max)
{
    if (val > max)
    {
        val = max;
    }
    lv_label_set_text_fmt(p_dis_val, "%0.1f", val);
//    lv_arc_set_range(p_dis_arc, 0, max);
//    lv_arc_set_value(p_dis_arc, val);
}

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_activity_layout_create(lv_obj_t * parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t * disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_set_style_pad_all(p_window, 0, 0);

    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_set_style_bg_color(p_window, lv_color_black(), 0);
#if LV_GDX_PATCH_REMOVE_BORDER
#else
    lv_obj_set_style_border_width(p_window, 0, 0);
#endif

    // Activity Title
    lv_obj_t *p_title = lv_label_create(p_window);
    lv_label_set_text(p_title, "Activity");
    lv_obj_set_style_bg_color(p_title, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_title, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_title, lv_color_make(0xFF, 0x8D, 0x3A), 0);
    lv_obj_align(p_title, LV_ALIGN_TOP_LEFT, 16, 4);
    lv_obj_set_size(p_title, 120, 20);

    // Timestamp label
    p_time = lv_label_create(p_window);
    lv_label_set_text_fmt(p_time, "%02d:%02d%s", 8, 0, "AM");
    lv_obj_set_style_bg_color(p_time, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_time, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_time, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align(p_time, LV_ALIGN_TOP_RIGHT, -16, 4);
    lv_obj_set_size(p_title, 120, 20);

    // TODO
    // Add gradient effect.
    // p_kcal_arc = lv_img_create(p_window);
    // lv_obj_set_pos(p_kcal_arc, 16, 36);
    // lv_img_set_src(p_kcal_arc, &wd_img_calories_bar_gximg_22);

    // p_step_arc = lv_img_create(p_window);
    // lv_obj_set_pos(p_step_arc, 46, 66);
    // lv_img_set_src(p_step_arc, &wd_img_steps_bar_gximg_34);

    // p_dis_arc = lv_img_create(p_window);
    // lv_obj_set_pos(p_dis_arc, 76, 96);
    // lv_img_set_src(p_dis_arc, &wd_img_distance_bar_gximg_12);

    // Calories icon
    lv_obj_t *p_cal_icon = lv_img_create(p_window);
    lv_img_set_src(p_cal_icon, &wd_img_calories_icon);
    lv_obj_align(p_cal_icon, LV_ALIGN_BOTTOM_LEFT, 32, -74);

    p_kcal_val = lv_label_create(p_window);
    lv_obj_set_size(p_kcal_val, 80, 20);
    lv_obj_set_style_text_align(p_kcal_val, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_kcal_val, "%04d", 2345);
    lv_obj_set_style_bg_color(p_kcal_val, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_kcal_val, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_kcal_val, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align_to(p_kcal_val, p_cal_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

    lv_obj_t *p_kcal_unit = lv_label_create(p_window);
    lv_obj_set_size(p_kcal_unit, 80, 20);
    lv_obj_set_style_text_align(p_kcal_unit, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(p_kcal_unit, "Kcal");
    lv_obj_set_style_bg_color(p_kcal_unit, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_kcal_unit, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_kcal_unit, lv_color_make(0x99, 0x99, 0x99), 0);
    lv_obj_align_to(p_kcal_unit, p_cal_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 38);

    // Step icon
    lv_obj_t *p_step_icon = lv_img_create(p_window);
    lv_img_set_src(p_step_icon, &wd_img_step_icon);
    lv_obj_align(p_step_icon, LV_ALIGN_BOTTOM_MID, 0, -74);

    p_step_val = lv_label_create(p_window);
    lv_obj_set_size(p_step_val, 80, 20);
    lv_obj_set_style_text_align(p_step_val, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_step_val, "%04d", 23450);
    lv_obj_set_style_bg_color(p_step_val, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_step_val, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_step_val, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align_to(p_step_val, p_step_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

    lv_obj_t *p_step_unit = lv_label_create(p_window);
    lv_obj_set_size(p_step_unit, 80, 20);
    lv_obj_set_style_text_align(p_step_unit, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(p_step_unit, "Steps");
    lv_obj_set_style_bg_color(p_step_unit, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_step_unit, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_step_unit, lv_color_make(0x99, 0x99, 0x99), 0);
    lv_obj_align_to(p_step_unit, p_step_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 38);

    // Distance icon
    lv_obj_t *p_dis_icon = lv_img_create(p_window);
    lv_img_set_src(p_dis_icon, &wd_img_distance_icon);
    lv_obj_align(p_dis_icon, LV_ALIGN_BOTTOM_RIGHT, -32, -74);

    p_dis_val = lv_label_create(p_window);
    lv_obj_set_size(p_dis_val, 80, 20);
    lv_obj_set_style_text_align(p_dis_val, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_dis_val, "%0.1f", 2.3);
    lv_obj_set_style_bg_color(p_dis_val, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_dis_val, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_dis_val, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align_to(p_dis_val, p_dis_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

    lv_obj_t *p_dis_unit = lv_label_create(p_window);
    lv_obj_set_size(p_dis_unit, 80, 20);
    lv_obj_set_style_text_align(p_dis_unit, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(p_dis_unit, "KM");
    lv_obj_set_style_bg_color(p_dis_unit, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_dis_unit, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_dis_unit, lv_color_make(0x99, 0x99, 0x99), 0);
    lv_obj_align_to(p_dis_unit, p_dis_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 38);

    #if LV_GDX_PATCH_USE_FAST_TILEVIEW
    time_update_cb(NULL);
    lv_obj_add_event_cb(p_window, activity_layout_event_cb, LV_EVENT_ALL, NULL);
    #else
    if (NULL == p_refr_timer)
    {
        p_refr_timer = lv_timer_create(time_update_cb, 60000, NULL);
    }
    #endif

    _set_day_kcals(2345, 5000);
    _set_day_steps(23450, 32767);
    _set_day_distance(2.3, 5);


#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
