# CMake wrapper for Linux / macOS. Default: Unix Makefiles + Release.
#   make
#   make BUILD_DIR=build-ninja CMAKE_GENERATOR=Ninja
#   make clean

BUILD_DIR         ?= build
CMAKE             ?= cmake
CMAKE_GENERATOR  ?= Unix Makefiles
CMAKE_BUILD_TYPE ?= Release

.PHONY: all clean

all:
	$(CMAKE) -S . -B $(BUILD_DIR) -G "$(CMAKE_GENERATOR)" -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
	$(CMAKE) --build $(BUILD_DIR) --parallel

clean:
	rm -rf $(BUILD_DIR)
