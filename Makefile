linux_libs = libpulse xcb-image

build/:
	mkdir build

build/linux_game_debug.so: build/
	gcc -Wall -g -shared -fpic -o build/linux_game_debug.so.tmp \
		src/linux/platform.cpp \
		src/input.cpp \
		src/game.cpp
	@# We use mv instead of outputting to the file directly so that we can support hot reloading... otherwise, dlopen
	@# could (and observably always) will open a partially-written library file thanks to GCC.
	mv build/linux_game_debug.so.tmp build/linux_game_debug.so

build/linux_debug: build/ build/linux_game_debug.so
	gcc -Wall -g -o build/linux_debug \
		src/input.cpp \
		src/utils.cpp \
		src/linux/platform.cpp \
		src/linux/pulseaudio.cpp \
		src/linux/xcb.cpp \
		src/linux/main.cpp \
		-std=c++14 -lstdc++ -ldl `pkg-config --cflags --libs $(linux_libs)`

build/mac_debug: build/
	gcc -Wall -g -o build/mac_debug src/mac/main.mm -framework AppKit

.PHONY: clean
clean:
	rm -rf build
