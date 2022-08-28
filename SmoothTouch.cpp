/*
 * @Author: Mm1KEE
 * @GitHub: https://github.com/Mm1KEE/SmoothTouch
 * @Date: 2022-08-28 20:43:59
 * @LastEditTime: 2022-08-29 00:20:09
 * @Description: Custom XPT2046 touch input library with built-in denoising.
 * 
 * Copyright (c) 2022 by Mm1KEE, All Rights Reserved. 
 */
#include "SmoothTouch.h"

bool SmoothTouch::begin(bool initMax, bool liveUpdateMax) {
  if (_debufInfo) {
    while (!Serial && (millis() <= 2000));
  }
  _touch.begin();
  _touch.setRotation(_rotation);
  //while (!Serial && (millis() <= 2000));
  //while (!Serial);
  _initMax = initMax;
  _liveUpdateMax = liveUpdateMax;
  if (initMax) {
    writeMaxMin(1000, 0, 0);
    writeMaxMin(3600, 0, 1);
    writeMaxMin(1000, 1, 0);
    writeMaxMin(3600, 1, 1);
  }
  xMin = readMaxMin(1, 0);
  yMin = readMaxMin(0, 0);
  xMax = readMaxMin(1, 1);
  yMax = readMaxMin(0, 1);
  xRatio = float((xMax - xMin) / _resW);
  yRatio = float((yMax - yMin) / _resH);
  if (_debufInfo) {
    Serial.println("max x:" + String(xMax) + "\r\nmin x:" + String(xMin));
    Serial.println("max y:" + String(yMax) + "\r\nmin y:" + String(yMin));
    Serial.println("xRatio:" + String(xRatio));
    Serial.println("yRatio:" + String(yRatio));
  }
  return true;
}

void SmoothTouch::setResolution(unsigned int resW, unsigned int resH) {
  _resW = resW;
  _resH = resH;
}

void SmoothTouch::setDebugInfo(bool debugInfo) {
  _debufInfo = debugInfo;
}

void SmoothTouch::update() {
  //Serial.println("last state0:" + s[lastState0] + ",last state1:" + s[lastState1]);
  if (_touch.touched())
  {
    TS_Point p = _touch.getPoint();
    touchX = p.x;
    touchY = p.y;
    if (p.z >= 1300  ) {
      //Serial.println("raw x:" + String(p.x) + " ,prev x:" + String(prevTouchX));
      //Serial.println("raw y:" + String(p.y) + " ,prev y:" + String(prevTouchY));
      prevTouchX = touchX;
      prevTouchY = touchY;
      if (i >= 3) {
        i = 0;
        rawDataFilter(xArray, yArray);
        mouseXAvg = getXAvg();
        mouseYAvg = getYAvg();
        //Mouse.move(char(mouseXAvg / xRatio) , char(mouseYAvg / yRatio));
        xo = int((mouseXAvg - prevMouseX) / xRatio / 3);
        yo = int((mouseYAvg - prevMouseY) / yRatio / 1.5);
        if (_debufInfo) {
          Serial.println("x avg:" + String(mouseXAvg) + ",prev x avg:" + String(prevMouseX) + ",xo:" + String(xo));
          Serial.println("y avg:" + String(mouseYAvg) + ",prev x avg:" + String(prevMouseY) + ",yo:" + String(yo));
        }
        if (lastState1 != released) Mouse.move(xo , yo);
        prevMouseX = mouseXAvg;
        prevMouseY = mouseYAvg;
        /*Serial.println("xd0:" + String(xDif[0]) + ",xd1:" + String(xDif[1]) + ",xd2:" + String(xDif[2]));
          Serial.println("yd0:" + String(yDif[0]) + ",yd1:" + String(yDif[1]) + ",yd2:" + String(yDif[2]));
          Serial.println("x0:" + String(xArray[0]) + ",x1:" + String(xArray[1]) + ",x2:" + String(xArray[2]));
          Serial.println("y0:" + String(yArray[0]) + ",y1:" + String(yArray[1]) + ",y2:" + String(yArray[2]));
          Serial.println("x avg:" + String(mouseXAvg) + ",y avg:" + String(mouseYAvg) + ", i:" + String(i));*/
        i = 0;
        //Mouse.move(mouseX,mouseY);
        mouseXAvg = 0;
        mouseYAvg = 0;

        //stateTime = millis();
        switch (lastState1) {
          case released: {
              lastTouchTime = millis();
              longpress = 0;
              if (lastState0 == touched && lastState1 == released) {
                if (_debufInfo) {
                  Serial.print("last release:" + String(lastReleaseTime));
                  Serial.print(",last touch:" + String(lastTouchTime));
                  Serial.println(",interval:" + String(lastTouchTime - lastReleaseTime));
                }
                if (lastTouchTime - lastReleaseTime <= doubleclickInterval) {
                  if (_debufInfo)Serial.println("doubleclick");
                  state = doubleClick;
                  Mouse.press();
                }
              }
              else state = touched;
              break;
            }
          case touched: {
              state = touched;
              if (xo <= longpressDeadzone && yo <= longpressDeadzone) {
                if (longpress++ >= longpressMax && millis() - lastTouchTime >= longpressMaxTime && lastState0 != doubleClick) {
                  if (_debufInfo)Serial.println("last state0:" + s[lastState0] + ",last state1:" + s[lastState1]);
                  //Mouse.release();
                  Mouse.click(MOUSE_RIGHT);
                  Serial.println("longpress");
                  longpress = 0;
                  lastTouchTime = millis();
                  state = longPress;
                }
              }
              else {
                longpress = 0;
                lastTouchTime = millis();
              }
              lastPoint = p;
              break;
            }
          case longPress: {
              state = longPress;
              break;
            }
          case doubleClick: {
              state = touched;
              break;
            }
        }
        if (lastState1 != state) {
          lastState0 = lastState1;
          lastState1 = state;
        }

      }
      else {
        xArray[i] = touchX;
        yArray[i] = touchY;
        //Serial.println("x:" + String(mouseXAvg) + ",y:" + String(mouseYAvg) + ", i:" + String(i));
        i++;
      }
      if (_liveUpdateMax) updateMax();
      untouchedI = 0;
    }
  }
  else {
    /*switch (lastState1) {
      case touched: {
          lastReleaseTime = millis();
          Serial.println("touch released");
          break;
        }
      case doubleClick: {
          Mouse.release();
          lastReleaseTime = millis();
          Serial.println("double click released");
          break;
        }
      case longPress: {
          Mouse.release(MOUSE_RIGHT);
          lastReleaseTime = millis();
          Serial.println("right mouse released");
          break;
        }
      case released: {
          break;
        }
      }*/

    if (untouchedI++ >= 1000) {
      //Serial.println("----------released");
      if (Mouse.isPressed())Mouse.release();
      if (Mouse.isPressed(MOUSE_RIGHT)) Mouse.release(MOUSE_RIGHT);
      if (lastState1 != released) {
        lastState0 = lastState1;
        lastState1 = released;
        lastReleaseTime = millis();
        longpress = 0;
        if (_debufInfo)Serial.println("----------released");
      }
      untouchedI = 0;
    }
  }
}

