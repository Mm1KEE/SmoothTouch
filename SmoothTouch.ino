#include <Keyboard.h>

#include <SPI.h>

#include "SmoothTouch.h"
XPT2046_Touchscreen touch(PIN_SPI_SS, 255);
SmoothTouch st(touch, 2);

void setup() {
  Serial.begin(115200);
  st.setDebugInfo();
  st.setResolution(800, 400);
  st.begin(1, 0);
}

void loop() {
  st.update();
}
