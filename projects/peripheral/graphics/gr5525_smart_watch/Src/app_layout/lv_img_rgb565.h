/**
 * @file lv_img_rgb565.h
 *
 */

#ifndef __LV_IMG_RGB565_H__
#define __LV_IMG_RGB565_H__

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

lv_obj_t *lv_img_rgb565_create(lv_obj_t *parent);

void lv_img_rgb565_set_src(lv_obj_t * obj, const void * src);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __LV_IMG_RGB565_H__
