#ifndef CORE_H
#define CORE_H

#include "testmode.h"
#include "parser.h"

void run(struct ClassFile *c, struct r_method_info *main);

#ifdef _TESTMODE_
	struct r_method_info *get_main_method(struct ClassFile *c);
#endif

#endif

