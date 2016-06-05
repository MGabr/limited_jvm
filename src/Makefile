FLAGS=-Wall -m32

all:
	make parser.o
	make string_pool.o
	make options.o
	make testmode.o
	make core.o
	make resolve.o

parser.o:
	gcc ${FLAGS} -c parser.c

string_pool.o:
	gcc ${FLAGS} -c string_pool.c

options.o:
	gcc ${FLAGS} -c options.c

core.o:
	gcc ${FLAGS} -c core.c

testmode.o:
	gcc ${FLAGS} -c testmode.c

resolve.o:
	gcc ${FLAGS} -c resolve.c

clean:
	rm parser.o string_pool.o options.o core.o testmode.o resolve.o

