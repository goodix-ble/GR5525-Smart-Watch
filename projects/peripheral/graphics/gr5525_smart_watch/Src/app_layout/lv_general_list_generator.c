#include "lv_general_list_generator.h"

#define TITLE_HIGHT (60)

#define LIST_AREA_PAD_TOP (40)
#define LIST_AREA_PAD_BOTTOM (60)

#define LIST_ITEM_WIDTH (250)
#define LIST_ITEM_HEIGHT (60)
#define LIST_ITEM_MARGIN (20)

#define LIST_ITEM_TITLE_PAD_LEFT (15)

#define ENABLE_REACTIVE_LABEL_POSITION 0

#define LV_STYLE_CONST_LOCAL_INIT(style_name) static const lv_style_t style_name = { .v_p = { .const_props = style_name##_PROPS }, .has_group = 0xFF, .prop1 = LV_STYLE_PROP_ANY }

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

static const lv_style_const_prop_t LIST_AREA_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(DISP_HOR_RES),
    LV_STYLE_CONST_HEIGHT(DISP_VER_RES - TITLE_HIGHT),
    LV_STYLE_CONST_Y(TITLE_HIGHT),
    LV_STYLE_CONST_PAD_TOP(LIST_AREA_PAD_TOP),
    LV_STYLE_CONST_PAD_BOTTOM(LIST_AREA_PAD_BOTTOM),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_STYLE_PROPS[] = {
    // LV_STYLE_CONST_WIDTH(LIST_ITEM_WIDTH),
    LV_STYLE_CONST_HEIGHT(LIST_ITEM_HEIGHT),
    LV_STYLE_PROP_INV,
};

static const lv_style_const_prop_t LIST_ITEM_TITLE_STYLE_PROPS[] = {
    LV_STYLE_CONST_WIDTH(164),
    LV_STYLE_CONST_TEXT_COLOR(LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)),
    LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_20),
    LV_STYLE_CONST_BG_COLOR(LV_COLOR_MAKE(0, 0, 0)),
    
    LV_STYLE_PROP_INV,
};

LV_STYLE_CONST_INIT(GLIST_WINDOW_STYLE, WINDOW_STYLE_PROPS);
LV_STYLE_CONST_INIT(GLIST_TITLE_STYLE, TITLE_STYLE_PROPS);
LV_STYLE_CONST_INIT(GLIST_LIST_AREA_STYLE, LIST_AREA_STYLE_PROPS);
LV_STYLE_CONST_INIT(GLIST_LIST_ITEM_STYLE, LIST_ITEM_STYLE_PROPS);
LV_STYLE_CONST_INIT(GLIST_LIST_ITEM_TITLE_STYLE, LIST_ITEM_TITLE_STYLE_PROPS);

#if ENABLE_REACTIVE_LABEL_POSITION
static lv_coord_t s_min_title_offset;
#endif // ENABLE_REACTIVE_LABEL_POSITION

static lv_obj_t *create_list_item(lv_obj_t *parent, lv_coord_t item_width, const general_list_item_t *item, const lv_img_dsc_t *suffix);
static void list_item_clicked_event_cb(lv_event_t *event);

