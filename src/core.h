#ifndef CORE_H
#define CORE_H

#include "testmode.h"
#include "parser.h"

void run(struct ClassFile *c, u1 *startCode);

#ifdef _TESTMODE_
	struct method_info *get_main_method(struct ClassFile *c);
#endif

#endif

