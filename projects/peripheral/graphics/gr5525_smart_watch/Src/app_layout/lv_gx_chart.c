/**
 * @file lv_gx_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gx_chart.h"
#if LV_GDX_PATCH_USE_GX_CHART > 0

#include "lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_gx_chart_class

#define LV_GX_CHART_HDIV_DEF 3
#define LV_GX_CHART_VDIV_DEF 5
#define LV_GX_CHART_POINT_CNT_DEF 10
#define LV_GX_CHART_LABEL_MAX_TEXT_LENGTH 16

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_gx_chart_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gx_chart_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gx_chart_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void draw_series_line(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx);
static void invalidate_point(lv_obj_t * obj, uint16_t i);
static void new_points_alloc(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint32_t cnt, lv_coord_t ** a);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_gx_chart_class = {
    .constructor_cb = lv_gx_chart_constructor,
    .destructor_cb = lv_gx_chart_destructor,
    .event_cb = lv_gx_chart_event,
    .width_def = LV_PCT(100),
    .height_def = LV_DPI_DEF * 2,
    .instance_size = sizeof(lv_gx_chart_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_gx_chart_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_gx_chart_set_type(lv_obj_t * obj, lv_gx_chart_type_t type)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    if(gx_chart->type == type) return;

    gx_chart->type = type;

    lv_gx_chart_refresh(obj);
}

void lv_gx_chart_set_point_count(lv_obj_t * obj, uint16_t cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    if(gx_chart->point_cnt == cnt) return;

    lv_gx_chart_series_t * ser;

    if(cnt < 1) cnt = 1;

    _LV_LL_READ_BACK(&gx_chart->series_ll, ser) {
        if(!ser->y_ext_buf_assigned) new_points_alloc(obj, ser, cnt, &ser->y_points);
        ser->start_point = 0;
    }

    gx_chart->point_cnt = cnt;

    lv_gx_chart_refresh(obj);
}

void lv_gx_chart_set_range(lv_obj_t * obj, lv_gx_chart_axis_t axis, lv_coord_t min, lv_coord_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    max = max == min ? max + 1 : max;

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    switch(axis) {
        case LV_GX_CHART_AXIS_PRIMARY_Y:
            gx_chart->ymin[0] = min;
            gx_chart->ymax[0] = max;
            break;
        case LV_GX_CHART_AXIS_SECONDARY_Y:
            gx_chart->ymin[1] = min;
            gx_chart->ymax[1] = max;
            break;
        case LV_GX_CHART_AXIS_PRIMARY_X:
            gx_chart->xmin[0] = min;
            gx_chart->xmax[0] = max;
            break;
        case LV_GX_CHART_AXIS_SECONDARY_X:
            gx_chart->xmin[1] = min;
            gx_chart->xmax[1] = max;
            break;
        default:
            LV_LOG_WARN("Invalid axis: %d", axis);
            return;
    }

    lv_gx_chart_refresh(obj);
}

void lv_gx_chart_set_update_mode(lv_obj_t * obj, lv_gx_chart_update_mode_t update_mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    if(gx_chart->update_mode == update_mode) return;

    gx_chart->update_mode = update_mode;
    lv_obj_invalidate(obj);
}

lv_gx_chart_type_t lv_gx_chart_get_type(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    return gx_chart->type;
}

uint16_t lv_gx_chart_get_point_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    return gx_chart->point_cnt;
}

uint16_t lv_gx_chart_get_x_start_point(const lv_obj_t * obj, lv_gx_chart_series_t * ser)
{
    LV_UNUSED(obj);
    LV_ASSERT_NULL(ser);

    return ser->start_point;
}

void lv_gx_chart_get_point_pos_by_id(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint16_t id, lv_point_t * p_out)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(ser);
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    if(id >= gx_chart->point_cnt) {
        LV_LOG_WARN("Invalid index: %d", id);
        p_out->x = 0;
        p_out->y = 0;
        return;
    }

    lv_coord_t w = (int32_t)lv_obj_get_content_width(obj);
    lv_coord_t h = (int32_t)lv_obj_get_content_height(obj);

    if(gx_chart->type == LV_GX_CHART_TYPE_LINE) {
        p_out->x = (w * id) / (gx_chart->point_cnt - 1);
    }

#if LV_GDX_PATCH_REMOVE_BORDER
    lv_coord_t border_width = 0;
#else
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
#endif
    p_out->x += lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    p_out->x -= lv_obj_get_scroll_left(obj);

    int32_t temp_y = 0;
    temp_y = (int32_t)((int32_t)ser->y_points[id] - gx_chart->ymin[ser->y_axis_sec]) * h;
    temp_y = temp_y / (gx_chart->ymax[ser->y_axis_sec] - gx_chart->ymin[ser->y_axis_sec]);
    p_out->y = h - temp_y;
    p_out->y += lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
    p_out->y -= lv_obj_get_scroll_top(obj);
}

void lv_gx_chart_refresh(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_invalidate(obj);
}

/*======================
 * Series
 *=====================*/

