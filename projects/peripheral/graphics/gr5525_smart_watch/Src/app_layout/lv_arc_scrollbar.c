/**
 * @file lv_arc_scrollbar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_arc_scrollbar.h"

#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_arc_scrollbar_class

#define IS_X_IN(x, area) ((x) >= ((area)->x1) && (x) <= ((area)->x2))
#define IS_Y_IN(y, area) ((y) >= ((area)->y1) && (y) <= ((area)->y2))
#define IS_POINT_IN(x, y, area) (IS_X_IN(x, area) && IS_Y_IN(y, area))

#define EXPAND_AREA(a) (a)->x1, (a)->y1, (a)->x2, (a)->y2

#define Y_MAX (60)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_arc_scrollbar_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void lv_arc_scrollbar_destructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void lv_arc_scrollbar_event(const lv_obj_class_t *class_p, lv_event_t *evt);
static void draw_main(lv_event_t *evt);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_arc_scrollbar_class = {
    .constructor_cb = lv_arc_scrollbar_constructor,
    .destructor_cb = lv_arc_scrollbar_destructor,
    .event_cb = lv_arc_scrollbar_event,
    .width_def = DISP_HOR_RES,
    .height_def = DISP_VER_RES,
    .instance_size = sizeof(lv_arc_scrollbar_t),
    .base_class = &lv_obj_class,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *lv_arc_scrollbar_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_arc_scrollbar_attach(lv_obj_t *obj, lv_obj_t *attached)
{
    lv_arc_scrollbar_t *arc = (lv_arc_scrollbar_t *)obj;
    arc->attached_obj = attached;
    arc->ind_height = -1;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_arc_scrollbar_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    // lv_obj_add_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_arc_scrollbar_destructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    /* Your Code Here */
}

static void lv_arc_scrollbar_event(const lv_obj_class_t *class_p, lv_event_t *evt)
{
    LV_UNUSED(class_p);
    lv_arc_scrollbar_t *arc = (lv_arc_scrollbar_t *)evt->current_target;
    if (evt->code == LV_EVENT_DRAW_MAIN_BEGIN)
    {
        if (arc->ind_height < 0)
        {
            arc->pad_top = lv_obj_get_style_pad_top(arc->attached_obj, 0);
            arc->total_height = lv_obj_get_scroll_top(arc->attached_obj) + lv_obj_get_height(arc->attached_obj) + lv_obj_get_scroll_bottom(arc->attached_obj) - arc->pad_top - lv_obj_get_style_pad_bottom(arc->attached_obj, 0);
            arc->ind_height = 2 * Y_MAX * DISP_VER_RES / arc->total_height;
        }
    }
    else if (evt->code == LV_EVENT_DRAW_MAIN)
    {
        draw_main(evt);
    }
    else if (evt->code == LV_EVENT_COVER_CHECK)
    {
        lv_cover_check_info_t * info = lv_event_get_param(evt);
        info->res = LV_COVER_RES_NOT_COVER;
    }
}

static void draw_main(lv_event_t *evt)
{
    lv_arc_scrollbar_t *arc = (lv_arc_scrollbar_t *)evt->current_target;

    const lv_coord_t width = 2;
    const lv_coord_t radius = 178;
    const lv_coord_t x0 = 180;
    const lv_coord_t y0 = 180;

    lv_coord_t x = radius;
    lv_coord_t y = 0;
    lv_coord_t dx = 1;
    lv_coord_t dy = 1;
    lv_coord_t err = dx - (radius << 1);

    lv_draw_ctx_t *draw_ctx = (lv_draw_ctx_t *)evt->param;
    uint16_t *p_fb = draw_ctx->buf;
    uint16_t buf_width = lv_area_get_width(draw_ctx->buf_area);

    lv_coord_t scroll_offset = -1 * arc->attached_obj->spec_attr->scroll.y - arc->pad_top;
    float ind_offset = 2.f * Y_MAX * scroll_offset / arc->total_height;
    lv_coord_t ind_y_start = y0 - Y_MAX + ind_offset;
    lv_coord_t ind_y_end = ind_y_start + arc->ind_height + 1;

    if (draw_ctx->clip_area->y2 < (y0 - Y_MAX) || draw_ctx->clip_area->y1 > (y0 + Y_MAX))
    {
        return;
    }

    if (draw_ctx->clip_area->x2 < (x0 - Y_MAX) || draw_ctx->clip_area->x1 > (x0 + Y_MAX))
    {
        return;
    }

    while (y <= Y_MAX)
    {
        uint16_t dst_y = y0 + y;
        uint16_t dst_y2 = y0 - y;
        uint16_t y_color = 0xFFFF;
        uint16_t y2_color = 0xFFFF;

        if (dst_y >= ind_y_start && dst_y <= ind_y_end)
        {
            y_color = 0x07E0;
        }

        if (dst_y2 >= ind_y_start && dst_y2 <= ind_y_end)
        {
            y2_color = 0x07E0;
        }

        if ((x0 + x) > draw_ctx->buf_area->x2)
        {
            x = draw_ctx->buf_area->x2 - x0;
        }

        for (uint8_t i = width; i > 0; i--)
        {
            uint16_t __x = x0 + x - i - draw_ctx->buf_area->x1;
            if (IS_Y_IN(dst_y, draw_ctx->clip_area))
            {
                uint16_t __y = dst_y - draw_ctx->buf_area->y1;
                p_fb[__x + __y * buf_width] = y_color;
            }

            if (IS_Y_IN(dst_y2, draw_ctx->clip_area))
            {
                uint16_t __y2 = dst_y2 - draw_ctx->buf_area->y1;
                p_fb[__x + __y2 * buf_width] = y2_color;
            }
        }

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