lv_obj_t *lv_general_list_create(lv_obj_t *parent, general_list_dsc_t *dsc)
{
    lv_obj_t *p_window = lv_obj_create(parent);
    lv_obj_add_style(p_window, (lv_style_t *)&GLIST_WINDOW_STYLE, 0);

    lv_coord_t item_width = LIST_ITEM_WIDTH;
    if (dsc->item_width != 0)
    {
        item_width = dsc->item_width;
    }

    // Title
    if (dsc->title)
    {
        lv_obj_t *p_title = lv_label_create(p_window);
        lv_obj_add_style(p_title, (lv_style_t *)&GLIST_TITLE_STYLE, 0);
        lv_label_set_text_static(p_title, dsc->title);
    }

    // List Area
    lv_obj_t *p_list_area = lv_obj_create(p_window);
    lv_obj_add_style(p_list_area, (lv_style_t *)&GLIST_LIST_AREA_STYLE, 0);

    if (dsc->item_num > 3)
    {
        lv_obj_add_flag(p_list_area, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_scrollbar_mode(p_list_area, LV_SCROLLBAR_MODE_OFF);
    }

    lv_obj_add_event_cb(p_list_area, list_item_clicked_event_cb, LV_EVENT_ALL, (void *)dsc->callback);
#if ENABLE_REACTIVE_LABEL_POSITION
    s_min_title_offset = DISP_HOR_RES;
#endif // ENABLE_REACTIVE_LABEL_POSITION

    // List Item
    for (uint16_t i = 0; i < dsc->item_num; i++)
    {
        lv_obj_t *p_item = create_list_item(p_list_area, item_width, &dsc->items[i], dsc->suffix);
        p_item->user_data = ((void *)(uintptr_t)(i));
        lv_obj_add_flag(p_item, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_set_pos(p_item, (DISP_HOR_RES - item_width) / 2, i * (LIST_ITEM_HEIGHT + LIST_ITEM_MARGIN));
    }

#if ENABLE_REACTIVE_LABEL_POSITION
    int32_t label_id = -1;
    if (dsc->suffix)
    {
        label_id = -2;
        s_min_title_offset -= dsc->suffix->header.w / 2;
    }

    for (uint16_t i = 0; i < dsc->item_num; i++)
    {
        lv_obj_t *p_item = lv_obj_get_child(p_list_area, i);
        lv_obj_t *p_item_title = lv_obj_get_child(p_item, label_id);
        lv_obj_align(p_item_title, LV_ALIGN_LEFT_MID, s_min_title_offset, 0);
    }
#endif // ENABLE_REACTIVE_LABEL_POSITION

    return p_window;
}

static lv_obj_t *create_list_item(lv_obj_t *parent, lv_coord_t item_width, const general_list_item_t *item, const lv_img_dsc_t *suffix)
{
    // Item
    lv_obj_t *p_item = lv_obj_create(parent);
    lv_obj_add_style(p_item, (lv_style_t *)&GLIST_LIST_ITEM_STYLE, 0);
    lv_obj_set_width(p_item, item_width);

    // Icon
    if (item->icon)
    {
        lv_obj_t *p_icon = lv_img_create(p_item);
        lv_img_set_src(p_icon, item->icon);
        lv_obj_set_y(p_icon, (LIST_ITEM_HEIGHT - item->icon->header.h) / 2);
    }

    // Title
    if (item->title)
    {
        lv_obj_t *p_title = lv_label_create(p_item);
        lv_obj_add_style(p_title, (lv_style_t *)&GLIST_LIST_ITEM_TITLE_STYLE, 0);
        lv_label_set_text_static(p_title, item->title);
#if ENABLE_REACTIVE_LABEL_POSITION
        lv_obj_refr_size(p_title);
        lv_coord_t w = lv_obj_get_width(p_title);
        lv_coord_t toffset;
        if (item->icon)
        {
            toffset = (item_width + item->icon->header.w - w) / 2;
        }
        else
        {
            toffset = (item_width - w) / 2;
        }
        if (toffset < s_min_title_offset)
        {
            s_min_title_offset = toffset;
        }
#else
        if (item->icon)
        {
            lv_obj_align(p_title, LV_ALIGN_LEFT_MID, item->icon->header.w + LIST_ITEM_TITLE_PAD_LEFT, 0);
        }
        else
        {
            lv_obj_align(p_title, LV_ALIGN_LEFT_MID, LIST_ITEM_TITLE_PAD_LEFT, 0);
        }
#endif // ENABLE_REACTIVE_LABEL_POSITION
    }

    // Suffix
    if (suffix)
    {
        lv_obj_t *p_suffix = lv_img_create(p_item);
        lv_img_set_src(p_suffix, suffix);
        lv_obj_align(p_suffix, LV_ALIGN_RIGHT_MID, 0, 0);
    }

    return p_item;
}

static void list_item_clicked_event_cb(lv_event_t *event)
{
    general_list_event_callback_t cb = (general_list_event_callback_t)event->user_data;
    if (event->target == event->current_target)
    {
        return;
    }

    if (event->code == LV_EVENT_SHORT_CLICKED)
    {
        cb((uint16_t)((uintptr_t)event->target->user_data), event);
    }
    else if (event->code == LV_EVENT_LONG_PRESSED)
    {
        cb((uint16_t)((uintptr_t)event->target->user_data), event);
    }
}