lv_gx_chart_series_t * lv_gx_chart_add_series(lv_obj_t * obj, lv_color_t color, lv_gx_chart_axis_t axis)
{
    LV_LOG_INFO("begin");

    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_gx_chart_t * gx_chart    = (lv_gx_chart_t *)obj;
    lv_gx_chart_series_t * ser = _lv_ll_ins_head(&gx_chart->series_ll);
    LV_ASSERT_MALLOC(ser);
    if(ser == NULL) return NULL;

    lv_coord_t def = LV_GX_CHART_POINT_NONE;

    ser->color  = color;
    ser->y_points = lv_mem_alloc(sizeof(lv_coord_t) * gx_chart->point_cnt);
    LV_ASSERT_MALLOC(ser->y_points);

    if(ser->y_points == NULL) {
        _lv_ll_remove(&gx_chart->series_ll, ser);
        lv_mem_free(ser);
        return NULL;
    }

    ser->start_point = 0;
    ser->y_ext_buf_assigned = false;
    ser->hidden = 0;
    ser->x_axis_sec = axis & LV_GX_CHART_AXIS_SECONDARY_X ? 1 : 0;
    ser->y_axis_sec = axis & LV_GX_CHART_AXIS_SECONDARY_Y ? 1 : 0;

    uint16_t i;
    lv_coord_t * p_tmp = ser->y_points;
    for(i = 0; i < gx_chart->point_cnt; i++) {
        *p_tmp = def;
        p_tmp++;
    }

    return ser;
}

void lv_gx_chart_remove_series(lv_obj_t * obj, lv_gx_chart_series_t * series)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(series);

    lv_gx_chart_t * gx_chart    = (lv_gx_chart_t *)obj;
    if(!series->y_ext_buf_assigned && series->y_points) lv_mem_free(series->y_points);

    _lv_ll_remove(&gx_chart->series_ll, series);
    lv_mem_free(series);

    return;
}

void lv_gx_chart_hide_series(lv_obj_t * gx_chart, lv_gx_chart_series_t * series, bool hide)
{
    LV_ASSERT_OBJ(gx_chart, MY_CLASS);
    LV_ASSERT_NULL(series);

    series->hidden = hide ? 1 : 0;
    lv_gx_chart_refresh(gx_chart);
}


void lv_gx_chart_set_series_color(lv_obj_t * gx_chart, lv_gx_chart_series_t * series, lv_color_t color)
{
    LV_ASSERT_OBJ(gx_chart, MY_CLASS);
    LV_ASSERT_NULL(series);

    series->color = color;
    lv_gx_chart_refresh(gx_chart);
}

void lv_gx_chart_set_x_start_point(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint16_t id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    if(id >= gx_chart->point_cnt) return;
    ser->start_point = id;
}

lv_gx_chart_series_t * lv_gx_chart_get_series_next(const lv_obj_t * obj, const lv_gx_chart_series_t * ser)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    if(ser == NULL) return _lv_ll_get_head(&gx_chart->series_ll);
    else return _lv_ll_get_next(&gx_chart->series_ll, ser);
}

/*=====================
 * Set/Get value(s)
 *====================*/


