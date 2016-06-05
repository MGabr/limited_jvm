#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "parser.h"
#include "options.h"
#include "core.h"
#include "string_pool.h"
#include "resolve.h"
#include "opcodes.h"

/* ======================= TESTMODE PART ========================= */

#include "testmode.h"

#ifdef _TESTMODE_
	#define NEXT() \
if (instrs_counter == 0) {\
	start_state.pc = pc;\
	start_state.cp = cp;\
	start_state.frame = frame;\
	start_state.optop = optop;\
} else if (instrs_counter == instrs_until_terminate) {\
	instrs_counter = 0;\
	state.pc = pc;\
	state.cp = cp;\
	state.frame = frame;\
	state.optop = optop;\
	return;\
}\
instrs_counter++;\
goto *table[*pc++];

	#define STATIC 
#endif

/* =============================================================== */


#ifndef _TESTMODE_
	#define NEXT() goto *table[*pc++];
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

static void *allocate_stack(void)
{
	void *stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, 
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (stack == MAP_FAILED) {
		fprintf(stderr, 
			"Can not run program: Could not allocate space for stack.\n");
	}
	return stack;
}

void run(struct ClassFile *c, u1 *startCode)
{
	u1 *pc; // program counter
	struct cp_info *cp; // constant pool
	void *frame; // current frame
	u4 *optop; // top of operand stack, u4 for 32 bit adresses

	// layout of a frame on the jvm stack:
	//
	// arg1	 <- frame
	// arg2
	// ...
	// argn
	// oldframe
	// oldpc
	// oldcp	<- optop (if no operand is on the operand stack)
	//

	pc = startCode;
	cp = c->constant_pool;
	frame = allocate_stack();
	optop = frame + 1; // one local variable before beginning of operand stack

	// TODO: put input String array in local variable

	void *table[255];

	table[LDC1] = &&ldc1;
	table[LDC2] = &&ldc2;
	table[LDC2W] = &&ldc2w;
	table[ALOAD] = &&aload;
	table[ALOAD_0] = &&aload_0;
	table[ALOAD_1] = &&aload_1;
	table[ALOAD_2] = &&aload_2;
	table[ALOAD_3] = &&aload_3;
	table[RETURN] = &&_return;
	table[INVOKENONVIRTUAL] = &&invokenonvirtual;
	table[INVOKESTATIC] = &&invokestatic;

	NEXT();

	ldc1:
		if (!IS_RESOLVED(cp, *pc)) {
			resolve_const(cp, *pc);
		}
		*++optop = (u4) cp[*pc].r_any_info.ptr;
		pc++;
		NEXT();
	ldc2:
		;
		u2 index = ((u2) *pc) << sizeof(u1) * 8 | *(pc + 1);
		if (!IS_RESOLVED(cp, index)) {
			resolve_const(cp, index);
		}
		*++optop = (u4) cp[index].r_any_info.ptr;
		pc += 2;
		NEXT();
	ldc2w:

	aload:
	aload_0:
	aload_1:
	aload_2:
	aload_3:

	_return:

	invokenonvirtual:
	invokestatic:

	return;
}

void run_main(struct ClassFile *c)
{
	run(c, get_main_method(c)->attributes[0].code_attribute.code);
}

