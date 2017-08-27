FLAGS=-Wall -Wextra -Wpedantic -Wwrite-strings -Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline

zergmap: zergmap.c math_functions.o
	gcc $(CFLAGS) -o zergmap zergmap.c math_functions.o -lm

math_functions.o: math_functions.c
	gcc $(CFLGS) $< -c -o math_functions.o -lm

debug: CFLAGS += -g
debug: all

profile: CFLAGS += -pg
profile: all

clean:
	rm -f zergmap
