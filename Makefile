CC            = gcc
#CFLAGS       = -O1 -finput-charset=utf-8 -fexec-charset=utf-8
CFLAGS        = -g3 -ggdb -O3 -finput-charset=utf-8 -fexec-charset=utf-8
LIBS          = -lodbc
PROGRAM       = test 

all:            $(PROGRAM)

test: test.o
	$(CC) test.o -o test $(LIBS) 

test.o:	test.c
	$(CC) test.c $(CFLAGS) -c -o test.o

clean:
	rm -f *.o *~ $(PROGRAM) 
