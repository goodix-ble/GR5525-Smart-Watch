/**
 * @file lv_simplified_obj.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_simplified_obj.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_simplified_obj_class
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_simplified_obj_event(const lv_obj_class_t *class_p, lv_event_t *evt);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_simplified_obj_class = {
    .constructor_cb = NULL,
    .destructor_cb = NULL,
    .event_cb = lv_simplified_obj_event,
    .width_def = LV_DPI_DEF,
    .height_def = LV_DPI_DEF,
    .instance_size = sizeof(lv_obj_t),
    .base_class = &lv_obj_class,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *lv_simplified_obj_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_simplified_obj_event(const lv_obj_class_t *class_p, lv_event_t *evt)
{
    lv_event_code_t code = evt->code;
    if (code == LV_EVENT_DRAW_MAIN || code == LV_EVENT_COVER_CHECK)
    {
        class_p->base_class->event_cb(class_p->base_class, evt);
    }
}
