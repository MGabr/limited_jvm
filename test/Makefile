FLAGS=-Wall -m32 -I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread
LIBS=-I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread
GTEST_HOME=~/usr/gtest

all:
	make Tests

Tests: ParserTest.o StringPoolTest.o StringPoolConflictTest.o MainTest.o LdcTest.o SignatureTest.o ResolveTest.o InvokestaticTest.o NativeTest.o HelloWorldTest.o IntegerTest.o IntegerNegativeTest.o IntegerLogicalTest.o ControlTest.o CompareControlTest.o FloatTest.o DoubleTest.o ../src/parser.o ../src/string_pool.o ../src/options.o ../src/core.o ../src/resolve.o ../src/testmode.o ../src/signature.o ../src/native.o ../src/java.o ../src/warn.o
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

../src/options.o:
	cd ../src && make options.o

MainTest.o: HelloWorld.class
	g++ ${FLAGS} -c MainTest.cpp ${LIBS}

../src/core.o:
	cd ../src && make core.o

../src/resolve.o:
	cd ../src && make resolve.o

LdcTest.o: HelloWorld.class
	g++ ${FLAGS} -c LdcTest.cpp ${LIBS}

../src/testmode.o:
	cd ../src && make testmode.o

SignatureTest.o:
	g++ ${FLAGS} -c SignatureTest.cpp ${LIBS}

../src/signature.o:
	cd ../src && make signature.o

ResolveTest.o:
	g++ ${FLAGS} -c ResolveTest.cpp ${LIBS}

HelloWorld.class:
	cd java && javac HelloWorld.java && mv $@ ../classes/.

InvokestaticTest.o: InvokestaticTest.class
	g++ ${FLAGS} -c InvokestaticTest.cpp ${LIBS}

InvokestaticTest.class:
	cd java && javac InvokestaticTest.java && mv $@ ../classes/.

NativeTest.o: HelloWorld.class
	g++ ${FLAGS} -c NativeTest.cpp ${LIBS}

../src/native.o:
	cd ../src && make native.o

HelloWorldTest.o: HelloWorld.class
	g++ ${FLAGS} -c HelloWorldTest.cpp ${LIBS}

../src/java.o:
	cd ../src && make java.o

IntegerTest.o: IntegerTest.class
	g++ ${FLAGS} -c IntegerTest.cpp ${LIBS}

IntegerTest.class:
	cd java && javac IntegerTest.java && mv $@ ../classes/.

IntegerNegativeTest.o: IntegerNegativeTest.class
	g++ ${FLAGS} -c IntegerNegativeTest.cpp ${LIBS}

IntegerNegativeTest.class:
	cd java && javac IntegerNegativeTest.java && mv $@ ../classes/.

IntegerLogicalTest.o: IntegerLogicalTest.class
	g++ ${FLAGS} -c IntegerLogicalTest.cpp ${LIBS}

IntegerLogicalTest.class:
	cd java && javac IntegerLogicalTest.java && mv $@ ../classes/.

ControlTest.o: ControlTest.class
	g++ ${FLAGS} -c ControlTest.cpp ${LIBS}

ControlTest.class:
	cd java && javac ControlTest.java && mv $@ ../classes/.

CompareControlTest.o: CompareControlTest.class
	g++ ${FLAGS} -c CompareControlTest.cpp ${LIBS}

CompareControlTest.class:
	cd java && javac CompareControlTest.java && mv $@ ../classes/.

FloatTest.o: FloatTest.class
	g++ ${FLAGS} -c FloatTest.cpp ${LIBS}

FloatTest.class:
	cd java && javac FloatTest.java && mv $@ ../classes/.

../src/warn.o:
	cd ../src && make warn.o

DoubleTest.o: DoubleTest.class
	g++ ${FLAGS} -c DoubleTest.cpp ${LIBS}

DoubleTest.class:
	cd java && javac DoubleTest.java && mv $@ ../classes/.

clean:
	rm classes/* Tests ParserTest.o StringPoolTest.o StringPoolConflictTest.o MainTest.o LdcTest.o SignatureTest.o ResolveTest.o InvokestaticTest.o NativeTest.o HelloWorldTest.o IntegerTest.o IntegerNegativeTest.o IntegerLogicalTest.o ControlTest.o CompareControlTest.o FloatTest.o DoubleTest.o ../src/parser.o ../src/string_pool.o ../src/options.o ../src/core.o ../src/resolve.o ../src/testmode.o ../src/signature.o ../src/native.o ../src/java.o

