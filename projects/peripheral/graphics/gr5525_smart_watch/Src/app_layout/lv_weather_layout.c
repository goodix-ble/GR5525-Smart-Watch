#include "app_key.h"
#include "app_rtc.h"
#include "lvgl.h"
#include "lv_img_dsc_list.h"

#include <stdio.h>

enum
{
    WEATHER_TYPE_CLEAR = 0,
    WEATHER_TYPE_CLOUDY,
    WEATHER_TYPE_DRIZZLE,
    WEATHER_TYPE_DUST,
    WEATHER_TYPE_GLOOMY,
    WEATHER_TYPE_HEAVY_RAIN,
    WEATHER_TYPE_HEAVY_SNOW,
    WEATHER_TYPE_LIGHT_SNOW,
    WEATHER_TYPE_MODERATE_RAIN,
    WEATHER_TYPE_MODERATE_SNOW,
} weather_type_t;

typedef struct
{
    uint8_t min_tem_val;
    uint8_t max_tem_val;
} future_tem_val_t;

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static lv_obj_t *p_time = NULL;
static lv_obj_t *p_loc_label = NULL;
static lv_obj_t *p_weather_icon = NULL;
static lv_obj_t *p_cur_tem_label = NULL;
static lv_obj_t *p_weather_label = NULL;
static lv_obj_t *p_next_three_day_weather_icon[3] = {NULL};
static lv_obj_t *p_next_three_day_tem_val[3] = {NULL};
static lv_obj_t *p_next_three_day_date[3] = {NULL};
static lv_timer_t *p_refr_timer = NULL;

static const char *const WEATHER_TYPES[] = {"Clear", "Cloudy", "Drizzle", "Dust", "Gloomy", "Heavy Rain", "Heavy Snow", "Light Snow", "Moderate Rain", "Moderate Snow"};
static const lv_img_dsc_t *WEATHER_ICONS[] = {&wd_img_weather_clear,
                                              &wd_img_weather_clear,
                                              &wd_img_weather_clear,
                                              &wd_img_weather_clear,
                                              &wd_img_weather_clear,
                                              &wd_img_weather_clear,
                                              &wd_img_weather_clear,
                                              &wd_img_weather_clear};

/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */
static void time_update_cb(lv_timer_t *p_timer);

/*
 * PUBLIC VARS DEFINITIONS
 *****************************************************************************************
 */
lv_obj_t *lv_weather_layout_create(lv_obj_t *parent_tv_obj);

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
static void weather_layout_event_cb(lv_event_t * e)
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

// static void _weather_icon_update(lv_obj_t *p_obj, uint8_t weather_type)
// {
//     switch (weather_type)
//     {
//     case WEATHER_TYPE_CLEAR:
//         lv_img_set_src(p_obj, &wd_img_weather_Clear);
//         break;
//     case WEATHER_TYPE_CLOUDY:
//         lv_img_set_src(p_obj, &wd_img_weather_cloudy);
//         break;
//     case WEATHER_TYPE_DRIZZLE:
//         lv_img_set_src(p_obj, &wd_img_weather_Drizzle);
//         break;
//     case WEATHER_TYPE_DUST:
//         lv_img_set_src(p_obj, &wd_img_weather_dust);
//         break;
//     case WEATHER_TYPE_GLOOMY:
//         lv_img_set_src(p_obj, &wd_img_weather_gloomy);
//         break;
//     case WEATHER_TYPE_HEAVY_RAIN:
//         lv_img_set_src(p_obj, &wd_img_weather_heavy_rain);
//         break;
//     case WEATHER_TYPE_HEAVY_SNOW:
//         lv_img_set_src(p_obj, &wd_img_weather_heavy_snow);
//         break;
//     case WEATHER_TYPE_LIGHT_SNOW:
//         lv_img_set_src(p_obj, &wd_img_weather_light_snow);
//         break;
//     case WEATHER_TYPE_MODERATE_RAIN:
//         lv_img_set_src(p_obj, &wd_img_weather_moderate_rain);
//         break;
//     case WEATHER_TYPE_MODERATE_SNOW:
//         lv_img_set_src(p_obj, &wd_img_weather_moderate_snow);
//         break;
//     default:
//         break;
//     }
// }

