/**
 * @file lv_img_rgb565.c
 *
 * 只绘制背景图片，专门显示rgb565格式的图片。
 * 只接受lv_img_dsc_t格式的图片资源。
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_rgb565.h"


/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_img_rgb565_class
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_img_rgb565_event(const lv_obj_class_t *class_p, lv_event_t *evt);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_img_rgb565_class = {
    .constructor_cb = NULL,
    .destructor_cb = NULL,
    .event_cb = lv_img_rgb565_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_obj_t),
    .base_class = &lv_obj_class,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *lv_img_rgb565_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_img_rgb565_set_src(lv_obj_t * obj, const void * src)
{
    lv_obj_set_style_bg_img_src(obj, src, LV_PART_MAIN);

    // Update cache directly
    #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
    obj->style_cache.main_part.bg_img_src = src;
    #endif

    lv_img_dsc_t * p_img_dsc = (lv_img_dsc_t *)src;
    lv_obj_set_size(obj, p_img_dsc->header.w, p_img_dsc->header.h);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void blit_rgb565(lv_draw_ctx_t * draw_ctx, const lv_area_t * coords, lv_img_header_t * p_img_header, const void * p_img_data)
{
    bool                    union_ok;
    lv_area_t               safe_clip_area;

    // 1. intersect buff_area and clip_area, get new safe clip_area. this step may be redundancy(the result equals clip_area usually)
    // union_ok = _lv_area_intersect(&safe_clip_area, draw_ctx->clip_area, draw_ctx->buf_area);
    // if(!union_ok)
    //     return LV_RES_INV;
    safe_clip_area = *(draw_ctx->clip_area);

    // 2. intersect safe_clip_area and image coords, get new safe clip_area.
    union_ok = _lv_area_intersect(&safe_clip_area, &safe_clip_area, coords);
    if(!union_ok)
        return;

    // 3. compare the safe_clip_area and coords, find the image position info.
    uint32_t img_clip_w = safe_clip_area.x2 - safe_clip_area.x1 + 1;
    uint32_t img_clip_h = safe_clip_area.y2 - safe_clip_area.y1 + 1;
    uint32_t img_clip_offset_x = safe_clip_area.x1 - coords->x1;
    uint32_t img_clip_offset_y = safe_clip_area.y1 - coords->y1;

    uint32_t src_stride = p_img_header->w;
    uint32_t src_offset = (img_clip_offset_x + img_clip_offset_y * src_stride);
    uint32_t dst_stride = lv_area_get_width(draw_ctx->buf_area);
    uint32_t dst_offset = ((safe_clip_area.x1 - draw_ctx->buf_area->x1) + (safe_clip_area.y1 - draw_ctx->buf_area->y1) * dst_stride);

    const lv_color_t * p_src_buf = p_img_data;
    lv_color_t * p_dst_buf = draw_ctx->buf;
    uint32_t size_per_line = img_clip_w * sizeof(lv_color_t);

    p_src_buf += src_offset;
    p_dst_buf += dst_offset;

    if (src_stride == dst_stride && dst_stride == img_clip_w)
    {
        lv_memcpy(p_dst_buf, p_src_buf, img_clip_h * src_stride * sizeof(lv_color_t));
    }
    else {
        for (uint32_t y = img_clip_h; y != 0; y--)
        {
            lv_memcpy(p_dst_buf, p_src_buf, size_per_line);
            p_src_buf += src_stride;
            p_dst_buf += dst_stride;
        }
    }
}


extern lv_res_t decode_simple_rgb565(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                                                      const lv_area_t * coords, const void * src, int w, int h);

static void lv_img_rgb565_event(const lv_obj_class_t *class_p, lv_event_t *evt)
{
    lv_obj_t * obj = evt->current_target;
    lv_event_code_t code = evt->code;
    //class_p->base_class->event_cb(class_p->base_class, evt);
    if (code == LV_EVENT_DRAW_MAIN) {
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(evt);

        const void * rgb565_img_src;
        #if LV_GDX_PATCH_STYLE_CACHE_FOR_OBJ
        rgb565_img_src = obj->style_cache.main_part.bg_img_src;
        #else
        /*cached*/rgb565_img_src = lv_obj_get_style_bg_img_src(obj, LV_PART_MAIN);
        #endif

        #if 0
        lv_img_header_t header;
        lv_res_t res = lv_img_decoder_get_info(rgb565_img_src, &header);
        if (res  == LV_RES_OK && (header.cf == LV_IMG_CF_RGB565 || header.cf == LV_IMG_CF_GDX_SIMP_RGB565 || header.cf == LV_IMG_CF_TRUE_COLOR))
        {
            // center align
            lv_area_t area;
            area.x1 = coords->x1;
            area.y1 = coords->y1;
            area.x2 = area.x1 + header.w - 1;
            area.y2 = area.y1 + header.h - 1;
            decode_simple_rgb565(draw_ctx, NULL, &area, rgb565_img_src, header.w, header.h);
        }
        #else
        lv_area_t img_coords = obj->coords;
        lv_img_dsc_t * p_img_dsc = (lv_img_dsc_t *)rgb565_img_src;
        blit_rgb565(draw_ctx, &img_coords, &p_img_dsc->header, p_img_dsc->data);
        #endif
    }
    else {
        lv_obj_class.event_cb(class_p, evt);
    }
}
