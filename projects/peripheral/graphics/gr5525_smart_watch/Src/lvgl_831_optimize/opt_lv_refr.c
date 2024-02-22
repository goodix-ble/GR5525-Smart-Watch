/**
 * @file lv_refr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_refr.h"
#include "lv_disp.h"
#include "../hal/lv_hal_tick.h"
#include "../hal/lv_hal_disp.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_mem.h"
#include "../misc/lv_math.h"
#include "../misc/lv_gc.h"
#include "../draw/lv_draw.h"
#include "../font/lv_font_fmt_txt.h"
#include "../extra/others/snapshot/lv_snapshot.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
void lv_obj_redraw_opt(lv_draw_ctx_t * draw_ctx, lv_obj_t * obj);
lv_obj_t * lv_refr_get_top_obj_opt(const lv_area_t * area_p, lv_obj_t * obj);
void refr_obj_and_children_opt(lv_draw_ctx_t * draw_ctx, lv_obj_t * top_obj);
void refr_obj(lv_draw_ctx_t * draw_ctx, lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the screen refresh subsystem
 */
__attribute__((section("RAM_CODE")))
void lv_obj_redraw_opt(lv_draw_ctx_t * draw_ctx, lv_obj_t * obj)
{
    const lv_area_t * clip_area_ori = draw_ctx->clip_area;
    lv_area_t clip_coords_for_obj;

    /*Truncate the clip area to `obj size + ext size` area*/
    lv_area_t obj_coords_ext;
    lv_obj_get_coords(obj, &obj_coords_ext);
    lv_coord_t ext_draw_size = _lv_obj_get_ext_draw_size(obj);
    lv_area_increase(&obj_coords_ext, ext_draw_size, ext_draw_size);
    bool com_clip_res = _lv_area_intersect(&clip_coords_for_obj, clip_area_ori, &obj_coords_ext);
    /*If the object is visible on the current clip area OR has overflow visible draw it.
     *With overflow visible drawing should happen to apply the masks which might affect children */
    bool should_draw = com_clip_res || lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);

    #if LV_GDX_PATCH_SIMPLIFY_DRAW_EVENT
    // ignore dynamic event handler
    // and use simpified lv_obj_event_base()
    lv_event_t e;
    const lv_obj_class_t * base = obj->class_p;
    while(base && base->event_cb == NULL) base = base->base_class; // from lv_obj_event_base()
    should_draw = should_draw && base != NULL && base->event_cb != NULL;
    #endif

    if(should_draw) {
        draw_ctx->clip_area = &clip_coords_for_obj;

        #if LV_GDX_PATCH_SIMPLIFY_DRAW_EVENT
        e.target = obj;
        e.current_target = obj;
        e.user_data = NULL;
        e.param = draw_ctx;
        e.deleted = 0;
        e.stop_bubbling = 0;
        e.stop_processing = 0;
        e.code = LV_EVENT_DRAW_MAIN_BEGIN;
        base->event_cb(base, &e);
        e.code = LV_EVENT_DRAW_MAIN;
        base->event_cb(base, &e);
        e.code = LV_EVENT_DRAW_MAIN_END;
        base->event_cb(base, &e);
        #else
        lv_event_send(obj, LV_EVENT_DRAW_MAIN_BEGIN, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_MAIN, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_MAIN_END, draw_ctx);
        #endif
#if LV_USE_REFR_DEBUG
        lv_color_t debug_color = lv_color_make(lv_rand(0, 0xFF), lv_rand(0, 0xFF), lv_rand(0, 0xFF));
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_color.full = debug_color.full;
        draw_dsc.bg_opa = LV_OPA_20;
        draw_dsc.border_width = 1;
        draw_dsc.border_opa = LV_OPA_30;
        draw_dsc.border_color = debug_color;
        lv_draw_rect(draw_ctx, &draw_dsc, &obj_coords_ext);
#endif
    }

    /*With overflow visible keep the previous clip area to let the children visible out of this object too
     *With not overflow visible limit the clip are to the object's coordinates to clip the children*/
    lv_area_t clip_coords_for_children;
    bool refr_children = true;
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE)) {
        clip_coords_for_children  = *clip_area_ori;
    }
    else {
        if(!_lv_area_intersect(&clip_coords_for_children, clip_area_ori, &obj->coords)) {
            refr_children = false;
        }
    }

    if(refr_children) {
        draw_ctx->clip_area = &clip_coords_for_children;
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            refr_obj(draw_ctx, child);
        }
    }

    /*If the object was visible on the clip area call the post draw events too*/
    if(should_draw) {
        draw_ctx->clip_area = &clip_coords_for_obj;

        #if LV_GDX_PATCH_SIMPLIFY_DRAW_EVENT
        e.code = LV_EVENT_DRAW_POST_BEGIN;
        base->event_cb(base, &e);
        e.code = LV_EVENT_DRAW_POST;
        base->event_cb(base, &e);
        e.code = LV_EVENT_DRAW_POST_END;
        base->event_cb(base, &e);
        #else
        /*If all the children are redrawn make 'post draw' draw*/
        lv_event_send(obj, LV_EVENT_DRAW_POST_BEGIN, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_POST, draw_ctx);
        lv_event_send(obj, LV_EVENT_DRAW_POST_END, draw_ctx);
        #endif
    }

    draw_ctx->clip_area = clip_area_ori;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_GDX_PATCH_SIMPLIFY_COVER_CHECK_EVENT
