.PHONY: clean build test

all: build

build:
	@mkdir -p build
	@cd build/ && cmake .. && make -j4
	# cp --no-preserve=mode,ownership -r ${CEF_PACKAGE_PATH}/lib/* ./build/lib/
	# cp --no-preserve=mode,ownership -r ${CEF_PACKAGE_PATH}/share/cef/* ./build/lib/
	@cp build/compile_commands.json .

test: build
	cd build && ctest -V

clean:
	rm -rf build/
	rm -f compile_commands.json

run: build
	./build/web-browser
