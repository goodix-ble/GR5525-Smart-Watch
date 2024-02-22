#include "lv_volume_ctrl.h"
#include "lv_img_dsc_list.h"

#include <stdio.h>

#define MY_CLASS &lv_volume_ctrl_class

#define AS_VOL_CTRL(obj) ((lv_volume_ctrl_t *)obj)

static void lv_volume_ctrl_event(const lv_obj_class_t *class_p, lv_event_t *evt);
static void draw_main(lv_event_t *evt);

const lv_obj_class_t lv_volume_ctrl_class = {
    .constructor_cb = NULL,
    .destructor_cb = NULL,
    .event_cb = lv_volume_ctrl_event,
    .width_def = 220,
    .height_def = 50,
    .instance_size = sizeof(lv_volume_ctrl_t),
    .base_class = &lv_obj_class,
};

lv_obj_t *lv_volume_ctrl_create(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

int8_t lv_volume_ctrl_get_value(lv_obj_t *obj)
{
    return AS_VOL_CTRL(obj)->volume;
}

void lv_volume_ctrl_set_value(lv_obj_t *obj, int8_t value)
{
    if (value > 100)
    {
        value = 100;
    }
    else if (value < 0)
    {
        value  = 0;
    }

    if (AS_VOL_CTRL(obj)->volume != value)
    {
        AS_VOL_CTRL(obj)->volume = value;
        lv_obj_invalidate(obj);
    }
}

static void lv_volume_ctrl_event(const lv_obj_class_t *class_p, lv_event_t *evt)
{
    lv_event_code_t code = lv_event_get_code(evt);
    lv_obj_t *obj = lv_event_get_current_target(evt);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        lv_indev_t *indev = lv_event_get_indev(evt);
        lv_coord_t press_x = indev->proc.types.pointer.act_point.x;
        // int8_t volume = AS_VOL_CTRL(obj)->volume;
        int8_t vol_dir = 0;
        if (press_x >= obj->coords.x1 && press_x < obj->coords.x1 + 40)
        {
            // Press Minus
            vol_dir = -1;
        }
        else if (press_x > obj->coords.x2 - 40 && press_x <= obj->coords.x2)
        {
            // Press Plus
            vol_dir = 1;
        }

        lv_obj_invalidate(obj);
        lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &vol_dir);
    }
    else if (code == LV_EVENT_DRAW_MAIN)
    {
        draw_main(evt);
    }
}

static void draw_main(lv_event_t *evt)
{
    lv_draw_ctx_t *draw_ctx = lv_event_get_draw_ctx(evt);
    lv_obj_t *obj = lv_event_get_current_target(evt);

    // Draw Background
    lv_draw_img_dsc_t draw_dsc;
    lv_draw_img_dsc_init(&draw_dsc);
    lv_draw_img(draw_ctx, &draw_dsc, &obj->coords, &wd_img_volume_control_background);

    // Draw Button
    lv_area_t minus_area = {
        .x1 = obj->coords.x1 + 18,
        .y1 = obj->coords.y1 + 19,
        .x2 = minus_area.x1 + wd_img_volume_down.header.w - 1,
        .y2 = minus_area.y1 + wd_img_volume_down.header.h - 1,
    };

    lv_area_t plus_area = {
        .x1 = obj->coords.x2 - 18 - wd_img_volume_up.header.w,
        .y1 = obj->coords.y1 + 19,
        .x2 = plus_area.x1 + wd_img_volume_up.header.w - 1,
        .y2 = plus_area.y1 + wd_img_volume_up.header.h - 1,
    };


    bool disabled = lv_obj_has_state(obj, LV_STATE_DISABLED);
    if (disabled)
    {
        lv_draw_img(draw_ctx, &draw_dsc, &minus_area, &wd_img_volume_down_disabled);
        lv_draw_img(draw_ctx, &draw_dsc, &plus_area, &wd_img_volume_up_disabled);
    }
    else
    {
        lv_draw_img(draw_ctx, &draw_dsc, &minus_area, &wd_img_volume_down);
        lv_draw_img(draw_ctx, &draw_dsc, &plus_area, &wd_img_volume_up);
    }

    // Draw Indicator
    if ((AS_VOL_CTRL(obj)->volume > 0) && !disabled)
    {
        lv_area_t ind_area = {
            .x1 = obj->coords.x1 + 44,
            .y1 = obj->coords.y1 + 24,
            .x2 = ind_area.x1 + wd_img_volume_control_indicator.header.w * (AS_VOL_CTRL(obj)->volume / 100.f) - 1,
            .y2 = ind_area.y1 + wd_img_volume_control_indicator.header.h - 1,
        };

        lv_area_t ind_max_area = {
            .x1 = obj->coords.x1 + 44,
            .y1 = obj->coords.y1 + 24,
            .x2 = ind_area.x1 + wd_img_volume_control_indicator.header.w - 1,
            .y2 = ind_area.y1 + wd_img_volume_control_indicator.header.h - 1,
        };

        const lv_area_t * clip_area_ori = draw_ctx->clip_area;
        if(_lv_area_intersect(&ind_area, draw_ctx->clip_area, &ind_area))
        {
            draw_ctx->clip_area = &ind_area;
            lv_draw_img(draw_ctx, &draw_dsc, &ind_max_area, &wd_img_volume_control_indicator);
            draw_ctx->clip_area = clip_area_ori;
        }
    }
}
