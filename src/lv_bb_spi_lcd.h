/**
 * @file lv_bb_spi_lcd_h
 *
 */

#ifndef LV_BB_SPI_LCD_H
#define LV_BB_SPI_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <display/lv_display.h>

#if LV_USE_BB_SPI_LCD

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_display_t * lv_bb_spi_lcd_create(int iType);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_BB_SPI_LCD */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_BB_SPI_LCD_H */
