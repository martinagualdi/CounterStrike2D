.PHONY: all test clean editor client common server build

compile-debug:
	mkdir -p build/
	/snap/bin/cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug $(EXTRA_GENERATE)
	/snap/bin/cmake --build  build/ $(EXTRA_COMPILE)

run-tests: compile-debug
	./build/taller_tests

all: clean run-tests

clean:
	find build/ -type f \( -name '*.o' -o -name '*.a' -o -name '*.so' -o -name '*.exe' -o -name 'server' -o -name 'client' -o -name 'taller_tests' -o -name '*.dir' \) -delete
	find build/ -type d -name '*.dir' -exec rm -rf {} +
