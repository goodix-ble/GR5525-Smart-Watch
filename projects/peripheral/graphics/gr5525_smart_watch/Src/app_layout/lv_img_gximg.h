/**
 * @file lv_img_gximg.h
 *
 */

#ifndef __LV_IMG_GXIMG_H__
#define __LV_IMG_GXIMG_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t *lv_img_gximg_create(lv_obj_t *parent);
void lv_img_gximg_set_src(lv_obj_t *obj, const void* gximg_src);
void lv_img_gximg_set_pos(lv_obj_t *obj, lv_coord_t center_x, lv_coord_t center_y);
void lv_img_gximg_set_angle(lv_obj_t * obj, int16_t angle);
void lv_img_gximg_set_pivot_and_src(lv_obj_t *obj, lv_coord_t center_x, lv_coord_t center_y, const void* gximg_src);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __LV_IMG_GXIMG_H__
