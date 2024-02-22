/**
 * @file lv_virtual_list.h
 *
 */

#ifndef __LV_VIRTUAL_LIST_H__
#define __LV_VIRTUAL_LIST_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define LV_USE_VIRTUAL_LIST 1

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_obj_t *(*lv_virtual_list_create_item_widget_cb)(lv_obj_t *obj, void *user_data, void *p_widget_holder);
typedef void (*lv_virtual_list_update_item_widget_cb)(lv_obj_t *obj, void *user_data, void *p_widget_holder, uint32_t index, lv_obj_t *p_resued_item_obj, bool bindOrUnbind);
typedef void (*lv_virtual_list_item_clicked_cb)(lv_obj_t *obj, void *user_data, void *p_widget_holder, uint32_t index, lv_event_t * evt);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief create a virtual list.
 *
 * @param item_height           the height of data item.
 * @param widget_holder_size    this widget will allocate `widget_holder_size` buffer for referencing the widget for very data item. Example: sizeof(struct my_holder).
 * @param p_create_cb           the widget use this callback to create a new widget to display data item.
 * @param p_config_cb           the widget use this callback to config widget with the corresponding data item.
 */
lv_obj_t *lv_virtual_list_create(lv_obj_t *parent,
                                    lv_coord_t item_height,
                                    uint16_t widget_holder_size,
                                    lv_virtual_list_create_item_widget_cb p_create_cb,
                                    lv_virtual_list_update_item_widget_cb p_config_cb);

/**
 * @brief set the data to display.
 *
 * @param user_data this will appear in `lv_virtual_list_create_item_cb` and `lv_virtual_list_config_item_cb`.
 * @param data_item_cnt the total item count of data.
 */
void lv_virtual_list_update_data(lv_obj_t *obj, void *user_data, uint32_t data_item_cnt);

/**
 * @brief display space at beginning and tail of the list.
 */
void lv_virtual_list_set_padding_space(lv_obj_t *obj, lv_coord_t header, lv_coord_t tail, lv_coord_t gap_between_items);

/**
 * @brief require adding `LV_OBJ_FLAG_EVENT_BUBBLE` flag to data item. The clicked event arrived at lv_virtual_list will be delivered through this callback.
 *        Support following events:
 *        * LV_EVENT_SHORT_CLICKED;
 *        * LV_EVENT_LONG_PRESSED;
 *        * LV_EVENT_LONG_PRESSED_REPEAT;
 *        * LV_EVENT_CLICKED;
 */
void lv_virtual_list_set_item_clicked_cb(lv_obj_t *obj, lv_virtual_list_item_clicked_cb click_cb);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __LV_VIRTUAL_LIST_H__
