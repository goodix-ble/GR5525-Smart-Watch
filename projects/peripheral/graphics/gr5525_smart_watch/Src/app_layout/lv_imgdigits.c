#include "lv_imgdigits.h"
#include "lv_img_buf.h"

#include <stdio.h>
#include <math.h>

#define DEG2RAD 0.01745f

#define MY_CLASS &lv_imgdigits_class

static void lv_imgdigits_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void lv_imgdigits_destructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
static void lv_imgdigits_event(const lv_obj_class_t *class_p, lv_event_t *e);
static void draw_main_normal(lv_event_t *e);
static void draw_main_rotation(lv_event_t *e);

static void refresh_digits_cache(lv_imgdigits_t *w);
static lv_point_t calc_digits_size(lv_imgdigits_t *w);

static lv_point_t lv_imgdigits_get_transformed_size(lv_imgdigits_t *w)
{
    lv_point_t p = calc_digits_size(w);
    if (w->angle == 0)
    {
        return p;
    }

    lv_area_t transformed;
    // lv_point_t pivot = {
    //     .x = p.x / 2,
    //     .y = p.y / 2,
    // };
    lv_point_t pivot = {0, 0};

    _lv_img_buf_get_transformed_area(&transformed, p.x, p.y, w->angle, LV_IMG_ZOOM_NONE, &pivot);
    return (lv_point_t){lv_area_get_width(&transformed), lv_area_get_height(&transformed)};
}

static const lv_obj_class_t lv_imgdigits_class = {
    .constructor_cb = lv_imgdigits_constructor,
    .destructor_cb = lv_imgdigits_destructor,
    .event_cb = lv_imgdigits_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_imgdigits_t),
    .base_class = &lv_obj_class,
};

lv_obj_t *lv_imgdigits_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_imgdigits_set_digits_img(lv_obj_t *obj, const lv_img_dsc_t *digits_img[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;
    w->digits_img = (lv_img_dsc_t **)digits_img;
    lv_obj_invalidate(obj);
}

void lv_imgdigits_set_value(lv_obj_t *obj, int16_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;
    if (value != w->value)
    {
        w->value = value;
        w->cache_valid = 0;
        lv_obj_invalidate(obj);
        lv_obj_refresh_self_size(obj);
    }
}

void lv_imgdigits_set_digits(lv_obj_t *obj, uint8_t digits)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;
    w->digits = digits;
    lv_obj_invalidate(obj);
}

void lv_imgdigits_set_angle(lv_obj_t *obj, int16_t angle)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;
    w->angle = angle;
    lv_obj_invalidate(obj);
    lv_obj_refresh_ext_draw_size(obj);
}

void lv_imgdigits_set_text_align(lv_obj_t *obj, lv_text_align_t align)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;
    w->align = align;
    lv_obj_invalidate(obj);
}

void lv_imgdigits_set_mark_show(lv_obj_t *obj, bool show_prefix, bool show_negative, bool show_suffix)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;
    w->show_prefix = show_prefix;
    w->show_negative = show_negative;
    w->show_suffix = show_suffix;
    w->cache_valid = 0;
    lv_obj_refresh_self_size(obj);
    lv_obj_invalidate(obj);
}

static void lv_imgdigits_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;
    w->value = 0;
    w->digits = 0;
    w->cache_idx = 0;
    w->show_prefix = 1;
    w->show_suffix = 1;
    w->show_negative = 1;
    w->digits_img = NULL;
    lv_memset_00(w->digits_cache, sizeof(w->digits_cache));
    // w->pivot.x = LV_PCT(50);
    // w->pivot.y = LV_PCT(50);
    w->angle = 0;
    w->align = LV_TEXT_ALIGN_LEFT;
}

static void lv_imgdigits_destructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    // Do nothing
    LV_UNUSED(class_p);
    LV_UNUSED(obj);
}

