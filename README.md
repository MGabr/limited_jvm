# Limited JVM

An implementation of a limited subset of the JVM for the "SS2016 Abstrake Maschinen" course at TU Vienna.  

## Features

### What is supported

All primitive arithmetic and logical operations.  
All comparisons and control flow instructions.  
All primitive type, single-dimension array operations.  
Primitive type casts.  
Static functions and fields including loading of the required classes.  
Native println instructions for all primitive types and Strings (e.g. `private static native void println(int n);`)  


### What is not supported

Objects =(  
Interfaces  
Exceptions  
Garbage collection  
All other native methods such as reading from stdin.  
Other java language features added later such as annotations, generics, etc. in case they rely on unsupported or additional bytecode instructions.  


### What is not fully supported

Portability: The JVM was only tested on a 64-bit little endian computer running Ubuntu (compiled as 32-bit binary with the -m32 flag). Other architectures or operating systems might have some problems.  
Full standard compatibility: The results of floating point operations may not always conform to the guarantees given by the JVM standard. At other points there might also be deviations from the standard, especially in edge cases.  

A complete list of all supported bytecode instructions can be found in opcodes.h in the src folder.  


## How to use

### Compiling the JVM

To compile the JVM simply run `make` in the src folder.  
This will require a c compiler that supports GNU labels as values.  
In addition the -m32 flag which allows compiling the program to a 32-bit binary on a 64-bit computer has to be supported.  
To support the -m32 flag, the 32 bit version of libc has to be installed, this can be done on Debian or Ubuntu by:
```
sudo apt-get install gcc-multilib
sudo apt-get install g++-multilib
```
The later command is required for the tests.  

In settings.h you can customise the log output of the created JVM binary, but in most cases you will want to use the standard settings with `LOG_LEVEL` set to `LOG_LEVEL_ERROR` and `_LOG_INSTRS_` as well as `_TESTMODE_` commented out.  


### Running java class files

After you compiled the JVM, you have a binary named `java`.  
To run java class files move this binary and run `./java` in the root of your class folder structure (e.g. in the folder above "org" if your main class is in the package/folder "org.example.test") followed by the relative path and name of the main class (e.g. "org/example/test/MainClass" in the previous case if the main class is named "MainClass").  

In the folder manual_tests, there are some example java files whose compiled class file can be run with the limited JVM.  


### Running the tests

Install the required googletest framework as shared library:
```
wget https://github.com/google/googletest/archive/release-1.7.0.tar.gz
tar xf release-1.7.0.tar.gz
cd googletest-release-1.7.0
cmake -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DBUILD_SHARED_LIBS=ON
make
sudo cp -a include/gtest /usr/include
sudo cp -a libgtest_main.so libgtest.so /usr/lib/
```
(See also http://stackoverflow.com/a/13513907)

Undo the outcommented definition of `_TESTMODE_` in settings.h.  
Run `make` (or `make clean all` in case you already compiled the JVM before) in the test folder.  
Run `./Tests`.  


