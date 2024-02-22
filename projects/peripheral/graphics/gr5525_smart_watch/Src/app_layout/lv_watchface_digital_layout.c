#include "lvgl.h"
#include "lv_font.h"
#include "lv_img_dsc_list.h"
#include "lv_imgdigits.h"
#include "watchface_manager.h"

#include "app_rtc.h"

#include <stdio.h>

#define SHOW_ROTATED_DIGITS (1)
#define BAR_WIDTH (52)
#define BAR_HEIGHT (180)

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
typedef struct
{
    lv_obj_t *time_label;
    lv_obj_t *am_pm_label;
    lv_obj_t *date_label;
    lv_obj_t *week_label;
    lv_obj_t *sport_bar;
    lv_obj_t *calories_bar;
    lv_obj_t *step_bar;
    lv_obj_t *heartrate_box;
    lv_obj_t *battery_box;
} watchface_digital_ctx_t;

DECLARE_WF_REFR_CTX_FUNC(watchface_digital_ctx_t);

static const lv_style_const_prop_t TIME_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_40),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x0, 0x0, 0x0)),
    LV_STYLE_CONST_TEXT_LINE_SPACE(10),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t DATE_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xCB, 0xCB, 0xCB)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x0, 0x0, 0x0)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t WEEK_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0x94, 0x94, 0x94)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x0, 0x0, 0x0)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t GX_BAR_MAIN_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(BAR_WIDTH),
    LV_STYLE_CONST_HEIGHT(BAR_HEIGHT),
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_CONST_RADIUS(0),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x26, 0x26, 0x26)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t GX_BAR_SPORT_INDIC_STYLE_PROPS[] = {
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_CONST_RADIUS(0),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0xD9, 0x06, 0x57)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t GX_BAR_CAL_INDIC_STYLE_PROPS[] = {
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_CONST_RADIUS(0),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0xFE, 0x73, 0x15)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t GX_BAR_STEP_INDIC_STYLE_PROPS[] = {
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
    LV_STYLE_CONST_RADIUS(0),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x21, 0xD5, 0x94)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t HR_STATUS_BOX_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(120),
    LV_STYLE_CONST_HEIGHT(30),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_RIGHT),
    LV_STYLE_CONST_PAD_RIGHT(25),
    LV_STYLE_CONST_PAD_TOP(7),
    LV_STYLE_CONST_RADIUS(3),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(245, 23, 78)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x0, 0x0, 0x0)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t BAT_STATUS_BOX_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(120),
    LV_STYLE_CONST_HEIGHT(30),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_RIGHT),
    LV_STYLE_CONST_PAD_RIGHT(25),
    LV_STYLE_CONST_PAD_TOP(7),
    LV_STYLE_CONST_RADIUS(3),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(156, 220, 254)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0x0, 0x0, 0x0)),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(TIME_LABEL_STYLE, TIME_LABEL_STYLE_PROPS);
LV_STYLE_CONST_INIT(DATE_LABEL_STYLE, DATE_LABEL_STYLE_PROPS);
LV_STYLE_CONST_INIT(WEEK_LABEL_STYLE, WEEK_LABEL_STYLE_PROPS);
LV_STYLE_CONST_INIT(GX_BAR_MAIN_STYLE, GX_BAR_MAIN_STYLE_PROPS);
LV_STYLE_CONST_INIT(GX_BAR_SPORT_INDIC_STYLE, GX_BAR_SPORT_INDIC_STYLE_PROPS);
LV_STYLE_CONST_INIT(GX_BAR_CAL_INDIC_STYLE, GX_BAR_CAL_INDIC_STYLE_PROPS);
LV_STYLE_CONST_INIT(GX_BAR_STEP_INDIC_STYLE, GX_BAR_STEP_INDIC_STYLE_PROPS);
LV_STYLE_CONST_INIT(HR_STATUS_BOX_STYLE, HR_STATUS_BOX_STYLE_PROPS);
LV_STYLE_CONST_INIT(BAT_STATUS_BOX_STYLE, BAT_STATUS_BOX_STYLE_PROPS);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static lv_obj_t *gx_bar_create(lv_obj_t *obj)
{
    lv_obj_t *p_bar = lv_obj_create(obj);
    p_bar->user_data = lv_mem_alloc(4);
    lv_obj_add_style(p_bar, (lv_style_t *)&GX_BAR_MAIN_STYLE, 0);

    lv_obj_t *p_bar_indic = lv_obj_create(p_bar);
    lv_obj_set_size(p_bar_indic, BAR_WIDTH, BAR_HEIGHT);
    lv_obj_align(p_bar_indic, LV_ALIGN_BOTTOM_MID, 0, 0);
    return p_bar;
}

