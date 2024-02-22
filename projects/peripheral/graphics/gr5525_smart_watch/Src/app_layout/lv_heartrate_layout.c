#include "app_key.h"
#include "app_rtc.h"
#include "lvgl.h"
#include "lv_img_dsc_list.h"
#include "lv_port_gximg.h"
#include "lv_layout_router.h"
#include <stdio.h>

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static lv_obj_t *   _img_hr_animation_big     = NULL;
static lv_obj_t *   _img_hr_animation_small   = NULL;
static lv_obj_t *   _label_hr_realtime_val    = NULL;
static lv_obj_t *   _label_hr_max_val         = NULL;
static lv_obj_t *   _label_hr_min_val         = NULL;
static lv_timer_t * _hb_animation_timer       = NULL;
static bool         _is_show_big_hb_img       = true;
static bool         _is_hr_checked            = false;
//static bool         _hr_scene_switch_flag     = false;

static const lv_style_const_prop_t WINDOW_STYLE_PROPS[] = {
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_PAD_TOP(0),
    LV_STYLE_CONST_PAD_BOTTOM(0),
    LV_STYLE_CONST_PAD_LEFT(0),
    LV_STYLE_CONST_PAD_RIGHT(0),
#if LV_GDX_PATCH_REMOVE_BORDER
#else
    LV_STYLE_CONST_BORDER_WIDTH(0),
#endif
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t REALTIME_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_40),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t UNIT_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0x00, 0x9B, 0x47)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t MIN_MAX_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(HEARTRATE_WINDOW_STYLE, WINDOW_STYLE_PROPS);
LV_STYLE_CONST_INIT(REALTIME_LABEL_STYLE, REALTIME_LABEL_STYLE_PROPS);
LV_STYLE_CONST_INIT(UNIT_LABEL_STYLE, UNIT_LABEL_STYLE_PROPS);
LV_STYLE_CONST_INIT(MIN_MAX_LABEL_STYLE, MIN_MAX_LABEL_STYLE_PROPS);

/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */
static void         _set_hr_realtime_val(uint16_t val);
static void         _set_hr_min_max_val(lv_obj_t *p_obj, uint16_t val);
// static void         _set_hr_min_val(uint16_t val);
static void         _set_heart_beat_animation(bool active);
static void         _hb_timer_callback(lv_timer_t * tmr);
//static void         _clear_and_reset_state(void);
//static bool         _gesture_event_handler(lv_dir_t dir);

/*
 * PUBLIC VARS DEFINITIONS
 *****************************************************************************************
 */
lv_obj_t *lv_heartrate_layout_create(lv_obj_t * parent_tv_obj);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static void _set_hr_realtime_val(uint16_t val) {

    if(!_label_hr_realtime_val)
        return;

    if(val == 0) {
        _is_hr_checked = true;
        lv_label_set_text_static(_label_hr_realtime_val, "--");
    } else {
        _is_hr_checked = true;
        lv_label_set_text_fmt(_label_hr_realtime_val, "%d", val);
    }
    return;
}

static void _set_hr_min_max_val(lv_obj_t *p_obj, uint16_t val) {
    if(!_label_hr_min_val)
        return;

    if(val == 0 || val == 0xFF) {
        lv_label_set_text_static(p_obj, "--");
    } else {
        lv_label_set_text_fmt(p_obj, "%d", val);
    }
    return;
}

static void _set_heart_beat_animation(bool active) {
    if(active) {
        //set animation alive
        lv_timer_resume(_hb_animation_timer);
    } else {
        lv_timer_pause(_hb_animation_timer);
    }
}

