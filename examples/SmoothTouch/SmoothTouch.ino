/*
 * @Author: Mm1KEE
 * @GitHub: https://github.com/Mm1KEE/SmoothTouch
 * @Date: 2022-10-23 01:30:52
 * @LastEditTime: 2022-10-23 01:36:58
 * @Description: Example sketch for SmoothTouch library.
 * 
 * Copyright (c) 2022 by Mm1KEE, All Rights Reserved. 
 */
#include <Keyboard.h>
#include <SPI.h>
#include <SmoothTouch.h>

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