void SmoothTouch::updateMax() {
  if (touchX > xMax) {
    writeMaxMin(touchX, 1, 1);
    xMax = touchX;
    if (_debufInfo)Serial.println("max x :" + String(touchX));
  }
  if (touchX < xMin) {
    writeMaxMin(touchX, 1, 0);
    xMin = touchX;
    if (_debufInfo)Serial.println("min x :" + String(touchX));
  }
  if (touchY > yMax) {
    writeMaxMin(touchY, 0, 1);
    yMax = touchY;
    if (_debufInfo)Serial.println("max y :" + String(touchY));
  }
  if (touchY < yMin) {
    writeMaxMin(touchY, 0, 0);
    yMin = touchY;
    if (_debufInfo)Serial.println("min y :" + String(touchY));
  }
  if (Serial.available()) {
    if (Serial.readString() = "max") {
      xMin = readMaxMin(1, 0);
      yMin = readMaxMin(0, 0);
      xMax = readMaxMin(1, 1);
      yMax = readMaxMin(0, 1);
      Serial.println("max x:" + String(xMax) + "\r\nmin x:" + String(xMin));
      Serial.println("max y:" + String(yMax) + "\r\nmin y:" + String(yMin));
    }
  }
}

void SmoothTouch::writeMaxMin(unsigned int val, bool x, bool max) {
  if (x) {
    if (max) {
      EEPROM.update(xMax_address, val >> 8); EEPROM.update(xMax_address + 1, val);
    }
    else {
      EEPROM.update(xMin_address, val >> 8); EEPROM.update(xMin_address + 1, val);
    }
  }
  else {
    if (max) {
      EEPROM.update(yMax_address, val >> 8); EEPROM.update(yMax_address + 1, val);
    }
    else {
      EEPROM.update(yMin_address, val >> 8); EEPROM.update(yMin_address + 1, val);
    }
  }
}

unsigned int SmoothTouch::readMaxMin(bool x, bool max) {
  if (x) {
    if (max) {
      return ( EEPROM.read(xMax_address) << 8) + EEPROM.read(xMax_address + 1);
    }
    else {
      return ( EEPROM.read(xMin_address) << 8) + EEPROM.read(xMin_address + 1);
    }
  }
  else {
    if (max) {
      return (EEPROM.read(yMax_address) << 8) + EEPROM.read(yMax_address + 1);
    }
    else {
      return ( EEPROM.read(yMin_address) << 8) + EEPROM.read(yMin_address + 1);
    }
  }
}