static void gx_bar_set_indic_style(lv_obj_t *obj, lv_style_t *p_style)
{
    lv_obj_t *p_bar_indic = obj->spec_attr->children[0];
    lv_obj_add_style(p_bar_indic, p_style, 0);
}

static void gx_bar_set_range(lv_obj_t *obj, int range)
{
    memcpy(obj->user_data, &range, sizeof(int));
}

static void gx_bar_set_value(lv_obj_t *obj, int value)
{
    lv_obj_t *p_bar_indic = obj->spec_attr->children[0];
    int height = value * 180 / *(int *)(obj->user_data);
    lv_obj_set_height(p_bar_indic, height);
    lv_obj_align(p_bar_indic, LV_ALIGN_BOTTOM_MID, 0, 0);
}

static void watchface_digital_refr_cb(lv_timer_t *p_timer)
{
    // Refresh Time Related

    // lv_label_set_text_fmt(s_time, "%02d\n%02d", time.hour, time.min);
    // lv_label_set_text(s_ampm, time.hour > 12 ? "PM" : "AM");
    // lv_label_set_text_fmt(s_date, ISTR("%02d/%02d\n星期%s"), time.mon, time.date, ISTR(WEEKDAY_TO_CHN_LUT[time.week]));
    watchface_digital_ctx_t *ctx = wf_mngr_get_refr_ctx(p_timer);

    gx_bar_set_range(ctx->sport_bar, 100);
    gx_bar_set_value(ctx->sport_bar, 80);
    gx_bar_set_range(ctx->calories_bar, 100);
    gx_bar_set_value(ctx->calories_bar, 80);
    gx_bar_set_range(ctx->step_bar, 100);
    gx_bar_set_value(ctx->step_bar, 80);
}

