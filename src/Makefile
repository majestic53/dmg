# SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
# SPDX-License-Identifier: MIT

AR?=ar
CC?=cc

INCLUDES:=$(subst ../include,-I../include,$(shell find ../include -type d)) \
	$(subst .,-I.,$(shell find . -type d)) \
	$(shell sdl2-config --cflags)
OBJECTS:=$(patsubst %.c,%.o,$(shell find . -name "*.c"))
TARGET:=../libdmg.a

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	@rm -f $(OBJECTS) $(TARGET)

.PHONY: patch
patch:
	@sed -i "s/PATCH .*/PATCH 0x$(shell git rev-parse --short HEAD)/g" common.h

.PHONY: strip
strip:
	@strip --strip-unneeded -R .note -R .comment $(TARGET)

$(TARGET): $(OBJECTS)
	@$(AR) -rcs $@ $(OBJECTS)

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
