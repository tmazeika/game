all: build/linux_debug build/mac_debug

build/:
	mkdir build

build/linux_debug: build/
	gcc -g -lstdc++ -lX11 -o build/linux_debug src/linux/main.cpp

build/mac_debug: build/
	gcc -g -framework Cocoa -o build/mac_debug src/mac/main.mm

.PHONY: clean
clean:
	rm -rf build
