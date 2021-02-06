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

BIN=test-video

FLAGS=-std=c11 -Wall -Werror
FLAGS_BIN=
FLAGS_LIB=-lSDL2 -lSDL2main

build: build_test link run

build_test: test_video.o test_system_video.o

test_video.o: $(DIR_ROOT)video.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_ROOT)video.c -o $(DIR_BUILD_TEST)test_video.o

test_system_video.o: $(DIR_SRC_SYSTEM)video.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM)video.c -o $(DIR_BUILD_TEST)test_system_video.o

link:
	@echo ''
	@echo '--- BUILDING VIDEO TEST --------------------'
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TEST)test_video.o $(DIR_BUILD_TEST)test_system_video.o $(DIR_BUILD)common_error.o $(DIR_BUILD)common_trace.o $(FLAGS_LIB) -o $(DIR_BUILD_TEST)$(BIN)
	@echo '--- DONE -----------------------------------'
	@echo ''

run:
	@echo ''
	@echo '--- RUNNING VIDEO TEST ---------------------'
	@cd $(DIR_BUILD_TEST) && if ./$(BIN) $(FLAGS_BIN); \
	then \
		echo '--- PASSED ---------------------------------'; \
	else \
		echo '--- FAILED ---------------------------------'; \
		exit 1; \
	fi
	@echo ''
