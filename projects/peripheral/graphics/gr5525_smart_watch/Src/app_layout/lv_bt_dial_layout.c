#include "lvgl.h"
#include "lv_font.h"
#include "lv_layout_router.h"
#include "lv_img_dsc_list.h"
#include "lv_layout_router.h"

#include "bt_phonecall_controller.h"

#define ARRAY_SIZE(arr) (sizeof((arr))/sizeof((arr)[0]))

typedef enum
{
    DIAL_BUTTON_NUM_1 = 0,
    DIAL_BUTTON_NUM_2,
    DIAL_BUTTON_NUM_3,
    DIAL_BUTTON_NUM_4,
    DIAL_BUTTON_NUM_5,
    DIAL_BUTTON_NUM_6,
    DIAL_BUTTON_NUM_7,
    DIAL_BUTTON_NUM_8,
    DIAL_BUTTON_NUM_9,
    DIAL_BUTTON_BACKSPACE,
    DIAL_BUTTON_NUM_0,
    DIAL_BUTTON_CALL,
    DIAL_BUTTON_MAX,
} dial_button_type_t;

static const lv_img_dsc_t *const DIAL_BUTTON_LIST[] = {
    &wd_img_dial_number_1,
    &wd_img_dial_number_2,
    &wd_img_dial_number_3,
    &wd_img_dial_number_4,
    &wd_img_dial_number_5,
    &wd_img_dial_number_6,
    &wd_img_dial_number_7,
    &wd_img_dial_number_8,
    &wd_img_dial_number_9,
    &wd_img_dial_backspace,
    &wd_img_dial_number_0,
    &wd_img_bt_device_phonecall,
};

/*
 * STATIC VARS DEFINITIONS
 *****************************************************************************************
 */

static char s_dial_number[16] = {0};
static lv_obj_t *s_dial_label;

const static lv_style_const_prop_t DIAL_LABEL_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(200),
    LV_STYLE_CONST_HEIGHT(30),
    LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(255, 255, 255)),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_28),
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(DIAL_LABEL_STYLE, DIAL_LABEL_STYLE_PROPS);

/*
 * STATIC METHODS DECLARATION
 *****************************************************************************************
 */
static lv_obj_t *_create_button(lv_obj_t *parent, dial_button_type_t type);

static void _append(uint8_t num);
static void _backspace(void);
static char* _getdialnumber(void);

static void dial_number_click_cb(lv_event_t *event);
static void dial_button_click_cb(lv_event_t *event);

/*
 * PUBLIC VARS DEFINITIONS
 *****************************************************************************************
 */

/*
 * GLOBAL METHODS IMPLEMENT
 *****************************************************************************************
 */
lv_obj_t *lv_bt_dial_layout_create(lv_obj_t *parent)
{
    lv_obj_t *p_window = lv_obj_create(parent);
    // lv_obj_remove_style_all(p_window);
    lv_obj_set_size(p_window, DISP_HOR_RES, DISP_VER_RES);
    lv_obj_set_style_radius(p_window, LV_RADIUS_CIRCLE, 0);

    // Dial Number
    s_dial_label = lv_label_create(p_window);
    lv_obj_align(s_dial_label, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_add_style(s_dial_label, (lv_style_t *)&DIAL_LABEL_STYLE, 0);
    s_dial_number[0] = 0;
    lv_label_set_text_static(s_dial_label, s_dial_number);

    // Number Button Matrix
    for (uint8_t i = DIAL_BUTTON_NUM_1; i < DIAL_BUTTON_MAX; i++)
    {
        _create_button(p_window, (dial_button_type_t)i);
    }

    return p_window;
}


/*
 * STATIC METHODS IMPLEMENT
 *****************************************************************************************
 */
static lv_obj_t *_create_button(lv_obj_t *parent, dial_button_type_t type)
{
    uint8_t idx = (uint8_t)type;
    lv_obj_t *btn = lv_img_create(parent);
    lv_obj_align(btn, LV_ALIGN_CENTER, 60 + (idx % 3) * 85 - 140, 75 + (idx / 3) * 65 - 155);
    lv_img_set_src(btn, DIAL_BUTTON_LIST[idx]);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    if (type == DIAL_BUTTON_BACKSPACE || type == DIAL_BUTTON_CALL)
    {
        lv_obj_add_event_cb(btn, dial_button_click_cb, LV_EVENT_SHORT_CLICKED, (void *)type);
    }
    else
    {
        lv_obj_add_event_cb(btn, dial_number_click_cb, LV_EVENT_SHORT_CLICKED, (void *)((uintptr_t)(type > 9 ? 0 : (idx + 1))));
    }
    return btn;
}

static inline void _append(uint8_t num)
{
    uint8_t i = 0;
    while(s_dial_number[i] != 0)
    {
        i++;
    }

    if (i < (ARRAY_SIZE(s_dial_number) - 1))
    {
        s_dial_number[i] = '0' + num;
        s_dial_number[++i] = 0;
    }
    if (i > 11)
    {
        lv_obj_set_style_text_font(s_dial_label, &lv_font_montserrat_20, 0);
    }
    lv_obj_invalidate(s_dial_label);
#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
    update_line_info(s_dial_label);
#endif
}

static inline void _backspace(void)
{
    uint8_t i = 0;
    while(s_dial_number[i] != 0)
    {
        i++;
    }
    if (i > 0)
    {
        s_dial_number[--i] = 0;
    }
    if (i < 12)
    {
        lv_obj_set_style_text_font(s_dial_label, &lv_font_montserrat_28, 0);
    }
    lv_obj_invalidate(s_dial_label);
#if LV_GDX_PATCH_CACHE_LABEL_LINE_INFO > 0u
    update_line_info(s_dial_label);
#endif
}

static inline char* _getdialnumber(void)
{
    return s_dial_number;
}

static void dial_number_click_cb(lv_event_t *event)
{
    uintptr_t i = (uintptr_t)event->user_data;
    _append((uint8_t)i);
}

static void dial_button_click_cb(lv_event_t *event)
{
    dial_button_type_t btn = (dial_button_type_t)((uintptr_t)event->user_data);

    switch(btn)
    {
        case DIAL_BUTTON_BACKSPACE:
            _backspace();
            break;

        case DIAL_BUTTON_CALL:
            if (s_dial_number[1] == '\0')
            {
                if (s_dial_number[0] == '0')
                {
                    strcpy(s_dial_number, "15902863478");
                }
                else if (s_dial_number[0] == '1')
                {
                    strcpy(s_dial_number, "18190823771");
                }
            }

            printf("Make Phone Call to %s\n", _getdialnumber());
            //Always jump to calling layout for better "user experience"
            bt_phonecall_controller_make_call(s_dial_number);
            lv_layout_router_show_calling_layout(NULL);
            // lv_obj_add_state(event->current_target, LV_STATE_DISABLED);
            break;
    }
}
