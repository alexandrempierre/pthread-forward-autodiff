CC := gcc

all: clean function.o main.c
	$(CC) function.o main.c -o main.out -Wall --pedantic -lm -lrt -lpthread -g

function.o: function.c
	$(CC) $^ -o $@ -c -Wall --pedantic -std=c99 -lm -lrt -g

clean:
	find . -maxdepth 1 -name "*.o" -delete
	find . -maxdepth 1 -name "*.out" -delete