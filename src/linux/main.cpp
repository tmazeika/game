#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <chrono>
#include <cstring>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <pulse/pulseaudio.h>
#include "../game.cpp"

#define min(a, b) ((a) < (b) ? (a) : (b))

const xcb_keycode_t KEY_ESC = 9;

xcb_window_t
createWindow(xcb_connection_t* conn, xcb_screen_t* screen, uint32_t width,
        uint32_t height) {
    xcb_window_t window = xcb_generate_id(conn);
    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t maskValues[] = {
            screen->black_pixel,
            XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_KEY_PRESS,
    };
    xcb_create_window(conn, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0,
            width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual, mask, maskValues);

    // Set window title.
    const char* WINDOW_TITLE = "game";
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME,
            XCB_ATOM_STRING, sizeof(char) * 8, strlen(WINDOW_TITLE),
            WINDOW_TITLE);

    xcb_map_window(conn, window);
    xcb_flush(conn);
    return window;
}

void destroyWindow(xcb_connection_t* conn, xcb_window_t window) {
    xcb_destroy_window(conn, window);
}

void contextStateCb(pa_context* context, void* mainloop) {
    pa_threaded_mainloop_signal((pa_threaded_mainloop*) mainloop, 0);
}

void streamStateCb(pa_stream* stream, void* mainloop) {
    pa_threaded_mainloop_signal((pa_threaded_mainloop*) mainloop, 0);
}

void streamWriteCb(pa_stream* stream, size_t requestedBytes, void* userdata) {
    static uint64_t counter = 0;
    size_t bytes_remaining = requestedBytes;
    while (bytes_remaining > 0) {
        uint8_t* buffer = nullptr;
        size_t bytes_to_fill = 44100;
        size_t i;

        if (bytes_to_fill > bytes_remaining) {
            bytes_to_fill = bytes_remaining;
        }

        pa_stream_begin_write(stream, (void**) &buffer, &bytes_to_fill);

        const float hz = 261.6255653005986f;
        for (i = 0; i < bytes_to_fill; i += 2) {
            const float t = fmodf((float) counter, 44100.0f / hz) / (44100.0f / hz);

            // middle c
//            uint8_t v = (counter <= 44100/262/2) ? 0x11 : 0;
            uint8_t v = (uint8_t) (((sinf(t * 2.0f * 3.14159f) + 1.0f) / 2.0f) * 100.0f);
            buffer[i] = v;
            buffer[i + 1] = v;
            counter++;
        }

        pa_stream_write(stream, buffer, bytes_to_fill, nullptr, 0LL,
                PA_SEEK_RELATIVE);
        bytes_remaining -= bytes_to_fill;
    }
}

