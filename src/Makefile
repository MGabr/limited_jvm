FLAGS=-Wall

all:
	make parser.o
	make string_pool.o

parser.o:
	gcc ${FLAGS} -c parser.c

string_pool.o:
	gcc ${FLAGS} -c string_pool.c

options.o:
	gcc ${FLAGS} -c options.c

clean:
	rm parser.o string_pool.o options.o

