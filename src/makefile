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

DIR_BIN_INCLUDE=../bin/include/
DIR_BIN_LIB=../bin/lib/
DIR_BUILD=../build/
DIR_INCLUDE=../include/
DIR_SRC=./

FLAGS=-march=native -std=c11 -Wall -Werror

LIB=libdmg.a

build: build_base

build_base: base_dmg.o base_runtime.o

base_dmg.o: $(DIR_SRC)dmg.c $(DIR_INCLUDE)dmg.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)dmg.c -o $(DIR_BUILD)base_dmg.o

base_runtime.o: $(DIR_SRC)runtime.c $(DIR_INCLUDE)runtime.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)runtime.c -o $(DIR_BUILD)base_runtime.o

archive:
	@echo ''
	@echo '--- BUILDING LIBRARY -----------------------'
	ar rcs $(DIR_BIN_LIB)$(LIB) $(DIR_BUILD)base_dmg.o $(DIR_BUILD)base_runtime.o
	cp $(DIR_INCLUDE)dmg.h $(DIR_BIN_INCLUDE)
	@echo '--- DONE -----------------------------------'
	@echo ''
