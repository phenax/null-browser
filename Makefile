
.PHONY: build
build:
	mkdir -p build
	cd build/ && cmake ..
	cd build/ && make
	cp build/compile_commands.json .

