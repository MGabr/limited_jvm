FLAGS=-Wall -m32 -I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread
LIBS=-I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread
GTEST_HOME=~/usr/gtest

all:
	make Tests

Tests: ParserTest.o StringPoolTest.o StringPoolConflictTest.o MainTest.o LdcTest.o SignatureTest.o ResolveTest.o InvokestaticTest.o NativeTest.o HelloWorldTest.o IntegerTest.o IntegerNegativeTest.o IntegerLogicalTest.o ControlTest.o CompareControlTest.o FloatTest.o DoubleTest.o LongTest.o CastTest.o ReturnTest.o CompareTest.o FactorialTest.o PrintPrimitivesTest.o StaticTest.o StaticDoubleTest.o ArrayTest.o PrimitivesArrayTest.o SwitchPrintTest.o ../src/parser.o ../src/string_pool.o LongLogicalTest.o ClassLoadingTest.o ../src/options.o ../src/core.o ../src/resolve.o ../src/testmode.o ../src/signature.o ../src/native.o ../src/java.o ../src/warn.o ../src/opcode_strings.o
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

LongTest.o: LongTest.class
	g++ ${FLAGS} -c LongTest.cpp ${LIBS}

LongTest.class:
	cd java && javac LongTest.java && mv $@ ../classes/.

CastTest.o: CastTest.class
	g++ ${FLAGS} -c CastTest.cpp ${LIBS}

CastTest.class:
	cd java && javac CastTest.java && mv $@ ../classes/.

ReturnTest.o: ReturnTest.class
	g++ ${FLAGS} -c ReturnTest.cpp ${LIBS}

ReturnTest.class:
	cd java && javac ReturnTest.java && mv $@ ../classes/.

CompareTest.o: CompareTest.class
	g++ ${FLAGS} -c CompareTest.cpp ${LIBS}

CompareTest.class:
	cd java && javac CompareTest.java && mv $@ ../classes/.

LongLogicalTest.o: LongLogicalTest.class
	g++ ${FLAGS} -c LongLogicalTest.cpp ${LIBS}

LongLogicalTest.class:
	cd java && javac LongLogicalTest.java && mv $@ ../classes/.

ClassLoadingTest.o: ClassLoadingTest.class ClassLoadingTest2.class ClassLoadingTest3.class
	g++ ${FLAGS} -c ClassLoadingTest.cpp ${LIBS}

ClassLoadingTest.class:
	cd java && javac ClassLoadingTest.java && mv $@ ../classes/.

ClassLoadingTest2.class:
	cd java && javac ClassLoadingTest2.java && mv $@ ../classes/.

ClassLoadingTest3.class:
	cd java/apackage && javac ClassLoadingTest3.java && mv $@ ../../classes/apackage/.

../src/opcode_strings.o:
	cd ../src && make opcode_strings.o

FactorialTest.o: FactorialTest.class
	g++ ${FLAGS} -c FactorialTest.cpp ${LIBS}

FactorialTest.class:
	cd java && javac FactorialTest.java && mv $@ ../classes/.

PrintPrimitivesTest.o: PrintPrimitivesTest.class
	g++ ${FLAGS} -c PrintPrimitivesTest.cpp ${LIBS}

PrintPrimitivesTest.class:
	cd java && javac PrintPrimitivesTest.java && mv $@ ../classes/.

StaticTest.o: StaticTest.class StaticTest2.class StaticTest3.class
	g++ ${FLAGS} -c StaticTest.cpp ${LIBS}

StaticTest.class:
	cd java && javac StaticTest.java && mv $@ ../classes/.

StaticTest2.class:
	cd java && javac StaticTest2.java && mv $@ ../classes/.

StaticTest3.class:
	cd java/apackage && javac StaticTest3.java && mv $@ ../../classes/apackage/.

StaticDoubleTest.o: StaticDoubleTest.class StaticDoubleTest2.class StaticDoubleTest3.class
	g++ ${FLAGS} -c StaticDoubleTest.cpp ${LIBS}

StaticDoubleTest.class:
	cd java && javac StaticDoubleTest.java && mv $@ ../classes/.

StaticDoubleTest2.class:
	cd java && javac StaticDoubleTest2.java && mv $@ ../classes/.

StaticDoubleTest3.class:
	cd java/apackage && javac StaticDoubleTest3.java && mv $@ ../../classes/apackage/.

ArrayTest.o: ArrayTest.class
	g++ ${FLAGS} -c ArrayTest.cpp ${LIBS}

ArrayTest.class:
	cd java && javac ArrayTest.java && mv $@ ../classes/.

PrimitivesArrayTest.o: PrimitivesArrayTest.class
	g++ ${FLAGS} -c PrimitivesArrayTest.cpp ${LIBS}

PrimitivesArrayTest.class:
	cd java && javac PrimitivesArrayTest.java && mv $@ ../classes/.

SwitchPrintTest.o: SwitchPrintTest.class
	g++ ${FLAGS} -c SwitchPrintTest.cpp ${LIBS}

SwitchPrintTest.class:
	cd java && javac SwitchPrintTest.java && mv $@ ../classes/.

clean:
	rm classes/*.class classes/apackage/*.class Tests ParserTest.o StringPoolTest.o StringPoolConflictTest.o MainTest.o LdcTest.o SignatureTest.o ResolveTest.o InvokestaticTest.o NativeTest.o HelloWorldTest.o IntegerTest.o IntegerNegativeTest.o IntegerLogicalTest.o ControlTest.o CompareControlTest.o FloatTest.o DoubleTest.o LongTest.o CastTest.o ReturnTest.o CompareTest.o LongLogicalTest.o ClassLoadingTest.o FactorialTest.o PrintPrimitivesTest.o StaticTest.o StaticDoubleTest.o ArrayTest.o PrimitivesArrayTest.o SwitchPrintTest.o ../src/parser.o ../src/string_pool.o ../src/options.o ../src/core.o ../src/resolve.o ../src/testmode.o ../src/signature.o ../src/native.o ../src/java.o ../src/warn.o ../src/opcode_strings.o

