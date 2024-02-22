/**
 * @file lv_arc_scrollbar.h
 *
 */

#ifndef __LV_ARC_SCROLLBAR_H__
#define __LV_ARC_SCROLLBAR_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    lv_obj_t obj;
    lv_obj_t *attached_obj;
    lv_coord_t total_height;
    lv_coord_t pad_top;
    lv_coord_t ind_height;
} lv_arc_scrollbar_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t *lv_arc_scrollbar_create(lv_obj_t *parent);

void lv_arc_scrollbar_attach(lv_obj_t *obj, lv_obj_t *attached);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __LV_ARC_SCROLLBAR_H__
