#ifndef TESTMODE_H
#define TESTMODE_H

/**
 * If _TESTMODE_ is defined, the vm regulary hands over control to a test_hook.
 * Comment this out during normal execution of the vm.
 */
#define _TESTMODE_

struct state {
	u1 *pc;
	struct cp_info *cp;
	void *frame;
	u1 *optop;
};

/**
 * A test function that gets called after a certain number of instructions has
 * been executed.
 *
 * @detail instrs_until_test_hook specify the number of instructions after which
 *	   the test function will be called.
 *
 * @param the current state of the vm
 */
extern void (*test_hook)(struct state *);

/**
 * Specifies the number of instructions after which the test function will be 
 * called
 */
extern int instrs_until_test_hook;

/**
 * Counter to count the number of instructions that have been executed.
 * DO NOT change this counter.
 */
extern int instrs_counter;

#endif

