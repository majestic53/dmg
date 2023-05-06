# SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
# SPDX-License-Identifier: MIT

TEST=$(shell find . -type d -not -path "." -not -path "*common*")

.PHONY: all
all:
	@for test in $(TEST); do \
		if [ -f $$test/Makefile ]; then \
			if ! make --no-print-directory -C $$test; then \
				exit 1; \
			fi \
		fi \
	done

.PHONY: clean
clean:
	@for test in $(TEST); do \
		if [ -f $$test/Makefile ]; then \
			make --no-print-directory -C $$test clean; \
		fi \
	done

.PHONY: run
run:
	@result=0; \
	for test in $(TEST); do \
		if [ -f $$test/Makefile ]; then \
			if ! make --no-print-directory -C $$test run; then \
				result=1; \
			fi; \
			make --no-print-directory -C $$test coverage; \
		fi \
	done; \
	if [ $$result -ne 0 ]; then \
		exit 1; \
	fi
