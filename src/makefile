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

DIR_BIN_INCLUDE=../bin/include/
DIR_BIN_LIB=../bin/lib/
DIR_BUILD=../build/
DIR_INCLUDE=../include/
DIR_INCLUDE_COMMON=../include/common/
DIR_INCLUDE_SYSTEM=../include/system/
DIR_INCLUDE_TYPE=../include/type/
DIR_SRC=./
DIR_SRC_COMMON=./common/
DIR_SRC_SERVICE=./service/
DIR_SRC_SYSTEM=./system/
DIR_SRC_TYPE=./type/

FLAGS=-march=native -std=c11 -Wall -Werror

LIB=libdmg.a

build: build_base build_common build_service build_system build_type

build_base: base_dmg.o base_runtime.o

base_dmg.o: $(DIR_SRC)dmg.c $(DIR_INCLUDE)dmg.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)dmg.c -o $(DIR_BUILD)base_dmg.o

base_runtime.o: $(DIR_SRC)runtime.c $(DIR_INCLUDE)runtime.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)runtime.c -o $(DIR_BUILD)base_runtime.o

build_common: common_error.o common_trace.o common_version.o

common_error.o: $(DIR_SRC_COMMON)error.c $(DIR_INCLUDE_COMMON)error.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)error.c -o $(DIR_BUILD)common_error.o

common_trace.o: $(DIR_SRC_COMMON)trace.c $(DIR_INCLUDE_COMMON)trace.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)trace.c -o $(DIR_BUILD)common_trace.o

common_version.o: $(DIR_SRC_COMMON)version.c $(DIR_INCLUDE_COMMON)version.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)version.c -o $(DIR_BUILD)common_version.o

build_service: service_save.o service_sdl.o

service_save.o: $(DIR_SRC_SERVICE)save.c $(DIR_INCLUDE)service.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SERVICE)save.c -o $(DIR_BUILD)service_save.o

service_sdl.o: $(DIR_SRC_SERVICE)sdl.c $(DIR_INCLUDE)service.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SERVICE)sdl.c -o $(DIR_BUILD)service_sdl.o

build_system: system_audio.o system_joypad.o system_memory.o system_processor.o system_serial.o system_timer.o system_video.o

system_audio.o: $(DIR_SRC_SYSTEM)audio.c $(DIR_INCLUDE_SYSTEM)audio.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM)audio.c -o $(DIR_BUILD)system_audio.o

system_joypad.o: $(DIR_SRC_SYSTEM)joypad.c $(DIR_INCLUDE_SYSTEM)joypad.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM)joypad.c -o $(DIR_BUILD)system_joypad.o

system_memory.o: $(DIR_SRC_SYSTEM)memory.c $(DIR_INCLUDE_SYSTEM)memory.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM)memory.c -o $(DIR_BUILD)system_memory.o

system_processor.o: $(DIR_SRC_SYSTEM)processor.c $(DIR_INCLUDE_SYSTEM)processor.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM)processor.c -o $(DIR_BUILD)system_processor.o

system_serial.o: $(DIR_SRC_SYSTEM)serial.c $(DIR_INCLUDE_SYSTEM)serial.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM)serial.c -o $(DIR_BUILD)system_serial.o

system_timer.o: $(DIR_SRC_SYSTEM)timer.c $(DIR_INCLUDE_SYSTEM)timer.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM)timer.c -o $(DIR_BUILD)system_timer.o

system_video.o: $(DIR_SRC_SYSTEM)video.c $(DIR_INCLUDE_SYSTEM)video.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM)video.c -o $(DIR_BUILD)system_video.o

build_type: type_bank.o type_bootrom.o type_buffer.o type_cartridge.o type_mapper.o

type_bank.o: $(DIR_SRC_TYPE)bank.c $(DIR_INCLUDE_TYPE)bank.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_TYPE)bank.c -o $(DIR_BUILD)type_bank.o

type_bootrom.o: $(DIR_SRC_TYPE)bootrom.c $(DIR_INCLUDE_TYPE)bootrom.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_TYPE)bootrom.c -o $(DIR_BUILD)type_bootrom.o

type_buffer.o: $(DIR_SRC_TYPE)buffer.c $(DIR_INCLUDE_TYPE)buffer.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_TYPE)buffer.c -o $(DIR_BUILD)type_buffer.o

type_cartridge.o: $(DIR_SRC_TYPE)cartridge.c $(DIR_INCLUDE_TYPE)cartridge.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_TYPE)cartridge.c -o $(DIR_BUILD)type_cartridge.o

type_mapper.o: $(DIR_SRC_TYPE)mapper.c $(DIR_INCLUDE_TYPE)mapper.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_TYPE)mapper.c -o $(DIR_BUILD)type_mapper.o

archive:
	@echo ''
	@echo '--- BUILDING LIBRARY -----------------------'
	ar rcs $(DIR_BIN_LIB)$(LIB) $(DIR_BUILD)base_dmg.o $(DIR_BUILD)base_runtime.o \
		$(DIR_BUILD)common_error.o $(DIR_BUILD)common_trace.o $(DIR_BUILD)common_version.o \
		$(DIR_BUILD)service_save.o $(DIR_BUILD)service_sdl.o \
		$(DIR_BUILD)system_audio.o $(DIR_BUILD)system_joypad.o $(DIR_BUILD)system_memory.o $(DIR_BUILD)system_processor.o \
			$(DIR_BUILD)system_serial.o $(DIR_BUILD)system_timer.o $(DIR_BUILD)system_video.o \
		$(DIR_BUILD)type_bank.o $(DIR_BUILD)type_bootrom.o $(DIR_BUILD)type_buffer.o $(DIR_BUILD)type_cartridge.o \
			$(DIR_BUILD)type_mapper.o
	cp $(DIR_INCLUDE)dmg.h $(DIR_BIN_INCLUDE)
	@echo '--- DONE -----------------------------------'
	@echo ''
