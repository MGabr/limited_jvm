/**
 * @file core.h
 * @author Markus Gabriel
 * @brief Core of the virtual machine
 *
 * The core of the virtual machine interpreter.
 * Contains most of the code for the instructions supported by the VM.
 * Uses indirect token threading.
 */
#ifndef CORE_H
#define CORE_H

#include "testmode.h"
#include "parser.h"

/**
 * Executes the code of the given method of the given class.
 *
 * @param c the class file whose (main) method should be executed
 * @param main the method whose code should be executed, will usually be the 
 * 			   main method, but can also be another method (e.g. for testing)
 */
void run(struct ClassFile *c, struct r_method_info *main);

/**
 * Executes the code of the main method of the given class.
 *
 * @param c the class file whose main method should be executed
 */
void run_main(struct ClassFile *c);

/**
 * Returns a reference to the main method of the given class.
 *
 * @detail class has to have a main method, otherwise the program is terminated
 *		   with an error message
 *
 * @param c the class file whose main method to get
 */
struct r_method_info *get_main_method(struct ClassFile *c);

#endif

