linux_libs = libpulse xcb-image xcb

build/:
	mkdir build

build/linux_debug: build/
	gcc -Wall -g -o build/linux_debug \
		src/game.cpp \
		src/linux/platform.cpp \
		src/linux/pulseaudio.cpp \
		src/linux/xcb.cpp \
		src/linux/main.cpp \
		-std=c++14 -lstdc++ -lm `pkg-config --cflags --libs $(linux_libs)`

build/mac_debug: build/
	gcc -Wall -g -o build/mac_debug src/mac/main.mm -framework AppKit

.PHONY: clean
clean:
	rm -rf build
