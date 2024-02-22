#include "app_key.h"
#include "app_rtc.h"
#include "app_log.h"
#include "lvgl.h"
#include "lv_img_dsc_list.h"


#include <stdio.h>

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static lv_obj_t   *p_date            = NULL;
static lv_obj_t   *p_week            = NULL;
static lv_obj_t   *p_char_icon       = NULL;
static lv_obj_t   *p_char_val        = NULL;
static lv_obj_t   *p_connect_icon    = NULL;
static lv_obj_t   *p_dont_off_btn    = NULL;
static lv_obj_t   *p_raise_wrist_btn = NULL;
static lv_obj_t   *p_brightness_btn  = NULL;
static lv_obj_t   *p_info_btn        = NULL;
static lv_obj_t   *p_flashlight_btn  = NULL;
static lv_obj_t   *p_setting_btn     = NULL;
static lv_timer_t *p_refr_timer      = NULL;

static const char *const p_week_str[] = {"MON", "TUE", "WED", "THUR", "FRI", "SAT", "SUN"};

static const lv_style_const_prop_t LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(STATUS_LABEL_STYLE, LABEL_STYLE_PROPS);


/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */
static void _btn_event_cb(lv_event_t *e);
//static lv_obj_t *_status_bar_btn_create(lv_obj_t *p_obj, void *p_src);

/*
 * PUBLIC VARS DEFINITIONS
 *****************************************************************************************
 */
lv_obj_t *lv_status_layout_create(lv_obj_t * parent_tv_obj);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static void _btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *p_target = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        lv_state_t state = lv_obj_get_state(p_target);
        if (state == LV_STATE_CHECKED)
        {
            if (p_dont_off_btn == p_target)
            {
               //lv_img_set_src(p_target->spec_attr->children[0], &wd_img_quick_tab_dont_on);
               lv_img_set_src(p_target, &wd_img_quick_tab_dont_on);
            }
            else if (p_raise_wrist_btn == p_target)
            {
                //lv_img_set_src(p_target->spec_attr->children[0], &wd_img_quick_tab_taiwan_on);
                lv_img_set_src(p_target, &wd_img_quick_tab_taiwan_on);
            }
        }
        else
        {
            if (p_dont_off_btn == p_target)
            {
                //lv_img_set_src(p_target->spec_attr->children[0], &wd_img_quick_tab_dont_off);
                lv_img_set_src(p_target, &wd_img_quick_tab_dont_off);
            }
            else if (p_raise_wrist_btn == p_target)
            {
                //lv_img_set_src(p_target->spec_attr->children[0], &wd_img_quick_tab_dont_off);
                lv_img_set_src(p_target, &wd_img_quick_tab_taiwan_off);
            }
        }

        #if LV_GDX_PATCH_USE_FAST_TILEVIEW
        if (p_target == p_setting_btn) {
            #include "lv_layout_router.h"
            lv_layout_router_show_settings(p_target);
        }
        #endif
    }
}

