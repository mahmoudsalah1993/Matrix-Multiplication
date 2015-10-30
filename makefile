CC=gcc

CFLAGS = -pthread

finish: main.o
	$(CC) $(CFLAGS) main.o -o matMultp

main: main.c matrix_library.h
	$(CC) $(CFLAGS) main.c -o main.o

clean:
	rm  main.o matMultp
