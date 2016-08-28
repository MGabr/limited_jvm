/**
 * @file native.h
 * @author Markus Gabriel
 * @brief Interface for native method calls
 *
 * Interface and implementation of custom native method calls.
 * Does not support JNI currently.
 */
#ifndef NATIVE_H
#define NATIVE_H

#include "parser.h"

/**
 * Initializes the native methods.
 *
 * @detail Since this vm currently does not support JNI, only a set of 
 * 		   predefined native methods is supported:
 *			* void println(String s)
 */
extern void init_natives(void);

/**
 * Calls a native method.
 * 
 * @detail Only supports the methods set in init_natives
 *
 * @param method the native method to call
 * @param optop the pointer to the top of the operand stack
 *		  (where the method parameters are located)
 */
extern void call_native(struct r_method_info *method, u4 *optop);

#endif