void lv_gx_chart_set_all_value(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    uint16_t i;
    for(i = 0; i < gx_chart->point_cnt; i++) {
        ser->y_points[i] = value;
    }
    ser->start_point = 0;
    lv_gx_chart_refresh(obj);
}

void lv_gx_chart_set_next_value(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    ser->y_points[ser->start_point] = value;
    invalidate_point(obj, ser->start_point);
    ser->start_point = (ser->start_point + 1) % gx_chart->point_cnt;
    invalidate_point(obj, ser->start_point);
}

void lv_gx_chart_set_next_value2(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t x_value, lv_coord_t y_value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;

    ser->x_points[ser->start_point] = x_value;
    ser->y_points[ser->start_point] = y_value;
    ser->start_point = (ser->start_point + 1) % gx_chart->point_cnt;
    invalidate_point(obj, ser->start_point);
}

void lv_gx_chart_set_value_by_id(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint16_t id, lv_coord_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;

    if(id >= gx_chart->point_cnt) return;
    ser->y_points[id] = value;
    invalidate_point(obj, id);
}

void lv_gx_chart_set_value_by_id2(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint16_t id, lv_coord_t x_value,
                               lv_coord_t y_value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;

    if(id >= gx_chart->point_cnt) return;
    ser->x_points[id] = x_value;
    ser->y_points[id] = y_value;
    invalidate_point(obj, id);
}

