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
# along with this program. If not, see <http://www.gnu.org/licenses/>.

DIR_BUILD=../../build/
DIR_BUILD_TOOL=../../build/tool/
DIR_INCLUDE=../include/
DIR_INCLUDE_ASSEMBLER=../include/assembler/
DIR_SRC=./
DIR_SRC_ASSEMBLER=./assembler/

FLAGS=-std=c11 -Wall -Werror

LIB=libdmg_tool.a

build: build_assembler build_base

build_assembler: assembler_stream.o

assembler_stream.o: $(DIR_SRC_ASSEMBLER)stream.c $(DIR_INCLUDE_ASSEMBLER)stream.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_ASSEMBLER)stream.c -o $(DIR_BUILD_TOOL)assembler_stream.o

build_base: base_syntax.o base_trace.o

base_syntax.o: $(DIR_SRC)syntax.c $(DIR_INCLUDE)syntax.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)syntax.c -o $(DIR_BUILD_TOOL)base_syntax.o

base_trace.o: $(DIR_SRC)trace.c $(DIR_INCLUDE)trace.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)trace.c -o $(DIR_BUILD_TOOL)base_trace.o

archive:
	@echo ''
	@echo '--- BUILDING TOOL LIBRARY ------------------'
	ar rcs $(DIR_BUILD_TOOL)$(LIB) $(DIR_BUILD_TOOL)assembler_stream.o \
		$(DIR_BUILD_TOOL)base_syntax.o $(DIR_BUILD_TOOL)base_trace.o \
		$(DIR_BUILD)common_error.o $(DIR_BUILD)common_trace.o $(DIR_BUILD)common_version.o
	@echo '--- DONE -----------------------------------'
	@echo ''
