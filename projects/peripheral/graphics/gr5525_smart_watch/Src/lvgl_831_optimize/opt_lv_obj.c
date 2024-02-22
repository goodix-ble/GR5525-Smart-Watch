/**
 * @file lv_obj.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "lv_indev.h"
#include "lv_refr.h"
#include "lv_group.h"
#include "lv_disp.h"
#include "lv_theme.h"
#include "../misc/lv_assert.h"
#include "../draw/lv_draw.h"
#include "../misc/lv_anim.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_async.h"
#include "../misc/lv_fs.h"
#include "../misc/lv_gc.h"
#include "../misc/lv_math.h"
#include "../misc/lv_log.h"
#include "../hal/lv_hal.h"
#include "../extra/lv_extra.h"
#include <stdint.h>
#include <string.h>

#include "lv_conf.h"

#if LV_USE_GPU_STM32_DMA2D
    #include "../draw/stm32_dma2d/lv_gpu_stm32_dma2d.h"
#endif

#if LV_USE_GPU_SWM341_DMA2D
    #include "../draw/swm341_dma2d/lv_gpu_swm341_dma2d.h"
#endif

#if LV_USE_GPU_NXP_PXP && LV_USE_GPU_NXP_PXP_AUTO_INIT
    #include "../draw/nxp/pxp/lv_gpu_nxp_pxp.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_obj_class
#define LV_OBJ_DEF_WIDTH    (LV_DPX(100))
#define LV_OBJ_DEF_HEIGHT   (LV_DPX(50))
#define STYLE_TRANSITION_MAX 32

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_scrollbar(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx);
static lv_res_t scrollbar_init_draw_dsc(lv_obj_t * obj, lv_draw_rect_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*=====================
 * Setter functions
 *====================*/

/*-----------------
 * Attribute set
 *----------------*/

/*=======================
 * Getter functions
 *======================*/

/*-------------------
 * OTHER FUNCTIONS
 *------------------*/

/**********************
 *   STATIC FUNCTIONS
 **********************/
#if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
void lv_obj_update_style_cache_for_label_dsc_only(lv_obj_t * obj) {
    obj->style_cache.main_part.text_opa = lv_obj_get_style_text_opa(obj, LV_PART_MAIN);
    if(obj->style_cache.main_part.text_opa <= LV_OPA_MIN) return;
    obj->style_cache.main_part.text_color_filtered = lv_obj_get_style_text_color_filtered(obj, LV_PART_MAIN);
    obj->style_cache.main_part.text_letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
    obj->style_cache.main_part.text_line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    obj->style_cache.main_part.text_decor = lv_obj_get_style_text_decor(obj, LV_PART_MAIN);
    obj->style_cache.main_part.text_font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    obj->style_cache.main_part.text_align = lv_obj_get_style_text_align(obj, LV_PART_MAIN);
}

void lv_obj_update_style_cache_for_obj(lv_obj_t * obj) {
    //obj->style_cache.main_part.width = lv_obj_get_style_width(obj, LV_PART_MAIN);
    //obj->style_cache.main_part.height = lv_obj_get_style_height(obj, LV_PART_MAIN);

    obj->style_cache.main_part.pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    obj->style_cache.main_part.pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    obj->style_cache.main_part.pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    obj->style_cache.main_part.pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

#if LV_GDX_PATCH_REMOVE_BORDER
#else
    obj->style_cache.main_part.border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
#endif

    obj->style_cache.main_part.opa = lv_obj_get_style_opa(obj, LV_PART_MAIN);
    obj->style_cache.main_part.bg_opa = lv_obj_get_style_bg_opa(obj, LV_PART_MAIN);
    obj->style_cache.main_part.bg_color = lv_obj_get_style_bg_color(obj, LV_PART_MAIN);

    obj->style_cache.main_part.base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);

    obj->style_cache.main_part.blend_mode = lv_obj_get_style_blend_mode(obj, LV_PART_MAIN);
    obj->style_cache.main_part.bg_img_src = lv_obj_get_style_bg_img_src(obj, LV_PART_MAIN);

    obj->style_cache.main_part.clip_corner = lv_obj_get_style_clip_corner(obj, LV_PART_MAIN);
