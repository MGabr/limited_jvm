FLAGS=-Wall -I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread
LIBS=-I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread
GTEST_HOME=~/usr/gtest

all:
	make Tests

Tests: ParserTest.o StringPoolTest.o StringPoolConflictTest.o MainTest.o ../src/parser.o ../src/string_pool.o ../src/options.o ../src/core.o
	make ParserTest.o
	make StringPoolTest.o
	make StringPoolConflictTest.o
	g++ ${FLAGS} -o $@ Tests.cpp $^ ${LIBS}

ParserTest.o: HelloWorld.class
	g++ ${FLAGS} -c ParserTest.cpp ${LIBS}

../src/parser.o:
	cd ../src && make parser.o

StringPoolTest.o:
	g++ ${FLAGS} -c StringPoolTest.cpp ${LIBS}

StringPoolConflictTest.o:
	g++ ${FLAGS} -c StringPoolConflictTest.cpp ${LIBS}

../src/string_pool.o:
	cd ../src && make string_pool.o

../src/optons.o:
	cd ../src && make options.o

MainTest.o:
	g++ ${FLAGS} -c MainTest.cpp ${LIBS}

../src/core.o:
	cd ../src && make core.o

HelloWorld.class:
	cd java && javac HelloWorld.java && mv $@ ../classes/.


clean:
	rm classes/* Tests ParserTest.o StringPoolTest.o StringPoolConflictTest.o MainTest.o ../src/parser.o ../src/string_pool.o ../src/options.o

