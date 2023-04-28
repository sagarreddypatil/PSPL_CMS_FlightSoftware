all: build

build: deps
	-@sh -c 'mkdir build 2>/dev/null || true'
	@cd build && cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" .. && ninja

release: deps
	-@rm -rf release
	-@mkdir release
	cd release && cmake -DCMAKE_BUILD_TYPE=Release -G "Ninja" .. && ninja

clean:
	rm -rf build

deps: external/pico-sdk external/mpack

clean-deps:
	rm -rf external

external/pico-sdk:
	git clone --depth 1 --branch 1.5.0 https://github.com/raspberrypi/pico-sdk.git external/pico-sdk
	cd external/pico-sdk && git submodule update --init lib/tinyusb

external/mpack:
	git clone --depth 1 --branch v1.1.1 https://github.com/ludocode/mpack.git external/mpack

docs:
	doxygen Doxyfile

.PHONY: build
.PHONY: release
.PHONY: docs