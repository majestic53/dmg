# SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
# SPDX-License-Identifier: MIT

CC?=gcc
VERBOSE?=@

INCLUDES:=$(subst ../include,-I../include,$(shell find ../include -type d)) \
	$(subst .,-I.,$(shell find . -type d))
LIBRARIES:=$(subst ..,-L..,$(shell find .. -maxdepth 0 -type d)) -ldmg \
	$(shell sdl2-config --libs)
OBJECTS:=$(patsubst %.c,%.o,$(shell find . -name "*.c"))
TARGET:=../dmg

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	@rm -f $(OBJECTS) $(TARGET)

.PHONY: strip
strip:
	@strip --strip-all -R .note -R .comment $(TARGET)

$(TARGET): $(OBJECTS)
	$(VERBOSE)$(CC) $(CFLAGS) $(OBJECTS) $(LIBRARIES) -o $@

%.o: %.c
	$(VERBOSE)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
