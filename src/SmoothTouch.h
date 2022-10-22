/*
 * @Author: Mm1KEE
 * @GitHub: https://github.com/Mm1KEE/SmoothTouch
 * @Date: 2022-08-28 20:47:36
 * @LastEditTime: 2022-10-23 01:35:59
 * @Description: Custom XPT2046 touch input library with built-in denoising.
 * 
 * Copyright (c) 2022 by Mm1KEE, All Rights Reserved. 
 */

#ifndef _SMOOTHTOUCH_H_
#define _SMOOTHTOUCH_H_

#include<SimpleFilter.h>
#include <XPT2046_Touchscreen.h>
#include <Mouse.h>
#include <EEPROM.h>

enum TouchState {
  touched,
  released,
  longPress,
  doubleClick
};

class SmoothTouch {
  public:
    SmoothTouch(XPT2046_Touchscreen &touch, char rotation): _touch(touch), _rotation(rotation) {}
    bool begin( bool initMax = 0, bool liveUpdateMax = 0);
    void setResolution(unsigned int resW = 800, unsigned int resH = 400);
    void setMouseMaxSample(int count);
    void setDebugInfo(bool debugInfo=1);
    bool update();
    bool isTouched();

  private:
    XPT2046_Touchscreen _touch;
    void updateMax();
    void writeMaxMin(unsigned int val, bool x, bool max);
    unsigned int readMaxMin(bool x, bool max);
    unsigned int xMin_address = 4;
    unsigned int yMin_address = 6;
    unsigned int xMax_address = 8;
    unsigned int yMax_address = 10;
    unsigned int xMin, yMin , xMax, yMax;
    char _rotation;
    int mouseX = 0, mouseY = 0, prevMouseX = 0, prevMouseY = 0;
    int mouseXAvg=0, mouseYAvg=0;
    unsigned int _resW, _resH;
    float xRatio = 1,yRatio = 1;
    int touchX = 0, touchY = 0, prevTouchX = 0, prevTouchY = 0;
    int mouseMaxSample=3;
    //int deadzone = 1;
    int xo, yo;
    int xArray[9], yArray[9];
    byte i;
    unsigned long lastReleaseTime, lastTouchTime, stateTime;
    unsigned char longpressDeadzone = 5;
    unsigned char longpressMax = 10;
    unsigned int longpressMaxTime = 1000;
    unsigned int doubleclickInterval = 150;
    unsigned int longpress, untouchedI;
    TouchState state = released, lastState1 = released, lastState0 = released;
    TS_Point lastPoint;
    bool _initMax, _liveUpdateMax,_debufInfo;
    String s[4] = {"touched", "released", "longPress", "doubleClick"};
    long lastReportTime=0;
};

#endif
