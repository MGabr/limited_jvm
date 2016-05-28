#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "options.h"
#include "core.h"
#include "string_pool.h"

#include "testmode.h"

#ifdef _TESTMODE_
	#define STATIC 
#else
	#define STATIC static
#endif


STATIC struct method_info *get_main_method(struct ClassFile *c)
{
	struct cp_info *cp = c->constant_pool;
	int i;
	struct method_info *mi;

	const char *main_str = find_string("main");

	for (i = 0; i < c->methods_count; i++) {
		mi = &c->methods[i];
		if (cp[mi->name_index].r_utf8_info.str == main_str 
			&& !strcmp(cp[mi->signature_index].r_utf8_info.str,
				"([Ljava/lang/String;)V")) {
			break;
		}
	}

	if (mi == NULL) {
		fprintf(stderr, "Can not run program: No main method found.\n");
		exit(1);
	}

	return mi;
}