static lv_obj_t *_status_bar_btn_create(lv_obj_t *p_obj, void *p_src)
{
    lv_obj_t *p_img = lv_img_create(p_obj);
    lv_obj_set_size(p_img, 80, 80);
    lv_img_set_src(p_img, p_src);
    lv_obj_add_flag(p_img, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(p_img, _btn_event_cb, LV_EVENT_CLICKED, NULL);
    return p_img;
}

static void status_bar_refr_cb(lv_timer_t *p_timer)
{
    app_rtc_time_t time;
    app_rtc_get_time(&time);

    lv_label_set_text_fmt(p_date, "%02d/%02d", time.mon, time.date);
    // printf("%d", time.week);
    lv_label_set_text(p_week, p_week_str[time.week]);

    uint8_t bat_power = 100;
    const lv_img_dsc_t *p_elec_img;
    if (bat_power >= 90)
    {
        p_elec_img = &wd_img_quick_elc_100;
    }
    // else if (bat_power >= 80)
    // {
    //     p_elec_img = &wd_img_quick_elc_80;
    // }
    // else if (bat_power >= 60)
    // {
    //     p_elec_img = &wd_img_quick_elc_60;
    // }
    // else if (bat_power >= 40)
    // {
    //     p_elec_img = &wd_img_quick_elc_40;
    // }
    // else if (bat_power >= 20)
    // {
    //     p_elec_img = &wd_img_quick_elc_20;
    // }
    // else
    // {
    //     p_elec_img = &wd_img_quick_elc_5;
    // }
    lv_img_set_src(p_char_icon, p_elec_img);
    lv_label_set_text_fmt(p_char_val, "%d%%", bat_power);
}

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void status_layout_event_cb(lv_event_t * e)
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
            p_refr_timer = lv_timer_create(status_bar_refr_cb, 15000, NULL);
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

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_status_layout_destory(void)
{
    return NULL;
}

lv_obj_t *lv_status_layout_create(lv_obj_t * parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t * disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

#define NAMED_OBJ(x) x->user_data = #x

    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_set_style_pad_all(p_window, 0, 0);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_set_style_bg_color(p_window, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(p_window, LV_OPA_COVER, 0);
    NAMED_OBJ(p_window);

    // Date
    p_date = lv_label_create(p_window);
    lv_label_set_text_fmt(p_date, "%02d/%02d", 11, 28);
    lv_obj_add_style(p_date, (lv_style_t *)&STATUS_LABEL_STYLE, 0);
    lv_obj_align(p_date, LV_ALIGN_TOP_MID, 0, 20);
    NAMED_OBJ(p_date);

    // Week
    p_week = lv_label_create(p_window);
    lv_label_set_text(p_week, p_week_str[0]);
    lv_obj_add_style(p_week, (lv_style_t *)&STATUS_LABEL_STYLE, 0);
    lv_obj_align(p_week, LV_ALIGN_TOP_MID, 0, 56);
    NAMED_OBJ(p_week);

    // Electricity value
    p_char_val = lv_label_create(p_window);
    lv_label_set_text_fmt(p_char_val, "%d%%", 100);
    lv_obj_add_style(p_char_val, (lv_style_t *)&STATUS_LABEL_STYLE, 0);
    lv_obj_align(p_char_val, LV_ALIGN_TOP_MID, -12, 92);
    NAMED_OBJ(p_char_val);

    // Electricity icon
    p_char_icon = lv_img_create(p_window);
    lv_img_set_src(p_char_icon, &wd_img_quick_elc_100);
    lv_obj_align_to(p_char_icon, p_char_val, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    NAMED_OBJ(p_char_icon);

    // Conection On/Off icon
    p_connect_icon = lv_img_create(p_window);
    lv_img_set_src(p_connect_icon, &wd_img_connect_icon_off);
    lv_obj_align_to(p_connect_icon, p_char_val, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    NAMED_OBJ(p_connect_icon);

    // Create a panel
    lv_obj_t *p_panel = lv_obj_create(p_window);
    lv_obj_set_style_pad_all(p_panel, 0, LV_STATE_DEFAULT);
    lv_obj_set_size(p_panel, 272, 176);
    lv_obj_set_style_bg_color(p_panel, lv_color_black(), 0);
    lv_obj_align(p_panel, LV_ALIGN_BOTTOM_MID, 0, -60);
    NAMED_OBJ(p_panel);

    p_dont_off_btn = _status_bar_btn_create(p_panel, (void *)&wd_img_quick_tab_dont_off);
    lv_obj_align(p_dont_off_btn, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_clear_flag(p_dont_off_btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    NAMED_OBJ(p_dont_off_btn);

    p_raise_wrist_btn = _status_bar_btn_create(p_panel, (void *)&wd_img_quick_tab_taiwan_off);
    lv_obj_align(p_raise_wrist_btn, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_clear_flag(p_raise_wrist_btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    NAMED_OBJ(p_raise_wrist_btn);

    p_brightness_btn = _status_bar_btn_create(p_panel, (void *)&wd_img_quick_tab_brightness);
    lv_obj_align(p_brightness_btn, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_clear_flag(p_brightness_btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    NAMED_OBJ(p_brightness_btn);

    p_info_btn = _status_bar_btn_create(p_panel, (void *)&wd_img_quick_tab_about);
    lv_obj_align(p_info_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_clear_flag(p_info_btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    NAMED_OBJ(p_info_btn);

    p_flashlight_btn = _status_bar_btn_create(p_panel, (void *)&wd_img_quick_tab_flashlight);
    lv_obj_align(p_flashlight_btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_clear_flag(p_flashlight_btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    NAMED_OBJ(p_flashlight_btn);

    p_setting_btn = _status_bar_btn_create(p_panel, (void *)&wd_img_quick_tab_setting);
    lv_obj_align(p_setting_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_clear_flag(p_setting_btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    NAMED_OBJ(p_setting_btn);

    #if LV_GDX_PATCH_USE_FAST_TILEVIEW
    status_bar_refr_cb(NULL);
    lv_obj_add_event_cb(p_window, status_layout_event_cb, LV_EVENT_ALL, NULL);
    #else
    if (NULL == p_refr_timer)
    {
        p_refr_timer = lv_timer_create(status_bar_refr_cb, 15000, NULL);
    }
    #endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
