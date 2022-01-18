#ifndef GAME_XCB_H
#define GAME_XCB_H

#include "../graphics.h"
#include "../input.h"

struct XCB;

XCB* initXCB(const char* title);

Input pollXCBEvents(XCB* xcb);

Window getXCBWindow(XCB* xcb);

void updateXCBGraphics(XCB* xcb);

void destroyXCB(XCB* xcb);

#endif