#if LV_GDX_PATCH_REMOVE_BORDER
#else
    obj->style_cache.main_part.border_post = lv_obj_get_style_border_post(obj, LV_PART_MAIN);
#endif
    obj->style_cache.main_part.radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);
#if !LV_GDX_PATCH_DISABLE_TRANSFORM
    obj->style_cache.main_part.transform_width = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
    obj->style_cache.main_part.transform_height = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
#endif
    if (obj->style_cache.enable_label_dsc)
    {
        lv_obj_update_style_cache_for_label_dsc_only(obj);
    }
}
#endif

static void lv_obj_draw(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t * info = lv_event_get_param(e);
        if(info->res == LV_COVER_RES_MASKED) return;

        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        bool cache_available = (obj->style_cache.cache_invalid == 0) && (obj->style_cache.cached_for_state == obj->state);
        bool status = cache_available ? obj->style_cache.main_part.clip_corner : lv_obj_get_style_clip_corner(obj, LV_PART_MAIN);
        //bool status = obj->style_cache.main_part.clip_corner;
        #else
        /*cached*/bool status = lv_obj_get_style_clip_corner(obj, LV_PART_MAIN);
        #endif
        if(status) {
            info->res = LV_COVER_RES_MASKED;
            return;
        }

        /*Most trivial test. Is the mask fully IN the object? If no it surely doesn't cover it*/
        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        lv_coord_t r;
        lv_coord_t w;
        lv_coord_t h;
        if (cache_available) {
            r = obj->style_cache.main_part.radius;
            #if LV_GDX_PATCH_DISABLE_TRANSFORM
            w = 0;
            h = 0;
            #else
            w = obj->style_cache.main_part.transform_width;
            h = obj->style_cache.main_part.transform_height;
            #endif
        }
        else {
            r = lv_obj_get_style_radius(obj, LV_PART_MAIN);
            #if LV_GDX_PATCH_DISABLE_TRANSFORM
            w = 0;
            h = 0;
            #else
            w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
            h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
            #endif
        }
        #else
        /*cached*/lv_coord_t r = lv_obj_get_style_radius(obj, LV_PART_MAIN);
        #if LV_GDX_PATCH_DISABLE_TRANSFORM
        lv_coord_t w = 0;
        lv_coord_t h = 0;
        #else
        /*cached*/lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
        /*cached*/lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
        #endif
        #endif
        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.x1 -= w;
        coords.x2 += w;
        coords.y1 -= h;
        coords.y2 += h;

        if(_lv_area_is_in(info->area, &coords, r) == false) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        lv_opa_t opa = obj->style_cache.main_part.bg_opa;
        #else
        /*cached*/lv_opa_t opa = lv_obj_get_style_bg_opa(obj, LV_PART_MAIN);
        #endif
        if(opa < LV_OPA_MAX) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        info->res = LV_COVER_RES_COVER;

    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        /*If the border is drawn later disable loading its properties*/
#if LV_GDX_PATCH_REMOVE_BORDER
#else
        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        if(obj->style_cache.main_part.border_post) {
            draw_dsc.border_post = 1;
        }
        #else
        /*cached*/if(lv_obj_get_style_border_post(obj, LV_PART_MAIN)) {
            draw_dsc.border_post = 1;
        }
        #endif
#endif
        lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &draw_dsc);

#if LV_GDX_PATCH_REDUCE_REDUN > 0u
        /* Find empty drawing, and return directly */
        {
            if((draw_dsc.bg_opa <= LV_OPA_MIN) && ((draw_dsc.bg_img_opa <= LV_OPA_MIN) || (draw_dsc.bg_img_src == NULL))) {
                return;
            }
        }
#endif


        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        #if !LV_GDX_PATCH_DISABLE_TRANSFORM
        lv_coord_t w = obj->style_cache.main_part.transform_width;
        lv_coord_t h = obj->style_cache.main_part.transform_height;
        #else
        lv_coord_t w = 0;
        lv_coord_t h = 0;
        #endif
        #else
        /*cached*/lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
        /*cached*/lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
        #endif

        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.x1 -= w;
        coords.x2 += w;
        coords.y1 -= h;
        coords.y2 += h;

        lv_obj_draw_part_dsc_t part_dsc;
        lv_obj_draw_dsc_init(&part_dsc, draw_ctx);
        part_dsc.class_p = MY_CLASS;
        part_dsc.type = LV_OBJ_DRAW_PART_RECTANGLE;
        part_dsc.rect_dsc = &draw_dsc;
        part_dsc.draw_area = &coords;
        part_dsc.part = LV_PART_MAIN;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_dsc);

