CC=gcc
CFLAGS=-I.

myprogram: main.o utility.o
    $(CC) -o myprogram main.o utility.o

main.o: main.c
    $(CC) -c main.c $(CFLAGS)

utility.o: utility.c utility.h
    $(CC) -c utility.c $(CFLAGS)

clean:
    rm -f *.o myprogram

