debug:
	mkdir -p build/Debug && cd build/Debug && cmake -DCMAKE_BUILD_TYPE=Debug ../.. && make -j 8

release:
	mkdir -p build/Release && cd build/Release && cmake -DCMAKE_BUILD_TYPE=Release ../.. && make -j 8

clean:
	rm -rf build

cleanInstall:
	sudo rm -rf /usr/local/include/sdl_graphic
	sudo rm -rf /usr/local/lib/libsdl_graphic.so

r: release

d: debug

copyRelease:
	sudo cp build/Release/lib/libsdl_graphic.so /usr/local/lib

copyDebug:
	sudo cp build/Debug/lib/libsdl_graphic.so /usr/local/lib

copyHeaders:
	sudo mkdir -p /usr/local/include/sdl_graphic
	sudo cp src/*.hh /usr/local/include/sdl_graphic
	sudo cp src/*.hxx /usr/local/include/sdl_graphic

install: r copyRelease copyHeaders

installD: d copyDebug copyHeaders
