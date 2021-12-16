#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <cstdlib>
#include <cstdio>

typedef unsigned char byte;

union pixel {
    struct {
        byte b;
        byte g;
        byte r;
    };
    unsigned int value;
};

class LinuxWindow {
public:
    LinuxWindow(unsigned int width, unsigned int height, pixel pixels[])
            : width(width), height(height) {
        display = XOpenDisplay(nullptr);
        if (display == nullptr) {
            fprintf(stderr, "Failed to connect to X server\n");
            exit(-1);
        }
        unsigned long black = BlackPixel(display, DefaultScreen(display));
        window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0,
                                     width, height, 0, black, black);
        XSelectInput(display, window, StructureNotifyMask);
        XMapWindow(display, window);
        gc = XCreateGC(display, window, 0, nullptr);
        image = XCreateImage(display,
                             XDefaultVisual(display, DefaultScreen(display)),
                             24, ZPixmap, 0, (char*) pixels, width, height, 32,
                             0);
        XInitImage(image);
        while (true) {
            XEvent e;
            XNextEvent(display, &e);
            if (e.type == MapNotify) {
                break;
            }
        }
    }

    void drawPixels() {
        XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);
        XFlush(display);
    }

    ~LinuxWindow() {
        XCloseDisplay(display);
    }

private:
    unsigned int width;
    unsigned int height;
    Display* display;
    Window window;
    GC gc;
    XImage* image;
};

int main() {
    const unsigned int width = 512, height = 1024;
    {
        auto* pixels = (pixel*) calloc(width * height, sizeof(pixel));
        LinuxWindow w(width, height, pixels);
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < width * height; j++) {
                pixels[j].b += 10;
                pixels[j].g -= 10;
            }
            w.drawPixels();
            sleep(1);
        }
        free(pixels);
    }
    exit(0);
}
