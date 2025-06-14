.PHONY: all test clean editor client common server build install

build:
	mkdir -p build/
	cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug $(EXTRA_GENERATE)
	cmake --build  build/ $(EXTRA_COMPILE)

run-tests:
	./taller_tests

all: clean build run-tests

build-and-test: build run-tests

clean:
	find build/ -type f \( -name '*.o' -o -name '*.a' -o -name '*.so' -o -name '*.exe' -o -name 'server' -o -name 'client' -o -name 'taller_tests' -o -name '*.dir' \) -delete
	find build/ -type d -name '*.dir' -exec rm -rf {} +
	rm -f client server taller_editor taller_tests

install: build-and-test
	sudo bash ./install.sh
