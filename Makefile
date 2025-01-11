CC = gcc
CFLAGS = -Wall -Wextra -I./app
LDFLAGS = -lcheck -lm -lsubunit
SRC = app/args.c app/env_path.c

check:
	$(CC) tests/*.c $(SRC) $(CFLAGS) $(LDFLAGS) -o test_runner
	./test_runner
	rm -f test_runner

.PHONY: check
