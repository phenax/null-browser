.PHONY: clean build build-release test check

all: build

PREFIX = "${PWD}/build/installdir"

build:
	@mkdir -p build
	@cd build/ && cmake .. -DCMAKE_INSTALL_PREFIX="${PREFIX}" && make -j12
	# cp --no-preserve=mode,ownership -r ${CEF_PACKAGE_PATH}/lib/* ./build/lib/
	# cp --no-preserve=mode,ownership -r ${CEF_PACKAGE_PATH}/share/cef/* ./build/lib/
	@cp build/compile_commands.json .

install: build-release
	cd build && cmake --install . --prefix "${PREFIX}"

test: build
	cd build && QT_QPA_PLATFORM=offscreen ctest -V

clean:
	rm -rf build/
	rm -f compile_commands.json

run: build
	./build/null-browser

debug:
	DEBUG=1 make build
	gdb ./build/null-browser

build-release:
	RELEASE=1 make build

check:
	clang-format -i ./src/**/*.{hpp,cpp}
	clang-tidy --config-file=.clang-tidy ./src/**/*.{hpp,cpp}

