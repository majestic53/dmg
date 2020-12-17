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

DIR_BIN=../bin/
DIR_BIN_INCLUDE=../bin/include/
DIR_BIN_LIB=../bin/lib/
DIR_BUILD=../build/
DIR_DOCS=../docs/
DIR_INCLUDE=../include/
DIR_INCLUDE_COMMON=../include/common/
DIR_INCLUDE_COMMON_MAPPER=../include/common/mapper/
DIR_INCLUDE_SYSTEM=../include/system/
DIR_INCLUDE_SYSTEM_PROCESSOR=../include/system/processor/
DIR_SRC=./
DIR_SRC_COMMON=./common/
DIR_SRC_COMMON_MAPPER=./common/mapper/
DIR_SRC_SERVICE=./service/
DIR_SRC_SYSTEM=./system/
DIR_SRC_SYSTEM_PROCESSOR=./system/processor/

FLAGS=-std=c11 -Wall -Werror

LIB=libdmg.a

build: build_base build_common build_common_mapper build_service build_system build_system_processor

build_base: base_dmg.o base_runtime.o base_service.o

base_dmg.o: $(DIR_SRC)dmg.c $(DIR_INCLUDE)dmg.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)dmg.c -o $(DIR_BUILD)base_dmg.o

base_runtime.o: $(DIR_SRC)runtime.c $(DIR_INCLUDE)runtime.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)runtime.c -o $(DIR_BUILD)base_runtime.o

base_service.o: $(DIR_SRC)service.c $(DIR_INCLUDE)service.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC)service.c -o $(DIR_BUILD)base_service.o

build_common: common_bank.o common_bootrom.o common_buffer.o common_cartridge.o common_error.o common_mapper.o common_trace.o common_version.o

common_bank.o: $(DIR_SRC_COMMON)bank.c $(DIR_INCLUDE_COMMON)bank.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)bank.c -o $(DIR_BUILD)common_bank.o

common_bootrom.o: $(DIR_SRC_COMMON)bootrom.c $(DIR_INCLUDE_COMMON)bootrom.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)bootrom.c -o $(DIR_BUILD)common_bootrom.o

common_buffer.o: $(DIR_SRC_COMMON)buffer.c $(DIR_INCLUDE_COMMON)buffer.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)buffer.c -o $(DIR_BUILD)common_buffer.o

common_cartridge.o: $(DIR_SRC_COMMON)cartridge.c $(DIR_INCLUDE_COMMON)cartridge.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)cartridge.c -o $(DIR_BUILD)common_cartridge.o

common_error.o: $(DIR_SRC_COMMON)error.c $(DIR_INCLUDE_COMMON)error.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)error.c -o $(DIR_BUILD)common_error.o

common_mapper.o: $(DIR_SRC_COMMON)mapper.c $(DIR_INCLUDE_COMMON)mapper.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)mapper.c -o $(DIR_BUILD)common_mapper.o

common_trace.o: $(DIR_SRC_COMMON)trace.c $(DIR_INCLUDE_COMMON)trace.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)trace.c -o $(DIR_BUILD)common_trace.o

common_version.o: $(DIR_SRC_COMMON)version.c $(DIR_INCLUDE_COMMON)version.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON)version.c -o $(DIR_BUILD)common_version.o

build_common_mapper: common_mapper_mbc1.o common_mapper_mbc2.o common_mapper_mbc3.o common_mapper_mbc5.o

common_mapper_mbc1.o: $(DIR_SRC_COMMON_MAPPER)mbc1.c $(DIR_INCLUDE_COMMON_MAPPER)mbc1.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON_MAPPER)mbc1.c -o $(DIR_BUILD)common_mapper_mbc1.o

common_mapper_mbc2.o: $(DIR_SRC_COMMON_MAPPER)mbc2.c $(DIR_INCLUDE_COMMON_MAPPER)mbc2.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON_MAPPER)mbc2.c -o $(DIR_BUILD)common_mapper_mbc2.o

common_mapper_mbc3.o: $(DIR_SRC_COMMON_MAPPER)mbc3.c $(DIR_INCLUDE_COMMON_MAPPER)mbc3.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON_MAPPER)mbc3.c -o $(DIR_BUILD)common_mapper_mbc3.o

common_mapper_mbc5.o: $(DIR_SRC_COMMON_MAPPER)mbc5.c $(DIR_INCLUDE_COMMON_MAPPER)mbc5.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_COMMON_MAPPER)mbc5.c -o $(DIR_BUILD)common_mapper_mbc5.o

build_service: service_sdl.o

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

build_system_processor: system_processor_instruction.o

system_processor_instruction.o: $(DIR_SRC_SYSTEM_PROCESSOR)instruction.c $(DIR_INCLUDE_SYSTEM_PROCESSOR)instruction.h
	$(CC) $(FLAGS) $(BUILD_FLAGS) -c $(DIR_SRC_SYSTEM_PROCESSOR)instruction.c -o $(DIR_BUILD)system_processor_instruction.o

archive:
	@echo ''
	@echo '--- BUILDING LIBRARY -----------------------'
	ar rcs $(DIR_BIN_LIB)$(LIB) $(DIR_BUILD)base_dmg.o $(DIR_BUILD)base_runtime.o $(DIR_BUILD)base_service.o \
		$(DIR_BUILD)common_bank.o $(DIR_BUILD)common_bootrom.o $(DIR_BUILD)common_buffer.o $(DIR_BUILD)common_cartridge.o \
			$(DIR_BUILD)common_error.o $(DIR_BUILD)common_mapper.o $(DIR_BUILD)common_mapper_mbc1.o $(DIR_BUILD)common_mapper_mbc2.o \
			$(DIR_BUILD)common_mapper_mbc3.o $(DIR_BUILD)common_mapper_mbc5.o $(DIR_BUILD)common_trace.o $(DIR_BUILD)common_version.o \
		$(DIR_BUILD)service_sdl.o \
		$(DIR_BUILD)system_audio.o $(DIR_BUILD)system_joypad.o $(DIR_BUILD)system_memory.o $(DIR_BUILD)system_processor.o \
			$(DIR_BUILD)system_serial.o $(DIR_BUILD)system_timer.o $(DIR_BUILD)system_video.o \
		$(DIR_BUILD)system_processor_instruction.o
	cp $(DIR_INCLUDE)dmg.h $(DIR_BIN_INCLUDE)
	cp $(DIR_DOCS)readme.txt $(DIR_BIN)
	@echo '--- DONE -----------------------------------'
	@echo ''
