linux_libs = libpulse xcb-image

build/:
	mkdir build

build/linux_game_debug.so: build/
	gcc -Wall -g -shared -o build/linux_game_debug.so src/game.cpp

build/linux_debug: build/ build/linux_game_debug.so
	gcc -Wall -g -o build/linux_debug \
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
