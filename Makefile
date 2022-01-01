linux_libs = libpulse xcb xcb-image

build/:
	mkdir build

build/linux_debug: build/
	gcc -Wall -g -o build/linux_debug src/linux/main.cpp -std=c++14 -lstdc++ -lm `pkg-config --cflags --libs $(linux_libs)`

build/mac_debug: build/
	gcc -Wall -g -o build/mac_debug src/mac/main.mm -framework AppKit

.PHONY: clean
clean:
	rm -rf build
