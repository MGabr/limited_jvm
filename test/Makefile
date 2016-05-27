FLAGS=-Wall -I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread
LIBS=-I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread
GTEST_HOME=~/usr/gtest

all:
	make ParserTest

ParserTest: HelloWorld.class
	cd ../src && make parser.o
	g++ ${FLAGS} -o ParserTest ParserTest.cpp ../src/parser.o ${LIBS}

HelloWorld.class:
	cd java && javac HelloWorld.java && mv HelloWorld.class ../classes/.

clean:
	rm classes/*
	rm ParserTest

