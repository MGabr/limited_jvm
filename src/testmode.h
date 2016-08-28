/**
 * @file testmode.h
 * @author Markus Gabriel
 * @brief Definitions for testing
 *
 * Definition of global variables which can be used by testing code and which
 * should be set by the code to test if it wants to offer this testing 
 * interface.
 */
#ifndef TESTMODE_H
#define TESTMODE_H

#include "parser.h"
#include "settings.h" // for _TESTMODE_ constant

struct state {
	u1 *pc;
	struct ClassFile *c;
	struct cp_info *cp;
	u4 *frame;
	u4 *optop;
	u4 localc;
};

/**
 * The global variable in which the start state of the vm is saved.
 *
 * @detail has to be set by the code to test
 */
extern struct state start_state;

/**
 * The global variable in which the state of the vm is saved.
 *
 * @detail has to be set by the code to test
 */
extern struct state state;

/**
 * Specifies the number of instructions after which the test function will be 
 * called. If this is set to 0 (= uninitialized), the vm is not terminated 
 * prematurely.
 *
 * @detail has to be set by the testing code
 */
extern int instrs_until_terminate;

/**
 * Counter to count the number of instructions that have been executed.
 * DO NOT change this counter.
 */
extern int instrs_counter;

/**
 * Initializes the test mode.
 *
 * @detail has to be set by the code to test
 */
void init_testmode(void);

#endif

