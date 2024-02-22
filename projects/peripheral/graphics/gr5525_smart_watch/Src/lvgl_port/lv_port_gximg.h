#ifndef __LV_PORT_GXIMG_H__
#define __LV_PORT_GXIMG_H__

#include "lvgl.h"
#include "lv_img_buf.h"

#define LV_GXIMG_DECLARE(var_name) extern const lv_gximg_dsc_t var_name;
#define LV_GXIMG_BUNDLE_DECLARE(var_name) extern const lv_gximg_bundle_dsc_t var_name;

typedef struct
{
    lv_img_header_t header;
    uint16_t center_x;
    uint16_t center_y;
    uint16_t start_y;
    uint16_t total_lines : 15;
    uint16_t line_table : 1;
    uint32_t data_size;
    const uint8_t *data;
} lv_gximg_dsc_t;

typedef struct
{
    lv_img_header_t header;
    const lv_gximg_dsc_t* group[16];
} lv_gximg_bundle_dsc_t;


#if LV_GDX_PATCH_GX_IMG
void lv_gximg_init(void);
void lv_port_gximg_set_img_pos(lv_obj_t *img, lv_coord_t center_x, lv_coord_t center_y);
void lv_port_gximg_draw(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *draw_dsc, const lv_area_t *coords, const void *src, bool flip_x, bool flip_y);
void lv_port_gximg_draw_bundle(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *draw_dsc, const lv_area_t *coords, const void *src);
#else
#define LV_IMG_CF_GDX_GXIMG         22
#define LV_IMG_CF_GDX_GXIMG_BUNDLE  23
#define lv_gximg_init(void)
#define lv_port_gximg_set_img_pos(img, center_x, center_y)
#define lv_port_gximg_draw(draw_ctx, draw_dsc, coords, src, flip_x, flip_y)
#define lv_port_gximg_draw_bundle(draw_ctx, draw_dsc, coords, src)
#endif // LV_GDX_PATCH_GX_IMG

#endif // __LV_PORT_GXIMG_H__