// static void _weather_label_update(lv_obj_t *p_obj, uint8_t weather_type)
// {
//     switch (weather_type)
//     {
//     case WEATHER_TYPE_CLEAR:
//         lv_label_set_text(p_obj, "");
//         break;
//     case WEATHER_TYPE_CLOUDY:
//         lv_label_set_text(p_obj, "Cloudy");
//         break;
//     case WEATHER_TYPE_DRIZZLE:
//         lv_label_set_text(p_obj, &wd_img_weather_Drizzle);
//         break;
//     case WEATHER_TYPE_DUST:
//         lv_label_set_text(p_obj, &wd_img_weather_dust);
//         break;
//     case WEATHER_TYPE_GLOOMY:
//         lv_label_set_text(p_obj, &wd_img_weather_gloomy);
//         break;
//     case WEATHER_TYPE_HEAVY_RAIN:
//         lv_label_set_text(p_obj, &wd_img_weather_heavy_rain);
//         break;
//     case WEATHER_TYPE_HEAVY_SNOW:
//         lv_label_set_text(p_obj, &wd_img_weather_heavy_snow);
//         break;
//     case WEATHER_TYPE_LIGHT_SNOW:
//         lv_label_set_text(p_obj, &wd_img_weather_light_snow);
//         break;
//     case WEATHER_TYPE_MODERATE_RAIN:
//         lv_label_set_text(p_obj, &wd_img_weather_moderate_rain);
//         break;
//     case WEATHER_TYPE_MODERATE_SNOW:
//         lv_label_set_text(p_obj, &wd_img_weather_moderate_snow);
//         break;
//     default:
//         break;
//     }
// }

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
void weather_layout_cur_weather_update(char *p_loc, int16_t tem_val, uint8_t weather_type)
{
    // Location label
    lv_label_set_text(p_loc_label, p_loc);

    // Weather icon
    lv_img_set_src(p_weather_icon, WEATHER_ICONS[weather_type]);

    // Temperature value
    lv_label_set_text_fmt(p_cur_tem_label, "%d", tem_val);

    // Weather label
    lv_label_set_text(p_weather_label, WEATHER_TYPES[weather_type]);
}

void weather_layout_future_weather_update(uint8_t *p_weather_type, future_tem_val_t *p_tem_val)
{
    app_rtc_time_t time;
    app_rtc_get_time(&time);

    // Location label
    for (int i = 0; i < 3; i++)
    {
        lv_img_set_src(p_next_three_day_weather_icon[i], WEATHER_ICONS[p_weather_type[i]]);
        lv_label_set_text_fmt(p_next_three_day_tem_val[i], "%d/%d°", p_tem_val[i].min_tem_val, p_tem_val[i].max_tem_val);
        lv_label_set_text_fmt(p_next_three_day_date[i], "%02d/%02d", time.mon, time.date+i+1);
    }
}

void lv_weather_layout_destory(void)
{
    if (p_refr_timer != NULL)
    {
        lv_timer_del(p_refr_timer);
        p_refr_timer = NULL;
    }
}

