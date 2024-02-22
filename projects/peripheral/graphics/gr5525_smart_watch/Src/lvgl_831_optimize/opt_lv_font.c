/**
 * @file lv_font.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_font.h"
#include "../misc/lv_utils.h"
#include "../misc/lv_log.h"
#include "../misc/lv_assert.h"

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
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_GDX_PATCH_GLYPH_IN_LABEL_DSC
/**
 * Get the bitmap and descriptor of a glyph, for reduce function calls.
 */
const uint8_t *  lv_font_get_glyph_bitmap_and_dsc(const lv_font_t * font_p, lv_font_glyph_dsc_t * dsc_out, uint32_t letter,
                           uint32_t letter_next)
{

    LV_ASSERT_NULL(font_p);
    LV_ASSERT_NULL(dsc_out);

    const lv_font_t * f = font_p;
    const uint8_t * p_glyph_bitmap = NULL;

    dsc_out->resolved_font = NULL;

    while(f) {
        p_glyph_bitmap = font_p->get_glyph_bitmap(f, letter);
        if (p_glyph_bitmap != NULL)
        {
            f->get_glyph_dsc(f, dsc_out, letter, letter_next);
            dsc_out->resolved_font = f;
            return p_glyph_bitmap;
        }

        f = f->fallback;
    }

    if(letter < 0x20 ||
       letter == 0xf8ff || /*LV_SYMBOL_DUMMY*/
       letter == 0x200c) { /*ZERO WIDTH NON-JOINER*/
        dsc_out->box_w = 0;
        dsc_out->adv_w = 0;
    }
    else {
#if LV_USE_FONT_PLACEHOLDER
        dsc_out->box_w = font_p->line_height / 2;
        dsc_out->adv_w = dsc_out->box_w + 2;
#else
        dsc_out->box_w = 0;
        dsc_out->adv_w = 0;
#endif
    }

    dsc_out->resolved_font = NULL;
    dsc_out->box_h = font_p->line_height;
    dsc_out->ofs_x = 0;
    dsc_out->ofs_y = 0;
    dsc_out->bpp   = 1;
    dsc_out->is_placeholder = true;

    return p_glyph_bitmap;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/