#if LV_DRAW_COMPLEX
        /*With clip corner enabled draw the bg img separately to make it clipped*/
        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        bool clip_corner = (obj->style_cache.main_part.clip_corner != 0 && draw_dsc.radius != 0) ? true : false;
        #else
        /*cached*/bool clip_corner = (lv_obj_get_style_clip_corner(obj, LV_PART_MAIN) && draw_dsc.radius != 0) ? true : false;
        #endif
        const void * bg_img_src = draw_dsc.bg_img_src;
        if(clip_corner) {
            draw_dsc.bg_img_src = NULL;
        }
#endif

        if (bg_img_src)
        {
            lv_draw_rect(draw_ctx, &draw_dsc, &coords);
        } else {
            lv_draw_rect(draw_ctx, &draw_dsc, &coords);
        }


#if LV_DRAW_COMPLEX
        if(clip_corner) {
            lv_draw_mask_radius_param_t * mp = lv_mem_buf_get(sizeof(lv_draw_mask_radius_param_t));
            lv_draw_mask_radius_init(mp, &obj->coords, draw_dsc.radius, false);
            /*Add the mask and use `obj+8` as custom id. Don't use `obj` directly because it might be used by the user*/
            lv_draw_mask_add(mp, obj + 8);

            if(bg_img_src) {
                draw_dsc.bg_opa = LV_OPA_TRANSP;
#if LV_GDX_PATCH_REMOVE_BORDER
#else
                draw_dsc.border_opa = LV_OPA_TRANSP;
#endif
#if LV_GDX_PATCH_REMOVE_OUTLINE
#else
                draw_dsc.outline_opa = LV_OPA_TRANSP;
#endif
#if LV_GDX_PATCH_REMOVE_SHADOW
#else
                draw_dsc.shadow_opa = LV_OPA_TRANSP;
#endif
                draw_dsc.bg_img_src = bg_img_src;
                lv_draw_rect(draw_ctx, &draw_dsc, &coords);
            }

        }
#endif
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_dsc);
    }
    else if(code == LV_EVENT_DRAW_POST) {
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
#if LV_GDX_PATCH_DISABLE_SCROLLABLE_DEFAULT
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE))
        {
            draw_scrollbar(obj, draw_ctx);
        }
#else
        draw_scrollbar(obj, draw_ctx);
#endif

#if LV_DRAW_COMPLEX
        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        if(obj->style_cache.main_part.clip_corner) {
        #else
        /*cached*/if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) {
        #endif
            lv_draw_mask_radius_param_t * param = lv_draw_mask_remove_custom(obj + 8);
            if(param) {
                lv_draw_mask_free_param(param);
                lv_mem_buf_release(param);
            }
        }
#endif

        /*If the border is drawn later disable loading other properties*/
#if LV_GDX_PATCH_REMOVE_BORDER
#else
        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        bool status = obj->style_cache.main_part.border_post;
        #else
        /*cached*/bool status = lv_obj_get_style_border_post(obj, LV_PART_MAIN);
        #endif

        if(status) {
            lv_draw_rect_dsc_t draw_dsc;
            lv_draw_rect_dsc_init(&draw_dsc);
            draw_dsc.bg_opa = LV_OPA_TRANSP;
            draw_dsc.bg_img_opa = LV_OPA_TRANSP;
#if LV_GDX_PATCH_REMOVE_OUTLINE
#else
            draw_dsc.outline_opa = LV_OPA_TRANSP;
#endif
#if LV_GDX_PATCH_REMOVE_SHADOW
#else
            draw_dsc.shadow_opa = LV_OPA_TRANSP;
#endif
            lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &draw_dsc);

#if LV_GDX_PATCH_REDUCE_REDUN > 0u
            /* Find empty drawing, and return directly */
            {
                if((draw_dsc.bg_opa <= LV_OPA_MIN) && ((draw_dsc.bg_img_opa <= LV_OPA_MIN) || (draw_dsc.bg_img_src == NULL))) {
                    return;
                }
            }
