CMAKE_FLAGS := -G "Ninja"

all: build-compdb

build-compdb: build
	@echo "Generating Intellisense Files in background" && compdb -p build/ list > .vscode/compile_commands.json 2>/dev/null &

host: CMAKE_FLAGS += -DPICO_PLATFORM=host
host: build

build: CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=Debug
build: cmake

release: CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=Release
release: cmake

cmake: deps build-folder
	@cd build && cmake $(CMAKE_FLAGS) .. && ninja

build-folder:
	-@sh -c 'mkdir build 2>/dev/null || true'

clean:
	rm -rf build

deps: external/pico-sdk external/mpack

clean-deps:
	rm -rf external

external/pico-sdk:
	git clone --depth 1 --branch 1.5.0 https://github.com/raspberrypi/pico-sdk.git external/pico-sdk
	cd external/pico-sdk && git submodule update --init lib/tinyusb
	cd external/pico-sdk && git submodule update --init lib/mbedtls

external/mpack:
	git clone --depth 1 --branch v1.1.1 https://github.com/ludocode/mpack.git external/mpack

docs:
	doxygen Doxyfile

.PHONY: cmake
.PHONY: docs