static void watchface_digital_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_DELETE)
    {
        watchface_digital_ctx_t *ctx = (watchface_digital_ctx_t *)e->current_target->user_data;
        lv_mem_free(ctx->sport_bar->user_data);
        lv_mem_free(ctx->calories_bar->user_data);
        lv_mem_free(ctx->step_bar->user_data);
        ctx->sport_bar->user_data = NULL;
        ctx->calories_bar->user_data = NULL;
        ctx->step_bar->user_data = NULL;
    }
}

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_watchface_digital_clock_layout_create(lv_obj_t *parent)
{
    lv_gximg_init();

    // Background
    lv_obj_t *p_window = lv_obj_create(parent);
    lv_obj_remove_style_all(p_window);
    lv_obj_clear_flag(p_window, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_set_scrollbar_mode(p_window, LV_SCROLLBAR_MODE_OFF);
    lv_obj_center(p_window);

    watchface_digital_ctx_t *ctx = wf_mngr_create_refr_ctx(p_window, 500, watchface_digital_refr_cb);

    lv_obj_t *p_icon;

    // Time
    ctx->time_label = lv_label_create(p_window);
    lv_obj_set_size(ctx->time_label, 50, 90);
    lv_obj_add_style(ctx->time_label, (lv_style_t *)&TIME_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ctx->time_label, "%02d\n%02d", 12, 30);
    lv_obj_set_pos(ctx->time_label, 54, 44);

    // AM/PM
    ctx->am_pm_label = lv_label_create(p_window);
    lv_obj_set_size(ctx->am_pm_label, 40, 20);
    lv_obj_add_style(ctx->am_pm_label, (lv_style_t *)&DATE_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_label_set_text(ctx->am_pm_label, "PM");
    lv_obj_align_to(ctx->am_pm_label, ctx->time_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 46);

    // Date
    ctx->date_label = lv_label_create(p_window);
    lv_obj_set_size(ctx->date_label, 50, 20);
    lv_obj_add_style(ctx->date_label, (lv_style_t *)&DATE_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ctx->date_label, "%02d/%02d", 12, 24);
    lv_obj_set_pos(ctx->date_label, 54, 220);

    // Week
    ctx->week_label = lv_label_create(p_window);
    lv_obj_set_size(ctx->week_label, 100, 20);
    lv_obj_add_style(ctx->week_label, (lv_style_t *)&WEEK_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ctx->week_label, "%s", "Tuesday");
    lv_obj_set_pos(ctx->week_label, 54, 256);

    // Heart Rate
    ctx->heartrate_box = lv_label_create(p_window);
    lv_obj_set_size(ctx->heartrate_box, 160, 24);
    lv_obj_align_to(ctx->heartrate_box, ctx->time_label, LV_ALIGN_OUT_RIGHT_TOP, 20, -2);
    lv_obj_add_style(ctx->heartrate_box, (lv_style_t *)&HR_STATUS_BOX_STYLE, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ctx->heartrate_box, "%d", 80);

    p_icon = lv_img_create(ctx->heartrate_box);
    lv_img_set_src(p_icon, &wd_img_vivid_clock_hr);
    lv_obj_align(p_icon, LV_ALIGN_LEFT_MID, 50, 2);

    // Battery
    ctx->battery_box = lv_label_create(p_window);
    lv_obj_set_size(ctx->battery_box, 160, 24);
    lv_obj_align_to(ctx->battery_box, ctx->time_label, LV_ALIGN_OUT_RIGHT_TOP, 35, 22);
    lv_obj_add_style(ctx->battery_box, (lv_style_t *)&BAT_STATUS_BOX_STYLE, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ctx->battery_box, "%d%%", 80);

    p_icon = lv_img_create(ctx->battery_box);
    lv_img_set_src(p_icon, &wd_img_quick_elc_100);
    lv_obj_align(p_icon, LV_ALIGN_LEFT_MID, 46, 2);

    // Bars
    ctx->sport_bar = gx_bar_create(p_window);
    lv_obj_set_pos(ctx->sport_bar, 144, 100);
    gx_bar_set_indic_style(ctx->sport_bar, (lv_style_t *)&GX_BAR_SPORT_INDIC_STYLE);
    gx_bar_set_range(ctx->sport_bar, 100);
    gx_bar_set_value(ctx->sport_bar, 50);

    p_icon = lv_img_create(p_window);
    lv_img_set_src(p_icon, &wd_img_digital2_heart_small);
    lv_obj_align_to(p_icon, ctx->sport_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    ctx->calories_bar = gx_bar_create(p_window);
    lv_obj_set_pos(ctx->calories_bar, 202, 100);
    gx_bar_set_indic_style(ctx->calories_bar, (lv_style_t *)&GX_BAR_CAL_INDIC_STYLE);
    gx_bar_set_range(ctx->calories_bar, 100);
    gx_bar_set_value(ctx->calories_bar, 70);

    p_icon = lv_img_create(p_window);
    lv_img_set_src(p_icon, &wd_img_digital2_consume_small);
    lv_obj_align_to(p_icon, ctx->calories_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    ctx->step_bar = gx_bar_create(p_window);
    lv_obj_set_pos(ctx->step_bar, 260, 100);
    gx_bar_set_indic_style(ctx->step_bar, (lv_style_t *)&GX_BAR_STEP_INDIC_STYLE);
    gx_bar_set_range(ctx->step_bar, 100);
    gx_bar_set_value(ctx->step_bar, 30);

    p_icon = lv_img_create(p_window);
    lv_img_set_src(p_icon, &wd_img_digital2_exercise_small);
    lv_obj_align_to(p_icon, ctx->step_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_add_event_cb(p_window, watchface_digital_event_cb, LV_EVENT_ALL, NULL);

    return p_window;
}
