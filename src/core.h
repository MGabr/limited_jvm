#ifndef CORE_H
#define CORE_H

#include "testmode.h"

#ifdef _TESTMODE_
	#include "parser.h"
	struct method_info *get_main_method(struct ClassFile *c);
#endif

#endif

