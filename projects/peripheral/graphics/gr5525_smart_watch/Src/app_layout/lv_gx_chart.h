/**
 * @file lv_gx_chart.h
 *
 */

#ifndef LV_GX_CHART_H
#define LV_GX_CHART_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

#if LV_GDX_PATCH_USE_GX_CHART > 0

/*********************
 *      DEFINES
 *********************/

/**Default value of points. Can be used to not draw a point*/
#if LV_USE_LARGE_COORD
#define LV_GX_CHART_POINT_NONE (INT32_MAX)
#else
#define LV_GX_CHART_POINT_NONE (INT16_MAX)
#endif
LV_EXPORT_CONST_INT(LV_GX_CHART_POINT_NONE);

#define LV_GDX_PATCH_GX_CHART_DISABLE_PART_DRAW_DSC_EVT (1)
/**********************
 *      TYPEDEFS
 **********************/

/**
 * Chart types
 */
enum {
    LV_GX_CHART_TYPE_NONE,     /**< Don't draw the series*/
    LV_GX_CHART_TYPE_LINE,     /**< Connect the points with lines*/
};
typedef uint8_t lv_gx_chart_type_t;

/**
 * Chart update mode for `lv_gx_chart_set_next`
 */
enum {
    LV_GX_CHART_UPDATE_MODE_SHIFT,     /**< Shift old data to the left and add the new one the right*/
    LV_GX_CHART_UPDATE_MODE_CIRCULAR,  /**< Add the new data in a circular way*/
};
typedef uint8_t lv_gx_chart_update_mode_t;

/**
 * Enumeration of the axis'
 */
enum {
    LV_GX_CHART_AXIS_PRIMARY_Y     = 0x00,
    LV_GX_CHART_AXIS_SECONDARY_Y   = 0x01,
    LV_GX_CHART_AXIS_PRIMARY_X     = 0x02,
    LV_GX_CHART_AXIS_SECONDARY_X   = 0x04,
    _LV_GX_CHART_AXIS_LAST
};
typedef uint8_t lv_gx_chart_axis_t;

/**
 * Descriptor a chart series
 */
typedef struct {
    lv_coord_t * x_points;
    lv_coord_t * y_points;
    lv_color_t color;
    uint16_t start_point;
    uint8_t hidden : 1;
    uint8_t x_ext_buf_assigned : 1;
    uint8_t y_ext_buf_assigned : 1;
    uint8_t x_axis_sec : 1;
    uint8_t y_axis_sec : 1;
} lv_gx_chart_series_t;

typedef struct {
    lv_obj_t obj;
    // const void * src;      /**< Image source: Pointer to an array or a file or a symbol */
    lv_ll_t series_ll;     /**< Linked list for the series (stores lv_gx_chart_series_t)*/
    lv_coord_t ymin[2];
    lv_coord_t ymax[2];
    lv_coord_t xmin[2];
    lv_coord_t xmax[2];
    uint16_t point_cnt;    /**< Point number in a data line*/
    lv_gx_chart_type_t type  : 3; /**< Line or column chart*/
    lv_gx_chart_update_mode_t update_mode : 1;
} lv_gx_chart_t;

extern const lv_obj_class_t lv_gx_chart_class;

/**
 * `type` field in `lv_obj_draw_part_dsc_t` if `class_p = lv_gx_chart_class`
 * Used in `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END`
 */
typedef enum {
    // LV_GX_CHART_DRAW_PART_DIV_LINE_INIT,    /**< Used before/after drawn the div lines*/
    // LV_GX_CHART_DRAW_PART_DIV_LINE_HOR,     /**< Used for each horizontal division lines*/
    // LV_GX_CHART_DRAW_PART_DIV_LINE_VER,     /**< Used for each vertical division lines*/
    LV_GX_CHART_DRAW_PART_LINE_AND_POINT,   /**< Used on line and scatter charts for lines and points*/
    // LV_GX_CHART_DRAW_PART_BAR,              /**< Used on bar charts for the rectangles*/
    // LV_GX_CHART_DRAW_PART_CURSOR,           /**< Used on cursor lines and points*/
    // LV_GX_CHART_DRAW_PART_TICK_LABEL,       /**< Used on tick lines and labels*/
} lv_gx_chart_draw_part_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a chart object
 * @param parent    pointer to an object, it will be the parent of the new chart
 * @return          pointer to the created chart
 */
lv_obj_t * lv_gx_chart_create(lv_obj_t * parent);

/**
 * Set a new type for a chart
 * @param obj       pointer to a chart object
 * @param type      new type of the chart (from 'lv_gx_chart_type_t' enum)
 */
void lv_gx_chart_set_type(lv_obj_t * obj, lv_gx_chart_type_t type);
/**
 * Set the number of points on a data line on a chart
 * @param obj       pointer to a chart object
 * @param cnt       new number of points on the data lines
 */
void lv_gx_chart_set_point_count(lv_obj_t * obj, uint16_t cnt);

/**
 * Set the minimal and maximal y values on an axis
 * @param obj       pointer to a chart object
 * @param axis      `LV_CHART_AXIS_PRIMARY_Y` or `LV_CHART_AXIS_SECONDARY_Y`
 * @param min       minimum value of the y axis
 * @param max       maximum value of the y axis
 */
void lv_gx_chart_set_range(lv_obj_t * obj, lv_gx_chart_axis_t axis, lv_coord_t min, lv_coord_t max);

/**
 * Set update mode of the chart object. Affects
 * @param obj       pointer to a chart object
 * @param mode      the update mode
 */
void lv_gx_chart_set_update_mode(lv_obj_t * obj, lv_gx_chart_update_mode_t update_mode);