int main() {
    pa_threaded_mainloop* mainloop = pa_threaded_mainloop_new();
    pa_mainloop_api* mainloopApi = pa_threaded_mainloop_get_api(mainloop);
    pa_context* paContext = pa_context_new(mainloopApi, "game");
    pa_context_set_state_callback(paContext, &contextStateCb, mainloop);
    pa_threaded_mainloop_lock(mainloop);
    pa_threaded_mainloop_start(mainloop);
    pa_context_connect(paContext, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
    while (pa_context_get_state(paContext) != PA_CONTEXT_READY) {
        pa_threaded_mainloop_wait(mainloop);
    }

    const pa_sample_spec audioSampleSpec = {
            .format = PA_SAMPLE_U8,
            .rate = 44100,
            .channels = 2
    };
    pa_stream* audioStream = pa_stream_new(paContext, "game-audio",
            &audioSampleSpec, nullptr);
    pa_stream_set_state_callback(audioStream, &streamStateCb, mainloop);
    pa_stream_set_write_callback(audioStream, &streamWriteCb, mainloop);
    pa_stream_connect_playback(audioStream, nullptr, nullptr, PA_STREAM_NOFLAGS,
            nullptr, nullptr);
    while (pa_stream_get_state(audioStream) != PA_STREAM_READY) {
        pa_threaded_mainloop_wait(mainloop);
    }
    pa_threaded_mainloop_unlock(mainloop);

    uint32_t width = 1024, height = 720;

    xcb_connection_t* conn = xcb_connect(nullptr, nullptr);
    const uint64_t maxRequestSize = xcb_get_maximum_request_length(conn) << 2;
    xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    xcb_window_t window = createWindow(conn, screen, width, height);

    xcb_atom_t deleteWindowAtom;
    // Enable receiving window close events.
    {
        xcb_intern_atom_cookie_t protocolsCookie = xcb_intern_atom(conn, 1, 12,
                "WM_PROTOCOLS");
        xcb_intern_atom_cookie_t deleteWindowCookie = xcb_intern_atom(conn, 0,
                16,
                "WM_DELETE_WINDOW");
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

    xcb_gcontext_t gc = xcb_generate_id(conn);
    {
        uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
        uint32_t maskValues[] = {screen->black_pixel, 0};
        xcb_create_gc(conn, gc, window, mask, maskValues);
    }

    xcb_pixmap_t pixmap = xcb_generate_id(conn);
    xcb_create_pixmap(conn, 24, pixmap, window, width, height);

    size_t pixelsSize = width * height * sizeof(Pixel);
    auto pixels = (Pixel*) malloc(pixelsSize);

    uint32_t prevWidth = 0;
    uint32_t prevHeight = 0;
    const auto startTime = std::chrono::high_resolution_clock::now();
    auto prevTime = startTime;

    bool running = true;
    while (running) {
        // Handle X events.
        while (xcb_generic_event_t* event = xcb_poll_for_event(conn)) {
            switch (event->response_type & ~0x80) {
                case 0: {
                    fprintf(stderr, "Unknown XCB error event\n");
                    break;
                }
                case XCB_CLIENT_MESSAGE: {
                    auto clientMessageEvent = (xcb_client_message_event_t*) event;
                    // Handle window close event.
                    if (clientMessageEvent->data.data32[0] ==
                        deleteWindowAtom) {
                        running = false;
                    }
                    break;
                }
                case XCB_CONFIGURE_NOTIFY: {
                    auto configureNotifyEvent = (xcb_configure_notify_event_t*) event;
                    width = configureNotifyEvent->width;
                    height = configureNotifyEvent->height;
                    break;
                }
                case XCB_KEY_PRESS: {
                    auto keyPressEvent = (xcb_key_press_event_t*) event;
                    if (keyPressEvent->detail == KEY_ESC) {
                        running = false;
                    }
                    break;
                }
            }
            free(event);
        }

        const auto nowTime = std::chrono::high_resolution_clock::now();
        const auto deltaTime = nowTime - prevTime;
        prevTime = nowTime;
        printf("*** frame stats: %.2fms\n",
                (float) std::chrono::duration_cast<std::chrono::nanoseconds>(
                        deltaTime).count() / 1000000.0f);

        if (width != prevWidth || height != prevHeight) {
            prevWidth = width;
            prevHeight = height;
            pixelsSize = width * height * sizeof(Pixel);
            pixels = (Pixel*) realloc(pixels, pixelsSize);
            xcb_free_pixmap(conn, pixmap);
            pixmap = xcb_generate_id(conn);
            xcb_create_pixmap(conn, 24, pixmap, window, width, height);
        }

        // Draw.
        render(width, height, pixels,
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                        deltaTime).count());
        xcb_image_t* image = xcb_image_create_native(conn, width, height,
                XCB_IMAGE_FORMAT_Z_PIXMAP, 24, nullptr, ~0, nullptr);
        image->data = (uint8_t*) pixels;
        const size_t rowSize = width * sizeof(Pixel);
        const size_t maxPixelsSizePerPut =
                maxRequestSize - sizeof(xcb_put_image_request_t);
        if (pixelsSize > maxPixelsSizePerPut) {
            const uint16_t rowsPerPut = maxPixelsSizePerPut / rowSize;
            for (uint16_t y = 0; (uint32_t) y < height; y += rowsPerPut) {
                const uint16_t rows = min(rowsPerPut, height - y);
                xcb_put_image(conn, image->format, pixmap, gc,
                        width, rows,
                        0, (int16_t) y, 0,
                        image->depth,
                        rows * rowSize,
                        &image->data[y * rowSize]);
            }
        } else {
            xcb_image_put(conn, pixmap, gc, image, 0, 0, 0);
        }
        xcb_image_destroy(image);
        xcb_copy_area(conn, pixmap, window, gc, 0, 0, 0, 0, width, height);
        xcb_flush(conn);
        usleep(1000);
    }

    free(pixels);
    xcb_free_pixmap(conn, pixmap);
    xcb_free_gc(conn, gc);
    destroyWindow(conn, window);
    xcb_disconnect(conn);
    pa_stream_disconnect(audioStream);
    pa_threaded_mainloop_stop(mainloop);
    pa_threaded_mainloop_free(mainloop);
    return 0;
}
