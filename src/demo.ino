#include <lvgl.h>

#include <bb_spi_lcd.h>
#include <lv_bb_spi_lcd.h>

#include <bb_captouch.h>

#include <demos/lv_demos.h>

BBCapTouch bbct;
TOUCHINFO ti;

uint16_t touchMinX = TOUCH_MIN_X, touchMaxX = TOUCH_MAX_X, touchMinY = TOUCH_MIN_Y, touchMaxY = TOUCH_MAX_Y;

uint32_t lastTick = 0;

void touch_read( lv_indev_t * indev, lv_indev_data_t * data ) {
  if(bbct.getSamples(&ti)) {
    for (int i=0; i<ti.count; i++) {
      Serial.print("raw touch x: ");
      Serial.print(ti.x[i]);
      Serial.print(" y: ");
      Serial.print(ti.y[i]);

      if(ti.x[i] < touchMinX) touchMinX = ti.x[i];
      if(ti.x[i] > touchMaxX) touchMaxX = ti.x[i];
      if(ti.y[i] < touchMinY) touchMinY = ti.y[i];
      if(ti.y[i] > touchMaxY) touchMaxY = ti.y[i];

      //Map this to the pixel position
      data->point.x = map(ti.x[i], touchMinX, touchMaxX, 1, lv_display_get_horizontal_resolution(NULL)); // X touch mapping
      data->point.y = map(ti.y[i], touchMinY, touchMaxY, 1, lv_display_get_vertical_resolution(NULL)); // Y touch mapping
      data->state = LV_INDEV_STATE_PRESSED;

      Serial.print("mapped touch x: ");
      Serial.print(data->point.x);
      Serial.print(" y: ");
      Serial.println(data->point.y);
      
      return;
    }
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup() {
  Serial.begin(115200);

  // Initialise LVGL
  lv_init();
  lv_display_t* disp = lv_bb_spi_lcd_create(DISPLAY_TYPE);

  // Initialize touch screen
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);

  // Register touch
  lv_indev_t* indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);  
  lv_indev_set_read_cb(indev, touch_read);

  // Show demo
  lv_demo_widgets();

  Serial.println("Setup done");
}

void loop() {   
  lv_tick_inc(millis() - lastTick); //Update the tick timer. Tick is new for LVGL 9
  lastTick = millis();
  lv_timer_handler();               //Update the UI
  delay(5);
}