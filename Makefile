FLAGS:=-mtune=native\ -std=c11\ -Wall\ -Werror\ -Wextra
FLAGS_DEBUG:=CFLAGS=$(FLAGS)\ -g
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
	@make --no-print-directory -C src strip

.PHONY: clean
clean:
	@make --no-print-directory -C src clean

.PHONY: debug
debug: clean
	@make --no-print-directory -C src patch
	@make --no-print-directory -C src $(FLAGS_DEBUG) -j$(THREADS)