#endif
            #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
            #if !LV_GDX_PATCH_DISABLE_TRANSFORM
            lv_coord_t w = obj->style_cache.main_part.transform_width;
            lv_coord_t h = obj->style_cache.main_part.transform_height;
            #else
            lv_coord_t w = 0;
            lv_coord_t h = 0;
            #endif
            #else
            /*cached*/lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
            /*cached*/lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
            #endif
            lv_area_t coords;
            lv_area_copy(&coords, &obj->coords);
            coords.x1 -= w;
            coords.x2 += w;
            coords.y1 -= h;
            coords.y2 += h;

            lv_obj_draw_part_dsc_t part_dsc;
            lv_obj_draw_dsc_init(&part_dsc, draw_ctx);
            part_dsc.class_p = MY_CLASS;
            part_dsc.type = LV_OBJ_DRAW_PART_BORDER_POST;
            part_dsc.rect_dsc = &draw_dsc;
            part_dsc.draw_area = &coords;
            part_dsc.part = LV_PART_MAIN;
            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_dsc);

            lv_draw_rect(draw_ctx, &draw_dsc, &coords);
            lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_dsc);
        }
#endif
    }
}

static void draw_scrollbar(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx)
{

    lv_area_t hor_area;
    lv_area_t ver_area;
    lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);

    if(lv_area_get_size(&hor_area) <= 0 && lv_area_get_size(&ver_area) <= 0) return;

    lv_draw_rect_dsc_t draw_dsc;
    lv_res_t sb_res = scrollbar_init_draw_dsc(obj, &draw_dsc);
    if(sb_res != LV_RES_OK) return;

    lv_obj_draw_part_dsc_t part_dsc;
    lv_obj_draw_dsc_init(&part_dsc, draw_ctx);
    part_dsc.class_p = MY_CLASS;
    part_dsc.type = LV_OBJ_DRAW_PART_SCROLLBAR;
    part_dsc.rect_dsc = &draw_dsc;
    part_dsc.part = LV_PART_SCROLLBAR;

    if(lv_area_get_size(&hor_area) > 0) {
        part_dsc.draw_area = &hor_area;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_dsc);
        lv_draw_rect(draw_ctx, &draw_dsc, &hor_area);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_dsc);
    }
    if(lv_area_get_size(&ver_area) > 0) {
        part_dsc.draw_area = &ver_area;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_dsc);
        part_dsc.draw_area = &ver_area;
        lv_draw_rect(draw_ctx, &draw_dsc, &ver_area);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_dsc);
    }
}

/**
 * Initialize the draw descriptor for the scrollbar
 * @param obj pointer to an object
 * @param dsc the draw descriptor to initialize
 * @return LV_RES_OK: the scrollbar is visible; LV_RES_INV: the scrollbar is not visible
 */
static lv_res_t scrollbar_init_draw_dsc(lv_obj_t * obj, lv_draw_rect_dsc_t * dsc)
{
    lv_draw_rect_dsc_init(dsc);
    dsc->bg_opa = lv_obj_get_style_bg_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->bg_opa > LV_OPA_MIN) {
        dsc->bg_color = lv_obj_get_style_bg_color(obj, LV_PART_SCROLLBAR);
    }

#if LV_GDX_PATCH_REMOVE_BORDER
#else
    dsc->border_opa = lv_obj_get_style_border_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->border_opa > LV_OPA_MIN) {
        dsc->border_width = lv_obj_get_style_border_width(obj, LV_PART_SCROLLBAR);
        if(dsc->border_width > 0) {
            dsc->border_color = lv_obj_get_style_border_color(obj, LV_PART_SCROLLBAR);
        }
        else {
            dsc->border_opa = LV_OPA_TRANSP;
        }
    }
#endif

#if LV_DRAW_COMPLEX
#if LV_GDX_PATCH_REMOVE_SHADOW
#else
    dsc->shadow_opa = lv_obj_get_style_shadow_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->shadow_opa > LV_OPA_MIN) {
        dsc->shadow_width = lv_obj_get_style_shadow_width(obj, LV_PART_SCROLLBAR);
        if(dsc->shadow_width > 0) {
            dsc->shadow_spread = lv_obj_get_style_shadow_spread(obj, LV_PART_SCROLLBAR);
            dsc->shadow_color = lv_obj_get_style_shadow_color(obj, LV_PART_SCROLLBAR);
        }
        else {
            dsc->shadow_opa = LV_OPA_TRANSP;
        }
    }
