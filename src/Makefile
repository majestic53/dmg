OBJECTS:=$(patsubst %.c,%.o,$(shell find . -name "*.c"))
TARGET:=../dmg

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
	@strip --strip-all -R .note -R .comment $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(shell sdl2-config --libs) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(subst .,-I.,$(shell find . -type d)) $(shell sdl2-config --cflags) -c $< -o $@
