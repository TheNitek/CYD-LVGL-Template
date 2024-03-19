/**
 * @file lv_bb_spi_lcd.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_bb_spi_lcd.h"
#if LV_USE_BB_SPI_LCD

#include <bb_spi_lcd.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    BB_SPI_LCD * lcd;
} lv_bb_spi_lcd_t;

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
#ifdef ESP_PLATFORM
    draw_buf = heap_caps_malloc(draw_buf_size, MALLOC_CAP_8BIT);
#else
    draw_buf = malloc(draw_buf_size);
#endif
    dsc->lcd->fillScreen(TFT_BLACK);
    lv_display_set_driver_data(disp, (void *)dsc);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, (void *)draw_buf, NULL, draw_buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static uint32_t checksum(uint16_t *pData, int iCount)
{
uint32_t sum = 0;
    for (int i=1; i<iCount-1; i+=2) { // skip every other pixel to improve speed
        sum += pData[i] * i;
    }
    return sum;
} /* checksum() */

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
#ifdef FUTURE
#define RECT_COUNT 16
static uint32_t u32RectHash[RECT_COUNT];
static uint32_t u32RectSum[RECT_COUNT];
static int iIndex = 0; // last rectangle
uint32_t u32Hash, u32Sum;
int i;

    u32Sum = checksum((uint16_t *)px_map, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1));
    u32Hash = area->x1 + (area->y1 << 3) + (area->x2 << 6) + (area->y2 << 9);
    for (i = 0; i<RECT_COUNT; i++) { // did we draw this rectangle already?
        if (u32RectHash[i] == u32Hash) {
            if (u32Sum == u32RectSum[i]) {
             //  printf("can skip area\n");
               lv_display_flush_ready(disp);
               return;
            }
            u32RectSum[i] = u32Sum; // save the new sum
            break;
        }
    }
    if (i == RECT_COUNT) { // new rectangle, add it to the list
        u32RectHash[iIndex] = u32Hash;
        u32RectSum[iIndex] = u32Sum;
        iIndex = (iIndex + 1) & (RECT_COUNT-1); // must be power of 2
    }
#endif // FUTURE
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

#endif /*LV_BB_SPI_LCD */
