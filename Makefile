.PHONY: clean build

all: build

build:
	@mkdir -p build
	@cd build/ && cmake .. && make
	# cp --no-preserve=mode,ownership -r ${CEF_PACKAGE_PATH}/lib/* ./build/lib/
	# cp --no-preserve=mode,ownership -r ${CEF_PACKAGE_PATH}/share/cef/* ./build/lib/
	cp build/compile_commands.json .

clean:
	rm -rf build/
	rm -f compile_commands.json

run:
	./build/web-browser

# -DCMAKE_BUILD_TYPE=Release
