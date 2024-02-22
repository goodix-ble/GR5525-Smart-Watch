/**
 * @file lv_draw_sw_letter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#include "../../hal/lv_hal_disp.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_style.h"
#include "../../font/lv_font.h"
#include "../../core/lv_refr.h"

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
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

__attribute__((unused)) LV_ATTRIBUTE_FAST_MEM void draw_letter_normal_opt(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,
                                                     const lv_point_t * pos, lv_font_glyph_dsc_t * g, const uint8_t * map_p)
{
#if LV_GDX_PATCH_DRAW_FONT_WITH_PALETTE
    // Note: currently only supports the case when bpp is 4.
    const uint16_t *p_color_palette = dsc->color_palette;
    uint16_t *p_buf = (uint16_t *)draw_ctx->buf;

    lv_area_t letter_area;
    letter_area.x1 = pos->x;
    letter_area.y1 = pos->y;
    letter_area.x2 = letter_area.x1 + g->box_w - 1;
    letter_area.y2 = letter_area.y1 + g->box_h - 1;

    #if 1 //optimize glyph render
    lv_area_t temp_area_1;
    lv_area_t glyph_cliped_area;
    if (_lv_area_intersect(&temp_area_1, draw_ctx->buf_area, draw_ctx->clip_area))
    {
        if (_lv_area_intersect(&glyph_cliped_area, &letter_area, &temp_area_1))
        {
            int32_t buf_idx;
            int32_t bitmap_idx;
            lv_coord_t buf_area_width = draw_ctx->buf_area->x2 - draw_ctx->buf_area->x1 + 1;
            lv_coord_t bitmap_width = g->box_w;
            lv_coord_t glyph_cliped_area_width = glyph_cliped_area.x2 - glyph_cliped_area.x1 + 1;
            lv_coord_t glyph_cliped_area_height = glyph_cliped_area.y2 - glyph_cliped_area.y1 + 1;
            lv_coord_t glyph_cliped_area_margin_left_in_buf_area = glyph_cliped_area.x1 - draw_ctx->buf_area->x1;
            lv_coord_t glyph_cliped_area_margin_left_in_letter_area = glyph_cliped_area.x1 - letter_area.x1;
            // 第一行在目标缓存的起始位置
            int32_t buf_idx_start_pos = (glyph_cliped_area.y1 - draw_ctx->buf_area->y1) * buf_area_width + glyph_cliped_area_margin_left_in_buf_area;
            // 第一行在图形数据的起始位置
            int32_t bitmap_idx_start_pos = (glyph_cliped_area.y1 - letter_area.y1) * bitmap_width + glyph_cliped_area_margin_left_in_letter_area;
            // for (int row_idx = glyph_cliped_area.y1; row_idx <= glyph_cliped_area.y2; row_idx++)
            for (int row_idx = glyph_cliped_area_height; row_idx != 0; row_idx--)
            {
                buf_idx = buf_idx_start_pos;
                bitmap_idx = bitmap_idx_start_pos;
                buf_idx_start_pos += buf_area_width;
                bitmap_idx_start_pos += bitmap_width;
                // //for (int col_idx = glyph_cliped_area.x1; col_idx <= glyph_cliped_area.x2; col_idx++)
                // for (int col_idx = glyph_cliped_area_width; col_idx != 0; col_idx--)
                // {
                //     if (bitmap_idx & 1)
                //     {
                //         p_buf[buf_idx] = p_color_palette[map_p[bitmap_idx>>1] & 0xF];
                //     }
                //     else
                //     {
                //         p_buf[buf_idx] = p_color_palette[map_p[bitmap_idx>>1] >> 4];
                //     }
                //     buf_idx++;
                //     bitmap_idx++;
                // }
                // 细化后再分别处理
                bool is_even_cliped_area_width = (glyph_cliped_area_width & 1) == 0;
                if ((bitmap_idx & 1) == 0) {
                    int bitmap_byte_idx = bitmap_idx>>1;
                    if (is_even_cliped_area_width) {
                        for (int col_idx = glyph_cliped_area_width; col_idx != 0; col_idx-=2) {
                            const uint8_t bits = map_p[bitmap_byte_idx];
                            p_buf[buf_idx] = p_color_palette[bits >> 4]; // 偶数位，第一个
                            buf_idx++;
                            p_buf[buf_idx] = p_color_palette[bits & 0xF]; // 奇数位，第二个
                            buf_idx++;
                            bitmap_byte_idx++;
                        }
                    }
                    else {
                        for (int col_idx = glyph_cliped_area_width - 1; col_idx != 0; col_idx-=2) {
                            const uint8_t bits = map_p[bitmap_byte_idx];
                            p_buf[buf_idx] = p_color_palette[bits >> 4];
                            buf_idx++;
                            p_buf[buf_idx] = p_color_palette[bits & 0xF];
                            buf_idx++;
                            bitmap_byte_idx++;
                        }
                        p_buf[buf_idx] = p_color_palette[map_p[bitmap_byte_idx] >> 4];
                    }
                }
                else {
                    // 接下来是bitmap idx为奇数的情况，
                    int bitmap_byte_idx = bitmap_idx>>1;
                    if (is_even_cliped_area_width) {
                        // 如果缓存为偶数，只需要对调一下bit处理顺序即可
                        uint8_t bits = map_p[bitmap_byte_idx];
                        for (int col_idx = glyph_cliped_area_width; col_idx != 0; col_idx-=2) {
                            p_buf[buf_idx] = p_color_palette[bits & 0xF]; // 奇数位，第二个
                            buf_idx++;
                            bitmap_byte_idx++;
                            bits = map_p[bitmap_byte_idx];
                            p_buf[buf_idx] = p_color_palette[bits >> 4]; // 偶数位，第一个
                            buf_idx++;
                        }
                    }
                    else {
                        // 如果缓存也是奇数，先处理一个像素后再按偶数处理
                        uint8_t bits = map_p[bitmap_byte_idx];
                        p_buf[buf_idx] = p_color_palette[bits & 0xF]; // 奇数位，第二个
                        buf_idx++;
                        bitmap_byte_idx++;
                        for (int col_idx = glyph_cliped_area_width - 1; col_idx != 0; col_idx-=2) {
                            const uint8_t bits = map_p[bitmap_byte_idx];
                            p_buf[buf_idx] = p_color_palette[bits >> 4]; // 偶数位，第一个
                            buf_idx++;
                            p_buf[buf_idx] = p_color_palette[bits & 0xF]; // 奇数位，第二个
                            buf_idx++;
                            bitmap_byte_idx++;
                        }
                    }
                }
            }
        }
    }
    #else //!optimize glyph render
    lv_area_t temp_area_1;
    lv_area_t temp_area_2;
    if (_lv_area_intersect(&temp_area_1, draw_ctx->buf_area, draw_ctx->clip_area))
    {
        if (_lv_area_intersect(&temp_area_2, &letter_area, &temp_area_1))
        {
            int32_t buf_idx;
            int32_t bitmap_idx;
            for (int row_idx = temp_area_2.y1; row_idx <= temp_area_2.y2; row_idx++)
            {
                buf_idx = (row_idx - draw_ctx->buf_area->y1) * (draw_ctx->buf_area->x2 - draw_ctx->buf_area->x1 + 1) + temp_area_2.x1 - draw_ctx->buf_area->x1;
                bitmap_idx = (row_idx - letter_area.y1) * g->box_w + temp_area_2.x1 - letter_area.x1;
                for (int col_idx = temp_area_2.x1; col_idx <= temp_area_2.x2; col_idx++)
                {
                    if (bitmap_idx & 1)
                    {
                        p_buf[buf_idx] = p_color_palette[map_p[bitmap_idx>>1] & 0xF];
                    }
                    else
                    {
                        p_buf[buf_idx] = p_color_palette[map_p[bitmap_idx>>1] >> 4];
                    }
                    buf_idx++;
                    bitmap_idx++;
                }
            }
        }
    }
    #endif //optimize glyph render
#else
#endif
}
