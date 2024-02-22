/**
 * @file lv_obj_pos.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#if LV_GDX_PATCH_SIMPLIFY_GET_SELF_SIZE_EVENT
static inline void _lv_obj_pos_send_event_directly(lv_obj_t * obj, lv_event_code_t event_code, void * param)
{
    lv_event_t e;
    e.user_data = NULL;
    e.param = param;
    e.deleted = 0;
    e.stop_bubbling = 0;
    e.stop_processing = 0;
    e.target = obj;
    e.current_target = obj;
    e.code = event_code;

    const lv_obj_class_t * base = obj->class_p;
    for (;base != NULL;)
    {
        if (base->event_cb != NULL) {
            base->event_cb(base, &e);
            break;
        }
        base = base->base_class;
    }
}

lv_coord_t lv_obj_get_self_width_opt(const lv_obj_t * obj)
{
    lv_point_t p = {0, LV_COORD_MIN};
    _lv_obj_pos_send_event_directly((lv_obj_t *)obj, LV_EVENT_GET_SELF_SIZE, &p);
    return p.x;
}

lv_coord_t lv_obj_get_self_height_opt(const lv_obj_t * obj)
{
    lv_point_t p = {LV_COORD_MIN, 0};
    _lv_obj_pos_send_event_directly((lv_obj_t *)obj, LV_EVENT_GET_SELF_SIZE, &p);
    return p.y;
}
#else
// use original
#endif
