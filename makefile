# DMG
# Copyright (C) 2020-2021 David Jolly
#
# DMG is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DMG is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

LEVEL?=0

SLOTS=12

DIR_BIN=./bin/
DIR_BIN_INCLUDE=./bin/include/
DIR_BIN_BIN=./bin/bin/
DIR_BIN_LIB=./bin/lib/
DIR_BUILD=./build/
DIR_BUILD_TEST=./build/test/
DIR_BUILD_TOOL=./build/tool/
DIR_ROOT=./
DIR_SRC=./src/
DIR_TEST_AUDIO=./test/audio/
DIR_TEST_GENERATOR=./test/generator/
DIR_TEST_JOYPAD=./test/joypad/
DIR_TEST_MEMORY=./test/memory/
DIR_TEST_PROCESSOR=./test/processor/
DIR_TEST_SERIAL=./test/serial/
DIR_TEST_TIMER=./test/timer/
DIR_TEST_VIDEO=./test/video/
DIR_TOOL=./tool/
DIR_TOOL_SRC=./tool/src/

SERVICE=SDL

BUILD_DEBUG=BUILD_FLAGS=-g\ -DCOLOR\ -D$(SERVICE)\ -DLEVEL=
BUILD_DEBUG_TEST=BUILD_FLAGS=-g\ -DCOLOR\ -DUNITTEST\ -DLEVEL=
BUILD_RELEASE=BUILD_FLAGS=-O3\ -DNDEBUG\ -DCOLOR\ -D$(SERVICE)\ -DLEVEL=
BUILD_RELEASE_TEST=BUILD_FLAGS=-O3\ -DNDEBUG\ -DCOLOR\ -DUNITTEST\ -DLEVEL=

all: release
debug: clean setup library_debug tool_debug
debug_test: clean setup library_debug test_debug
release: clean setup library_release tool_release
release_test: clean setup library_release test_release
package: release tool_package

analyze:
	@echo ''
	cloc $(DIR_ROOT)
	@echo ''
	cppcheck --enable=all --std=c11 $(DIR_ROOT)

clean:
	rm -rf $(DIR_BIN)
	rm -rf $(DIR_BUILD)

setup:
	mkdir -p $(DIR_BUILD)
	mkdir -p $(DIR_BUILD_TOOL)

library_debug:
	cd $(DIR_SRC) && make $(BUILD_DEBUG)$(LEVEL) build -j$(SLOTS)
	cd $(DIR_SRC) && make archive
	cd $(DIR_TOOL_SRC) && make $(BUILD_DEBUG)$(LEVEL) build
	cd $(DIR_TOOL_SRC) && make archive

library_release:
	cd $(DIR_SRC) && make $(BUILD_RELEASE)$(LEVEL) build -j$(SLOTS)
	cd $(DIR_SRC) && make archive
	cd $(DIR_TOOL_SRC) && make $(BUILD_RELEASE)$(LEVEL) build
	cd $(DIR_TOOL_SRC) && make archive

test_debug:
	mkdir -p $(DIR_BUILD_TEST)
	cd $(DIR_TEST_AUDIO) && make $(BUILD_DEBUG_TEST)$(LEVEL) build
	cd $(DIR_TEST_GENERATOR) && make $(BUILD_DEBUG_TEST)$(LEVEL) build
	cd $(DIR_TEST_JOYPAD) && make $(BUILD_DEBUG_TEST)$(LEVEL) build
	cd $(DIR_TEST_MEMORY) && make $(BUILD_DEBUG_TEST)$(LEVEL) build
	cd $(DIR_TEST_PROCESSOR) && make $(BUILD_DEBUG_TEST)$(LEVEL) build
	cd $(DIR_TEST_SERIAL) && make $(BUILD_DEBUG_TEST)$(LEVEL) build
	cd $(DIR_TEST_TIMER) && make $(BUILD_DEBUG_TEST)$(LEVEL) build
	cd $(DIR_TEST_VIDEO) && make $(BUILD_DEBUG_TEST)$(LEVEL) build

test_release:
	mkdir -p $(DIR_BUILD_TEST)
	cd $(DIR_TEST_AUDIO) && make $(BUILD_RELEASE_TEST)$(LEVEL) build
	cd $(DIR_TEST_GENERATOR) && make $(BUILD_RELEASE_TEST)$(LEVEL) build
	cd $(DIR_TEST_JOYPAD) && make $(BUILD_RELEASE_TEST)$(LEVEL) build
	cd $(DIR_TEST_MEMORY) && make $(BUILD_RELEASE_TEST)$(LEVEL) build
	cd $(DIR_TEST_PROCESSOR) && make $(BUILD_RELEASE_TEST)$(LEVEL) build
	cd $(DIR_TEST_SERIAL) && make $(BUILD_RELEASE_TEST)$(LEVEL) build
	cd $(DIR_TEST_TIMER) && make $(BUILD_RELEASE_TEST)$(LEVEL) build
	cd $(DIR_TEST_VIDEO) && make $(BUILD_RELEASE_TEST)$(LEVEL) build

tool_debug:
	cd $(DIR_TOOL) && make $(BUILD_DEBUG)$(LEVEL) build_debug

tool_release:
	cd $(DIR_TOOL) && make $(BUILD_RELEASE)$(LEVEL) build_release

tool_package:
	mkdir -p $(DIR_BIN_BIN)
	mkdir -p $(DIR_BIN_INCLUDE)
	mkdir -p $(DIR_BIN_LIB)
	cd $(DIR_TOOL) && make package
