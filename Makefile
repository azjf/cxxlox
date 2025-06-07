LOCAL_PATH := $(shell pwd)

CXX := g++
CXXFLAGS := -std=c++20 -Wall -Werror -Wfatal-errors -g

OUT_DIR := $(LOCAL_PATH)/build

export CXX CXXFLAGS OUT_DIR

default:
	@mkdir -p $(OUT_DIR)
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean
