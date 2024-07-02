CC            = gcc
#CFLAGS       = -O1 -I /usr/irissys/dev/iris-callin/include
CFLAGS        = -g3 -ggdb -O3 -I /usr/irissys/dev/iris-callin/include
LIBS          = -lodbc
PROGRAM       = test 

all:            $(PROGRAM)

test: test.o
	$(CC) test.o -o test $(LIBS) 

test.o:	test.c
	$(CC) test.c $(CFLAGS) -c -o test.o

clean:
	rm -f *.o *~ $(PROGRAM) 
