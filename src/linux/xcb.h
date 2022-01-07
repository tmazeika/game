#ifndef GAME_XCB_H
#define GAME_XCB_H

#include "../graphics.h"
#include "../input.h"

struct XCB;

struct XCBGraphicsInfo {
    uint16_t width;
    uint16_t height;
    Pixel* pixels;
};

XCB* initXCB(const char* title);

Input pollXCBEvents(XCB* xcb);

XCBGraphicsInfo getXCBGraphicsInfo(XCB* xcb);

void updateXCBGraphics(XCB* xcb);

void destroyXCB(XCB* xcb);

#endif
