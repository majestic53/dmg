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

DIR_BIN=../bin/
DIR_BIN_LIB=../bin/lib/
DIR_BUILD=../build/
DIR_BUILD_TOOL=../build/tool/
DIR_LAUNCHER=./launcher/
DIR_SRC=./src/
DIR_UTILITY_ROM_INFO=./utility/rom_info/
DIR_UTILITY_SAVE_INFO=./utility/save_info/

BIN_LAUNCHER=dmg
BIN_UTILITY_ROM_INFO=dmg-rom-info
BIN_UTILITY_SAVE_INFO=dmg-save-info

FLAGS=-march=native -std=c11 -Wall -Werror
FLAGS_BIN=
FLAGS_LIB=-lSDL2 -lSDL2main

LIB=libdmg.a
LIB_LAUNCHER=libdmg-launcher.a
LIB_UTILITY_ROM_INFO=libdmg-utility-rom-info.a
LIB_UTILITY_SAVE_INFO=libdmg-utility-save-info.a

build: clean setup build_tool build_tool_common build_tool_utility archive link

clean:
	rm -rf $(DIR_BUILD_TOOL)

setup:
	mkdir -p $(DIR_BUILD_TOOL)

build_tool: tool_launcher.o

tool_launcher.o: $(DIR_LAUNCHER)launcher.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_LAUNCHER)launcher.c -o $(DIR_BUILD_TOOL)tool_launcher.o

build_tool_common: tool_common_file.o

tool_common_file.o: $(DIR_SRC)file.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)file.c -o $(DIR_BUILD_TOOL)tool_common_file.o

build_tool_utility: tool_utility_rom_info.o tool_utility_save_info.o

tool_utility_rom_info.o: $(DIR_UTILITY_ROM_INFO)rom_info.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_UTILITY_ROM_INFO)rom_info.c -o $(DIR_BUILD_TOOL)tool_utility_rom_info.o

tool_utility_save_info.o: $(DIR_UTILITY_SAVE_INFO)save_info.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_UTILITY_SAVE_INFO)save_info.c -o $(DIR_BUILD_TOOL)tool_utility_save_info.o

archive:
	@echo ''
	@echo '--- BUILDING TOOL LIBRARY ------------------'
	ar rcs $(DIR_BUILD_TOOL)$(LIB_LAUNCHER) $(DIR_BUILD_TOOL)tool_launcher.o $(DIR_BUILD_TOOL)tool_common_file.o
	ar rcs $(DIR_BUILD_TOOL)$(LIB_UTILITY_ROM_INFO) $(DIR_BUILD_TOOL)tool_utility_rom_info.o $(DIR_BUILD_TOOL)tool_common_file.o $(DIR_BUILD)common_version.o
	ar rcs $(DIR_BUILD_TOOL)$(LIB_UTILITY_SAVE_INFO) $(DIR_BUILD_TOOL)tool_utility_save_info.o $(DIR_BUILD_TOOL)tool_common_file.o $(DIR_BUILD)common_version.o
	@echo '--- DONE -----------------------------------'

link:
	@echo ''
	@echo '--- BUILDING TOOL --------------------------'
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TOOL)$(LIB_LAUNCHER) $(DIR_BIN_LIB)$(LIB) $(FLAGS_LIB) -o $(DIR_BIN)$(BIN_LAUNCHER) $(FLAGS_BIN)
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TOOL)$(LIB_UTILITY_ROM_INFO) $(FLAGS_LIB) -o $(DIR_BIN)$(BIN_UTILITY_ROM_INFO) $(FLAGS_BIN)
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TOOL)$(LIB_UTILITY_SAVE_INFO) $(FLAGS_LIB) -o $(DIR_BIN)$(BIN_UTILITY_SAVE_INFO) $(FLAGS_BIN)
	@echo '--- DONE -----------------------------------'
	@echo ''
