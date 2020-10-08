# DMG
# Copyright (C) 2020 David Jolly
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

LEVEL?=0

SLOTS=12

DIR_BIN=./bin/
DIR_BIN_INCLUDE=./bin/include/
DIR_BIN_LIB=./bin/lib/
DIR_BUILD=./build/
DIR_BUILD_TEST=./build/test/
DIR_ROOT=./
DIR_SRC=./src/
DIR_TEST_SERIAL=./test/serial/
DIR_TEST_TIMER=./test/timer/
DIR_TOOL=./tool/

BUILD_DEBUG=BUILD_FLAGS=-g\ -DCOLOR\ -DLEVEL=
BUILD_RELEASE=BUILD_FLAGS=-O3\ -DNDEBUG\ -DCOLOR\ -DLEVEL=

all: release
debug: clean setup build_debug
release: clean setup build_release

clean:
	rm -rf $(DIR_BIN)
	rm -rf $(DIR_BUILD)

setup:
	mkdir -p $(DIR_BIN_INCLUDE)
	mkdir -p $(DIR_BIN_LIB)
	mkdir -p $(DIR_BUILD)
	mkdir -p $(DIR_BUILD_TEST)

build_debug:
	cd $(DIR_SRC) && make $(BUILD_DEBUG)$(LEVEL) build -j$(SLOTS)
	cd $(DIR_SRC) && make archive
	cd $(DIR_TEST_SERIAL) && make $(BUILD_DEBUG)$(LEVEL) build
	cd $(DIR_TEST_TIMER) && make $(BUILD_DEBUG)$(LEVEL) build
	cd $(DIR_TOOL) && make $(BUILD_DEBUG)$(LEVEL) build

build_release:
	cd $(DIR_SRC) && make $(BUILD_RELEASE)$(LEVEL) build -j$(SLOTS)
	cd $(DIR_SRC) && make archive
	cd $(DIR_TEST_SERIAL) && make $(BUILD_RELEASE)$(LEVEL) build
	cd $(DIR_TEST_TIMER) && make $(BUILD_RELEASE)$(LEVEL) build
	cd $(DIR_TOOL) && make $(BUILD_RELEASE)$(LEVEL) build

analyze:
	@echo ''
	cloc $(DIR_ROOT)
	@echo ''
	cppcheck --enable=all --std=c11 $(DIR_SRC) $(DIR_TOOL)
