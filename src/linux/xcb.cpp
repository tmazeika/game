#include "xcb.h"
#include "../utils.h"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

const xcb_keycode_t KEY_ESC = 9;
const xcb_keycode_t KEY_L = 46;
const xcb_button_t KEY_W = 25;
const xcb_button_t KEY_A = 38;
const xcb_button_t KEY_S = 39;
const xcb_button_t KEY_D = 40;
const xcb_button_t BTN_LEFT = 1;
const xcb_button_t BTN_MIDDLE = 2;
const xcb_button_t BTN_RIGHT = 3;
const xcb_button_t BTN_SCROLL_UP = 4;
const xcb_button_t BTN_SCROLL_DOWN = 5;

const uint16_t initialWidth = 1920;
const uint16_t initialHeight = 1080;

struct XCB {
    xcb_connection_t* conn;
    xcb_window_t window;
    size_t maxPixelsSizePerPut;
    xcb_atom_t deleteWindowAtom;
    xcb_gcontext_t gc;
    xcb_pixmap_t pixmap;
    size_t pixelsSize;
    Pixel* pixels;
    uint16_t width;
    uint16_t height;
    Input prevInput;
};

XCB* initXCB(const char* title) {
    xcb_connection_t* conn = xcb_connect(nullptr, nullptr);
    const size_t maxRequestSize = xcb_get_maximum_request_length(conn) << 2;
    xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    xcb_window_t window = xcb_generate_id(conn);
    {
        const uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        const uint32_t maskValues[] = {
            screen->black_pixel,
            XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_KEY_PRESS |
            XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS |
            XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION,
        };
        xcb_create_window(conn, XCB_COPY_FROM_PARENT, window, screen->root, 0,
            0,
            initialWidth, initialHeight, 0,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual, mask, maskValues);
        xcb_change_property(conn, XCB_PROP_MODE_REPLACE, window,
            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
            getStringLength(title), title);
        xcb_change_property(conn, XCB_PROP_MODE_REPLACE, window,
            XCB_ATOM_WM_TRANSIENT_FOR, XCB_ATOM_WINDOW, 32,
            sizeof(window), &window);
        xcb_map_window(conn, window);
        xcb_flush(conn);
    }

    // Enable receiving window close events.
    xcb_atom_t deleteWindowAtom;
    {
        xcb_intern_atom_cookie_t protocolsCookie = xcb_intern_atom(conn, 1, 12,
            "WM_PROTOCOLS");
        xcb_intern_atom_cookie_t deleteWindowCookie = xcb_intern_atom(conn, 0,
            16, "WM_DELETE_WINDOW");
        xcb_intern_atom_reply_t* protocolsReply = xcb_intern_atom_reply(conn,
            protocolsCookie, nullptr);
        xcb_intern_atom_reply_t* deleteWindowReply = xcb_intern_atom_reply(conn,
            deleteWindowCookie, nullptr);
        xcb_change_property(conn, XCB_PROP_MODE_REPLACE, window,
            protocolsReply->atom, XCB_ATOM_ATOM, 32, 1,
            &deleteWindowReply->atom);
        deleteWindowAtom = deleteWindowReply->atom;
        free(deleteWindowReply);
        free(protocolsReply);
    }

    const xcb_gcontext_t gc = xcb_generate_id(conn);
    {
        const uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
        const uint32_t maskValues[] = {screen->black_pixel, 0};
        xcb_create_gc(conn, gc, window, mask, maskValues);
    }

    const xcb_pixmap_t pixmap = xcb_generate_id(conn);
    xcb_create_pixmap(conn, 24, pixmap, window, initialWidth,
        initialHeight);

    const size_t pixelsSize =
        initialWidth * initialHeight * sizeof(Pixel);
    auto pixels = (Pixel*) malloc(pixelsSize);

    return new XCB{
        .conn = conn,
        .window = window,
        .maxPixelsSizePerPut = maxRequestSize -
                               sizeof(xcb_put_image_request_t),
        .deleteWindowAtom = deleteWindowAtom,
        .gc = gc,
        .pixmap = pixmap,
        .pixelsSize = pixelsSize,
        .pixels = pixels,
        .width = initialWidth,
        .height = initialHeight,
    };
}

