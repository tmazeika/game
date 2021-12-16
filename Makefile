all: linux_debug mac_debug

build/:
	mkdir build

linux_debug: build/
	gcc -g -lstdc++ -lX11 -o build/linux_debug src/linux/main.cpp

mac_debug: build/
	gcc -g -framework Foundation -o build/mac_debug src/mac/main.mm

.PHONY: clean
clean:
	rm -rf build
