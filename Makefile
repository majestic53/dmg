FLAGS:=-mtune=native\ -std=c11\ -Wall\ -Werror\ -Wextra\ -Wno-unused-parameter
FLAGS_DEBUG:=CFLAGS=$(FLAGS)\ -g3\ -fsanitize=address,undefined
FLAGS_RELEASE:=CFLAGS=$(FLAGS)\ -O3

ifeq ($(shell uname -s),Linux)
THREADS?=$(shell grep -c ^processor /proc/cpuinfo)
else
THREADS?=1
endif

.PHONY: all
all: clean
	@make --no-print-directory -C src patch
	@make --no-print-directory -C src $(FLAGS_RELEASE) -j$(THREADS)
	@make --no-print-directory -C tool $(FLAGS_RELEASE) -j$(THREADS)
	@make --no-print-directory -C tool strip

.PHONY: clean
clean:
	@make --no-print-directory -C src clean
	@make --no-print-directory -C tool clean

.PHONY: debug
debug: clean
	@make --no-print-directory -C src patch
	@make --no-print-directory -C src $(FLAGS_DEBUG) -j$(THREADS)
	@make --no-print-directory -C tool $(FLAGS_DEBUG) -j$(THREADS)
