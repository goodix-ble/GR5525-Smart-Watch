#include "lv_port_gximg.h"
#include "lv_img.h"


#if LV_GDX_PATCH_GX_IMG

#define EXPAND_AREA(a) (a)->x1, (a)->y1, (a)->x2, (a)->y2

typedef struct
{
    uint16_t start_x : 12;
    uint16_t padding : 2;
    uint16_t keep_inline : 1;
    uint16_t has_alpha : 1;
    uint16_t width;
    uint16_t seg_data[];
} gximg_segment_t;

typedef struct
{
    uint32_t offset : 22;
    uint32_t y : 10;
} gximg_line_info_t;

typedef struct
{
    const lv_gximg_dsc_t *dsc;
    const gximg_line_info_t *line_table;
    uint32_t table_size;
    uint32_t used_size;
    gximg_segment_t *p_segment;
} gximg_draw_ctx_t;

static void gximg_draw_ctx_init(gximg_draw_ctx_t *ctx, const lv_gximg_dsc_t *dsc);
static const gximg_segment_t *gximg_seek_first_segment(gximg_draw_ctx_t *ctx);
static const gximg_segment_t *gximg_seek_next_segment(gximg_draw_ctx_t *ctx);
static const gximg_segment_t *gximg_jump_to_line(gximg_draw_ctx_t *ctx, uint16_t line);
static uint32_t gximg_calc_segment_data_size(gximg_segment_t *p_segment);

static lv_res_t gximg_decoder_info(lv_img_decoder_t *decoder, const void *src, lv_img_header_t *header);
static lv_res_t gximg_decoder_open(lv_img_decoder_t *decoder, lv_img_decoder_dsc_t *dsc);

static bool s_gximg_initialized = false;

void lv_gximg_init(void)
{
    if (!s_gximg_initialized)
    {
        lv_img_decoder_t *decoder = lv_img_decoder_create();
        lv_img_decoder_set_info_cb(decoder, gximg_decoder_info);
        lv_img_decoder_set_open_cb(decoder, gximg_decoder_open);
        s_gximg_initialized = true;
    }
}

__attribute__((section("RAM_CODE")))
void lv_port_gximg_draw(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *draw_dsc, const lv_area_t *coords, const void *src, bool flip_x, bool flip_y)
{
    uint8_t *draw_buf = draw_ctx->buf;
    uint16_t dst_w = lv_area_get_width(draw_ctx->buf_area);
    const uint8_t pixelsize = (LV_COLOR_DEPTH / 8);

    if (coords->y1 > draw_ctx->buf_area->y2 || coords->x1 > draw_ctx->buf_area->x2)
    {
        return;
    }

    uint16_t y_in_buf = LV_MAX(coords->y1 - draw_ctx->buf_area->y1, 0);
    uint16_t x_in_buf = LV_MAX(coords->x1 - draw_ctx->buf_area->x1, 0);
    uint8_t * dst_addr = draw_buf + (y_in_buf * dst_w + x_in_buf) * pixelsize;

    uint16_t y_min = LV_MAX(draw_ctx->clip_area->y1 - coords->y1, 0);
    uint16_t y_max = draw_ctx->clip_area->y2 - coords->y1;
    uint16_t x_min = LV_MAX(draw_ctx->clip_area->x1 - coords->x1, 0);
    uint16_t x_max = draw_ctx->clip_area->x2 - coords->x1;
    const lv_gximg_dsc_t *gximg_dsc = (lv_gximg_dsc_t *)src;

    int16_t y = gximg_dsc->start_y;

    // Create GXIMG Descriptor
    gximg_draw_ctx_t ctx;
    gximg_draw_ctx_init(&ctx, gximg_dsc);

    const gximg_segment_t *p_seg = gximg_seek_first_segment(&ctx);
    if (!p_seg->keep_inline)
    {
        y = 0xFFFF; // Experimental
    }

    while (p_seg)
    {
        if (!p_seg->keep_inline)
        {
            y++;
        }
        int16_t dy;
        if (flip_y)
        {
            dy = gximg_dsc->header.h - y - 1;

            if (dy > y_max)
            {
                // dy is not in the drawing area, keep searching
                // p_seg = gximg_seek_next_segment(&ctx);
                // continue;
                y = gximg_dsc->header.h - y_max - 1;
                dy = y_max;
                p_seg = gximg_jump_to_line(&ctx, y);
                if (!p_seg)
                {
                    return;
                }
            }

            // if dy is smaller than y_min, means we no longer need draw anymore
            if (dy < y_min)
            {
                break;
            }

            dy -= y_min;
        }
        else
        {
            // Find the real Y from image to draw
            if (y < y_min)
            {
                // p_seg = gximg_seek_next_segment(&ctx);
                // continue;
                y = y_min;
                p_seg = gximg_jump_to_line(&ctx, y);
                if (!p_seg)
                {
                    return;
                }
            }

            // if dy is beyond limitation, end draw
            if (y > y_max)
            {
                break;
            }

            // src_y1 (the real Y) => dst_y1 (0)
            dy = y - y_min;
        }

        int16_t start_x = p_seg->start_x;
        uint16_t *p_linestart = (uint16_t *)(dst_addr + (dy * dst_w) * pixelsize);
        if (p_seg->has_alpha)
        {
            // This segment has alpha info
            uint8_t *alpha_data = (uint8_t *)(p_seg->seg_data + p_seg->width);
            for (uint16_t i = 0; i < p_seg->width; i++)
            {
                int16_t dst_x = start_x + i;

                if (flip_x)
                {
                    dst_x = gximg_dsc->header.w - dst_x;
                    if (dst_x > x_max)
                    {
                        continue;
                    }
                    else if (dst_x < x_min)
                    {
                        break;
                    }
                }
                else
                {
                    if (dst_x < x_min)
                    {
                        continue;
                    }
                    else if (dst_x > x_max)
                    {
                        break;
                    }
                }

                dst_x -= x_min;

                uint16_t pixel = p_seg->seg_data[i];
                uint8_t alpha = alpha_data[i];

                if (alpha == 255)
                {
                    // Opaque, simple copy
                    p_linestart[dst_x] = pixel;
                }
                else
                {
                    // Translucent, do alpha blending
                    lv_color_t src1 = {.full = pixel};
                    lv_color_t src2 = {.full = p_linestart[dst_x]};
                    lv_color_t dst = lv_color_mix(src1, src2, alpha);
                    p_linestart[dst_x] = dst.full;
                }
            }
        }
        else
        {
            for (uint16_t i = 0; i < p_seg->width; i++)
            {
                uint16_t dst_x = start_x + i;
                if (flip_x)
                {
                    dst_x = gximg_dsc->header.w - dst_x;
                    if (dst_x > x_max)
                    {
                        continue;
                    }
                    else if (dst_x < x_min)
                    {
                        break;
                    }
                }
                else
                {
                    if (dst_x < x_min)
                    {
                        continue;
                    }
                    else if (dst_x > x_max)
                    {
                        break;
                    }
                }

                dst_x -= x_min;
                uint16_t pixel = p_seg->seg_data[i];
                p_linestart[dst_x] = pixel;
            }
        }
        p_seg = gximg_seek_next_segment(&ctx);
    }
}

