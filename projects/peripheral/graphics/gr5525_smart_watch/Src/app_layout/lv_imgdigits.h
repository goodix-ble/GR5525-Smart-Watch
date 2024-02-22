#ifndef __LV_IMGDIGITS_H__
#define __LV_IMGDIGITS_H__

#include "lvgl.h"

enum _e_lv_imgdigits_digit_index
{
    LV_IMGDIGITS_DIGIT_0 = 0,
    LV_IMGDIGITS_DIGIT_1,
    LV_IMGDIGITS_DIGIT_2,
    LV_IMGDIGITS_DIGIT_3,
    LV_IMGDIGITS_DIGIT_4,
    LV_IMGDIGITS_DIGIT_5,
    LV_IMGDIGITS_DIGIT_6,
    LV_IMGDIGITS_DIGIT_7,
    LV_IMGDIGITS_DIGIT_8,
    LV_IMGDIGITS_DIGIT_9,
    LV_IMGDIGITS_MARK_PREFIX,
    LV_IMGDIGITS_MARK_NEGATIVE,
    LV_IMGDIGITS_MARK_SUFFIX,
    LV_IMGDIGITS_LENGTH,
};

typedef struct
{
    lv_obj_t obj;
    int16_t value;
    uint8_t digits;
    uint8_t cache_idx : 4;
    uint8_t cache_valid : 1;
    uint8_t show_prefix : 1;
    uint8_t show_suffix : 1;
    uint8_t show_negative : 1;
    lv_img_dsc_t **digits_img;
    lv_img_dsc_t *digits_cache[8];
    int16_t angle;
    lv_text_align_t align;
} lv_imgdigits_t;

lv_obj_t *lv_imgdigits_create(lv_obj_t *parent);

void lv_imgdigits_set_digits_img(lv_obj_t *obj, const lv_img_dsc_t *digits_img[]);

void lv_imgdigits_set_value(lv_obj_t *obj, int16_t value);

void lv_imgdigits_set_digits(lv_obj_t *obj, uint8_t digits);

void lv_imgdigits_set_angle(lv_obj_t *obj, int16_t angle);

void lv_imgdigits_set_text_align(lv_obj_t *obj, lv_text_align_t align);

void lv_imgdigits_set_mark_show(lv_obj_t *obj, bool show_prefix, bool show_negative, bool show_suffix);

#endif // __LV_IMGDIGITS_H__
