all: build

build: pico-sdk
	-@sh -c 'mkdir build 2>/dev/null || true'
	@cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make

release: pico-sdk
	-@rm -rf release
	-@mkdir release
	cd release && cmake -DCMAKE_BUILD_TYPE=Release .. && make

clean:
	rm -rf build

pico-sdk:
	git clone --depth 1 --branch 1.5.0 https://github.com/raspberrypi/pico-sdk.git
	cd pico-sdk && git submodule update --init lib/tinyusb

clean-sdk:
	rm -rf pico-sdk

docs:
	doxygen Doxyfile

.PHONY: build
.PHONY: release
.PHONY: docs