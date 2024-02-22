#ifndef __LV_GENERAL_LIST_GENERATOR_H__
#define __LV_GENERAL_LIST_GENERATOR_H__

#include "lvgl.h"
#include <stdint.h>

#define LIST_SIZE(arr) (sizeof((arr))/sizeof((arr[0])))

typedef void (*general_list_event_callback_t)(uint16_t index, lv_event_t *event);

typedef struct
{
    const char *title;
    const lv_img_dsc_t *icon;
} general_list_item_t;

typedef struct
{
    const char *title;
    const general_list_item_t *items;
    uint16_t item_num;
    lv_coord_t item_width;
    const lv_img_dsc_t *suffix;
    general_list_event_callback_t callback;
} general_list_dsc_t;

lv_obj_t *lv_general_list_create(lv_obj_t *parent, general_list_dsc_t *dsc);

#endif // __LV_GENERAL_LIST_GENERATOR_H__