static void lv_imgdigits_event(const lv_obj_class_t *class_p, lv_event_t *e)
{
    LV_UNUSED(class_p);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_current_target(e);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;
    if (code == LV_EVENT_GET_SELF_SIZE)
    {
        lv_point_t *p = lv_event_get_param(e);
        *p = lv_imgdigits_get_transformed_size(w);
    }
    else if (code == LV_EVENT_REFR_EXT_DRAW_SIZE)
    {
        lv_coord_t *s = lv_event_get_param(e);
        if (w->angle != 0)
        {
            lv_point_t plain_size = calc_digits_size(w);
            // lv_point_t pivot = {
            //     .x = plain_size.x / 2,
            //     .y = plain_size.y / 2,
            // };
            lv_point_t pivot = {0, 0};
            lv_area_t a;
            _lv_img_buf_get_transformed_area(&a, plain_size.x, plain_size.y, w->angle, LV_IMG_ZOOM_NONE, &pivot);
            lv_coord_t pad_ori = *s;
            *s = LV_MAX(*s, pad_ori - a.x1);
            *s = LV_MAX(*s, pad_ori - a.y1);
            *s = LV_MAX(*s, pad_ori + a.x2 - plain_size.x);
            *s = LV_MAX(*s, pad_ori + a.y2 - plain_size.y);
        }
    }
    else if (code == LV_EVENT_DRAW_MAIN)
    {
        if (w->angle == 0)
        {
            draw_main_normal(e);
        }
        else
        {
            draw_main_rotation(e);
        }
    }
}

static void draw_main_normal(lv_event_t *e)
{
    // Draw Digits
    lv_obj_t *obj = lv_event_get_current_target(e);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;

    if (!w->digits_img || !w->cache_idx)
    {
        // No digits available
        return;
    }

    // Calculate how the label is aligned
    lv_coord_t x_offset = 0;

    // alignment is valid only if object width is larger than draw content width
    lv_point_t size = lv_imgdigits_get_transformed_size(w);

    if (lv_obj_get_width(obj) > size.x)
    {
        if (w->align == LV_TEXT_ALIGN_CENTER)
        {
            x_offset = (lv_obj_get_width(obj) - size.x) / 2;
        }
        else if (w->align == LV_TEXT_ALIGN_RIGHT)
        {
            x_offset = (lv_obj_get_width(obj) - size.x);
        }
    }

    // Draw digits
    lv_draw_img_dsc_t draw_dsc;
    lv_draw_img_dsc_init(&draw_dsc);
    lv_area_t coords = {
        .x1 = obj->coords.x1,
        .y1 = obj->coords.y1,
    };

    coords.x1 += x_offset;

#if LV_VERSION_CHECK(8, 2, 0)
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
#else
    const lv_area_t *clip_area = lv_event_get_param(e);
    lv_area_t img_clip_area;
    _lv_area_intersect(&img_clip_area, clip_area, &obj->coords);
#endif

    uint8_t cache_idx = w->cache_idx;

    while (cache_idx-- != 0)
    {
        coords.x2 = coords.x1 + w->digits_cache[cache_idx]->header.w - 1;
        coords.y2 = coords.y1 + w->digits_cache[cache_idx]->header.h - 1;
#if LV_VERSION_CHECK(8, 2, 0)
        lv_draw_img(draw_ctx, &draw_dsc, &coords, w->digits_cache[cache_idx]);
#else
        lv_draw_img(&coords, &img_clip_area, w->digits_cache[cache_idx], &draw_dsc);
#endif
        lv_area_move(&coords, w->digits_cache[cache_idx]->header.w, 0);
    }
}