static void _show_big_hr_image(bool state) {
    if(state) {
        lv_obj_add_flag(_img_hr_animation_small, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_img_hr_animation_big, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(_img_hr_animation_small, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_img_hr_animation_big, LV_OBJ_FLAG_HIDDEN);
    }
}

static void _hb_timer_callback(lv_timer_t * tmr)
{
    _show_big_hr_image(_is_show_big_hb_img);

    if(_is_hr_checked) {
        _is_show_big_hb_img = !_is_show_big_hb_img;
    } else {
        _is_show_big_hb_img = true;
    }

    // Set the simulated heartrate value.
    _set_hr_realtime_val(80);
    _set_hr_min_max_val(_label_hr_max_val, 140);
    _set_hr_min_max_val(_label_hr_min_val, 70);
}

//static void _clear_and_reset_state(void)
//{
//    _set_hr_realtime_val(0);
//    _set_hr_min_max_val(_label_hr_max_val, 0);
//    _set_hr_min_max_val(_label_hr_max_val, 0);
//    _is_show_big_hb_img = true;
//    _show_big_hr_image(true);
//    _set_heart_beat_animation(false);
//}

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
static void heartrate_event_cb(lv_event_t * e)
{
    if (e->code == LV_EVENT_DELETE)
    {
        if (_hb_animation_timer)
        {
            lv_timer_del(_hb_animation_timer);
            _hb_animation_timer = NULL;
        }
    }
    else if (e->code == LV_EVENT_READY)
    {
        if (_hb_animation_timer)
        {
            _set_heart_beat_animation(true);
        }
        else
        {
            _hb_animation_timer = lv_timer_create(_hb_timer_callback, 350, NULL);
        }
    }
    else if (e->code == LV_EVENT_CANCEL)
    {
        if (_hb_animation_timer)
        {
            lv_timer_pause(_hb_animation_timer);
        }
    }
}

static void btn_event_cb(lv_event_t * e) {
    lv_obj_t *p_target = lv_event_get_target(e);
    lv_layout_router_show_heartrate_chart(p_target);
}
#else
static void btn_event_cb(lv_event_t * e) {
}
#endif

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_heartrate_layout_create(lv_obj_t * parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t * disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif

    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_add_style(p_window, (lv_style_t *)&HEARTRATE_WINDOW_STYLE, 0);

    _img_hr_animation_big = lv_img_create(p_window);
    lv_img_set_src(_img_hr_animation_big, &wd_img_heart_icon_70);
    lv_obj_set_pos(_img_hr_animation_big, 145, 40);

    _img_hr_animation_small = lv_img_create(p_window);
    lv_img_set_src(_img_hr_animation_small, &wd_img_heart_icon_50);
    lv_obj_set_pos(_img_hr_animation_small, 155, 50);

    _label_hr_realtime_val = lv_label_create(p_window);
    lv_obj_add_flag(_label_hr_realtime_val, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(_label_hr_realtime_val, btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_label_set_text_fmt(_label_hr_realtime_val, "%d", 80);
    lv_obj_add_style(_label_hr_realtime_val, (lv_style_t *)&REALTIME_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_obj_align(_label_hr_realtime_val, LV_ALIGN_TOP_MID, 0, 125);

    lv_obj_t *p_label_hr_unit = lv_label_create(p_window);
    lv_label_set_text_static(p_label_hr_unit, "bpm");
    lv_obj_add_style(p_label_hr_unit, (lv_style_t *)&UNIT_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_obj_align_to(p_label_hr_unit, _label_hr_realtime_val, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    lv_obj_t *p_img_hr_max = lv_img_create(p_window);
    lv_img_set_src(p_img_hr_max, &wd_img_heartrate_upper);
    lv_obj_set_pos(p_img_hr_max, 60, 260);

    _label_hr_max_val = lv_label_create(p_window);
    lv_label_set_text_fmt(_label_hr_max_val, "%d", 120);
    lv_obj_add_style(_label_hr_max_val, (lv_style_t *)&MIN_MAX_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_obj_set_pos(_label_hr_max_val, 90, 250);

    lv_obj_t *p_img_hr_min = lv_img_create(p_window);
    lv_img_set_src(p_img_hr_min, &wd_img_heartrate_lower);
    lv_obj_set_pos(p_img_hr_min, 240, 260);

    _label_hr_min_val = lv_label_create(p_window);
    lv_label_set_text_fmt(_label_hr_min_val, "%d", 60);
    lv_obj_add_style(_label_hr_min_val, (lv_style_t *)&MIN_MAX_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_obj_set_pos(_label_hr_min_val, 270, 250);

#if LV_GDX_PATCH_USE_FAST_TILEVIEW
    _hb_timer_callback(NULL);
    lv_obj_add_event_cb(p_window, heartrate_event_cb, LV_EVENT_ALL, NULL);
#else
    if(NULL == _hb_animation_timer){
        _hb_animation_timer = lv_timer_create(_hb_timer_callback, 350, NULL);
    }
//    _clear_and_reset_state();
    _set_heart_beat_animation(true);
#endif


#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