lv_obj_t *lv_weather_layout_create(lv_obj_t *parent_tv_obj)
{
#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_obj_enable_style_refresh(false);
    lv_disp_t * disp = lv_obj_get_disp(parent_tv_obj);
    lv_disp_enable_invalidation(disp, false);
#endif
    lv_obj_t *p_window = lv_obj_create(parent_tv_obj);
    lv_obj_set_style_pad_all(p_window, 0, 0);

    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
//    lv_obj_clear_flag(p_window, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(p_window, lv_color_black(), 0);
#if LV_GDX_PATCH_REMOVE_BORDER
#else
    lv_obj_set_style_border_width(p_window, 0, 0);
#endif

    // Weather Title
    lv_obj_t *p_title = lv_label_create(p_window);
    lv_obj_set_size(p_title, 120, 20);
    lv_obj_set_style_text_align(p_title, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(p_title, "Weather");
    lv_obj_set_style_bg_color(p_title, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_title, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_title, lv_color_make(0x53, 0xAC, 0xFF), 0);
    lv_obj_align(p_title, LV_ALIGN_TOP_LEFT, 16, 4);

    // Timestamp Label
    p_time = lv_label_create(p_window);
    lv_obj_set_size(p_time, 120, 20);
    lv_obj_set_style_text_align(p_time, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_text_fmt(p_time, "%02d:%02d%s", 8, 0, "AM");
    lv_obj_set_style_bg_color(p_time, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_time, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_time, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align(p_time, LV_ALIGN_TOP_RIGHT, -16, 4);

    // Location Label
    p_loc_label = lv_label_create(p_window);
    lv_obj_set_size(p_loc_label, 240, 20);
    lv_obj_set_style_text_align(p_loc_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(p_loc_label, "New York");
    lv_obj_set_style_bg_color(p_loc_label, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_loc_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_loc_label, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align(p_loc_label, LV_ALIGN_TOP_MID, 0, 40);

    // Weather Icon
    p_weather_icon = lv_img_create(p_window);
    lv_img_set_src(p_weather_icon, &wd_img_weather_clear);
    lv_obj_align(p_weather_icon, LV_ALIGN_CENTER, -56, -40);

    // Temperature Label
    p_cur_tem_label = lv_label_create(p_window);
    lv_obj_set_size(p_cur_tem_label, 80, 40);
    lv_obj_set_style_text_align(p_cur_tem_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_cur_tem_label, "%d℃", 12);
    lv_obj_set_style_bg_color(p_cur_tem_label, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_cur_tem_label, &lv_font_montserrat_40, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_cur_tem_label, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align_to(p_cur_tem_label, p_weather_icon, LV_ALIGN_OUT_RIGHT_MID, 12, 0);

    // Weather Label
    p_weather_label = lv_label_create(p_window);
    lv_obj_set_size(p_weather_label, 240, 20);
    lv_obj_set_style_text_align(p_weather_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(p_weather_label, "Cloudy");
    lv_obj_set_style_bg_color(p_weather_label, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_weather_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_weather_label, lv_color_make(0x99, 0x99, 0x99), 0);
    lv_obj_align(p_weather_label, LV_ALIGN_CENTER, 0, 2);

    // Dividing Line
    static lv_point_t line_pts[] = {{0, 0}, {200, 0}};
    lv_obj_t *p_line = lv_line_create(p_window);
    lv_obj_set_style_line_width(p_line, 2, 0);
    lv_obj_set_style_line_color(p_line, lv_color_make(0x4B, 0x4B, 0x4B), 0);
    lv_obj_set_style_line_rounded(p_line, 1, 0);
    lv_line_set_points(p_line, line_pts, 2);
    lv_obj_align_to(p_line, p_weather_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);

    // Tomorrow's weather icon
    p_next_three_day_weather_icon[0] = lv_img_create(p_window);
    lv_img_set_src(p_next_three_day_weather_icon[0], &wd_img_weather_clear);
    lv_obj_align(p_next_three_day_weather_icon[0], LV_ALIGN_BOTTOM_LEFT, 20, -64);

    p_next_three_day_tem_val[0] = lv_label_create(p_window);
    lv_obj_set_size(p_next_three_day_tem_val[0], 80, 20);
    lv_obj_set_style_text_align(p_next_three_day_tem_val[0], LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_next_three_day_tem_val[0], "%d/%d°", 12, 25);
    lv_obj_set_style_bg_color(p_next_three_day_tem_val[0], lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_next_three_day_tem_val[0], &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_next_three_day_tem_val[0], lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align_to(p_next_three_day_tem_val[0], p_next_three_day_weather_icon[0], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    p_next_three_day_date[0] = lv_label_create(p_window);
    lv_obj_set_size(p_next_three_day_date[0], 80, 20);
    lv_obj_set_style_text_align(p_next_three_day_date[0], LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_next_three_day_date[0], "%02d/%02d", 12, 7);
    lv_obj_set_style_bg_color(p_next_three_day_date[0], lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_next_three_day_date[0], &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_next_three_day_date[0], lv_color_make(0x66, 0x66, 0x66), 0);
    lv_obj_align_to(p_next_three_day_date[0], p_next_three_day_weather_icon[0], LV_ALIGN_OUT_BOTTOM_MID, 0, 38);

    // Weather icon of the day after tomorrow
    p_next_three_day_weather_icon[1] = lv_img_create(p_window);
    lv_img_set_src(p_next_three_day_weather_icon[1], &wd_img_weather_clear);
    lv_obj_align(p_next_three_day_weather_icon[1], LV_ALIGN_BOTTOM_MID, 0, -64);

    p_next_three_day_tem_val[1] = lv_label_create(p_window);
    lv_obj_set_size(p_next_three_day_tem_val[1], 80, 20);
    lv_obj_set_style_text_align(p_next_three_day_tem_val[1], LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_next_three_day_tem_val[1], "%d/%d°", 12, 25);
    lv_obj_set_style_bg_color(p_next_three_day_tem_val[1], lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_next_three_day_tem_val[1], &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_next_three_day_tem_val[1], lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align_to(p_next_three_day_tem_val[1], p_next_three_day_weather_icon[1], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    p_next_three_day_date[1] = lv_label_create(p_window);
    lv_obj_set_size(p_next_three_day_date[1], 80, 20);
    lv_obj_set_style_text_align(p_next_three_day_date[1], LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_next_three_day_date[1], "%02d/%02d", 12, 8);
    lv_obj_set_style_bg_color(p_next_three_day_date[1], lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_next_three_day_date[1], &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_next_three_day_date[1], lv_color_make(0x66, 0x66, 0x66), 0);
    lv_obj_align_to(p_next_three_day_date[1], p_next_three_day_weather_icon[1], LV_ALIGN_OUT_BOTTOM_MID, 0, 38);

    // Weather icon of three days from now
    p_next_three_day_weather_icon[2] = lv_img_create(p_window);
    lv_img_set_src(p_next_three_day_weather_icon[2], &wd_img_weather_clear);
    lv_obj_align(p_next_three_day_weather_icon[2], LV_ALIGN_BOTTOM_RIGHT, -20, -64);

    p_next_three_day_tem_val[2] = lv_label_create(p_window);
    lv_obj_set_size(p_next_three_day_tem_val[2], 80, 20);
    lv_obj_set_style_text_align(p_next_three_day_tem_val[2], LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_next_three_day_tem_val[2], "%d/%d°", 12, 25);
    lv_obj_set_style_bg_color(p_next_three_day_tem_val[2], lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_next_three_day_tem_val[2], &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_next_three_day_tem_val[2], lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_align_to(p_next_three_day_tem_val[2], p_next_three_day_weather_icon[2], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    p_next_three_day_date[2] = lv_label_create(p_window);
    lv_obj_set_size(p_next_three_day_date[2], 80, 20);
    lv_obj_set_style_text_align(p_next_three_day_date[2], LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(p_next_three_day_date[2], "%02d/%02d", 12, 9);
    lv_obj_set_style_bg_color(p_next_three_day_date[2], lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_text_font(p_next_three_day_date[2], &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(p_next_three_day_date[2], lv_color_make(0x66, 0x66, 0x66), 0);
    lv_obj_align_to(p_next_three_day_date[2], p_next_three_day_weather_icon[2], LV_ALIGN_OUT_BOTTOM_MID, 0, 38);

    #if LV_GDX_PATCH_USE_FAST_TILEVIEW
    time_update_cb(NULL);
    lv_obj_add_event_cb(p_window, weather_layout_event_cb, LV_EVENT_ALL, NULL);
    #else
    if (NULL == p_refr_timer)
    {
        p_refr_timer = lv_timer_create(time_update_cb, 60000, NULL);
    }
    #endif

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
