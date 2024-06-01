.PHONY: all
all: fold

.PHONY: clean
clean:
	rm -f fold

fold: 1-fold.c
	cc -Wall -Wextra -pedantic -Werror -O2 -o $@ $^
