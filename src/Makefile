INCLUDES:=$(subst ../include,-I../include,$(shell find ../include -type d)) \
	$(subst .,-I.,$(shell find . -type d))
OBJECTS:=$(patsubst %.c,%.o,$(shell find . -name "*.c"))

.PHONY: all
all: $(OBJECTS)

.PHONY: clean
clean:
	@rm -f $(OBJECTS)

.PHONY: patch
patch:
	@sed -i "s/PATCH .*/PATCH 0x$(shell git rev-parse --short HEAD)/g" common.h

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(shell sdl2-config --cflags) -c $< -o $@
