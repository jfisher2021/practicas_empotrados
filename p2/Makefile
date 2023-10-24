(CC) = gcc

(CFLAGS) = -I. -Wall -g -lpthread

all: cyclictestURJC

cyclictestURJC: cyclictestURJC.o
	$(CC) $(CFLAGS) -o cyclictestURJC cyclictestURJC.o

cyclictestURJC.o: cyclictestURJC.c
	$(CC) $(CFLAGS) -c cyclictestURJC.c

clean:
	rm -f *.o cyclictestURJC

