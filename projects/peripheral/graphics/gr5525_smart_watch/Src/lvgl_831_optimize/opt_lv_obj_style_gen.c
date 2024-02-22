#include "lv_obj.h"

#if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
lv_coord_t lv_obj_get_style_pad_top(const struct _lv_obj_t * obj, uint32_t part)
{
    if (obj->style_cache.cache_invalid || part != LV_PART_MAIN || obj->style_cache.cached_for_state != obj->state)
    {
        lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_TOP);
        return (lv_coord_t)v.num;
    }
    else
    {
        return obj->style_cache.main_part.pad_top;
    }
}

lv_coord_t lv_obj_get_style_pad_bottom(const struct _lv_obj_t * obj, uint32_t part)
{
    if (obj->style_cache.cache_invalid || part != LV_PART_MAIN || obj->style_cache.cached_for_state != obj->state)
    {
        lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_BOTTOM);
        return (lv_coord_t)v.num;
    }
    else
    {
        return obj->style_cache.main_part.pad_bottom;
    }
}

lv_coord_t lv_obj_get_style_pad_left(const struct _lv_obj_t * obj, uint32_t part)
{
    if (obj->style_cache.cache_invalid || part != LV_PART_MAIN || obj->style_cache.cached_for_state != obj->state)
    {
        lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_LEFT);
        return (lv_coord_t)v.num;
    }
    else
    {
        return obj->style_cache.main_part.pad_left;
    }
}

lv_coord_t lv_obj_get_style_pad_right(const struct _lv_obj_t * obj, uint32_t part)
{
    if (obj->style_cache.cache_invalid || part != LV_PART_MAIN || obj->style_cache.cached_for_state != obj->state)
    {
        lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_RIGHT);
        return (lv_coord_t)v.num;
    }
    else
    {
        return obj->style_cache.main_part.pad_right;
    }
}

#if LV_GDX_PATCH_REMOVE_BORDER
#else
lv_coord_t lv_obj_get_style_border_width(const struct _lv_obj_t * obj, uint32_t part)
{
    if (obj->style_cache.cache_invalid || part != LV_PART_MAIN || obj->style_cache.cached_for_state != obj->state)
    {
        lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_WIDTH);
        return (lv_coord_t)v.num;
    }
    else
    {
        return obj->style_cache.main_part.border_width;
    }
}
#endif

#if LV_GDX_PATCH_GLOBAL_BASE_DIR
// lv_base_dir_t g_global_style_base_dir = LV_BASE_DIR_LTR;
#else
lv_base_dir_t lv_obj_get_style_base_dir(const struct _lv_obj_t * obj, uint32_t part)
{
    if (obj->style_cache.cache_invalid || part != LV_PART_MAIN || obj->style_cache.cached_for_state != obj->state)
    {
        lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BASE_DIR);
        return (lv_base_dir_t)v.num;
    }
    else
    {
        return obj->style_cache.main_part.base_dir;
    }
}
#endif

lv_color_t lv_obj_get_style_bg_color(const struct _lv_obj_t * obj, uint32_t part)
{
    if (obj->style_cache.cache_invalid || part != LV_PART_MAIN || obj->style_cache.cached_for_state != obj->state)
    {
        lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_COLOR);
        return v.color;
    }
    else
    {
        return obj->style_cache.main_part.bg_color;
    }
}

lv_color_t lv_obj_get_style_bg_color_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t raw;
    raw.color = lv_obj_get_style_bg_color(obj, part);
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, raw);
    return v.color;
}
#else // LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
// use original
#endif // LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