Input pollXCBEvents(XCB* xcb) {
    const uint16_t prevWidth = xcb->width, prevHeight = xcb->height;
    Input curInput = xcb->prevInput;

    // Reset transitions.
    curInput.btnLeft.transitions = 0;
    curInput.btnMiddle.transitions = 0;
    curInput.btnRight.transitions = 0;
    curInput.keyL.transitions = 0;
    curInput.keyW.transitions = 0;
    curInput.keyA.transitions = 0;
    curInput.keyS.transitions = 0;
    curInput.keyD.transitions = 0;

    while (xcb_generic_event_t* event = xcb_poll_for_event(xcb->conn)) {
        switch (event->response_type & ~0x80) {
            case 0: {
                fprintf(stderr, "Unknown XCB error event\n");
                break;
            }
            case XCB_CLIENT_MESSAGE: {
                auto cmEvent = (xcb_client_message_event_t*) event;
                // Handle window close event.
                if (cmEvent->data.data32[0] == xcb->deleteWindowAtom) {
                    curInput.closeRequested = true;
                }
                break;
            }
            case XCB_CONFIGURE_NOTIFY: {
                auto cnEvent = (xcb_configure_notify_event_t*) event;
                xcb->width = cnEvent->width;
                xcb->height = cnEvent->height;
                break;
            }
            case XCB_KEY_PRESS: {
                auto kpEvent = (xcb_key_press_event_t*) event;
                // DEBUG: printf("Key press: %d\n", kpEvent->detail);
                if (kpEvent->detail == KEY_ESC) {
                    curInput.closeRequested = true;
                } else if (kpEvent->detail == KEY_L) {
                    onDown(&curInput.keyL, true);
                } else if (kpEvent->detail == KEY_W) {
                    onDown(&curInput.keyW, true);
                } else if (kpEvent->detail == KEY_A) {
                    onDown(&curInput.keyA, true);
                } else if (kpEvent->detail == KEY_S) {
                    onDown(&curInput.keyS, true);
                } else if (kpEvent->detail == KEY_D) {
                    onDown(&curInput.keyD, true);
                }
                break;
            }
            case XCB_KEY_RELEASE: {
                auto krEvent = (xcb_key_release_event_t*) event;
                if (krEvent->detail == KEY_L) {
                    onDown(&curInput.keyL, false);
                } else if (krEvent->detail == KEY_W) {
                    onDown(&curInput.keyW, false);
                } else if (krEvent->detail == KEY_A) {
                    onDown(&curInput.keyA, false);
                } else if (krEvent->detail == KEY_S) {
                    onDown(&curInput.keyS, false);
                } else if (krEvent->detail == KEY_D) {
                    onDown(&curInput.keyD, false);
                }
                break;
            }
            case XCB_BUTTON_PRESS: {
                auto bpEvent = (xcb_button_press_event_t*) event;
                if (bpEvent->detail == BTN_LEFT) {
                    onDown(&curInput.btnLeft, true);
                }
                if (bpEvent->detail == BTN_MIDDLE) {
                    onDown(&curInput.btnMiddle, true);
                }
                if (bpEvent->detail == BTN_RIGHT) {
                    onDown(&curInput.btnRight, true);
                }
                break;
            }
            case XCB_BUTTON_RELEASE: {
                auto brEvent = (xcb_button_release_event_t*) event;
                if (brEvent->detail == BTN_LEFT) {
                    onDown(&curInput.btnLeft, false);
                }
                if (brEvent->detail == BTN_MIDDLE) {
                    onDown(&curInput.btnMiddle, false);
                }
                if (brEvent->detail == BTN_RIGHT) {
                    onDown(&curInput.btnRight, false);
                }
                break;
            }
            case XCB_MOTION_NOTIFY: {
                auto mnEvent = (xcb_motion_notify_event_t*) event;
                curInput.mouseX = mnEvent->event_x;
                curInput.mouseY = mnEvent->event_y;
                break;
            }
        }
        free(event);
    }
    xcb->prevInput = curInput;

    // Resize pixel buffer if necessary.
    if (xcb->width != prevWidth || xcb->height != prevHeight) {
        xcb->pixelsSize = xcb->width * xcb->height * sizeof(Pixel);
        xcb->pixels = (Pixel*) realloc(xcb->pixels, xcb->pixelsSize);
        xcb_free_pixmap(xcb->conn, xcb->pixmap);
        xcb->pixmap = xcb_generate_id(xcb->conn);
        xcb_create_pixmap(xcb->conn, 24, xcb->pixmap, xcb->window, xcb->width,
            xcb->height);
    }

    return curInput;
}

Window getXCBWindow(XCB* xcb) {
    return Window{
        .pixels = xcb->pixels,
        .width = xcb->width,
        .height = xcb->height,
    };
}

void updateXCBGraphics(XCB* xcb) {
    xcb_image_t* image = xcb_image_create_native(xcb->conn, xcb->width,
        xcb->height, XCB_IMAGE_FORMAT_Z_PIXMAP, 24, nullptr, ~0, nullptr);
    image->data = (uint8_t*) xcb->pixels;
    const size_t rowSize = xcb->width * sizeof(Pixel);
    if (xcb->pixelsSize > xcb->maxPixelsSizePerPut) {
        const uint16_t rowsPerPut = xcb->maxPixelsSizePerPut / rowSize;
        for (uint16_t y = 0; y < xcb->height; y += rowsPerPut) {
            const uint16_t rows = min(rowsPerPut, xcb->height - y);
            // Assert that the below int16_t cast is OK.
            assert(y <= 0x7fff);
            xcb_put_image(xcb->conn, image->format, xcb->pixmap, xcb->gc,
                xcb->width, rows,
                0, (int16_t) y, 0,
                image->depth,
                rows * rowSize,
                &image->data[y * rowSize]);
        }
    } else {
        xcb_image_put(xcb->conn, xcb->pixmap, xcb->gc, image, 0, 0, 0);
    }
    xcb_image_destroy(image);
    xcb_copy_area(xcb->conn, xcb->pixmap, xcb->window, xcb->gc, 0, 0, 0, 0,
        xcb->width, xcb->height);
    xcb_flush(xcb->conn);
}

void destroyXCB(XCB* xcb) {
    xcb_disconnect(xcb->conn);
    delete xcb;
}
