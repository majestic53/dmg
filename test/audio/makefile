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

DIR_BIN=../../bin/
DIR_BUILD=../../build/
DIR_BUILD_TEST=../../build/test/
DIR_ROOT=./
DIR_SRC_SYSTEM=../../src/system/

BIN=test-audio

FLAGS=-std=c11 -Wall -Werror
FLAGS_BIN=
FLAGS_LIB=-lSDL2 -lSDL2main

build: build_test link run

build_test: test_audio.o test_system_audio.o

test_audio.o: $(DIR_ROOT)audio.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_ROOT)audio.c -o $(DIR_BUILD_TEST)test_audio.o

test_system_audio.o: $(DIR_SRC_SYSTEM)audio.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM)audio.c -o $(DIR_BUILD_TEST)test_system_audio.o

link:
	@echo ''
	@echo '--- BUILDING AUDIO TEST --------------------'
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TEST)test_audio.o $(DIR_BUILD_TEST)test_system_audio.o $(DIR_BUILD)common_trace.o $(FLAGS_LIB) -o $(DIR_BUILD_TEST)$(BIN)
	@echo '--- DONE -----------------------------------'
	@echo ''

run:
	@echo ''
	@echo '--- RUNNING AUDIO TEST ---------------------'
	@cd $(DIR_BUILD_TEST) && if ./$(BIN) $(FLAGS_BIN); \
	then \
		echo '--- PASSED ---------------------------------'; \
	else \
		echo '--- FAILED ---------------------------------'; \
		exit 1; \
	fi
	@echo ''