#endif

    lv_opa_t opa = lv_obj_get_style_opa(obj, LV_PART_SCROLLBAR);
    if(opa < LV_OPA_MAX) {
        dsc->bg_opa = (dsc->bg_opa * opa) >> 8;
#if LV_GDX_PATCH_REMOVE_BORDER
#else
        dsc->border_opa = (dsc->bg_opa * opa) >> 8;
#endif
#if LV_GDX_PATCH_REMOVE_SHADOW
#else
        dsc->shadow_opa = (dsc->bg_opa * opa) >> 8;
#endif
    }

#if (LV_GDX_PATCH_REMOVE_SHADOW && LV_GDX_PATCH_REMOVE_BORDER)
    if(dsc->bg_opa != LV_OPA_TRANSP) {
#elif LV_GDX_PATCH_REMOVE_SHADOW
    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->border_opa != LV_OPA_TRANSP) {
#elif LV_GDX_PATCH_REMOVE_BORDER
    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->shadow_opa != LV_OPA_TRANSP) {
#else
    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->border_opa != LV_OPA_TRANSP || dsc->shadow_opa != LV_OPA_TRANSP) {
#endif
        dsc->radius = lv_obj_get_style_radius(obj, LV_PART_SCROLLBAR);
        return LV_RES_OK;
    }
    else {
        return LV_RES_INV;
    }
#else
    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->border_opa != LV_OPA_TRANSP) return LV_RES_OK;
    else return LV_RES_INV;
#endif
}