static void draw_main_rotation(lv_event_t *e)
{
    // Draw Digits
    lv_obj_t *obj = lv_event_get_current_target(e);
    lv_imgdigits_t *w = (lv_imgdigits_t *)obj;

    if (!w->digits_img || !w->cache_idx)
    {
        // No digits available
        return;
    }

    // Force center alignment when rotated
    lv_point_t size = calc_digits_size(w);
    lv_coord_t x_offset = 0;
    if (lv_obj_get_width(obj) > size.x)
    {
        if (w->align == LV_TEXT_ALIGN_CENTER)
        {
            x_offset = (lv_obj_get_width(obj) - size.x) / 2;
        }
        else if (w->align == LV_TEXT_ALIGN_RIGHT)
        {
            x_offset = (lv_obj_get_width(obj) - size.x);
        }
    }

    // Draw digits
    lv_draw_img_dsc_t draw_dsc;
    // draw_dsc.pivot.x = size.x / 2;
    // draw_dsc.pivot.y = size.y / 2;
    lv_draw_img_dsc_init(&draw_dsc);
    draw_dsc.angle = w->angle;
    lv_area_t coords = {
        .x1 = obj->coords.x1,
        .y1 = obj->coords.y1,
    };

#if LV_VERSION_CHECK(8, 2, 0)
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
#else
    const lv_area_t *clip_area = lv_event_get_param(e);
    lv_area_t img_clip_area;
    _lv_img_buf_get_transformed_area(&img_clip_area, size.x, size.y, w->angle, LV_IMG_ZOOM_NONE, &draw_dsc.pivot);
    lv_area_move(&img_clip_area, obj->coords.x1, obj->coords.y1);
    _lv_area_intersect(&img_clip_area, clip_area, &img_clip_area);
#endif

    uint8_t cache_idx = w->cache_idx;

    float angle = DEG2RAD * w->angle / 10.f;

    coords.x1 += x_offset * cosf(angle);
    coords.y1 += x_offset * sinf(angle);

    while (cache_idx-- != 0)
    {
        coords.x2 = coords.x1 + w->digits_cache[cache_idx]->header.w - 1;
        coords.y2 = coords.y1 + w->digits_cache[cache_idx]->header.h - 1;
#if LV_VERSION_CHECK(8, 2, 0)
        lv_draw_img(draw_ctx, &draw_dsc, &coords, w->digits_cache[cache_idx]);
#else
        lv_draw_img(&coords, &img_clip_area, w->digits_cache[cache_idx], &draw_dsc);
#endif
        lv_point_t move_vec = {
            .x = w->digits_cache[cache_idx]->header.w * cosf(angle),
            .y = w->digits_cache[cache_idx]->header.w * sinf(angle),
        };
        lv_area_move(&coords, move_vec.x, move_vec.y);
    }
}

static void refresh_digits_cache(lv_imgdigits_t *w)
{
    w->cache_idx = 0;
    int16_t value = w->value;
    bool negative = false;

    if (!w->digits_img)
    {
        return;
    }

    if (w->digits_img[LV_IMGDIGITS_MARK_SUFFIX] && w->show_suffix)
    {
        w->digits_cache[w->cache_idx++] = w->digits_img[LV_IMGDIGITS_MARK_SUFFIX];
    }

    if (value == 0 && w->digits == 0)
    {
        w->digits_cache[w->cache_idx++] = w->digits_img[LV_IMGDIGITS_DIGIT_0];
    }
    else if (value < 0)
    {
        value = -1 * value;
        negative = w->show_negative;
    }

    if (w->digits == 0)
    {
        // No digits limit
        while (value > 0)
        {
            uint8_t d = value % 10;
            lv_img_dsc_t *c = w->digits_img[d];
            w->digits_cache[w->cache_idx++] = c;
            value /= 10;
        }
    }
    else
    {
        // Have digits limit
        uint8_t i = 0;
        while (i < w->digits)
        {
            uint8_t d = value % 10;
            lv_img_dsc_t *c = w->digits_img[d];
            w->digits_cache[w->cache_idx++] = c;
            value /= 10;
            i++;
        }
    }

    if (negative && w->digits_img[LV_IMGDIGITS_MARK_NEGATIVE])
    {
        w->digits_cache[w->cache_idx++] = w->digits_img[LV_IMGDIGITS_MARK_NEGATIVE];
    }

    if (w->digits_img[LV_IMGDIGITS_MARK_PREFIX] && w->show_prefix)
    {
        w->digits_cache[w->cache_idx++] = w->digits_img[LV_IMGDIGITS_MARK_PREFIX];
    }

    w->cache_valid = 1;
}

static lv_point_t calc_digits_size(lv_imgdigits_t *w)
{
    if (!w->cache_valid)
    {
        refresh_digits_cache(w);
    }

    uint8_t cache_idx = w->cache_idx;
    lv_point_t size = {0, 0};
    while (cache_idx-- != 0)
    {
        size.x += w->digits_cache[cache_idx]->header.w;
        size.y = LV_MAX(size.y, w->digits_cache[cache_idx]->header.h);
    }
    return size;
}
