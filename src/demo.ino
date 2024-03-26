#include <lvgl.h>

#include <bb_spi_lcd.h>
#include <display/lv_bb_spi_lcd.h>

#include <demos/lv_demos.h>

#ifdef TOUCH_CAPACITIVE
#include <bb_captouch.h>

BBCapTouch bbct;
uint16_t touchMinX = TOUCH_MIN_X, touchMaxX = TOUCH_MAX_X, touchMinY = TOUCH_MIN_Y, touchMaxY = TOUCH_MAX_Y;
#else
BB_SPI_LCD * lcd;
#endif

TOUCHINFO ti;

void touch_read( lv_indev_t * indev, lv_indev_data_t * data ) {

#ifdef TOUCH_CAPACITIVE
  // Capacitive touch needs to be mapped to display pixels
  if(bbct.getSamples(&ti)) {
    Serial.print("raw touch x: ");
    Serial.print(ti.x[0]);
    Serial.print(" y: ");
    Serial.println(ti.y[0]);

    if(ti.x[0] < touchMinX) touchMinX = ti.x[0];
    if(ti.x[0] > touchMaxX) touchMaxX = ti.x[0];
    if(ti.y[0] < touchMinY) touchMinY = ti.y[0];
    if(ti.y[0] > touchMaxY) touchMaxY = ti.y[0];

    //Map this to the pixel position
    data->point.x = map(ti.x[0], touchMinX, touchMaxX, 1, lv_display_get_horizontal_resolution(NULL)); // X touch mapping
    data->point.y = map(ti.y[0], touchMinY, touchMaxY, 1, lv_display_get_vertical_resolution(NULL)); // Y touch mapping
    data->state = LV_INDEV_STATE_PRESSED;
#else
  // Resistive touch is already mapped by the bb_spi_lcd library
  if(lcd->rtReadTouch(&ti)) {
    data->point.x = ti.x[0];
    data->point.y = ti.y[0];
#endif

    data->state = LV_INDEV_STATE_PRESSED;

    Serial.print("mapped touch x: ");
    Serial.print(data->point.x);
    Serial.print(" y: ");
    Serial.println(data->point.y);
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup() {
  Serial.begin(115200);

  // Initialise LVGL
  lv_init();
  lv_tick_set_cb(millis);
  lv_display_t* disp = lv_bb_spi_lcd_create(DISPLAY_TYPE);

#ifdef TOUCH_CAPACITIVE
  // Initialize touch screen
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
#else
  lv_bb_spi_lcd_t* dsc = (lv_bb_spi_lcd_t *)lv_display_get_driver_data(disp);
  lcd = dsc->lcd;
  lcd->rtInit(TOUCH_MOSI, TOUCH_MISO, TOUCH_CLK, TOUCH_CS);
#endif

  // Register touch
  lv_indev_t* indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);  
  lv_indev_set_read_cb(indev, touch_read);

  // Show demo
  lv_demo_widgets();

  Serial.println("Setup done");
}

void loop() {   
  lv_timer_periodic_handler();
}