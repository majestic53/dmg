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

DIR_ASSEMBLER=./assembler/
DIR_BIN=../bin/
DIR_BIN_LIB=../bin/lib/
DIR_BUILD=../build/
DIR_BUILD_TOOL=../build/tool/
DIR_DISASSEMBLER=./disassembler/
DIR_LAUNCHER=./launcher/
DIR_LIB=./lib/
DIR_RELEASE=./release/
DIR_UTILITY_ROM_FIX=./utility/rom_fix/
DIR_UTILITY_ROM_INFO=./utility/rom_info/
DIR_UTILITY_SAVE_INFO=./utility/save_info/

BIN_ASSEMBLER=dmg-asm
BIN_DISASSEMBLER=dmg-dasm
BIN_LAUNCHER=dmg
BIN_UTILITY_ROM_FIX=dmg-rom-fix
BIN_UTILITY_ROM_INFO=dmg-rom-info
BIN_UTILITY_SAVE_INFO=dmg-save-info

FLAGS=-std=c11 -Wall -Werror
FLAGS_BIN=
FLAGS_LIB=-lpthread -lreadline -lSDL2 -lSDL2main

LIB=libdmg.a
LIB_TOOL=libdmg_tool.a

RELEASE_PRE="dmg_"
RELEASE_EXT=".zip"

SHA256=sha256sum

build_debug: build_tool build_tool_utility link

build_release: build_tool build_tool_utility link

build_tool: tool_assembler.o tool_disassembler.o tool_launcher.o

tool_assembler.o: $(DIR_ASSEMBLER)assembler.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_ASSEMBLER)assembler.c -o $(DIR_BUILD_TOOL)tool_assembler.o

tool_disassembler.o: $(DIR_DISASSEMBLER)disassembler.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_DISASSEMBLER)disassembler.c -o $(DIR_BUILD_TOOL)tool_disassembler.o

tool_launcher.o: $(DIR_LAUNCHER)launcher.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_LAUNCHER)launcher.c -o $(DIR_BUILD_TOOL)tool_launcher.o

build_tool_utility: tool_utility_rom_fix.o tool_utility_rom_info.o tool_utility_save_info.o

tool_utility_rom_fix.o: $(DIR_UTILITY_ROM_FIX)rom_fix.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_UTILITY_ROM_FIX)rom_fix.c -o $(DIR_BUILD_TOOL)tool_utility_rom_fix.o

tool_utility_rom_info.o: $(DIR_UTILITY_ROM_INFO)rom_info.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_UTILITY_ROM_INFO)rom_info.c -o $(DIR_BUILD_TOOL)tool_utility_rom_info.o

tool_utility_save_info.o: $(DIR_UTILITY_SAVE_INFO)save_info.c
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_UTILITY_SAVE_INFO)save_info.c -o $(DIR_BUILD_TOOL)tool_utility_save_info.o

link:
	@echo ''
	@echo '--- BUILDING TOOL --------------------------'
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TOOL)tool_assembler.o $(DIR_BUILD_TOOL)$(LIB_TOOL) -o $(DIR_BIN)$(BIN_ASSEMBLER) $(FLAGS_BIN)
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TOOL)tool_disassembler.o $(DIR_BUILD_TOOL)$(LIB_TOOL) -o $(DIR_BIN)$(BIN_DISASSEMBLER) $(FLAGS_BIN)
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TOOL)tool_launcher.o $(DIR_BIN_LIB)$(LIB) $(DIR_BUILD_TOOL)$(LIB_TOOL) $(DIR_BUILD_TOOL)$(LIB_TOOL) $(FLAGS_LIB) \
		-o $(DIR_BIN)$(BIN_LAUNCHER) $(FLAGS_BIN)
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TOOL)tool_utility_rom_fix.o $(DIR_BUILD_TOOL)$(LIB_TOOL) -o $(DIR_BIN)$(BIN_UTILITY_ROM_FIX) $(FLAGS_BIN)
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TOOL)tool_utility_rom_info.o $(DIR_BUILD_TOOL)$(LIB_TOOL) -o $(DIR_BIN)$(BIN_UTILITY_ROM_INFO) $(FLAGS_BIN)
	$(CC) $(FLAGS) $(BUILD_FLAGS) $(DIR_BUILD_TOOL)tool_utility_save_info.o $(DIR_BUILD_TOOL)$(LIB_TOOL) -o $(DIR_BIN)$(BIN_UTILITY_SAVE_INFO) $(FLAGS_BIN)
	@echo '--- DONE -----------------------------------'
	@echo ''

release:
	@echo ''
	@echo '--- BUILDING RELEASE -----------------------'
	strip -s $(DIR_BIN)$(BIN_ASSEMBLER)
	strip -s $(DIR_BIN)$(BIN_DISASSEMBLER)
	strip -s $(DIR_BIN)$(BIN_LAUNCHER)
	strip -s $(DIR_BIN)$(BIN_UTILITY_ROM_FIX)
	strip -s $(DIR_BIN)$(BIN_UTILITY_ROM_INFO)
	strip -s $(DIR_BIN)$(BIN_UTILITY_SAVE_INFO)
	cd $(DIR_BIN) && $(SHA256) $(DIR_LIB)$(LIB) $(BIN_ASSEMBLER) $(BIN_DISASSEMBLER) $(BIN_LAUNCHER) $(BIN_UTILITY_ROM_FIX) $(BIN_UTILITY_ROM_INFO) \
		$(BIN_UTILITY_SAVE_INFO) > $(DIR_BIN)$(SHA256)
	cd $(DIR_BIN) && zip -r "$(RELEASE_PRE)`head -n 1 readme.txt | grep DMG | awk '{print $$2}'`$(RELEASE_EXT)" . && mkdir -p $(DIR_RELEASE) \
		&& mv $(RELEASE_PRE)*$(RELEASE_EXT) $(DIR_RELEASE)
	@echo '--- DONE -----------------------------------'
	@echo ''
