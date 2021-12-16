all: linux_debug

linux_debug:
	mkdir -p build
	gcc -g -lstdc++ -lX11 -o build/linux_debug src/linux/main.cpp

.PHONY: clean
clean:
	rm -rf build
