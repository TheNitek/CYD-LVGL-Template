/**
 * @file lv_bb_spi_lcd.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_bb_spi_lcd.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_bb_spi_lcd_create(int iType)
{
    void *draw_buf;
    int draw_buf_size;
    lv_bb_spi_lcd_t * dsc = (lv_bb_spi_lcd_t *)lv_malloc_zeroed(sizeof(lv_bb_spi_lcd_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    dsc->lcd = new BB_SPI_LCD();
    dsc->lcd->begin(iType);

    lv_display_t * disp = lv_display_create(dsc->lcd->width(), dsc->lcd->height());
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }
/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
    draw_buf_size = ((dsc->lcd->width() * dsc->lcd->height()) / 10) * (LV_COLOR_DEPTH / 8);
    draw_buf = heap_caps_malloc(draw_buf_size, MALLOC_CAP_8BIT);
    dsc->lcd->fillScreen(TFT_BLACK);
    lv_display_set_driver_data(disp, (void *)dsc);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, (void *)draw_buf, NULL, draw_buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_bb_spi_lcd_t * dsc = (lv_bb_spi_lcd_t *)lv_display_get_driver_data(disp);

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    uint16_t *p = (uint16_t *)px_map;

    // bb_spi_lcd needs big-endian
    for (int i=0; i<w*h; i++) {
        p[i] = __builtin_bswap16(p[i]);
    }
    dsc->lcd->setAddrWindow(area->x1, area->y1, w, h);
    dsc->lcd->pushPixels((uint16_t *)px_map, w * h); //, DRAW_TO_LCD | DRAW_WITH_DMA);

    lv_display_flush_ready(disp);
}