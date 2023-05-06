# SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
# SPDX-License-Identifier: MIT

CC?=gcc
VERBOSE?=@

INCLUDES=$(subst $(ROOT)test/common,-I$(ROOT)test/common,$(shell find $(ROOT)test/common -type d)) \
	$(subst $(ROOT)include,-I$(ROOT)include,$(shell find $(ROOT)include -type d)) \
	$(subst $(ROOT)src,-I$(ROOT)src,$(shell find $(ROOT)src -type d)) \
	$(shell sdl2-config --cflags)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES) $(shell find . -maxdepth 1 -name "*.c"))

.PHONY: all
all: test_$(TARGET)

.PHONY: clean
clean:
	@rm -f test_$(TARGET) $(OBJECTS) $(ROOT)$(SOURCE)$(TARGET).o \
		$(TARGET).c.gcov $(ROOT)$(SOURCE)$(TARGET).gcda $(ROOT)$(SOURCE)$(TARGET).gcno

.PHONY: coverage
coverage:
	@gcov $(ROOT)$(SOURCE)$(TARGET).c

.PHONY: run
run:
	@if ! ./test_$(TARGET); then \
		exit 1; \
	fi

test_$(TARGET): $(OBJECTS) $(ROOT)$(SOURCE)$(TARGET).o
	$(VERBOSE)$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage $(ROOT)$(SOURCE)$(TARGET).o $(OBJECTS) -o $@

$(ROOT)$(SOURCE)$(TARGET).o: $(ROOT)$(SOURCE)$(TARGET).c
	$(VERBOSE)$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage $(INCLUDES) -c $< -o $@

%.o: %.c
	$(VERBOSE)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
