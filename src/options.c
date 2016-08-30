/**
 * @file options.c
 * @author Markus Gabriel
 * @brief Default values for VM options
 *
 * Default values for the VM options specified in the options.h header file.
 */
#include <stdlib.h>

#include "options.h"

long stack_size = 128 * 1024;
int nr_string_buckets = 1009;
const char *classpaths[] = {"classes", NULL};

