#ifndef TESTMODE_H
#define TESTMODE_H

#include "parser.h"

/**
 * If _TESTMODE_ is defined, the vm terminates after a specified number of 
 * instructions and saves its state in a global variable.
 * Comment this out during normal execution of the vm.
 */
#define _TESTMODE_

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
 */
extern struct state start_state;

/**
 * The global variable in which the state of the vm is saved.
 */
extern struct state state;

/**
 * Specifies the number of instructions after which the test function will be 
 * called. If this is set to 0 (= uninitialized), the vm is not terminated 
 * prematurely.
 */
extern int instrs_until_terminate;

/**
 * Counter to count the number of instructions that have been executed.
 * DO NOT change this counter.
 */
extern int instrs_counter;

#endif

