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
DIR_SRC=./

FLAGS=-std=c11 -Wall -Werror

LIB=libdmg_tool.a

build: build_base

build_base: base_syntax.o base_trace.o

base_syntax.o: $(DIR_SRC)syntax.c $(DIR_INCLUDE)syntax.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)syntax.c -o $(DIR_BUILD_TOOL)base_syntax.o

base_trace.o: $(DIR_SRC)trace.c $(DIR_INCLUDE)trace.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)trace.c -o $(DIR_BUILD_TOOL)base_trace.o

archive:
	@echo ''
	@echo '--- BUILDING TOOL LIBRARY ------------------'
	ar rcs $(DIR_BUILD_TOOL)$(LIB) $(DIR_BUILD)common_version.o $(DIR_BUILD_TOOL)base_syntax.o $(DIR_BUILD_TOOL)base_trace.o
	@echo '--- DONE -----------------------------------'
	@echo ''
