#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <cstdlib>
#include <cstdio>

typedef int32_t i32;
typedef u_int8_t u8;
typedef u_int32_t u32;
typedef u_int64_t u64;

union pixel {
    struct {
        u8 b;
        u8 g;
        u8 r;
    };
    u32 value;
};

int main() {
    u32 width = 512, height = 1024;

    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        fprintf(stderr, "Failed to connect to X server\n");
        exit(-1);
    }
    u64 black = BlackPixel(display, DefaultScreen(display));
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,
                                        0, width, height, 0, black, black);
    XSelectInput(display, window, StructureNotifyMask | ExposureMask);
    XMapWindow(display, window);
    GC gc = XCreateGC(display, window, 0, nullptr);

    auto* pixels = (pixel*) calloc(width * height, sizeof(pixel));
    XImage* image = XCreateImage(display, XDefaultVisual(display, DefaultScreen(
            display)), 24, ZPixmap, 0, (char*) pixels, width, height, 32, 0);
    XInitImage(image);

    while (true) {
        XEvent e;
        XNextEvent(display, &e);
        // TODO: check next event, don't block
        if (e.type == Expose && e.xexpose.count == 0) {
            XDestroyImage(image);
            width = e.xexpose.width;
            height = e.xexpose.height;
            pixels = (pixel*) calloc(width * height, sizeof(pixel));
            image = XCreateImage(display, XDefaultVisual(display, DefaultScreen(
                                         display)), 24, ZPixmap, 0, (char*) pixels, width, height,
                                 32, 0);
            XInitImage(image);
        }
        if (e.type == MapNotify || (e.type == Expose && e.xexpose.count == 0)) {
            printf("Rendering...\n");
            for (int j = 0; j < width * height; j++) {
                pixels[j].b = 0x50;
                pixels[j].g = 0xa1;
            }
            XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);
            XFlush(display);
        }
    }

    free(pixels);
    XCloseDisplay(display);
    exit(0);
}
