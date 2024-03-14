CC=gcc
CFLAGS=-Wall -std=c17 -g

.PHONY: clean

all: countdown
	@echo "Compiling everything"

countdown: countdown.c
	@echo "Compiling countdown"
	$(CC) $(CFLAGS) -o countdown countdown.c

clean:
	@echo "Removing all binary files"
	rm -f countdown


