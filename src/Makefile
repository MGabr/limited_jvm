FLAGS=-Wall -m32
LIBS=-lm

all:
	make java

java: parser.o string_pool.o options.o testmode.o core.o resolve.o signature.o native.o java.o warn.o opcode_strings.o
	gcc ${FLAGS} -o $@ $^ ${LIBS}

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

signature.o:
	gcc ${FLAGS} -c signature.c

native.o:
	gcc ${FLAGS} -c native.c

java.o:
	gcc ${FLAGS} -c java.c

warn.o:
	gcc ${FLAGS} -c warn.c

opcode_strings.o:
	gcc ${FLAGS} -c opcode_strings.c

clean:
	rm parser.o string_pool.o options.o core.o testmode.o resolve.o signature.o native.o java.o warn.o opcode_strings.o java

