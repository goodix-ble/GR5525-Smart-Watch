#ifndef __LV_VOLUME_CTRL_H__
#define __LV_VOLUME_CTRL_H__

#include "lvgl.h"

typedef struct {
    lv_obj_t obj;
    int8_t volume;
} lv_volume_ctrl_t;

lv_obj_t *lv_volume_ctrl_create(lv_obj_t *parent);

int8_t lv_volume_ctrl_get_value(lv_obj_t *obj);
void lv_volume_ctrl_set_value(lv_obj_t *obj, int8_t value);

#endif // __LV_VOLUME_CTRL_H__
