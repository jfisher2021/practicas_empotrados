#crete make file for practica1

(CC) = gcc

(CFLAGS) = -Wall -g

all: practica1

practica1: practica1.o
	$(CC) $(CFLAGS) -o practica1 practica1.o

practica1.o: practica1.c
	$(CC) $(CFLAGS) -c practica1.c

clean:
	rm -f *.o practica1