#if LV_GDX_PATCH_RAM_CACHE
__attribute__((section("RAM_CODE"))) 
#endif
void lv_obj_event_opt(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_PRESSED) {
        lv_obj_add_state(obj, LV_STATE_PRESSED);
    }
    else if(code == LV_EVENT_RELEASED) {
        lv_obj_clear_state(obj, LV_STATE_PRESSED);
        void * param = lv_event_get_param(e);
        /*Go the checked state if enabled*/
        if(lv_indev_get_scroll_obj(param) == NULL && lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE)) {
            if(!(lv_obj_get_state(obj) & LV_STATE_CHECKED)) lv_obj_add_state(obj, LV_STATE_CHECKED);
            else lv_obj_clear_state(obj, LV_STATE_CHECKED);

            lv_res_t res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
    }
    else if(code == LV_EVENT_PRESS_LOST) {
        lv_obj_clear_state(obj, LV_STATE_PRESSED);
    }
    else if(code == LV_EVENT_STYLE_CHANGED) {
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);
        for(uint32_t i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            lv_obj_mark_layout_as_dirty(child);
        }
        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        obj->style_cache.cache_invalid = 1;
        #endif
    }
    else if(code == LV_EVENT_KEY) {
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE)) {
            char c = *((char *)lv_event_get_param(e));
            if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
                lv_obj_add_state(obj, LV_STATE_CHECKED);
            }
            else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
                lv_obj_clear_state(obj, LV_STATE_CHECKED);
            }

            /*With Enter LV_EVENT_RELEASED will send VALUE_CHANGE event*/
            if(c != LV_KEY_ENTER) {
                lv_res_t res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return;
            }
        }
        else if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_WITH_ARROW) && !lv_obj_is_editable(obj)) {
            /*scroll by keypad or encoder*/
            lv_anim_enable_t anim_enable = LV_ANIM_OFF;
            lv_coord_t sl = lv_obj_get_scroll_left(obj);
            lv_coord_t sr = lv_obj_get_scroll_right(obj);
            char c = *((char *)lv_event_get_param(e));
            if(c == LV_KEY_DOWN) {
                /*use scroll_to_x/y functions to enforce scroll limits*/
                lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) + lv_obj_get_height(obj) / 4, anim_enable);
            }
            else if(c == LV_KEY_UP) {
                lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) - lv_obj_get_height(obj) / 4, anim_enable);
            }
            else if(c == LV_KEY_RIGHT) {
                /*If the object can't be scrolled horizontally then scroll it vertically*/
                if(!((lv_obj_get_scroll_dir(obj) & LV_DIR_HOR) && (sl > 0 || sr > 0)))
                    lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) + lv_obj_get_height(obj) / 4, anim_enable);
                else
                    lv_obj_scroll_to_x(obj, lv_obj_get_scroll_x(obj) + lv_obj_get_width(obj) / 4, anim_enable);
            }
            else if(c == LV_KEY_LEFT) {
                /*If the object can't be scrolled horizontally then scroll it vertically*/
                if(!((lv_obj_get_scroll_dir(obj) & LV_DIR_HOR) && (sl > 0 || sr > 0)))
                    lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) - lv_obj_get_height(obj) / 4, anim_enable);
                else
                    lv_obj_scroll_to_x(obj, lv_obj_get_scroll_x(obj) - lv_obj_get_width(obj) / 4, anim_enable);
            }
        }
    }
    else if(code == LV_EVENT_FOCUSED) {
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS)) {
            lv_obj_scroll_to_view_recursive(obj, LV_ANIM_ON);
        }

        bool editing = false;
        editing = lv_group_get_editing(lv_obj_get_group(obj));
        lv_state_t state = LV_STATE_FOCUSED;

        /* Use the indev for then indev handler.
         * But if the obj was focused manually it returns NULL so try to
         * use the indev from the event*/
        lv_indev_t * indev = lv_indev_get_act();
        if(indev == NULL) indev = lv_event_get_indev(e);

        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type == LV_INDEV_TYPE_KEYPAD || indev_type == LV_INDEV_TYPE_ENCODER) state |= LV_STATE_FOCUS_KEY;
        if(editing) {
            state |= LV_STATE_EDITED;
            lv_obj_add_state(obj, state);
        }
        else {
            lv_obj_add_state(obj, state);
            lv_obj_clear_state(obj, LV_STATE_EDITED);
        }
    }
    else if(code == LV_EVENT_SCROLL_BEGIN) {
        lv_obj_add_state(obj, LV_STATE_SCROLLED);
    }
    else if(code == LV_EVENT_SCROLL_END) {
        lv_obj_clear_state(obj, LV_STATE_SCROLLED);
        if(lv_obj_get_scrollbar_mode(obj) == LV_SCROLLBAR_MODE_ACTIVE) {
            lv_area_t hor_area, ver_area;
            lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);
            lv_obj_invalidate_area(obj, &hor_area);
            lv_obj_invalidate_area(obj, &ver_area);
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_obj_clear_state(obj, LV_STATE_FOCUSED | LV_STATE_EDITED | LV_STATE_FOCUS_KEY);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        lv_coord_t align = lv_obj_get_style_align(obj, LV_PART_MAIN);
        uint16_t layout = lv_obj_get_style_layout(obj, LV_PART_MAIN);
        if(layout || align) {
            lv_obj_mark_layout_as_dirty(obj);
        }

        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            lv_obj_mark_layout_as_dirty(child);
        }
    }
    else if(code == LV_EVENT_CHILD_CHANGED) {
        lv_coord_t w = lv_obj_get_style_width(obj, LV_PART_MAIN);
        lv_coord_t h = lv_obj_get_style_height(obj, LV_PART_MAIN);
        lv_coord_t align = lv_obj_get_style_align(obj, LV_PART_MAIN);
        uint16_t layout = lv_obj_get_style_layout(obj, LV_PART_MAIN);
        if(layout || align || w == LV_SIZE_CONTENT || h == LV_SIZE_CONTENT) {
            lv_obj_mark_layout_as_dirty(obj);
        }
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t d = lv_obj_calculate_ext_draw_size(obj, LV_PART_MAIN);
        lv_event_set_ext_draw_size(e, d);
    }
    else if(code == LV_EVENT_DRAW_MAIN || code == LV_EVENT_DRAW_POST || code == LV_EVENT_COVER_CHECK) {
        lv_obj_draw(e);
    }
    #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
    else if(code == LV_EVENT_DRAW_MAIN_BEGIN) 
    {
        if (obj->style_cache.cache_invalid || obj->style_cache.cached_for_state != obj->state)
        {
            lv_obj_update_style_cache_for_obj(obj);
            obj->style_cache.cached_for_state = obj->state;
            obj->style_cache.cache_invalid = 0;
        }
    }
    #endif
}
