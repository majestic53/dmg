CC?=gcc

INCLUDES:=$(subst ../include,-I../include,$(shell find ../include -type d)) \
	$(subst .,-I.,$(shell find . -type d))
OBJECTS:=$(patsubst %.c,%.o,$(shell find . -name "*.c")) \
	$(shell find ../src -name "*.o")
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
	@$(CC) $(CFLAGS) $(OBJECTS) $(shell sdl2-config --libs) -o $@

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