void lv_port_gximg_draw_bundle(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *draw_dsc, const lv_area_t *coords, const void *src)
{
    uint16_t angle = (3600 - draw_dsc->angle) % 3600;
    uint8_t quadrant = angle / 900 + 1;
    uint8_t rsc_idx = (angle % 900) / 60;
    angle = rsc_idx * 60;

    bool flip_x = false;
    bool flip_y = false;

    switch (quadrant)
    {
        case 1:
            break;

        case 2:
            rsc_idx = 15 - rsc_idx;
            flip_x = true;
            break;

        case 3:
            flip_x = true;
            flip_y = true;
            break;

        case 4:
            rsc_idx = 15 - rsc_idx;
            flip_y = true;
            break;
    }

    if (angle == 900 || angle == 2700)
    {
        rsc_idx = 15;
        flip_x = false;
    }

    lv_point_t center_point = {
        .x = coords->x1 + draw_dsc->pivot.x,
        .y = coords->y1 + draw_dsc->pivot.y,
    };

    lv_gximg_bundle_dsc_t *bundle = (lv_gximg_bundle_dsc_t *)src;
    const lv_gximg_dsc_t *p_rsc = bundle->group[rsc_idx];

    lv_area_t coords_temp;
    lv_area_copy(&coords_temp, coords);

    if (flip_x)
    {
        coords_temp.x1 = center_point.x + p_rsc->center_x - p_rsc->header.w;
    }
    else
    {
        coords_temp.x1 = center_point.x - p_rsc->center_x;
    }

    if (flip_y)
    {
        coords_temp.y1 = center_point.y - p_rsc->header.h + p_rsc->center_y;
    }
    else
    {
        coords_temp.y1 = center_point.y - p_rsc->center_y;
    }

    // printf("Resources Index: %d\n", rsc_idx);
    // printf("Center Point:    (%d,%d)\n", center_point.x, center_point.y);
    // printf("Coords Temp:     (%d,%d)\n", coords_temp.x1, coords_temp.y1);
    // printf("Buf Area: (%d,%d)\n", draw_ctx->buf_area->x1, draw_ctx->buf_area->y1);
    // uint16_t x_min = LV_MAX(draw_ctx->clip_area->x1 - coords_temp.x1, 0);
    // printf("X_MIN:           %d\n", x_min);

    lv_port_gximg_draw(draw_ctx, draw_dsc, &coords_temp, p_rsc, flip_x, flip_y);
}