void lv_gx_chart_set_ext_y_array(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t array[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    if(!ser->y_ext_buf_assigned && ser->y_points) lv_mem_free(ser->y_points);
    ser->y_ext_buf_assigned = true;
    ser->y_points = array;
    lv_obj_invalidate(obj);
}

void lv_gx_chart_set_ext_x_array(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t array[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    if(!ser->x_ext_buf_assigned && ser->x_points) lv_mem_free(ser->x_points);
    ser->x_ext_buf_assigned = true;
    ser->x_points = array;
    lv_obj_invalidate(obj);
}

lv_coord_t * lv_gx_chart_get_y_array(const lv_obj_t * obj, lv_gx_chart_series_t * ser)
{
    LV_UNUSED(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    return ser->y_points;
}

lv_coord_t * lv_gx_chart_get_x_array(const lv_obj_t * obj, lv_gx_chart_series_t * ser)
{
    LV_UNUSED(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    return ser->x_points;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_gx_chart_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_gx_chart_t * gx_chart = (lv_gx_chart_t *)obj;

    _lv_ll_init(&gx_chart->series_ll, sizeof(lv_gx_chart_series_t));

    gx_chart->ymin[0] = 0;
    gx_chart->xmin[0] = 0;
    gx_chart->ymin[1] = 0;
    gx_chart->xmin[1] = 0;
    gx_chart->ymax[0] = 100;
    gx_chart->xmax[0] = 100;
    gx_chart->ymax[1] = 100;
    gx_chart->xmax[1] = 100;

    gx_chart->point_cnt   = LV_GX_CHART_POINT_CNT_DEF;
    gx_chart->type        = LV_GX_CHART_TYPE_LINE;
    gx_chart->update_mode = LV_GX_CHART_UPDATE_MODE_SHIFT;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_gx_chart_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_gx_chart_t * gx_chart = (lv_gx_chart_t *)obj;
    lv_gx_chart_series_t * ser;
    while(gx_chart->series_ll.head) {
        ser = _lv_ll_get_head(&gx_chart->series_ll);

        if(!ser->y_ext_buf_assigned) lv_mem_free(ser->y_points);

        _lv_ll_remove(&gx_chart->series_ll, ser);
        lv_mem_free(ser);
    }
    _lv_ll_clear(&gx_chart->series_ll);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_gx_chart_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    lv_res_t res;

    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    if(code == LV_EVENT_SIZE_CHANGED) {
        lv_obj_refresh_self_size(obj);
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_param(e);
        p->x = (int32_t)lv_obj_get_content_width(obj);
        p->y = (int32_t)lv_obj_get_content_height(obj);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);

        if(_lv_ll_is_empty(&gx_chart->series_ll) == false) {
            if(gx_chart->type == LV_GX_CHART_TYPE_LINE) draw_series_line(obj, draw_ctx);
        }
    }
}

static void draw_series_line(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx)
{
    lv_area_t clip_area;
    if(_lv_area_intersect(&clip_area, &obj->coords, draw_ctx->clip_area) == false)
    {
        return;
    }

    const lv_area_t * clip_area_ori = draw_ctx->clip_area;
    draw_ctx->clip_area = &clip_area;

    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    if(gx_chart->point_cnt < 2)
    {
        return;
    }

    uint16_t i;
    lv_point_t p1;
    lv_point_t p2;
#if LV_GDX_PATCH_REMOVE_BORDER
    lv_coord_t border_width = 0;
#else
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
#endif
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
    lv_coord_t w     = (int32_t)lv_obj_get_content_width(obj);
    lv_coord_t h     = (int32_t)lv_obj_get_content_height(obj);
    lv_coord_t x_ofs = obj->coords.x1 + pad_left;
    lv_coord_t y_ofs = obj->coords.y1 + pad_top;
    lv_gx_chart_series_t * ser;

    lv_area_t series_clip_area;
    bool mask_ret = _lv_area_intersect(&series_clip_area, &obj->coords, draw_ctx->clip_area);
    if(mask_ret == false)
    {
        return;
    }

    lv_draw_line_dsc_t line_dsc_default;
    lv_draw_line_dsc_init(&line_dsc_default);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &line_dsc_default);

    lv_draw_rect_dsc_t point_dsc_default;
    lv_draw_rect_dsc_init(&point_dsc_default);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &point_dsc_default);

    lv_coord_t point_w = lv_obj_get_style_width(obj, LV_PART_INDICATOR) / 2;
    lv_coord_t point_h = lv_obj_get_style_height(obj, LV_PART_INDICATOR) / 2;

    /*Do not bother with line ending is the point will over it*/
    if(LV_MIN(point_w, point_h) > line_dsc_default.width / 2) line_dsc_default.raw_end = 1;
    if(line_dsc_default.width == 1) line_dsc_default.raw_end = 1;

    /*If there are at least as much points as pixels then draw only vertical lines*/
    bool crowded_mode = gx_chart->point_cnt >= w ? true : false;

    /*Go through all data lines*/
    _LV_LL_READ_BACK(&gx_chart->series_ll, ser) {
        if(ser->hidden) continue;
        line_dsc_default.color = ser->color;
        point_dsc_default.bg_color = ser->color;

        lv_coord_t start_point = gx_chart->update_mode == LV_GX_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        p1.x = x_ofs;
        p2.x = x_ofs;

        lv_coord_t p_act = start_point;
        lv_coord_t p_prev = start_point;
        int32_t y_tmp = (int32_t)((int32_t)ser->y_points[p_prev] - gx_chart->ymin[ser->y_axis_sec]) * h;
        y_tmp  = y_tmp / (gx_chart->ymax[ser->y_axis_sec] - gx_chart->ymin[ser->y_axis_sec]);
        p2.y   = h - y_tmp + y_ofs;

#if LV_GDX_PATCH_GX_CHART_DISABLE_PART_DRAW_DSC_EVT
#else
        lv_obj_draw_part_dsc_t part_draw_dsc;
        lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
        part_draw_dsc.class_p = MY_CLASS;
        part_draw_dsc.type = LV_GX_CHART_DRAW_PART_LINE_AND_POINT;
        part_draw_dsc.part = LV_PART_ITEMS;
        part_draw_dsc.line_dsc = &line_dsc_default;
        part_draw_dsc.rect_dsc = &point_dsc_default;
        part_draw_dsc.sub_part_ptr = ser;
#endif // LV_GDX_PATCH_GX_CHART_DISABLE_PART_DRAW_DSC_EVT

        lv_coord_t y_min = p2.y;
        lv_coord_t y_max = p2.y;

        for(i = 0; i < gx_chart->point_cnt; i++) {
            p1.x = p2.x;
            p1.y = p2.y;

            if(p1.x > clip_area_ori->x2 + point_w + 1) break;
            p2.x = ((w * i) / (gx_chart->point_cnt - 1)) + x_ofs;

            p_act = (start_point + i) % gx_chart->point_cnt;

            y_tmp = (int32_t)((int32_t)ser->y_points[p_act] - gx_chart->ymin[ser->y_axis_sec]) * h;
            y_tmp = y_tmp / (gx_chart->ymax[ser->y_axis_sec] - gx_chart->ymin[ser->y_axis_sec]);
            p2.y  = h - y_tmp + y_ofs;

            if(p2.x < clip_area_ori->x1 - point_w - 1) {
                p_prev = p_act;
                continue;
            }

            /*Don't draw the first point. A second point is also required to draw the line*/
            if(i != 0) {
                if(crowded_mode) {
                    if(ser->y_points[p_prev] != LV_GX_CHART_POINT_NONE && ser->y_points[p_act] != LV_GX_CHART_POINT_NONE) {
                        /*Draw only one vertical line between the min and max y-values on the same x-value*/
                        y_max = LV_MAX(y_max, p2.y);
                        y_min = LV_MIN(y_min, p2.y);
                        if(p1.x != p2.x) {
                            lv_coord_t y_cur = p2.y;
                            p2.x--;         /*It's already on the next x value*/
                            p1.x = p2.x;
                            p1.y = y_min;
                            p2.y = y_max;
                            if(p1.y == p2.y) p2.y++;    /*If they are the same no line will be drawn*/
                            lv_draw_line(draw_ctx, &line_dsc_default, &p1, &p2);
                            p2.x++;         /*Compensate the previous x--*/
                            y_min = y_cur;  /*Start the line of the next x from the current last y*/
                            y_max = y_cur;
                        }
                    }
                }
                else {
                    lv_area_t point_area;
                    point_area.x1 = p1.x - point_w;
                    point_area.x2 = p1.x + point_w;
                    point_area.y1 = p1.y - point_h;
                    point_area.y2 = p1.y + point_h;

#if LV_GDX_PATCH_GX_CHART_DISABLE_PART_DRAW_DSC_EVT
#else
                    part_draw_dsc.id = i - 1;
                    part_draw_dsc.p1 = ser->y_points[p_prev] != LV_GX_CHART_POINT_NONE ? &p1 : NULL;
                    part_draw_dsc.p2 = ser->y_points[p_act] != LV_GX_CHART_POINT_NONE ? &p2 : NULL;
                    part_draw_dsc.draw_area = &point_area;
                    part_draw_dsc.value = ser->y_points[p_prev];

                    lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
#endif // LV_GDX_PATCH_GX_CHART_DISABLE_PART_DRAW_DSC_EVT

                    if(ser->y_points[p_prev] != LV_GX_CHART_POINT_NONE && ser->y_points[p_act] != LV_GX_CHART_POINT_NONE) {
                        lv_draw_line(draw_ctx, &line_dsc_default, &p1, &p2);
                    }

                    if(point_w && point_h && ser->y_points[p_prev] != LV_GX_CHART_POINT_NONE) {
                        lv_draw_rect(draw_ctx, &point_dsc_default, &point_area);
                    }

#if LV_GDX_PATCH_GX_CHART_DISABLE_PART_DRAW_DSC_EVT
#else
                    lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
#endif // LV_GDX_PATCH_GX_CHART_DISABLE_PART_DRAW_DSC_EVT
                }
            }
            p_prev = p_act;
        }

        /*Draw the last point*/
        if(!crowded_mode && i == gx_chart->point_cnt) {

            if(ser->y_points[p_act] != LV_GX_CHART_POINT_NONE) {
                lv_area_t point_area;
                point_area.x1 = p2.x - point_w;
                point_area.x2 = p2.x + point_w;
                point_area.y1 = p2.y - point_h;
                point_area.y2 = p2.y + point_h;
#if LV_GDX_PATCH_GX_CHART_DISABLE_PART_DRAW_DSC_EVT
                lv_draw_rect(draw_ctx, &point_dsc_default, &point_area);
#else
                part_draw_dsc.id = i - 1;
                part_draw_dsc.p1 = NULL;
                part_draw_dsc.p2 = NULL;
                part_draw_dsc.draw_area = &point_area;
                part_draw_dsc.value = ser->y_points[p_act];

                lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
                lv_draw_rect(draw_ctx, &point_dsc_default, &point_area);
                lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
#endif
            }
        }
    }

    draw_ctx->clip_area = clip_area_ori;
}

static void invalidate_point(lv_obj_t * obj, uint16_t i)
{
    lv_gx_chart_t * gx_chart  = (lv_gx_chart_t *)obj;
    if(i >= gx_chart->point_cnt) return;

    lv_coord_t w  = (int32_t)lv_obj_get_content_width(obj);
    lv_coord_t scroll_left = lv_obj_get_scroll_left(obj);

    /*In shift mode the whole chart changes so the whole object*/
    if(gx_chart->update_mode == LV_GX_CHART_UPDATE_MODE_SHIFT) {
        lv_obj_invalidate(obj);
        return;
    }

    if(gx_chart->type == LV_GX_CHART_TYPE_LINE) {
#if LV_GDX_PATCH_REMOVE_BORDER
        lv_coord_t bwidth = 0;
#else
        lv_coord_t bwidth = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
#endif
        lv_coord_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        lv_coord_t x_ofs = obj->coords.x1 + pleft + bwidth - scroll_left;
        lv_coord_t line_width = lv_obj_get_style_line_width(obj, LV_PART_ITEMS);
        lv_coord_t point_w = lv_obj_get_style_width(obj, LV_PART_INDICATOR);

        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.y1 -= line_width + point_w;
        coords.y2 += line_width + point_w;

        if(i < gx_chart->point_cnt - 1) {
            coords.x1 = ((w * i) / (gx_chart->point_cnt - 1)) + x_ofs - line_width - point_w;
            coords.x2 = ((w * (i + 1)) / (gx_chart->point_cnt - 1)) + x_ofs + line_width + point_w;
            lv_obj_invalidate_area(obj, &coords);
        }

        if(i > 0) {
            coords.x1 = ((w * (i - 1)) / (gx_chart->point_cnt - 1)) + x_ofs - line_width - point_w;
            coords.x2 = ((w * i) / (gx_chart->point_cnt - 1)) + x_ofs + line_width + point_w;
            lv_obj_invalidate_area(obj, &coords);
        }
    }
    else {
        lv_obj_invalidate(obj);
    }
}

static void new_points_alloc(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint32_t cnt, lv_coord_t ** a)
{
    if((*a) == NULL) return;

    lv_gx_chart_t * gx_chart = (lv_gx_chart_t *) obj;
    uint32_t point_cnt_old = gx_chart->point_cnt;
    uint32_t i;

    if(ser->start_point != 0) {
        lv_coord_t * new_points = lv_mem_alloc(sizeof(lv_coord_t) * cnt);
        LV_ASSERT_MALLOC(new_points);
        if(new_points == NULL) return;

        if(cnt >= point_cnt_old) {
            for(i = 0; i < point_cnt_old; i++) {
                new_points[i] =
                    (*a)[(i + ser->start_point) % point_cnt_old]; /*Copy old contents to new array*/
            }
            for(i = point_cnt_old; i < cnt; i++) {
                new_points[i] = LV_GX_CHART_POINT_NONE; /*Fill up the rest with default value*/
            }
        }
        else {
            for(i = 0; i < cnt; i++) {
                new_points[i] =
                    (*a)[(i + ser->start_point) % point_cnt_old]; /*Copy old contents to new array*/
            }
        }

        /*Switch over pointer from old to new*/
        lv_mem_free((*a));
        (*a) = new_points;
    }
    else {
        (*a) = lv_mem_realloc((*a), sizeof(lv_coord_t) * cnt);
        LV_ASSERT_MALLOC((*a));
        if((*a) == NULL) return;
        /*Initialize the new points*/
        if(cnt > point_cnt_old) {
            for(i = point_cnt_old - 1; i < cnt; i++) {
                (*a)[i] = LV_GX_CHART_POINT_NONE;
            }
        }
    }
}



#endif
