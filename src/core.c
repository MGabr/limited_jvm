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
#include "native.h"

/* ======================= TESTMODE PART ========================= */

#include "testmode.h"

#ifdef _TESTMODE_
	#define NEXT() \
if (instrs_counter == 0) {\
	start_state.pc = pc;\
	start_state.c = c;\
	start_state.cp = cp;\
	start_state.frame = frame;\
	start_state.optop = optop;\
	start_state.localc = localc;\
} else if (instrs_counter == instrs_until_terminate) {\
	instrs_counter = 0;\
	state.pc = pc;\
	state.c = c;\
	state.cp = cp;\
	state.frame = frame;\
	state.optop = optop;\
	state.localc = localc;\
	return;\
}\
instrs_counter++;\
goto *table[*pc++];
#endif

/* =============================================================== */


#ifndef _TESTMODE_
	#define NEXT() goto *table[*pc++];
#endif

#define TWO_BYTE_INDEX(pc) (((u2) *pc) << sizeof(u1) * 8 | *(pc + 1))


struct r_method_info *get_main_method(struct ClassFile *c)
{
	int i;
	struct r_method_info *mi;

	const char *main_str = find_string("main");
	const char *signature_str = find_string("([Ljava/lang/String;)V");
	if (main_str == NULL || signature_str == NULL) {
		fprintf(stderr, "Can not run program: No main method found (no main or signature string in string pool).\n");
		exit(1);
	}

	for (i = 0; i < c->methods_count; i++) {
		mi = &c->methods[i];
		if (mi->name == main_str && mi->signature == signature_str) {
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


#define GET_OLDLOCALC(frame, localc) (*((u4 *) frame + localc))
#define GET_OLDFRAME(frame, localc) (*((u4 **) frame + localc + 1))
#define GET_OLDPC(frame, localc) (*((u1 **) frame + localc + 2))
#define GET_OLDC(frame, localc) (*((struct ClassFile **) frame + localc + 3))

void run(struct ClassFile *c, struct r_method_info *main)
{
	//  ------------ 'registers' and initialization of vm ------------

	u1 *pc; // program counter
	struct cp_info *cp; // constant pool
	u4 *frame; // current frame
	u4 *optop; // top of operand stack, u4 for 32 bit adresses

	u4 localc; // local variable count

	// layout of a frame on the jvm stack:
	//
	// local1/arg1	<- frame
	// local2/arg2
	// ...
	// localn/argn
	// localn+1
	// ...
	// localm
	// oldlocalc	(<- frame + localc)
	// oldframe 
	// oldpc
	// oldc	<- optop (if no operand is on the operand stack)
	//

	pc = main->c_attr->code;
	cp = c->constant_pool;
	frame = allocate_stack();
	optop = frame + 4;

	localc = main->c_attr->max_locals;

	// TODO: put input String array in local variable
	// set oldpc to pointer to opcode used for exiting vm
	u1 impdep1_store = IMPDEP1;
	*((u1 **) frame + localc + 2) = &impdep1_store;
	// set oldc to current class 
	// (to prevent segmentation fault when receiving c->constant_pool in last
	// return)
	*((struct ClassFile **) frame + localc + 3) = c;
	// other old values are not needed here


	//  ------------ opcode table -----------------------------------

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
	table[IMPDEP1] = &&impdep1;

	
	// initialize set of predefined native method
	init_natives();

	// ------------- execute instructions ---------------------------

	NEXT();

	// some local variables used in the goto blocks
	// NOT used for side effects over different goto blocks
	u2 index;
	u4 *tmp_frame;

	ldc1:
		if (!IS_RESOLVED(cp, *pc)) {
			resolve_const(c, *pc);
		}
		*++optop = (u4) cp[*pc].r_any_info.ptr;
		pc++;
		NEXT();
	ldc2:
		index = TWO_BYTE_INDEX(pc);
		if (!IS_RESOLVED(cp, index)) {
			resolve_const(c, index);
		}
		*++optop = (u4) cp[index].r_any_info.ptr;
		pc += 2;
		NEXT();
	ldc2w:
		// TODO: TEST
		index = TWO_BYTE_INDEX(pc);
		*++optop = cp[index].longOrDouble_info.high_bytes;
		*++optop = cp[index].longOrDouble_info.low_bytes;
		pc += 2;
		NEXT();
	aload:
	aload_0:
	aload_1:
	aload_2:
	aload_3:

	_return:
		tmp_frame = frame;

		frame = GET_OLDFRAME(tmp_frame, localc);
		pc = GET_OLDPC(tmp_frame, localc);
		c = GET_OLDC(tmp_frame, localc);
		cp = c->constant_pool;

		localc = GET_OLDLOCALC(tmp_frame, localc);
		optop = tmp_frame - 1;

		NEXT();
	invokenonvirtual:
	invokestatic:
		index = TWO_BYTE_INDEX(pc);
		pc += 2;
		if (!IS_RESOLVED(cp, index)) {
			resolve_methodref(c, index);
		}
		struct r_methodref_info *m = &cp[index].r_methodref_info;
		struct r_method_info *m_block = m->r_method;
		struct Code_attribute *c_attr = m_block->c_attr;

		if (IS_NATIVE(m_block)) {
			call_native(m_block, optop);
			NEXT();
		}

		tmp_frame = optop + 1 - m_block->nargs;

		optop += c_attr->max_locals - m_block->nargs; // reuse arguments
		*++optop = (u4) localc;
		*++optop = (u4) frame; 
		*++optop = (u4) pc; 
		*++optop = (u4) c; 

		pc = c_attr->code; 
		c = m->r_class;
		cp = c->constant_pool;		
		frame = tmp_frame;
		localc = c_attr->max_locals;	

		NEXT();

	impdep1:
		// opcode reserved for implementation dependent debugger operations
		// we use this as simple exit
		return;

}

void run_main(struct ClassFile *c)
{
	run(c, get_main_method(c));
}