static inline void _lv_refr_send_event_directly(lv_obj_t * obj, lv_event_code_t event_code, void * param)
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
#endif

/**
 * Search the most top object which fully covers an area
 * @param area_p pointer to an area
 * @param obj the first object to start the searching (typically a screen)
 * @return
 */
lv_obj_t * lv_refr_get_top_obj_opt(const lv_area_t * area_p, lv_obj_t * obj)
{
    lv_obj_t * found_p = NULL;

    if(_lv_area_is_in(area_p, &obj->coords, 0) == false) return NULL;
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return NULL;
    if(_lv_obj_get_layer_type(obj) != LV_LAYER_TYPE_NONE) return NULL;

    /*If this object is fully cover the draw area then check the children too*/
    lv_cover_check_info_t info;
    info.res = LV_COVER_RES_COVER;
    info.area = area_p;
    #if LV_GDX_PATCH_SIMPLIFY_COVER_CHECK_EVENT
    _lv_refr_send_event_directly(obj, LV_EVENT_COVER_CHECK, &info);
    #else
    lv_event_send(obj, LV_EVENT_COVER_CHECK, &info);
    #endif
    if(info.res == LV_COVER_RES_MASKED) return NULL;

    int32_t i;
    int32_t child_cnt = lv_obj_get_child_cnt(obj);
    for(i = child_cnt - 1; i >= 0; i--) {
        lv_obj_t * child = obj->spec_attr->children[i];
        found_p = lv_refr_get_top_obj_opt(area_p, child);

        /*If a children is ok then break*/
        if(found_p != NULL) {
            break;
        }
    }

    /*If no better children use this object*/
    if(found_p == NULL && info.res == LV_COVER_RES_COVER) {
        found_p = obj;
    }

    return found_p;
}

/**
 * Make the refreshing from an object. Draw all its children and the youngers too.
 * @param top_p pointer to an objects. Start the drawing from it.
 * @param mask_p pointer to an area, the objects will be drawn only here
 */
void refr_obj_and_children_opt(lv_draw_ctx_t * draw_ctx, lv_obj_t * top_obj)
{
    /*Normally always will be a top_obj (at least the screen)
     *but in special cases (e.g. if the screen has alpha) it won't.
     *In this case use the screen directly*/
    if(top_obj == NULL) top_obj = lv_disp_get_scr_act(_lv_refr_get_disp_refreshing());
    if(top_obj == NULL) return;  /*Shouldn't happen*/

    /*Refresh the top object and its children*/
    refr_obj(draw_ctx, top_obj);

    #if LV_GDX_PATCH_SIMPLIFY_DRAW_EVENT
    // ignore dynamical event handler
    // and use simpified lv_obj_event_base()
    lv_event_t e;
    e.user_data = NULL;
    e.param = draw_ctx;
    e.deleted = 0;
    e.stop_bubbling = 0;
    e.stop_processing = 0;
    #endif

    /*Draw the 'younger' sibling objects because they can be on top_obj*/
    lv_obj_t * parent;
    lv_obj_t * border_p = top_obj;

    parent = lv_obj_get_parent(top_obj);

    /*Do until not reach the screen*/
    while(parent != NULL) {
        bool go = false;
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(parent);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = parent->spec_attr->children[i];
            if(!go) {
                if(child == border_p) go = true;
            }
            else {
                /*Refresh the objects*/
                refr_obj(draw_ctx, child);
            }
        }

        #if LV_GDX_PATCH_SIMPLIFY_DRAW_EVENT
        const lv_obj_class_t * base = parent->class_p;
        while(base && base->event_cb == NULL) base = base->base_class; // from lv_obj_event_base()
        if (base && base->event_cb != NULL)
        {
            e.target = parent;
            e.current_target = parent;
            e.code = LV_EVENT_DRAW_POST_BEGIN;
            base->event_cb(base, &e);
            e.code = LV_EVENT_DRAW_POST;
            base->event_cb(base, &e);
            e.code = LV_EVENT_DRAW_POST_END;
            base->event_cb(base, &e);
        }
        #else
        /*Call the post draw draw function of the parents of the to object*/
        lv_event_send(parent, LV_EVENT_DRAW_POST_BEGIN, (void *)draw_ctx);
        lv_event_send(parent, LV_EVENT_DRAW_POST, (void *)draw_ctx);
        lv_event_send(parent, LV_EVENT_DRAW_POST_END, (void *)draw_ctx);
        #endif

        /*The new border will be the last parents,
         *so the 'younger' brothers of parent will be refreshed*/
        border_p = parent;
        /*Go a level deeper*/
#if LV_GDX_PATCH_GET_PARENT_DIRECTLY
        parent = parent->parent;
#else
        /*optimized*/parent = lv_obj_get_parent(parent);
#endif
    }
}
