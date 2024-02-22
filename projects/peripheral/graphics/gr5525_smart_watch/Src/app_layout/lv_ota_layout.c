#include "app_key.h"
#include "app_rtc.h"
#include "app_log.h"
#include "lvgl.h"
#include "lv_img_dsc_list.h"
#include "gr55xx_sys_sdk.h"
#include "custom_config.h"

#include <stdio.h>

#ifndef VERSION
#define VERSION 0x0100
#endif // VERSION

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static lv_obj_t   *p_conn_status  = NULL;

static const lv_style_const_prop_t OTA_TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_40),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t OTA_TEXT_STYLE_PROPS[] = {
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(OTA_TITLE_STYLE, OTA_TITLE_STYLE_PROPS);
LV_STYLE_CONST_INIT(OTA_TEXT_STYLE, OTA_TEXT_STYLE_PROPS);

/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */

/*
 * PUBLIC VARS DEFINITIONS
 *****************************************************************************************
 */
lv_obj_t *lv_ota_layout_create(lv_obj_t * parent_tv_obj);

/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_ota_layout_destory(void)
{
    return NULL;
}

lv_obj_t *lv_ota_layout_create(lv_obj_t * parent_tv_obj)
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
    
    // OTA Title
    lv_obj_t *p_title = lv_label_create(p_window);
    lv_label_set_text_fmt(p_title, "%s", "OTA");
    lv_obj_add_style(p_title, (lv_style_t *)&OTA_TITLE_STYLE, LV_STATE_DEFAULT);
    lv_obj_align(p_title, LV_ALIGN_TOP_MID, 0, 120);

    // Current Version
    sdk_version_t sdk_version;
    sys_sdk_verison_get(&sdk_version);
    lv_obj_t *p_cur_version = lv_label_create(p_window);
    lv_label_set_text_fmt(p_cur_version, "Ver:0x%04X\nsdk:%d.%d.%d-%6x",
                          VERSION,
                          sdk_version.major,
                          sdk_version.minor,
                          sdk_version.build,
                          sdk_version.commit_id);
    lv_obj_add_style(p_cur_version, (lv_style_t *)&OTA_TEXT_STYLE, LV_STATE_DEFAULT);
    lv_obj_align_to(p_cur_version, p_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);

    // OTA Status
    p_conn_status = lv_label_create(p_window);
    lv_obj_set_size(p_conn_status, DISP_HOR_RES, LV_SIZE_CONTENT);
    lv_obj_add_style(p_conn_status, (lv_style_t *)&OTA_TEXT_STYLE, LV_STATE_DEFAULT);
    lv_obj_align_to(p_conn_status, p_cur_version, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);

#if LV_GDX_PATCH_DISABLE_STYLE_REFRESH && !LV_GDX_PATCH_USE_FAST_TILEVIEW
    lv_disp_enable_invalidation(disp, true);
    lv_obj_enable_style_refresh(true);
    lv_obj_refresh_style(p_window, LV_PART_ANY, LV_STYLE_PROP_ANY);
#endif

    return p_window;
}
