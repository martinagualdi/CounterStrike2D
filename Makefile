.PHONY: all test clean editor client common server build install

build:
	mkdir -p build/
	RUTA_BASE="" cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug -DTALLER_TESTS=ON -DINSTALADO=OFF $(EXTRA_GENERATE)
	cmake --build  build/ $(EXTRA_COMPILE)

run-tests:
	valgrind ./taller_tests

all: clean build run-tests

build-and-test: build run-tests

clean:
	find build/ -type f \( -name '*.o' -o -name '*.a' -o -name '*.so' -o -name '*.exe' -o -name 'server' -o -name 'client' -o -name 'taller_tests' -o -name '*.dir' \) -delete
	find build/ -type d -name '*.dir' -exec rm -rf {} +
	rm -f client server taller_editor taller_tests

install:
	make clean
	cmake -S . -B ./build -DCMAKE_BUILD_TYPE=debug -DTALLER_TESTS=ON -DINSTALADO=ON $(EXTRA_GENERATE)
	cmake --build build/ $(EXTRA_COMPILE)
	make run-tests
	chmod +x uninstall.sh
	sudo bash ./install.sh

uninstall:
	sudo bash ./uninstall.sh
