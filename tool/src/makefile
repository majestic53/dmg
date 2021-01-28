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

DIR_BUILD=../../build/
DIR_BUILD_TOOL=../../build/tool/
DIR_INCLUDE=../include/
DIR_INCLUDE_ASSEMBLER=../include/assembler/
DIR_INCLUDE_ASSEMBLER_COMMON=../include/assembler/common/
DIR_INCLUDE_COMMON=../include/common/
DIR_INCLUDE_LAUNCHER=../include/launcher/
DIR_SRC=./
DIR_SRC_ASSEMBLER=./assembler/
DIR_SRC_ASSEMBLER_COMMON=./assembler/common/
DIR_SRC_COMMON=./common/
DIR_SRC_LAUNCHER=./launcher/

FLAGS=-std=c11 -Wall -Werror

LIB=libdmg_tool.a

build: build_tool_assembler build_tool_assembler_common build_tool_common build_tool_launcher

build_tool_assembler: tool_assembler_generator.o tool_assembler_lexer.o tool_assembler_parser.o tool_assembler_stream.o

tool_assembler_generator.o: $(DIR_SRC_ASSEMBLER)generator.c $(DIR_INCLUDE_ASSEMBLER)generator.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_ASSEMBLER)generator.c -o $(DIR_BUILD_TOOL)tool_assembler_generator.o

tool_assembler_lexer.o: $(DIR_SRC_ASSEMBLER)lexer.c $(DIR_INCLUDE_ASSEMBLER)lexer.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_ASSEMBLER)lexer.c -o $(DIR_BUILD_TOOL)tool_assembler_lexer.o

tool_assembler_parser.o: $(DIR_SRC_ASSEMBLER)parser.c $(DIR_INCLUDE_ASSEMBLER)parser.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_ASSEMBLER)parser.c -o $(DIR_BUILD_TOOL)tool_assembler_parser.o

tool_assembler_stream.o: $(DIR_SRC_ASSEMBLER)stream.c $(DIR_INCLUDE_ASSEMBLER)stream.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_ASSEMBLER)stream.c -o $(DIR_BUILD_TOOL)tool_assembler_stream.o

build_tool_assembler_common: tool_assembler_common_string.o tool_assembler_common_token.o tool_assembler_common_tree.o

tool_assembler_common_string.o: $(DIR_SRC_ASSEMBLER_COMMON)string.c $(DIR_INCLUDE_ASSEMBLER_COMMON)string.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_ASSEMBLER_COMMON)string.c -o $(DIR_BUILD_TOOL)tool_assembler_common_string.o

tool_assembler_common_token.o: $(DIR_SRC_ASSEMBLER_COMMON)token.c $(DIR_INCLUDE_ASSEMBLER_COMMON)token.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_ASSEMBLER_COMMON)token.c -o $(DIR_BUILD_TOOL)tool_assembler_common_token.o

tool_assembler_common_tree.o: $(DIR_SRC_ASSEMBLER_COMMON)tree.c $(DIR_INCLUDE_ASSEMBLER_COMMON)tree.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_ASSEMBLER_COMMON)tree.c -o $(DIR_BUILD_TOOL)tool_assembler_common_tree.o

build_tool_common: tool_common_syntax.o tool_common_trace.o tool_common_usage.o tool_common_version.o

tool_common_syntax.o: $(DIR_SRC_COMMON)syntax.c $(DIR_INCLUDE_COMMON)syntax.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)syntax.c -o $(DIR_BUILD_TOOL)tool_common_syntax.o

tool_common_trace.o: $(DIR_SRC_COMMON)trace.c $(DIR_INCLUDE_COMMON)trace.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)trace.c -o $(DIR_BUILD_TOOL)tool_common_trace.o

tool_common_usage.o: $(DIR_SRC_COMMON)usage.c $(DIR_INCLUDE_COMMON)usage.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)usage.c -o $(DIR_BUILD_TOOL)tool_common_usage.o

tool_common_version.o: $(DIR_SRC_COMMON)version.c $(DIR_INCLUDE_COMMON)version.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)version.c -o $(DIR_BUILD_TOOL)tool_common_version.o

build_tool_launcher: tool_launcher_debug.o tool_launcher_socket.o

tool_launcher_debug.o: $(DIR_SRC_LAUNCHER)debug.c $(DIR_INCLUDE_LAUNCHER)debug.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_LAUNCHER)debug.c -o $(DIR_BUILD_TOOL)tool_launcher_debug.o

tool_launcher_socket.o: $(DIR_SRC_LAUNCHER)socket.c $(DIR_INCLUDE_LAUNCHER)socket.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_LAUNCHER)socket.c -o $(DIR_BUILD_TOOL)tool_launcher_socket.o

archive:
	@echo ''
	@echo '--- BUILDING TOOL LIBRARY ------------------'
	ar rcs $(DIR_BUILD_TOOL)$(LIB) $(DIR_BUILD_TOOL)tool_assembler_generator.o $(DIR_BUILD_TOOL)tool_assembler_lexer.o \
			$(DIR_BUILD_TOOL)tool_assembler_parser.o $(DIR_BUILD_TOOL)tool_assembler_stream.o \
		$(DIR_BUILD_TOOL)tool_assembler_common_string.o $(DIR_BUILD_TOOL)tool_assembler_common_token.o \
			$(DIR_BUILD_TOOL)tool_assembler_common_tree.o \
		$(DIR_BUILD_TOOL)tool_common_syntax.o $(DIR_BUILD_TOOL)tool_common_trace.o $(DIR_BUILD_TOOL)tool_common_usage.o \
			$(DIR_BUILD_TOOL)tool_common_version.o \
		$(DIR_BUILD)common_error.o $(DIR_BUILD)common_trace.o $(DIR_BUILD)common_version.o \
		$(DIR_BUILD_TOOL)tool_launcher_debug.o $(DIR_BUILD_TOOL)tool_launcher_socket.o
	@echo '--- DONE -----------------------------------'
	@echo ''
