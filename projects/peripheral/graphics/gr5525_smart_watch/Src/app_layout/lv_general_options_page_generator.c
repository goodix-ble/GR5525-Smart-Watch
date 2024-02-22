#include "lv_general_options_page_generator.h"

#define TITLE_HIGHT (60)

#define TIP_ITEM_WIDTH (250)
#define TIP_ITEM_HEIGHT (70)

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */
static const lv_style_const_prop_t WINDOW_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_WIDTH(DISP_VER_RES),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_HEIGHT(TITLE_HIGHT),
    LV_STYLE_CONST_X(0),
    LV_STYLE_CONST_Y((TITLE_HIGHT - 30)), // lv_font_montserrat_28.line_height = 30
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_28),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t TIP_STYLE_PROPS[] = {
    LV_STYLE_CONST_X(60),
    LV_STYLE_CONST_Y(100),
    LV_STYLE_CONST_WIDTH(TIP_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT(TIP_ITEM_HEIGHT),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t STATUS_STYLE_PROPS[] = {
    LV_STYLE_CONST_X(60),
    LV_STYLE_CONST_Y(150),
    LV_STYLE_CONST_WIDTH(TIP_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT(TIP_ITEM_HEIGHT),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t YES_BTN_STYLE[] = {
    LV_STYLE_CONST_WIDTH(220),
    LV_STYLE_CONST_HEIGHT(52),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(10, 115, 221)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_RADIUS(10),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t YES_LABEL_STYLE[] = {
    LV_STYLE_CONST_WIDTH(220),
    LV_STYLE_CONST_HEIGHT(20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(10, 115, 221)),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t NO_BTN_STYLE[] = {
    LV_STYLE_CONST_WIDTH(220),
    LV_STYLE_CONST_HEIGHT(52),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(41, 41, 41)),
    LV_STYLE_CONST_BG_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_RADIUS(10),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t NO_LABEL_STYLE[] = {
    LV_STYLE_CONST_WIDTH(220),
    LV_STYLE_CONST_HEIGHT(20),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(41, 41, 41)),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(GOPTIONS_WINDOW_STYLE, WINDOW_STYLE_PROPS);
LV_STYLE_CONST_INIT(GOPTIONS_TITLE_STYLE, TITLE_STYLE_PROPS);
LV_STYLE_CONST_INIT(GOPTIONS_TIP_STYLE, TIP_STYLE_PROPS);
LV_STYLE_CONST_INIT(GOPTIONS_STATUS_STYLE, STATUS_STYLE_PROPS);
LV_STYLE_CONST_INIT(GOPTIONS_YES_BTN_STYLE, YES_BTN_STYLE);
LV_STYLE_CONST_INIT(GOPTIONS_YES_LABEL_STYLE, YES_LABEL_STYLE);
LV_STYLE_CONST_INIT(GOPTIONS_NO_BTN_STYLE, NO_BTN_STYLE);
LV_STYLE_CONST_INIT(GOPTIONS_NO_LABEL_STYLE, NO_LABEL_STYLE);

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_general_options_page_create(lv_obj_t *parent, general_option_page_dsc_t *dsc)
{
    lv_obj_t *p_window = lv_obj_create(parent);
    lv_obj_add_style(p_window, (lv_style_t *)&GOPTIONS_WINDOW_STYLE, 0);

    // Title
    if (dsc->title)
    {
        lv_obj_t *p_title = lv_label_create(p_window);
        lv_obj_add_style(p_title, (lv_style_t *)&GOPTIONS_TITLE_STYLE, 0);
        lv_label_set_text_static(p_title, dsc->title);
    }

    // Tips Area
    if (dsc->tip_text)
    {
        lv_obj_t *p_label = lv_label_create(p_window);
        lv_obj_add_style(p_label, (lv_style_t *)&GOPTIONS_TIP_STYLE, 0);
        lv_label_set_text(p_label, dsc->tip_text);
    }

    // Status Area
    if (dsc->status_text)
    {
        lv_obj_t *p_status = lv_label_create(p_window);
        lv_obj_add_style(p_status, (lv_style_t *)&GOPTIONS_STATUS_STYLE, 0);
        lv_label_set_text(p_status, dsc->status_text);
        lv_obj_add_flag(p_status, LV_OBJ_FLAG_HIDDEN);
    }

    // Options Area
    lv_obj_t *p_yes_btn = lv_btn_create(p_window);
    lv_obj_add_style(p_yes_btn, (lv_style_t *)&GOPTIONS_YES_BTN_STYLE, LV_STATE_DEFAULT);
    lv_obj_align(p_yes_btn, LV_ALIGN_CENTER, 0, 34);
    lv_obj_t *p_yes_label = lv_label_create(p_yes_btn);
    lv_obj_add_style(p_yes_label, (lv_style_t *)&GOPTIONS_YES_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_label_set_text(p_yes_label, "YES");
    lv_obj_align(p_yes_label, LV_ALIGN_CENTER, 0, 0);
    if (dsc->callback)
    {
        uintptr_t __click_yes = 1;
        lv_obj_add_event_cb(p_yes_btn, dsc->callback, LV_EVENT_CLICKED, (void *)__click_yes);
    }

    lv_obj_t *p_no_btn = lv_btn_create(p_window);
    lv_obj_add_style(p_no_btn, (lv_style_t *)&GOPTIONS_NO_BTN_STYLE, LV_STATE_DEFAULT);
    lv_obj_align(p_no_btn, LV_ALIGN_CENTER, 0, 104);
    lv_obj_t *p_no_label = lv_label_create(p_no_btn);
    lv_label_set_text(p_no_label, "NO");
    lv_obj_add_style(p_no_label, (lv_style_t *)&GOPTIONS_NO_LABEL_STYLE, LV_STATE_DEFAULT);
    lv_obj_align(p_no_label, LV_ALIGN_CENTER, 0, 0);
    if (dsc->callback)
    {
        uintptr_t __click_yes = 0;
        lv_obj_add_event_cb(p_no_btn, dsc->callback, LV_EVENT_CLICKED, (void *)__click_yes);
    }

    return p_window;
}

void lv_general_options_page_status_text_set(lv_obj_t *window, const char *status_text)
{
    lv_label_set_text(window->spec_attr->children[2], status_text);
#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO
    update_line_info(window->spec_attr->children[2]);
#endif // LV_GDX_PATCH_CACHE_LABEL_LINE_INFO
}

void lv_general_options_page_show_status(lv_obj_t *window)
{
    for (int i = 0; i < window->spec_attr->child_cnt; i++)
    {
        if (i == 0 || i == 2)
        {
            lv_obj_clear_flag(window->spec_attr->children[i], LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(window->spec_attr->children[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

