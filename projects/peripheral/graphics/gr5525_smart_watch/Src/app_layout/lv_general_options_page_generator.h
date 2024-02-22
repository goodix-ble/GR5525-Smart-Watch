#ifndef __LV_GENERAL_OPTIONS_PAGE_GENERATOR_H__
#define __LV_GENERAL_OPTIONS_PAGE_GENERATOR_H__

#include "lvgl.h"
#include <stdint.h>

typedef struct
{
    const char *title;
    const char *tip_text;
    const char *status_text;
    lv_event_cb_t callback;
} general_option_page_dsc_t;

lv_obj_t *lv_general_options_page_create(lv_obj_t *parent, general_option_page_dsc_t *dsc);

void lv_general_options_page_status_text_set(lv_obj_t *window, const char *status_text);

void lv_general_options_page_show_status(lv_obj_t *window);

#endif // __LV_GENERAL_OPTIONS_PAGE_GENERATOR_H__

