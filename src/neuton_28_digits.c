/*******************************************************************************
 * Size: 28 px
 * Bpp: 2
 * Opts: --bpp 2 --size 28 --no-compress --font Neuton-Regular.ttf --range 48-58,45,32 --format lvgl -o neuton_28_digits.c
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
    0x0, 0xf, 0xff, 0x15, 0x40,

    /* U+0030 "0" */
    0x0, 0x0, 0x0, 0x2, 0xff, 0x40, 0xf, 0x97,
    0xe0, 0x2f, 0x1, 0xf0, 0x3d, 0x0, 0xf8, 0x7c,
    0x0, 0xfc, 0xfc, 0x0, 0xbc, 0xfc, 0x0, 0xbd,
    0xfc, 0x0, 0xbd, 0xfc, 0x0, 0xbd, 0xfc, 0x0,
    0xbd, 0xfc, 0x0, 0xbc, 0xbc, 0x0, 0xfc, 0x7d,
    0x0, 0xf8, 0x3f, 0x1, 0xf0, 0xf, 0x97, 0xd0,
    0x2, 0xfe, 0x0,

    /* U+0031 "1" */
    0x1, 0x6c, 0x0, 0xff, 0xf0, 0x1, 0x5f, 0xc0,
    0x0, 0x2f, 0x0, 0x0, 0xbc, 0x0, 0x2, 0xf0,
    0x0, 0xb, 0xc0, 0x0, 0x2f, 0x0, 0x0, 0xbc,
    0x0, 0x2, 0xf0, 0x0, 0xb, 0xc0, 0x0, 0x2f,
    0x0, 0x0, 0xbc, 0x0, 0x2, 0xf0, 0x0, 0x5f,
    0xe4, 0x1f, 0xff, 0xfe,

    /* U+0032 "2" */
    0x2, 0xff, 0x40, 0x2f, 0x5b, 0xe0, 0x3c, 0x2,
    0xf4, 0x38, 0x1, 0xf8, 0x34, 0x0, 0xfc, 0x34,
    0x0, 0xfc, 0x10, 0x0, 0xf8, 0x0, 0x1, 0xf4,
    0x0, 0x2, 0xf0, 0x0, 0x7, 0xc0, 0x0, 0xf,
    0x0, 0x0, 0x3c, 0x0, 0x0, 0xf0, 0xd, 0x7,
    0x80, 0xc, 0x2e, 0x0, 0x2c, 0xbf, 0xff, 0xfc,
    0xff, 0xff, 0xfc,

    /* U+0033 "3" */
    0x2, 0xff, 0x40, 0x2e, 0x5b, 0xe0, 0x3c, 0x3,
    0xf0, 0x38, 0x2, 0xf4, 0x38, 0x2, 0xf0, 0x10,
    0x3, 0xe0, 0x0, 0xb, 0x80, 0x0, 0x7c, 0x0,
    0x1, 0xff, 0x80, 0x0, 0x2f, 0xf0, 0x0, 0x3,
    0xf8, 0x10, 0x1, 0xfc, 0x34, 0x0, 0xfc, 0x34,
    0x0, 0xf8, 0x78, 0x2, 0xf0, 0x7e, 0x5b, 0xc0,
    0x1b, 0xf9, 0x0,

    /* U+0034 "4" */
    0x0, 0x0, 0x20, 0x0, 0x0, 0xf, 0x0, 0x0,
    0x3, 0xf0, 0x0, 0x0, 0xbf, 0x0, 0x0, 0x1f,
    0xe0, 0x0, 0x7, 0x3e, 0x0, 0x0, 0xd3, 0xe0,
    0x0, 0x38, 0x3e, 0x0, 0xf, 0x3, 0xe0, 0x2,
    0xd0, 0x3e, 0x0, 0x7f, 0xff, 0xff, 0xcb, 0xff,
    0xff, 0xfc, 0x0, 0x2, 0xe0, 0x0, 0x0, 0x2e,
    0x0, 0x0, 0x2, 0xe0, 0x0, 0x0, 0x2e, 0x0,
    0x0, 0x1, 0x0, 0x0,

    /* U+0035 "5" */
    0x2, 0xff, 0xf0, 0xf, 0xff, 0x80, 0x39, 0x54,
    0x0, 0xc0, 0x0, 0x7, 0x0, 0x0, 0x2d, 0x0,
    0x0, 0xff, 0xd0, 0x3, 0xff, 0xf0, 0x0, 0x1f,
    0xf0, 0x0, 0xf, 0xd1, 0x0, 0xf, 0x4d, 0x0,
    0x3d, 0x34, 0x0, 0xf0, 0xe0, 0x7, 0x83, 0xe4,
    0x7c, 0x6, 0xfe, 0x40,

    /* U+0036 "6" */
    0x0, 0x1, 0x80, 0x0, 0xf, 0x40, 0x0, 0x78,
    0x0, 0x1, 0xe0, 0x0, 0x3, 0xc0, 0x0, 0xf,
    0x0, 0x0, 0x1f, 0x0, 0x0, 0x3d, 0xbf, 0x80,
    0x7f, 0x9b, 0xf0, 0xbc, 0x1, 0xf8, 0xbc, 0x0,
    0xfc, 0xbc, 0x0, 0xbc, 0xbc, 0x0, 0xbc, 0x7d,
    0x0, 0xf8, 0x3f, 0x0, 0xf4, 0xf, 0x83, 0xd0,
    0x2, 0xfe, 0x0,

    /* U+0037 "7" */
    0x0, 0x0, 0x0, 0xbf, 0xff, 0xfc, 0xbf, 0xff,
    0xfc, 0xb0, 0x0, 0x34, 0xe0, 0x0, 0xb0, 0x80,
    0x0, 0xe0, 0x0, 0x2, 0xd0, 0x0, 0x3, 0xc0,
    0x0, 0xb, 0x80, 0x0, 0xf, 0x0, 0x0, 0x2f,
    0x0, 0x0, 0x3d, 0x0, 0x0, 0xbc, 0x0, 0x0,
    0xf8, 0x0, 0x1, 0xf0, 0x0, 0x3, 0xf0, 0x0,
    0x7, 0xd0, 0x0,

    /* U+0038 "8" */
    0x0, 0xbe, 0x40, 0x7, 0x87, 0xe0, 0xf, 0x1,
    0xf0, 0x2e, 0x0, 0xf4, 0x2f, 0x0, 0xf0, 0x1f,
    0xc0, 0xe0, 0xf, 0xf7, 0x80, 0x2, 0xff, 0x0,
    0x2, 0xff, 0xc0, 0x1f, 0xf, 0xf0, 0x3d, 0x2,
    0xfc, 0x7c, 0x0, 0xfc, 0x7c, 0x0, 0x7c, 0x3e,
    0x0, 0xb8, 0x1f, 0x92, 0xf0, 0x6, 0xfe, 0x40,

    /* U+0039 "9" */
    0x1, 0xfe, 0x0, 0x2d, 0x1f, 0x42, 0xe0, 0x1f,
    0xf, 0x40, 0x3e, 0x7c, 0x0, 0xfe, 0xf0, 0x3,
    0xfb, 0xd0, 0xf, 0xdf, 0x80, 0x3f, 0x3f, 0x46,
    0xfc, 0x3f, 0xf7, 0xe0, 0x4, 0xf, 0x0, 0x0,
    0x78, 0x0, 0x3, 0xc0, 0x0, 0x2d, 0x0, 0x2,
    0xd0, 0x0, 0xbd, 0x0, 0x6, 0x40, 0x0,

    /* U+003A ":" */
    0x3c, 0xbd, 0x3c, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x3c, 0xbd, 0x3c
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 67, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 127, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 5, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 215, .box_w = 11, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 202, .adv_w = 215, .box_w = 14, .box_h = 17, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 262, .adv_w = 215, .box_w = 11, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 306, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 215, .box_w = 12, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 408, .adv_w = 215, .box_w = 12, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 456, .adv_w = 215, .box_w = 11, .box_h = 17, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 503, .adv_w = 92, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = 0}
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
    .bpp = 2,
    .kern_classes = 0,
    .bitmap_format = 0,
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