void lv_port_gximg_set_img_pos(lv_obj_t *img, lv_coord_t center_x, lv_coord_t center_y)
{
    lv_gximg_bundle_dsc_t *bundle = (lv_gximg_bundle_dsc_t *)lv_img_get_src(img);
    lv_coord_t px = bundle->group[0]->center_x;
    lv_coord_t py = bundle->group[0]->center_y;
    lv_img_set_pivot(img, px, py);
    lv_coord_t xx = center_x - px;
    lv_coord_t yy = center_y - py;
    lv_obj_set_pos(img, xx, yy);
}

static void gximg_draw_ctx_init(gximg_draw_ctx_t *ctx, const lv_gximg_dsc_t *dsc)
{
    ctx->dsc = dsc;
    ctx->p_segment = NULL;
    ctx->used_size = 0;
    if (dsc->line_table)
    {
        ctx->line_table = (gximg_line_info_t *)dsc->data;
        ctx->table_size = dsc->total_lines - dsc->start_y - 1;
    }
    else
    {
        ctx->line_table = NULL;
        ctx->table_size = 0;
    }
}

static const gximg_segment_t *gximg_seek_first_segment(gximg_draw_ctx_t *ctx)
{
    const uint8_t *strt_ptr = ctx->dsc->data;
    if (ctx->line_table)
    {
        ctx->p_segment = (gximg_segment_t *)(ctx->line_table + ctx->table_size);
        ctx->used_size = ctx->table_size * sizeof(gximg_line_info_t) + sizeof(gximg_segment_t) + gximg_calc_segment_data_size(ctx->p_segment);
    }
    else
    {
        ctx->p_segment = (gximg_segment_t *)strt_ptr;
        ctx->used_size = sizeof(gximg_segment_t) + gximg_calc_segment_data_size(ctx->p_segment);
    }
    return ctx->p_segment;
}

static const gximg_segment_t *gximg_seek_next_segment(gximg_draw_ctx_t *ctx)
{
    if (!ctx->p_segment)
    {
        // No current segment, seek first instead
        return gximg_seek_first_segment(ctx);
    }

    if (ctx->used_size >= ctx->dsc->data_size)
    {
        // Already last node, return NULL
        // TODO: Should p_inst->p_segment also needs to be NULL?
        return NULL;
    }

    uint8_t *p_segdata = (uint8_t *)(ctx->p_segment->seg_data) + gximg_calc_segment_data_size(ctx->p_segment);
    ctx->p_segment = (gximg_segment_t *)p_segdata;
    ctx->used_size += sizeof(gximg_segment_t) + gximg_calc_segment_data_size(ctx->p_segment);

    return ctx->p_segment;
}

static const gximg_segment_t *gximg_jump_to_line(gximg_draw_ctx_t *ctx, uint16_t line)
{
    // TODO: mimic one without line table
    if (ctx->line_table)
    {
        uint8_t *p_data = (uint8_t *)(ctx->line_table + ctx->table_size);
        for (uint16_t i = 0; i < ctx->table_size; i++)
        {
            if (ctx->line_table[i].y == line)
            {
                ctx->p_segment = (gximg_segment_t *)(p_data + ctx->line_table[i].offset);
                ctx->used_size = ctx->table_size * sizeof(gximg_line_info_t) + ctx->line_table[i].offset + sizeof(gximg_segment_t) + gximg_calc_segment_data_size(ctx->p_segment);
                return ctx->p_segment;
            }
        }
    }
    return NULL;
}

static uint32_t gximg_calc_segment_data_size(gximg_segment_t *p_segment)
{
    return (p_segment->has_alpha + (LV_COLOR_DEPTH / 8)) * p_segment->width + p_segment->padding;
}

static lv_res_t gximg_decoder_info(lv_img_decoder_t *decoder, const void *src, lv_img_header_t *header)
{
#if LV_GDX_PATCH_GX_IMG > 0u
    lv_gximg_dsc_t *dsc = (lv_gximg_dsc_t  *)src;
    if (dsc->header.cf == LV_IMG_CF_GDX_GXIMG)
    {
        *header = dsc->header;
        return LV_RES_OK;
    }
    else if (dsc->header.cf == LV_IMG_CF_GDX_GXIMG_BUNDLE)
    {
        *header = ((lv_gximg_bundle_dsc_t *)src)->group[0]->header;
        header->cf = LV_IMG_CF_GDX_GXIMG_BUNDLE;
        return LV_RES_OK;
    }
#endif
    return LV_RES_INV;
}

static lv_res_t gximg_decoder_open(lv_img_decoder_t *decoder, lv_img_decoder_dsc_t *dsc)
{
    if (dsc->src_type == LV_IMG_SRC_VARIABLE)
    {
        lv_gximg_dsc_t *gximg_dsc = (lv_gximg_dsc_t *)dsc->src;
        if (gximg_dsc->data)
        {
            dsc->img_data = gximg_dsc->data;
            return LV_RES_OK;
        }
    }
    return LV_RES_INV;
}

#endif // LV_GDX_PATCH_GX_IMG
