# author: Brando
# date: 11/27/24
#

include external/libs/bflibc/makefiles/lib.mk 

help:
	@echo "Usage:"
	@echo "	make <target> <variables>"
	@echo ""
	@echo "Target(s):"
	@echo "	clean			cleans build and bin folder"
	@echo "	build 			builds release verions"
	@echo "	package			compresses build"
	@echo "	dependencies		builds all dependencies in the external directory"
	@echo "	clean-dependencies	builds all dependencies in the external directory"
	@echo "	clean-all		cleans local and dependency builds"
	@echo ""
	@echo "Variable(s):"
	@echo "	CONFIG		use this to change the build config. Accepts \"release\" (default), \"debug\", or \"test\""
	@echo "	IDENTITY	(macos only) \"Developer ID Application\" common name"
	@echo "	TEAMID 		(macos only) Organizational Unit"
	@echo "	EMAIL 		(macos only) Developer account email"
	@echo "	PW		(macos only) Developer account password"
	@echo ""
	@echo "Example(s):"
	@echo "	Build for release for macOS distribution"
	@echo "		make clean build codesign package notarize staple IDENTITY=\"\" TEAMID=\"\" EMAIL=\"\" PW=\"\""
	@echo "	Build for release for Linux distribution"
	@echo "		make clean build package"

COMPILER = g++
CPPSTD = -std=c++20
CONFIG = release
BUILD_PATH = build/$(CONFIG)
BIN_PATH = bin/$(CONFIG)
BUILD_TYPE = executable
SOURCE_EXT = cpp
HEADER_EXT = hpp
FILES = 

UNAME_S := $(shell uname -s)

ifneq ($(CONFIG),test) # test
LIBRARIES += \
	external/bin/libs/$(CONFIG)/bflibcpp/libbfcpp.a \
	external/bin/libs/$(CONFIG)/bflibc/libbfc.a 
endif

LINKS = $(BF_LIB_C_FLAGS)

### Release settings
ifeq ($(CONFIG),release) # release
MAIN_FILE = src/main.cpp
BIN_NAME = spellcheck
FLAGS = $(CPPFLAGS) -Isrc/ $(CPPSTD) -Iexternal/bin/libs/release

### Debug settings
else ifeq ($(CONFIG),debug) # debug
MAIN_FILE = src/main.cpp
BIN_NAME = spellcheck-debug
#ADDR_SANITIZER = -fsanitize=address
FLAGS = $(CPPFLAGS) -DDEBUG -g -Isrc/ $(ADDR_SANITIZER) $(CPPSTD) -Iexternal/bin/libs/debug

### Test settings
else ifeq ($(CONFIG),test) # test
MAIN_FILE = testbench/tests.cpp
BIN_NAME = spellcheck-test
#ADDR_SANITIZER = -fsanitize=address
FLAGS = $(CPPFLAGS) -DDEBUG -DTESTING -g -Isrc/ $(ADDR_SANITIZER) $(CPPSTD) -Iexternal/bin/libs/debug
LIBRARIES += \
	external/bin/libs/debug/bflibc/libbfc-debug.a \
	external/bin/libs/debug/bflibcpp/libbfcpp-debug.a \
	external/bin/libs/debug/bftest/libbftest-debug.a
endif # ($(CONFIG),...)

LIBS_MAKEFILES_PATH:=$(CURDIR)/external/libs/makefiles
include $(LIBS_MAKEFILES_PATH)/build.mk 

### Dependencies

dependencies:
	cd external && make build

clean-dependencies:
	cd external && make clean

clean-all: clean clean-dependencies

