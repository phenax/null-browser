.PHONY: clean build build-release build-source test run check

all: build-dev

PREFIX = "${PWD}/build/installdir"
LUA_PREFIX = ""

build-source:
	@mkdir -p build
	@cd build/ && cmake .. \
		-G Ninja \
		-DNULL_LUA_PREFIX="${LUA_PREFIX}" \
		-DCMAKE_INSTALL_PREFIX="${PREFIX}" \
		&& cmake --build . -j10

dev-setup:
	@cp build/compile_commands.json .

build-dev: build-source dev-setup

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
	./build/null-browser

debug:
	DEBUG=1 make build-dev
	gdb ./build/null-browser

check:
	clang-format -i ./src/**/*.{hpp,cpp}
	clang-tidy --config-file=.clang-tidy ./src/**/*.{hpp,cpp}

# appimage:
#   nix bundle --bundler github:ralismark/nix-appimage

