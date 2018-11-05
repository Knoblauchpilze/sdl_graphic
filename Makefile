debug:
	mkdir -p build/Debug && cd build/Debug && cmake -DCMAKE_BUILD_TYPE=Debug ../.. && make -j 8

release:
	mkdir -p build/Release && cd build/Release && cmake -DCMAKE_BUILD_TYPE=Release ../.. && make -j 8

clean:
	rm -rf build

r: release

d: debug

install: r
	sudo cp build/Release/lib/libsdl_graphic.so /usr/local/lib
	sudo mkdir -p /usr/local/include/sdl_graphic
	sudo cp src/*.hh /usr/local/include/sdl_graphic
	sudo cp src/*.hxx /usr/local/include/sdl_graphic



