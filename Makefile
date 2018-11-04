debug:
	mkdir -p build/Debug && cd build/Debug && cmake -DCMAKE_BUILD_TYPE=Debug ../.. && make -j 8

release:
	mkdir -p build/Release && cd build/Release && cmake -DCMAKE_BUILD_TYPE=Release ../.. && make -j 8

clean:
	rm -rf build

cleanSandbox:
	rm -rf sandbox

copyRelease:
	rsync -avH build/Release/lib build/Release/bin sandbox/

copyDebug:
	rsync -avH build/Debug/lib build/Debug/bin sandbox/


copy:
	mkdir -p sandbox/
	rsync -avH data sandbox/
	mv sandbox/data/*.sh sandbox/

sandbox: release copy copyRelease

distDebug: debug copy copyDebug

r: sandbox
	cd sandbox && ./run.sh local

d: distDebug
	cd sandbox && ./debug.sh local

