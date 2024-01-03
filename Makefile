CMAKE_FLAGS := -GNinja
CCACHE_EXISTS := $(shell command -v ccache 2> /dev/null)
COMPDB_EXISTS := $(shell command -v compdb 2> /dev/null)

ifdef CCACHE_EXISTS
    CMAKE_FLAGS += -DCMAKE_C_COMPILER_LAUNCHER=ccache
else
    $(warning ccache not found, build may be slower)
endif

all: build compdb

compdb: cmake
ifdef COMPDB_EXISTS
	@echo "Generating Intellisense Files in background" && compdb -p build/ list > .vscode/compile_commands.json 2>/dev/null &
else
	$(warning compdb not found, Intellisense might be broken)
endif

host: CMAKE_FLAGS += -DPICO_PLATFORM=host
host: build

build: CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=Debug
build: cmake

release: CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=Release
release: cmake

cmake: deps build-folder
	@cd build && cmake $(CMAKE_FLAGS) .. --log-level=ERROR && ninja

build-folder:
	-@sh -c 'mkdir build 2>/dev/null || true'

clean:
	rm -rf build

deps: external/pico-sdk external/mpack external/freertos

clean-deps:
	rm -rf external

external/pico-sdk:
	git clone --depth 1 --branch 1.5.0 https://github.com/raspberrypi/pico-sdk.git external/pico-sdk
	cd external/pico-sdk && git submodule update --init lib/tinyusb
	cd external/pico-sdk && git submodule update --init lib/mbedtls

external/mpack:
	git clone --depth 1 --branch v1.1.1 https://github.com/ludocode/mpack.git external/mpack

external/freertos:
	git clone --depth 1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git external/freertos

docs:
	doxygen Doxyfile

.PHONY: cmake
.PHONY: docs
