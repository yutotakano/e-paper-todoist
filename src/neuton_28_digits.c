/*******************************************************************************
 * Size: 28 px
 * Bpp: 3
 * Opts: --bpp 3 --size 28 --font Neuton-Regular.ttf --range 48-58,45,32 --format lvgl -o neuton_28_digits.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef NEUTON_28_DIGITS
#define NEUTON_28_DIGITS 1
#endif

#if NEUTON_28_DIGITS

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+002D "-" */
    0x3, 0xdb, 0xfd, 0xbd, 0xbb, 0x80,

    /* U+0030 "0" */
    0x3, 0xff, 0x87, 0x3f, 0xb2, 0x3, 0xb3, 0x56,
    0x1b, 0x0, 0xa9, 0x2a, 0x9d, 0xc, 0x82, 0x85,
    0x2, 0x40, 0x20, 0x40, 0x10, 0x18, 0x84, 0x28,
    0x2, 0x6, 0x60, 0x8, 0x1f, 0x88, 0x6, 0x40,
    0xff, 0x10, 0x3f, 0xf9, 0x64, 0x80, 0xf8, 0x80,
    0x6c, 0x0, 0x81, 0x98, 0x3, 0xc0, 0x30, 0x31,
    0x8, 0x22, 0x20, 0x12, 0x1, 0x0, 0x70, 0x6a,
    0x74, 0x42, 0x0, 0xf6, 0xd6, 0x2c, 0xc0, 0x0,

    /* U+0031 "1" */
    0x2, 0x4e, 0xf2, 0x6, 0xde, 0xc4, 0x7, 0xac,
    0x82, 0x7, 0xcd, 0xc8, 0xf, 0xff, 0xf8, 0x1f,
    0xfd, 0xf4, 0xd0, 0xa, 0x88, 0x5e, 0xc2, 0x1,
    0xdd, 0x40,

    /* U+0032 "2" */
    0x0, 0x66, 0xfd, 0x98, 0x1a, 0x65, 0x62, 0x32,
    0x80, 0x2c, 0xe6, 0xd5, 0x4, 0xc1, 0x8, 0xd,
    0xc0, 0x60, 0x2e, 0x6, 0x60, 0x18, 0x11, 0x3,
    0x10, 0x3a, 0xa0, 0x31, 0x0, 0xc0, 0x20, 0x39,
    0x80, 0xe0, 0x7e, 0xe0, 0xa0, 0x3e, 0x48, 0x38,
    0x1f, 0x1a, 0x31, 0x3, 0xc7, 0xc, 0x40, 0xf1,
    0xc7, 0x90, 0x19, 0x0, 0x59, 0x60, 0x22, 0x4c,
    0x4a, 0xe1, 0x3a, 0x3, 0x58, 0x7b, 0x72, 0x0,
    0x80, 0xfe, 0x20,

    /* U+0033 "3" */
    0x0, 0x66, 0xfd, 0x98, 0x1a, 0xe1, 0x2a, 0x32,
    0x81, 0x21, 0x12, 0x52, 0x11, 0x2, 0x40, 0x64,
    0x1, 0x80, 0x24, 0x7, 0xcc, 0x4, 0xa0, 0x66,
    0x11, 0x0, 0x88, 0x15, 0xe, 0x81, 0xe5, 0x9e,
    0x80, 0x79, 0xd2, 0x5a, 0x90, 0x39, 0xe6, 0x1,
    0x62, 0x7, 0x19, 0xc8, 0x8, 0x1, 0x81, 0xd0,
    0x2, 0x1, 0x10, 0x19, 0x1, 0x88, 0x20, 0x7c,
    0x82, 0x1c, 0xd, 0x0, 0x40, 0x27, 0x5b, 0x56,
    0x71, 0xe, 0xa4, 0x90, 0x26, 0x20, 0x0,

    /* U+0034 "4" */
    0x3, 0xf1, 0x80, 0x7f, 0xf0, 0x32, 0x3, 0xfe,
    0xc0, 0x7f, 0xf0, 0x29, 0x3, 0xfe, 0x6c, 0x43,
    0x3, 0xf2, 0x9c, 0xf, 0xf1, 0xb0, 0x81, 0xfe,
    0xc9, 0x81, 0xfe, 0xc2, 0x81, 0xfe, 0xa6, 0x81,
    0xfe, 0x68, 0x4d, 0xa2, 0x16, 0xd1, 0xc0, 0x9,
    0xc0, 0x44, 0xeb, 0xff, 0x90, 0x5f, 0xc0, 0x7f,
    0xf9, 0xbe, 0xa0, 0x60,

    /* U+0035 "5" */
    0x2, 0x9f, 0xf6, 0x3, 0x20, 0x39, 0x81, 0x8b,
    0xb7, 0x40, 0x22, 0x2a, 0x58, 0x81, 0x20, 0x40,
    0xfd, 0xc2, 0x20, 0x7c, 0x85, 0xd8, 0xc0, 0xfe,
    0x33, 0x1, 0xb7, 0xe8, 0x0, 0xe0, 0x3c, 0xf9,
    0x5, 0x4, 0x7, 0x70, 0x4, 0x56, 0x6, 0x20,
    0x7f, 0xf0, 0x50, 0xe, 0x6, 0x41, 0x12, 0xe,
    0x22, 0xe9, 0xa0, 0xd4, 0xee, 0xcb, 0x80, 0x0,

    /* U+0036 "6" */
    0x3, 0xe5, 0x0, 0xfc, 0x74, 0xc0, 0x7c, 0xb3,
    0xc8, 0xf, 0x2a, 0xe8, 0x1f, 0x1a, 0x60, 0x1f,
    0xb8, 0xe0, 0x7e, 0x64, 0xc0, 0x7e, 0x82, 0xcf,
    0xea, 0x40, 0x20, 0x10, 0x56, 0x16, 0x23, 0x0,
    0xd1, 0x48, 0x1, 0x1, 0x0, 0x40, 0x98, 0x4,
    0x7, 0xf2, 0x2, 0x20, 0x8, 0x1f, 0xb0, 0x4,
    0x6, 0x61, 0x4, 0x45, 0x3, 0x11, 0xc0, 0x70,
    0x61, 0x2c, 0x22, 0x0, 0x73, 0x5b, 0x1e, 0x60,
    0x0,

    /* U+0037 "7" */
    0x4, 0xff, 0x84, 0xdb, 0xfc, 0x48, 0xf, 0xe2,
    0x40, 0x3f, 0xf8, 0x50, 0xd8, 0xf, 0x40, 0xc3,
    0xa0, 0x79, 0xa0, 0x14, 0x81, 0xd0, 0x70, 0x3f,
    0x98, 0x40, 0x7e, 0x81, 0x81, 0xfc, 0xc5, 0x3,
    0xf4, 0x4, 0x80, 0xfc, 0xc5, 0x3, 0xf4, 0x0,
    0xc0, 0xfc, 0xc2, 0x3, 0xf3, 0x1, 0x0, 0xfd,
    0x1, 0x20, 0x3e, 0x44, 0x50, 0x3c,

    /* U+0038 "8" */
    0x2, 0x37, 0x65, 0x40, 0x73, 0xc7, 0xc5, 0x60,
    0x11, 0x86, 0x15, 0x43, 0x20, 0x28, 0x60, 0x48,
    0x3, 0x0, 0x42, 0x3, 0xcc, 0x6, 0x1, 0x80,
    0x10, 0x88, 0x6, 0x41, 0xef, 0xaa, 0x6, 0xc8,
    0x4, 0x14, 0xe, 0x20, 0x80, 0x3c, 0x81, 0x2c,
    0xf6, 0x10, 0x18, 0x80, 0xa2, 0x3, 0x90, 0xe,
    0x18, 0x4, 0x5, 0x48, 0x20, 0x62, 0x6, 0xa0,
    0x48, 0x4, 0x3, 0x70, 0xc1, 0x81, 0x54, 0x63,
    0x30, 0x3, 0xa9, 0x2e, 0x33, 0x0,

    /* U+0039 "9" */
    0x2, 0x7b, 0xf5, 0x20, 0x6b, 0x2e, 0xab, 0x30,
    0x11, 0x36, 0x2a, 0x8, 0x49, 0x8c, 0x4, 0xc0,
    0x54, 0x1, 0x81, 0xe5, 0xc0, 0xf8, 0x82, 0x4,
    0xc0, 0xfb, 0x0, 0xe0, 0x62, 0x1, 0x0, 0x4c,
    0xaa, 0x0, 0x8c, 0x44, 0xd3, 0x80, 0x40, 0xec,
    0xbb, 0x18, 0xa, 0x4, 0x51, 0x8, 0x22, 0x7,
    0xd0, 0x40, 0x3e, 0x82, 0x10, 0x3d, 0x5c, 0x60,
    0x63, 0x3b, 0xac, 0xe, 0x46, 0xe4, 0x6,

    /* U+003A ":" */
    0x3f, 0xa, 0xb, 0xa0, 0xb3, 0xf0, 0x1f, 0xfc,
    0x93, 0xf0, 0xa0, 0xba, 0xb
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 67, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 127, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 6, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 70, .adv_w = 215, .box_w = 11, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 96, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 234, .adv_w = 215, .box_w = 14, .box_h = 17, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 286, .adv_w = 215, .box_w = 11, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 342, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 407, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 461, .adv_w = 215, .box_w = 12, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 531, .adv_w = 215, .box_w = 11, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 594, .adv_w = 92, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xd
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 14, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 48, .range_length = 11, .glyph_id_start = 3,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 2,
    .bpp = 3,
    .kern_classes = 0,
    .bitmap_format = 1,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};

extern const lv_font_t lv_font_montserrat_16;


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t neuton_28_digits = {
#else
lv_font_t neuton_28_digits = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 18,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -5,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = &lv_font_montserrat_16,
#endif
    .user_data = NULL,
};



#endif /*#if NEUTON_28_DIGITS*/