/**
 * Get the type of a chart
 * @param obj       pointer to chart object
 * @return          type of the chart (from 'lv_gx_chart_t' enum)
 */
lv_gx_chart_type_t lv_gx_chart_get_type(const lv_obj_t * obj);

/**
 * Get the data point number per data line on chart
 * @param chart     pointer to chart object
 * @return          point number on each data line
 */
uint16_t lv_gx_chart_get_point_count(const lv_obj_t * obj);

/**
 * Get the current index of the x-axis start point in the data array
 * @param chart     pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @return          the index of the current x start point in the data array
 */
uint16_t lv_gx_chart_get_x_start_point(const lv_obj_t * obj, lv_gx_chart_series_t * ser);

/**
 * Get the position of a point to the chart.
 * @param chart     pointer to a chart object
 * @param ser       pointer to series
 * @param id        the index.
 * @param p_out     store the result position here
 */
void lv_gx_chart_get_point_pos_by_id(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint16_t id, lv_point_t * p_out);

/**
 * Refresh a chart if its data line has changed
 * @param   chart pointer to chart object
 */
void lv_gx_chart_refresh(lv_obj_t * obj);

/*======================
 * Series
 *=====================*/

/**
 * Allocate and add a data series to the chart
 * @param obj       pointer to a chart object
 * @param color     color of the data series
 * @param axis      the y axis to which the series should be attached (::LV_CHART_AXIS_PRIMARY_Y or ::LV_CHART_AXIS_SECONDARY_Y)
 * @return          pointer to the allocated data series
 */
lv_gx_chart_series_t * lv_gx_chart_add_series(lv_obj_t * obj, lv_color_t color, lv_gx_chart_axis_t axis);

/**
 * Deallocate and remove a data series from a chart
 * @param chart     pointer to a chart object
 * @param series    pointer to a data series on 'chart'
 */
void lv_gx_chart_remove_series(lv_obj_t * obj, lv_gx_chart_series_t * series);

/**
 * Hide/Unhide a single series of a chart.
 * @param obj       pointer to a chart object.
 * @param series    pointer to a series object
 * @param hide      true: hide the series
 */
void lv_gx_chart_hide_series(lv_obj_t * chart, lv_gx_chart_series_t * series, bool hide);

/**
 * Change the color of a series
 * @param obj       pointer to a chart object.
 * @param series    pointer to a series object
 * @param color     the new color of the series
 */
void lv_gx_chart_set_series_color(lv_obj_t * chart, lv_gx_chart_series_t * series, lv_color_t color);

/**
 * Set the index of the x-axis start point in the data array.
 * This point will be considers the first (left) point and the other points will be drawn after it.
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @param id        the index of the x point in the data array
 */
void lv_gx_chart_set_x_start_point(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint16_t id);

/**
 * Get the next series.
 * @param chart     pointer to a chart
 * @param ser      the previous series or NULL to get the first
 * @return          the next series or NULL if there is no more.
 */
lv_gx_chart_series_t * lv_gx_chart_get_series_next(const lv_obj_t * chart, const lv_gx_chart_series_t * ser);


/*=====================
 * Set/Get value(s)
 *====================*/

/**
 * Initialize all data points of a series with a value
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param value     the new value for all points. `LV_CHART_POINT_NONE` can be used to hide the points.
 */
void lv_gx_chart_set_all_value(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t value);

/**
 * Set the next point's Y value according to the update mode policy.
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param value     the new value of the next data
 */
void lv_gx_chart_set_next_value(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t value);

/**
 * Set the next point's X and Y value according to the update mode policy.
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param x_value   the new X value of the next data
 * @param y_value   the new Y value of the next data
 */
void lv_gx_chart_set_next_value2(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t x_value, lv_coord_t y_value);

/**
 * Set an individual point's y value of a chart's series directly based on its index
 * @param obj     pointer to a chart object
 * @param ser     pointer to a data series on 'chart'
 * @param id      the index of the x point in the array
 * @param value   value to assign to array point
 */
void lv_gx_chart_set_value_by_id(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint16_t id, lv_coord_t value);

/**
 * Set an individual point's x and y value of a chart's series directly based on its index
 * Can be used only with `LV_CHART_TYPE_SCATTER`.
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param id        the index of the x point in the array
 * @param x_value   the new X value of the next data
 * @param y_value   the new Y value of the next data
 */
void lv_gx_chart_set_value_by_id2(lv_obj_t * obj, lv_gx_chart_series_t * ser, uint16_t id, lv_coord_t x_value,
                               lv_coord_t y_value);

/**
 * Set an external array for the y data points to use for the chart
 * NOTE: It is the users responsibility to make sure the `point_cnt` matches the external array size.
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @param array     external array of points for chart
 */
void lv_gx_chart_set_ext_y_array(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t array[]);

/**
 * Set an external array for the x data points to use for the chart
 * NOTE: It is the users responsibility to make sure the `point_cnt` matches the external array size.
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @param array     external array of points for chart
 */
void lv_gx_chart_set_ext_x_array(lv_obj_t * obj, lv_gx_chart_series_t * ser, lv_coord_t array[]);

/**
 * Get the array of y values of a series
 * @param obj   pointer to a chart object
 * @param ser   pointer to a data series on 'chart'
 * @return      the array of values with 'point_count' elements
 */
lv_coord_t * lv_gx_chart_get_y_array(const lv_obj_t * obj, lv_gx_chart_series_t * ser);

/**
 * Get the array of x values of a series
 * @param obj   pointer to a chart object
 * @param ser   pointer to a data series on 'chart'
 * @return      the array of values with 'point_count' elements
 */
lv_coord_t * lv_gx_chart_get_x_array(const lv_obj_t * obj, lv_gx_chart_series_t * ser);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHART*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHART_H*/
