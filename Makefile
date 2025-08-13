.PHONY: clean build build-release dev-setup build-dev install build-source test run check fmt docs

all: build-dev

PREFIX = "${PWD}/build/installdir"
LUA_PREFIX = ""

build-source:
	@mkdir -p build
	@cd build/ && cmake .. \
		-G Ninja \
		-DCMAKE_INSTALL_PREFIX="${PREFIX}" \
		&& cmake --build . -j10

dev-setup:
	@cp build/compile_commands.json .

build-dev: build-source dev-setup
	cd build && cmake --install . --prefix "${PREFIX}"

build:
	RELEASE=1 make build-source LUA_PREFIX="${PREFIX}/lua"

install:
	cd build && cmake --install . --prefix "${PREFIX}"

test: build-source
	cd build && QT_QPA_PLATFORM=offscreen ctest -V

clean:
	ccache -C || true
	rm -rf build/
	rm -f compile_commands.json

run: build-dev
	./build/null-browser $(ARGS)

debug:
	DEBUG=1 make build-dev
	gdb ./build/null-browser

fmt:
	clang-format -i ./src/**/*.{hpp,cpp}

check: fmt
	clang-tidy --config-file=.clang-tidy ./src/**/*.{hpp,cpp}

docs:
	rm -rf ./docs/api;
	# API reference pages
	ldoc -p null-browser -t "Null browser api" \
		--merge --ignore --lls \
		--dir ./docs/api \
		./lua/null-browser/
	# Symbols json
	ldoc --merge --ignore --lls \
		./lua/null-browser/ \
		--dir ./docs/api \
		--filter ldoc_json_filter.lua;

# appimage:
#   nix bundle --bundler github:ralismark/nix-appimage

