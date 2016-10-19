#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <math.h>
#include <fenv.h>
#include <errno.h>

#include "parser.h"
#include "options.h"
#include "core.h"
#include "string_pool.h"
#include "resolve.h"
#include "opcodes.h"
#include "native.h"
#include "log.h"
#include "opcode_strings.h"
#include "settings.h"

#ifdef _LOG_INSTRS_
	#define LOG_INSTR fprintf(stderr, "- %s\n", opcode_strings[*pc])
#else
	#define LOG_INSTR 
#endif

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
	instrs_until_terminate = 0;\
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
LOG_INSTR;\
goto *table[*pc++];

	#define NEXT_AFTER_INITIALIZE_STATIC() \
if (instrs_counter == 0) {\
	NEXT();\
} else {\
	goto *table[*pc++];\
}
#endif

/* =============================================================== */


#ifndef _TESTMODE_
	#define NEXT() LOG_INSTR;\
goto *table[*pc++];
    #define NEXT_AFTER_INITIALIZE_STATIC() NEXT();
#endif


struct r_method_info *get_main_method(struct ClassFile *c)
{
	DEBUG("Entered %s\n", __func__);

	int i;
	struct r_method_info *mi;

	const char *main_str = find_string("main");
	const char *signature_str = find_string("([Ljava/lang/String;)V");
	if (main_str == NULL || signature_str == NULL) {
		ERROR("Can not run program: No main method found (no main or signature string in string pool).\n");
		exit(1);
	}

	for (i = 0; i < c->methods_count; i++) {
		mi = &c->methods[i];
		if (mi->name == main_str && mi->signature == signature_str) {
			break;
		}
	}

	if (mi == NULL) {
		ERROR("Can not run program: No main method found.\n");
		exit(1);
	}

	return mi;
}

static void *stack_start;

static void *allocate_stack(void)
{
	DEBUG("Entered %s\n", __func__);

	if (stack_start != NULL) {
		ERROR("Can not allocate space for stack: Already allocated space for stack.\n");
	}

	void *stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, 
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (stack == MAP_FAILED) {
		ERROR("Can not run program: Could not allocate space for stack.\n");
		exit(1);
	}

	stack_start = stack;
	return stack;
}

static void free_stack()
{
	DEBUG("Entered %s\n", __func__);

	if (munmap(stack_start, stack_size)) {
		WARN("Can not free stack: %s\n", strerror(errno));
	}
	stack_start = NULL;
}

static struct r_method_info *get_static_constructor(struct ClassFile *c)
{
	DEBUG("Entered %s\n", __func__);

	int i;
	struct r_method_info *mi;

	const char *clinit_str = find_string("<clinit>");
	if (clinit_str == NULL) {
		// class does not need to be initialized
		return NULL;
	}

	for (i = 0; i < c->methods_count; i++) {
		mi = &c->methods[i];
		if (mi->name == clinit_str) {
			break;
		}
	}

	return mi;
}

struct array {
	u4 length;
	void *arr;
};

#define TWO_BYTE_INDEX(pc) ((i2) (((u2) *((u1 *) (pc))) << 8 \
	| *((u1 *) (pc) + 1)))
#define FOUR_BYTE_INDEX(pc) ((i4) (((u4) *((u1 *) (pc))) << 24 \
	| ((u4) *((u1 *) (pc) + 1)) << 16 \
	| ((u4) *((u1 *) (pc) + 2)) << 8 \
	| (u4) *((u1 *) (pc) + 3)))
#define ISNAN(x) (x != x)

#define GET_OLDLOCALC(frame, localc) (*((u4 *) frame + localc))
#define GET_OLDFRAME(frame, localc) (*((u4 **) frame + localc + 1))
#define GET_OLDPC(frame, localc) (*((u1 **) frame + localc + 2))
#define GET_OLDC(frame, localc) (*((struct ClassFile **) frame + localc + 3))

void run(struct ClassFile *c, struct r_method_info *main)
{
	DEBUG("Entered %s\n", __func__);

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
	localc = main->c_attr->max_locals;
	optop = frame + localc + 3;


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

	void *table[256] = {
		&&nop, &&not_sup, &&iconst_m1, &&iconst_0, &&iconst_1,
		&&iconst_2, &&iconst_3, &&iconst_4, &&iconst_5, &&lconst_0,
		&&lconst_1, &&fconst_0, &&fconst_1, &&fconst_2, &&dconst_0,
		&&dconst_1, &&bipush, &&sipush, &&ldc1, &&ldc2,
		&&ldc2w, &&iload, &&lload, &&fload, &&dload,
		&&aload, &&iload_0, &&iload_1, &&iload_2, &&iload_3,
		&&lload_0, &&lload_1, &&lload_2, &&lload_3, &&fload_0,
		&&fload_1, &&fload_2, &&fload_3, &&dload_0, &&dload_1,
		&&dload_2, &&dload_3, &&aload_0, &&aload_1, &&aload_2,
		&&aload_3, &&iaload, &&laload, &&faload, &&daload,
		&&not_sup, &&baload, &&caload, &&saload, &&istore,
		&&lstore, &&fstore, &&dstore, &&astore, &&istore_0,
		&&istore_1, &&istore_2, &&istore_3, &&lstore_0, &&lstore_1,
		&&lstore_2, &&lstore_3, &&fstore_0, &&fstore_1, &&fstore_2,
		&&fstore_3, &&dstore_0, &&dstore_1, &&dstore_2, &&dstore_3,
		&&astore_0, &&astore_1, &&astore_2, &&astore_3, &&iastore,
		&&lastore, &&fastore, &&dastore, &&not_sup, &&bastore,
		&&castore, &&sastore, &&pop, &&pop2, &&dup,
		&&dup_x1, &&dup_x2, &&dup2, &&dup2_x1, &&dup2_x2,
		&&swap, &&iadd, &&ladd, &&fadd, &&dadd,
		&&isub, &&lsub, &&fsub, &&dsub, &&imul,
		&&lmul, &&fmul, &&dmul, &&idiv, &&ldiv,
		&&fdiv, &&ddiv, &&irem, &&lrem, &&frem,
		&&drem, &&ineg, &&lneg, &&fneg, &&dneg,
		&&ishl, &&lshl, &&ishr, &&lshr, &&iushr,
		&&lushr, &&iand, &&land, &&ior, &&lor,
		&&ixor, &&lxor, &&iinc, &&i2l, &&i2f,
		&&i2d, &&l2i, &&l2f, &&l2d, &&f2i,
		&&f2l, &&f2d, &&d2i, &&d2l, &&d2f,
		&&int2byte, &&int2char, &&int2short, &&lcmp, &&fcmpl,
		&&fcmpg, &&dcmpl, &&dcmpg, &&ifeq, &&ifne,
		&&iflt, &&ifge, &&ifgt, &&ifle, &&if_icmpeq,
		&&if_icmpne, &&if_icmplt, &&if_icmpge, &&if_icmpgt, &&if_icmple,
		&&not_sup, &&not_sup, &&_goto, &&not_sup, &&not_sup,
		&&tableswitch, &&lookupswitch, &&ireturn, &&lreturn, &&freturn,
		&&dreturn, &&areturn, &&_return, &&getstatic, &&putstatic,
		&&not_sup, &&not_sup, &&not_sup, &&not_sup, &&invokestatic,
		&&not_sup, &&not_sup, &&not_sup, &&newarray, &&not_sup,
		&&arraylength, &&not_sup, &&not_sup, &&not_sup, &&not_sup,
		&&not_sup, &&not_sup, &&not_sup, &&not_sup, &&not_sup,
		&&not_sup, &&not_sup, &&not_sup, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&no_inst,
		&&no_inst, &&no_inst, &&no_inst, &&no_inst, &&impdep1,
		&&not_sup
	};

	// initialize set of predefined native method
	init_natives();

	// set rounding mode to nearest number
	fesetround(FE_TONEAREST);

#ifdef _TESTMODE_
	init_testmode();
#endif

	// ------------- execute instructions ---------------------------

	// local variable that has to be set before calling initialize_static 
	struct ClassFile *clinit_c = c;
	clinit_c->static_initialized = 1;

	goto initialize_static;

	// some local variables used in the goto blocks
	// NOT used for side effects over different goto blocks
	u2 index;
	u4 *tmp_frame;
	u4 def_offset_addr_i;
	i4 *def_offset_addr;
	i4 low;
	struct r_fieldref_info *f;
	struct r_field_info *f_block;
	struct r_method_info *m_block;
	struct Code_attribute *c_attr;
	u4 tmp_u4;

	not_sup:
		ERROR(
			"Bytecode instruction %s not supported by this JVM.\n", 
			opcode_strings[*(pc - 1)]);
		exit(1);
	no_inst:
		ERROR("No instruction mapped to bytecode %d.\n", *(pc - 1));
		exit(1);

	nop:
		// do nothing
		NEXT();

	iconst_m1:
		// TODO: test
		*((i4 *) ++optop) = -1;
		NEXT();
	iconst_0:
		*((i4 *) ++optop) = 0;
		NEXT();
	iconst_1:
		*((i4 *) ++optop) = 1;
		NEXT();
	iconst_2:
		*((i4 *) ++optop) = 2;
		NEXT();
	iconst_3:
		*((i4 *) ++optop) = 3;
		NEXT();
	iconst_4:
		*((i4 *) ++optop) = 4;
		NEXT();
	iconst_5:
		*((i4 *) ++optop) = 5;
		NEXT();
	lconst_0:
		*((u8 *) ++optop) = 0l;
		optop++;
		NEXT();
	lconst_1:
		*((u8 *) ++optop) = 1l;
		optop++;
		NEXT();
	fconst_0:
		*((float *) ++optop) = 0.0f;
		NEXT();
	fconst_1:
		*((float *) ++optop) = 1.0f;
		NEXT();
	fconst_2:
		*((float *) ++optop) = 2.0f;
		NEXT();
	dconst_0:
		*((double *) ++optop) = 0.0;
		optop++;
		NEXT();
	dconst_1:
		*((double *) ++optop) = 1.0; 
		optop++;
		NEXT();
	bipush:
		*++optop = (i1) *pc++;
		NEXT();
	sipush:
		*++optop = (i2) TWO_BYTE_INDEX(pc);
		pc += 2;
		NEXT();
	ldc1:
		if (!IS_RESOLVED(cp, *pc)) {
			resolve_const(c, *pc);
		}
		*((void **) ++optop) = cp[*pc].r_any_info.ptr;
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
		index = TWO_BYTE_INDEX(pc);
		*((u8 *) ++optop) = *((u8 *) &cp[index].longOrDouble_info.first_bytes);
		++optop;
		pc += 2;
		NEXT();
	iload:
		*++optop = *(frame + *pc++);
		NEXT();
	lload:
		*((u8 *) ++optop) = *((u8 *) (frame + *pc++));
		optop++;
		NEXT();
	fload:
		*++optop = *(frame + *pc++);
		NEXT();	
	dload:
		*((double *) ++optop) = *((double *) (frame + *pc++));
		optop++;
		NEXT();
	aload:
		*++optop = *(frame + *pc++);
		NEXT();
	iload_0:
		*++optop = *frame;
		NEXT();
	iload_1:
		*++optop = *(frame + 1);
		NEXT();
	iload_2:
		*++optop = *(frame + 2);
		NEXT();
	iload_3:
		*++optop = *(frame + 3);
		NEXT();
	lload_0:
		*((u8 *) (++optop)) = *((u8 *) frame);
		optop++;
		NEXT();
	lload_1:
		*((u8 *) (++optop)) = *((u8 *) (frame + 1));
		optop++;
		NEXT();
	lload_2:
		*((u8 *) (++optop)) = *((u8 *) (frame + 2));
		optop++;
		NEXT();
	lload_3:
		*((u8 *) (++optop)) = *((u8 *) (frame + 3));
		optop++;
		NEXT();
	fload_0:
		*++optop = *frame;
		NEXT();
	fload_1:
		*++optop = *(frame + 1);
		NEXT();
	fload_2:
		*++optop = *(frame + 2);
		NEXT();
	fload_3:
		*++optop = *(frame + 3);
		NEXT();
	dload_0:
		*((double *) (++optop)) = *((double *) frame);
		optop++;
		NEXT();
	dload_1:
		*((double *) (++optop)) = *((double *) (frame + 1));
		optop++;
		NEXT();
	dload_2:
		*((double *) (++optop)) = *((double *) (frame + 2));
		optop++;
		NEXT();
	dload_3:
		*((double *) (++optop)) = *((double *) (frame + 3));
		optop++;
		NEXT();
	aload_0:
		*++optop = *frame;
		NEXT();
	aload_1:
		*++optop = *(frame + 1);
		NEXT();
	aload_2:
		*++optop = *(frame + 2);
		NEXT();
	aload_3:
		*++optop = *(frame + 3);
		NEXT();
	iaload:
		if (*(optop - 1) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*optop >= ((struct array *) *(optop - 1))->length || *optop < 0) {
			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		*(optop - 1) = ((u4 *) ((struct array *) *(optop - 1))->arr)[*optop];
		optop--;
		NEXT();
	laload:
		if (*(optop - 1) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*optop >= ((struct array *) *(optop - 1))->length || *optop < 0) {
			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		*(u8 *) (optop - 1)
			= ((u8 *) ((struct array *) *(optop - 1))->arr)[*optop];
		NEXT();
	faload:
		if (*(optop - 1) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*optop >= ((struct array *) *(optop - 1))->length || *optop < 0) {
			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		*(float *) (optop - 1)
			= ((float *) ((struct array *) *(optop - 1))->arr)[*optop];
		optop--;
		NEXT();
	daload:
		if (*(optop - 1) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*optop >= ((struct array *) *(optop - 1))->length || *optop < 0) {
			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		*(double *) (optop - 1)
			= ((double *) ((struct array *) *(optop - 1))->arr)[*optop];
		NEXT();

	baload:
		if (*(optop - 1) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*optop >= ((struct array *) *(optop - 1))->length || *optop < 0) {
			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		*(optop - 1) = ((u1 *) ((struct array *) *(optop - 1))->arr)[*optop];
		optop--;
		NEXT();
	caload:
		if (*(optop - 1) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*optop >= ((struct array *) *(optop - 1))->length || *optop < 0) {
			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		*(optop - 1) = ((u1 *) ((struct array *) *(optop - 1))->arr)[*optop];
		optop--;
		NEXT();
	saload:
		if (*(optop - 1) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*optop >= ((struct array *) *(optop - 1))->length || *optop < 0) {
			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		*(optop - 1) = ((u2 *) ((struct array *) *(optop - 1))->arr)[*optop];
		optop--;
		NEXT();

	istore:
		*(frame + *pc++) = *optop--;
		NEXT();
	lstore:
		*((u8 *) (frame + *pc++)) = *((u8 *) (optop - 1));
		optop -= 2;
		NEXT();
	fstore:
		*(frame + *pc++) = *optop--;
		NEXT();
	dstore:
		*((double *) (frame + *pc++)) = *((double *) (optop - 1));
		optop -= 2;
		NEXT();
	astore:
		*(frame + *pc++) = *optop--;
		NEXT();
	istore_0:
		*frame = *optop--;
		NEXT();
	istore_1:
		*(frame + 1) = *optop--;
		NEXT();
	istore_2:
		*(frame + 2) = *optop--;
		NEXT();
	istore_3:
		*(frame + 3) = *optop--;
		NEXT();
	lstore_0:
		*((u8 *) frame) = *((u8 *) (optop - 1));
		optop -= 2;
		NEXT();
	lstore_1:
		*((u8 *) (frame + 1)) = *((u8 *) (optop - 1));
		optop -= 2;
		NEXT();
	lstore_2:
		*((u8 *) (frame + 2)) = *((u8 *) (optop - 1));
		optop -= 2;
		NEXT();
	lstore_3:
		*((u8 *) (frame + 3)) = *((u8 *) (optop - 1));
		optop -= 2;
		NEXT();
	fstore_0:
		*frame = *optop--;
		NEXT();
	fstore_1:
		*(frame + 1) = *optop--;
		NEXT();
	fstore_2:
		*(frame + 2) = *optop--;
		NEXT();
	fstore_3:
		*(frame + 3) = *optop--;
		NEXT();
	dstore_0:
		*((double *) frame) = *((double *) (optop - 1));
		optop -= 2;
		NEXT();
	dstore_1:
		*((double *) (frame + 1)) = *((double *) (optop - 1));
		optop -= 2;
		NEXT();
	dstore_2:
		*((double *) (frame + 2)) = *((double *) (optop - 1));
		optop -= 2;
		NEXT();
	dstore_3:
		*((double *) (frame + 3)) = *((double *) (optop - 1));
		optop -= 2;
		NEXT();
	astore_0:
		*frame = *optop--;
		NEXT();
	astore_1:
		*(frame + 1) = *optop--;
		NEXT();
	astore_2:
		*(frame + 2) = *optop--;
		NEXT();
	astore_3:
		*(frame + 3) = *optop--;
		NEXT();
	iastore:
		if (*(optop - 2) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*(optop - 1) >= ((struct array *) *(optop - 2))->length
			|| *(optop - 1) < 0) {

			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		((u4 *) ((struct array *) *(optop - 2))->arr)[*(optop - 1)] = *optop;
		optop -= 3;
		NEXT();
	lastore:
		if (*(optop - 3) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*(optop - 2) >= ((struct array *) *(optop - 3))->length
			|| *(optop - 2) < 0) {

			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		((u8 *) ((struct array *) *(optop - 3))->arr)[*(optop - 2)]
			= *((u8 *) (optop - 1));
		optop -= 4;
		NEXT();
	fastore:
		if (*(optop - 2) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*(optop - 1) >= ((struct array *) *(optop - 2))->length
			|| *(optop - 1) < 0) {

			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		((float *) ((struct array *) *(optop - 2))->arr)[*(optop - 1)]
			= *((float *) optop);
		optop -= 3;
		NEXT();
	dastore:
		if (*(optop - 3) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*(optop - 2) >= ((struct array *) *(optop - 3))->length
			|| *(optop - 2) < 0) {

			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		((double *) ((struct array *) *(optop - 3))->arr)[*(optop - 2)]
			= *((double *) (optop - 1));
		optop -= 4;
		NEXT();

	bastore:
		if (*(optop - 2) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*(optop - 1) >= ((struct array *) *(optop - 2))->length
			|| *(optop - 1) < 0) {

			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		((u1 *) ((struct array *) *(optop - 2))->arr)[*(optop - 1)] = *optop;
		optop -= 3;
		NEXT();

	castore:
		if (*(optop - 2) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*(optop - 1) >= ((struct array *) *(optop - 2))->length
			|| *(optop - 1) < 0) {

			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		((u1 *) ((struct array *) *(optop - 2))->arr)[*(optop - 1)]
			= *optop;
		optop -= 3;
		NEXT();
	sastore:
		if (*(optop - 2) == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		if (*(optop - 1) >= ((struct array *) *(optop - 2))->length
			|| *(optop - 1) < 0) {

			ERROR("ArrayIndexOutOfBoundsException\n");
			exit(1);
		}
		((u2 *) ((struct array *) *(optop - 2))->arr)[*(optop - 1)]
			= *((u2 *) optop);
		optop -= 3;
		NEXT();
	pop:
		optop--;
		NEXT();
	pop2:
		optop -= 2;
		NEXT();
	dup:
		*(optop + 1) = *optop;
		optop++;
		NEXT();
	dup_x1:
		*(optop + 1) = *optop;
		*optop = *(optop - 1);
		*(optop - 1) = *(optop + 1);
		optop++;
		NEXT();
	dup_x2:
		*(optop + 1) = *optop;
		*optop = *(optop - 1);
		*(optop - 1) = *(optop - 2);
		*(optop - 2) = *(optop + 1);
		optop++;
		NEXT();
	dup2:
		*(optop + 1) = *(optop - 1);
		*(optop + 2) = *optop;
		optop += 2;
		NEXT();
	dup2_x1:
		*(optop + 2) = *optop;
		*(optop + 1) = *(optop - 1);
		*optop = *(optop - 2);
		*(optop - 1) = *(optop + 2);
		*(optop - 2) = *(optop + 1);
		optop += 2;
		NEXT();
	dup2_x2:
		*(optop + 2) = *optop;
		*(optop + 1) = *(optop - 1);
		*optop = *(optop - 2);
		*(optop - 1) = *(optop - 3);
		*(optop - 2) = *(optop + 2);
		*(optop - 3) = *(optop + 1);
		optop += 2;
		NEXT();
	swap:
		tmp_u4 = *optop;
		*optop = *(optop - 1);
		*(optop - 1) = tmp_u4;
		NEXT();
	iadd:
		optop--;
		*optop = (i4) *optop + (i4) *(optop + 1);
		NEXT();
	ladd:
		optop -= 2;
		*((i8 *) (optop - 1)) = *((i8 *) (optop - 1)) + *((i8 *) (optop + 1));
		NEXT();
	fadd:
		optop--;
		*((float *) optop) = *((float *) optop) + *((float *) optop + 1);
		NEXT();
	dadd:
		optop -= 2;
		*((double *) (optop - 1))
			= *((double *) (optop - 1)) + *((double *) (optop + 1));
		NEXT();
	isub:
		optop--;
		*optop = (i4) *optop - (i4) *(optop + 1);
		NEXT();
	lsub:
		optop -= 2;
		*((i8 *) (optop - 1)) = *((i8 *) (optop - 1)) - *((i8 *) (optop + 1));
		NEXT();
	fsub:
		optop--;
		*((float *) optop) = *((float *) optop) - *((float *) optop + 1);
		NEXT();
	dsub:
		optop -= 2;
		*((double *) (optop - 1))
			= *((double *) (optop - 1)) - *((double *) (optop + 1));
		NEXT();
	imul:
		optop--;
		*optop = (i4) *optop * (i4) *(optop + 1);
		NEXT();
	lmul:
		optop -= 2;
		*((i8 *) (optop - 1)) = *((i8 *) (optop - 1)) * *((i8 *) (optop + 1));
		NEXT();
	fmul:
		optop--;
		*((float *) optop) = *((float *) optop) * *((float *) optop + 1);
		NEXT();
	dmul:
		optop -= 2;
		*((double *) (optop - 1))
			= *((double *) (optop - 1)) * *((double *) (optop + 1));
		NEXT();
	idiv:
		if (*optop == 0) {
			ERROR("ArithmeticException: / by zero\n");
			exit(1);
		}
		optop--;
		*optop = (i4) *optop / (i4) *(optop + 1);
		NEXT();
	ldiv:
		if (*((i8 *) (optop - 1)) == 0) {
			ERROR("ArithmeticException: / by zero\n");
			exit(1);
		}
		optop -= 2;
		*((i8 *) (optop - 1)) = *((i8 *) (optop - 1)) / *((i8 *) (optop + 1));
		NEXT();
	fdiv:
		optop--;
		// TODO: correct floating point arithmetic implementation in C
		*((float *) optop) = *((float *) optop) / *((float *) optop + 1);
		NEXT();
	ddiv:
		optop -= 2;
		// TODO: correct floating point arithmetic implementation in C
		*((double *) (optop - 1))
			= *((double *) (optop - 1)) / *((double *) (optop + 1));
		NEXT();
	irem:
		// TODO: extra tests for special cases
		if (*optop == 0) {
			ERROR("ArithmeticException: %% by zero\n");
			exit(1);
		}
		optop--;
		// remainder instead of modulo like in c %-operator
		*optop = (i4) *optop - 
			((i4) *optop / (i4) *(optop + 1)) * (i4) *(optop + 1);
		NEXT();
	lrem:
		// TODO: extra tests for special cases
		if (*((i8 *) (optop - 1)) == 0) {
			ERROR("ArithmeticException: %% by zero\n");
			exit(1);
		}
		optop -= 2;
		// remainder instead of modulo like in c %-operator
		*((i8 *) (optop - 1)) = *((i8 *) (optop - 1)) -
			(*((i8 *) (optop - 1)) / *((i8 *) (optop + 1)))
				* *((i8 *) (optop + 1));
		NEXT();
	frem:
		if (*((float *) optop) == NAN || *((float *) optop - 1) == NAN
			|| *((float *) optop) == 0 || *((float *) optop - 1) == INFINITY) {
			optop--;
			*((float *) optop) = NAN;
			NEXT();
		}
		if (*((float *) optop) == INFINITY || *((float *) optop - 1) == 0) {
			optop--; // dividend is the result
			NEXT();
		}
		optop--;
		*((float *) optop) = *((float *) optop) -
			(((i4) (*((float *) optop) / *((float *) optop + 1)))
				* *((float *) optop + 1));
		NEXT();
	drem:
		optop -= 2;
		if (*((double *) (optop - 1)) == NAN 
			|| *((double *) (optop + 1)) == NAN
			|| *((double *) (optop - 1)) == INFINITY
			|| *((double *) (optop + 1)) == 0) {
			*((float *) optop) = NAN;
			NEXT();
		}
		if (*((double *) (optop - 1)) == 0 
			|| *((double *) (optop + 1)) == INFINITY) {
			NEXT(); // dividend is the result
		}
		*((double *) (optop - 1)) = *((double *) (optop - 1)) -
			(((i8) (*((double *) (optop - 1)) / *((double *) (optop + 1))))
				* *((double *) (optop + 1)));
		NEXT();
	ineg:
		*optop = - (i4) *optop;
		NEXT();
	lneg:
		*((i8 *) (optop - 1)) = - *((i8 *) (optop - 1));
		NEXT();
	fneg:
		*((float *) optop) = - *((float *) optop);
		NEXT();
	dneg:
		*((double *) (optop - 1)) = - *((double *) (optop - 1));
		NEXT();
	ishl:
		// TODO: is this the right arithmetic shift?
		optop--;
		// only the 5 lowest-order bits are used for shift
		*optop <<= (*(optop + 1) & 0x1f);
		NEXT();
	lshl:
		// TODO: is this the right arithmetic shift?
		optop--;
		// only the 6 lowest-order bits are used for shift
		*((u8 *) (optop - 1)) <<= (*(optop + 1) & 0x3f);
		NEXT();
	ishr:
		optop--;
		// only the 5 lowest-order bits are used for shift
		// TODO: arithmetic shift not portable
		*((i4 *) optop) >>= (*(optop + 1) & 0x1f);
		NEXT();
	lshr:
		optop--;
		// only the 6 lowest-order bits are used for shift
		// TODO: arithmetic shift not portable
		*((i8 *) (optop - 1)) >>= (*(optop + 1) & 0x3f);
		NEXT();
	iushr:
		optop--;
		// only the 5 lowest-order bits are used for shift
		*optop >>= (*(optop + 1) & 0x1f);
		NEXT();
	lushr:
		optop--;
		// only the 6 lowest-order bits are used for shift
		*((u8 *) (optop - 1)) >>= (*(optop + 1) & 0x3f);
		NEXT();
	iand:
		optop--;
		*optop &= *(optop + 1);
		NEXT();
	land:
		*((u8 *) (optop - 3)) &= *((u8 *) (optop - 1));
		optop -= 2;
		NEXT();
	ior:
		optop--;
		*optop |= *(optop + 1);
		NEXT();
	lor:
		*((u8 *) (optop - 3)) |= *((u8 *) (optop - 1));
		optop -= 2;
		NEXT();
	ixor:
		optop--;
		*optop ^= *(optop + 1);
		NEXT();
	lxor:
		*((u8 *) (optop - 3)) ^= *((u8 *) (optop - 1));
		optop -= 2;
		NEXT();
	iinc:
		*((i4 *) frame + *pc) += *((i1 *) pc + 1);
		pc += 2;
		NEXT();
	i2l:
		*((i8 *) optop) = (i8) *((i4 *) optop);
		optop++;
		NEXT();
	i2f:
		*((float *) optop) = (float) *((i4 *) optop);
		NEXT();
	i2d:
		*((double *) optop) = (double) *((i4 *) optop);
		optop++;
		NEXT();
	l2i:
		optop--;
		NEXT();
	l2f:
		*((float *) (optop - 1)) = (float) *((i8 *) (optop - 1));
		optop--;
		NEXT();
	l2d:
		*((double *) (optop - 1)) = (double) *((i8 *) (optop - 1));
		NEXT();
	f2i:
		if (*((float *) optop) == NAN) {
			*((i4 *) optop) = 0;
		} else if (*((float *) optop) <= INT32_MIN) {
			*((i4 *) optop) = INT32_MIN;
		} else if (*((float *) optop) >= INT32_MAX) {
			*((i4 *) optop) = INT32_MAX;
		} else {
			fesetround(FE_TOWARDZERO);
			*((i4 *) optop) = (i4) *((float *) optop);
			fesetround(FE_TONEAREST);
		}
		NEXT();
	f2l:
		if (*((float *) optop) == NAN) {
			*((i8 *) optop) = 0;
		} else if (*((float *) optop) <= INT64_MIN) {
			*((i8 *) optop) = INT64_MIN;
		} else if (*((float *) optop) >= INT64_MAX) {
			*((i8 *) optop) = INT64_MAX;
		} else {
			fesetround(FE_TOWARDZERO);
			*((i8 *) optop) = (i8) *((float *) optop);
			fesetround(FE_TONEAREST);
		}
		optop++;
		NEXT();
	f2d:
		*((double *) optop) = (double) *((float *) optop);
		optop++;
		NEXT();
	d2i:
		if (*((double *) (optop - 1)) == NAN) {
			*((i4 *) (optop - 1)) = 0;
		} else if (*((double *) (optop - 1)) <= INT32_MIN) {
			*((i4 *) (optop - 1)) = INT32_MIN;
		} else if (*((double *) (optop - 1)) >= INT32_MAX) {
			*((i4 *) (optop - 1)) = INT32_MAX;
		} else {
			fesetround(FE_TOWARDZERO);
			*((i4 *) (optop - 1)) = (i4) *((double *) (optop - 1));
			fesetround(FE_TONEAREST);
		}
		optop--;
		NEXT();
	d2l:
		if (*((double *) (optop - 1)) == NAN) {
			*((i8 *) (optop - 1)) = 0;
		} else if (*((double *) (optop - 1)) <= INT64_MIN) {
			*((i8 *) (optop - 1)) = INT64_MIN;
		} else if (*((double *) (optop - 1)) >= INT64_MAX) {
			*((i8 *) (optop - 1)) = INT64_MAX;
		} else {
			fesetround(FE_TOWARDZERO);
			*((i8 *) (optop - 1)) = (i8) *((double *) (optop - 1));
			fesetround(FE_TONEAREST);
		}
		NEXT();
	d2f:
		*((float *) (optop - 1)) = (float) *((double *) (optop - 1));
		optop--;
		NEXT();
	int2byte:
		*((i4 *) optop) = (i4) *((i1 *) optop); // sign extension through cast
		NEXT();
	int2char:
		*((u4 *) optop) = (u4) *((u2 *) optop); // zero extension through cast
		NEXT();			
	int2short:
		*((i4 *) optop) = (i4) *((i2 *) optop); // sign extension through cast
		NEXT();
	lcmp:
		if (*((i8 *) (optop - 3)) > *((i8 *) (optop - 1))) {
			*(optop - 3) = 1;
		} else if (*((i8 *) (optop - 3)) < *((i8 *) (optop - 1))) {
			*(optop - 3) = -1;
		} else {
			*(optop - 3) = 0;
		}
		optop -= 3;
		NEXT();
	fcmpl:
		if (*((float *) (optop - 1)) > *((float *) optop)) {
			*(optop - 1) = 1;
		} else if (*((float *) (optop - 1)) < *((float *) optop)
			|| ISNAN(*((float *) (optop - 1)))
			|| ISNAN(*((float *) optop))) {
			*(optop - 1) = -1;
		} else {
			*(optop - 1) = 0;
		}
		optop--;
		NEXT();
	fcmpg:
		if (*((float *) (optop - 1)) > *((float *) optop)
			|| ISNAN(*((float *) (optop - 1)))
			|| ISNAN(*((float *) optop))) {
			*(optop - 1) = 1;
		} else if (*((float *) (optop - 1)) < *((float *) optop)) {
			*(optop - 1) = -1;
		} else {
			*(optop - 1) = 0;
		}
		optop--;
		NEXT();
	dcmpl:
		if (*((double *) (optop - 3)) > *((double *) (optop - 1))) {
			*(optop - 3) = 1;
		} else if (*((double *) (optop - 3)) < *((double *) (optop - 1))
			|| ISNAN(*((double *) (optop - 3)))
			|| ISNAN(*((double *) (optop - 1)))) {
			*(optop - 3) = -1;
		} else {
			*(optop - 3) = 0;
		}
		optop -= 3;
		NEXT();
	dcmpg:
		if (*((double *) (optop - 3)) > *((double *) (optop - 1))
			|| ISNAN(*((double *) (optop - 3)))
			|| ISNAN(*((double *) (optop - 1)))) {
			*(optop - 3) = 1;
		} else if (*((double *) (optop - 3)) < *((double *) (optop - 1))) {
			*(optop - 3) = -1;
		} else {
			*(optop - 3) = 0;
		}
		optop -= 3;
		NEXT();
	ifeq:
		if (*optop == 0) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop--;
		NEXT();
	ifne:
		if (*optop != 0) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop--;
		NEXT();
	iflt:
		if ((i4) *optop < 0) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop--;
		NEXT();
	ifge:
		if ((i4) *optop >= 0) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop--;
		NEXT();
	ifgt:
		if ((i4) *optop > 0) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop--;
		NEXT();
	ifle:
		if ((i4) *optop <= 0) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop--;
		NEXT();
	if_icmpeq:
		if (*(optop - 1) == *optop) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop -= 2;
		NEXT();
	if_icmpne:
		if (*(optop - 1) != *optop) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop -= 2;
		NEXT();
	if_icmplt:
		if ((i4) *(optop - 1) < (i4) *optop) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop -= 2;
		NEXT();
	if_icmpge:
		if ((i4) *(optop - 1) >= (i4) *optop) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop -= 2;
		NEXT();
	if_icmpgt:
		if ((i4) *(optop - 1) > (i4) *optop) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop -= 2;
		NEXT();
	if_icmple:
		if ((i4) *(optop - 1) <= (i4) *optop) {
			pc += TWO_BYTE_INDEX(pc) - 1;
		} else {
			pc += 2;
		}
		optop -= 2;
		NEXT();

	_goto:
		pc += TWO_BYTE_INDEX(pc) - 1;
		NEXT();

	tableswitch:
		def_offset_addr_i = (u4) pc;
		while (def_offset_addr_i % 4 != 0) {
			def_offset_addr_i++;
		}
		def_offset_addr = (i4 *) def_offset_addr_i;
	
		low = FOUR_BYTE_INDEX(def_offset_addr + 1);	
		if (low < *((i4 *) optop) 
			&& *((i4 *) optop) < FOUR_BYTE_INDEX(def_offset_addr + 2)) { // high

			// lookup index (*optop - low) in table (def_offset_addr + 3)
			pc += FOUR_BYTE_INDEX(
				def_offset_addr + 3 + *((i4 *) optop) - low) - 1;
		} else {
			pc += FOUR_BYTE_INDEX(def_offset_addr) - 1;
		}
		optop--;
		NEXT();
	lookupswitch:
		def_offset_addr_i = (u4) pc;
		while (def_offset_addr_i % 4 != 0) {
			def_offset_addr_i++;
		}
		def_offset_addr = (i4 *) def_offset_addr_i;

		// binary search
		i4 *switchtable = def_offset_addr + 2;
		low = 0;
		i4 high = FOUR_BYTE_INDEX(def_offset_addr + 1); // npairs
		i4 middle = high / 2;

		while (low < high) {
			middle = low + (high - low) / 2;
			// middle * 2 + 1 as index, because we only want the second part
			// of the match - offset pairs
			if (FOUR_BYTE_INDEX(switchtable + middle * 2) 
					== *((i4 *) optop)) {
				// found matching offset
				pc += FOUR_BYTE_INDEX(switchtable + middle * 2 + 1) - 1;
				optop--;
				NEXT();
			} else if (FOUR_BYTE_INDEX(switchtable + middle * 2)
					 < *((i4 *) optop)) {
				low = middle + 1;
			} else {
				high = middle - 1;
			}
		}

		/*
		// linear search
		i4 npairs = FOUR_BYTE_INDEX(def_offset_addr + 1);
		i4 *switchtable = def_offset_addr + 2;
		i4 i;
		for (i = 0; i < npairs; i++) {
			if (FOUR_BYTE_INDEX(switchtable + i * 2) == *((i4 *) optop)) {
				pc += FOUR_BYTE_INDEX(switchtable + i * 2 + 1) - 1;
				NEXT();
			}
		}
		*/

		// no match, use default offset
		pc += FOUR_BYTE_INDEX(def_offset_addr) - 1;
		optop--;
		NEXT();
	ireturn:
		tmp_frame = frame;
		frame = GET_OLDFRAME(tmp_frame, localc);
		pc = GET_OLDPC(tmp_frame, localc);
		c = GET_OLDC(tmp_frame, localc);
		cp = c->constant_pool;

		localc = GET_OLDLOCALC(tmp_frame, localc);

		*tmp_frame = *optop; // return value
		optop = tmp_frame;

		NEXT();
	lreturn:
		tmp_frame = frame;
		frame = GET_OLDFRAME(tmp_frame, localc);
		pc = GET_OLDPC(tmp_frame, localc);
		c = GET_OLDC(tmp_frame, localc);
		cp = c->constant_pool;

		localc = GET_OLDLOCALC(tmp_frame, localc);

		*((u8 *) tmp_frame) = *((u8 *) (optop - 1)); // return value
		optop = tmp_frame + 1;

		NEXT();
	freturn:
		// Identical to ireturn
		tmp_frame = frame;
		frame = GET_OLDFRAME(tmp_frame, localc);
		pc = GET_OLDPC(tmp_frame, localc);
		c = GET_OLDC(tmp_frame, localc);
		cp = c->constant_pool;

		localc = GET_OLDLOCALC(tmp_frame, localc);

		*tmp_frame = *optop; // return value
		optop = tmp_frame;

		NEXT();
	dreturn:
		// Identical to lreturn
		tmp_frame = frame;
		frame = GET_OLDFRAME(tmp_frame, localc);
		pc = GET_OLDPC(tmp_frame, localc);
		c = GET_OLDC(tmp_frame, localc);
		cp = c->constant_pool;

		localc = GET_OLDLOCALC(tmp_frame, localc);

		*((u8 *) tmp_frame) = *((u8 *) (optop - 1)); // return value
		optop = tmp_frame + 1;

		NEXT();
	areturn:
		tmp_frame = frame;
		frame = GET_OLDFRAME(tmp_frame, localc);
		pc = GET_OLDPC(tmp_frame, localc);
		c = GET_OLDC(tmp_frame, localc);
		cp = c->constant_pool;

		localc = GET_OLDLOCALC(tmp_frame, localc);

		*tmp_frame = *optop; // return value
		optop = tmp_frame;

		NEXT();
	_return:
		tmp_frame = frame;

		frame = GET_OLDFRAME(tmp_frame, localc);
		pc = GET_OLDPC(tmp_frame, localc);
		c = GET_OLDC(tmp_frame, localc);
		cp = c->constant_pool;

		localc = GET_OLDLOCALC(tmp_frame, localc);
		optop = tmp_frame - 1;

		NEXT();
	getstatic:
		index = TWO_BYTE_INDEX(pc);
		pc += 2;
		if (!IS_RESOLVED(cp, index)) {
			resolve_fieldref(c, index);
		}
		f = &cp[index].r_fieldref_info;
		
		// in case we loaded a new class and need to initialize it
		if (!f->r_class->static_initialized) {
			pc -= 3; // we want to call getstatic again after the constructor
			clinit_c = f->r_class;
			f->r_class->static_initialized = 1;
			goto initialize_static;
		}

		f_block = f->r_field;
	
		if (!strcmp("D", f_block->signature)
			|| !strcmp("J", f_block->signature)) {

			*++optop = f_block->byte1;
			*++optop = f_block->byte2;
		} else if (!strncmp("[", f_block->signature, 1)
			|| !strcmp("B", f_block->signature)
			|| !strcmp("C", f_block->signature)
			|| !strcmp("F", f_block->signature)
			|| !strcmp("I", f_block->signature)
			|| !strcmp("S", f_block->signature)
			|| !strcmp("Z", f_block->signature)) {

			*++optop = f_block->byte1;
		} else {

			ERROR("getstatic: Signature \"%s\" not supported",
				f_block->signature);
			exit(1);
		}

		NEXT();
	putstatic:
		index = TWO_BYTE_INDEX(pc);
		pc += 2;
		if (!IS_RESOLVED(cp, index)) {
			resolve_fieldref(c, index);
		}
		f = &cp[index].r_fieldref_info;

		// in case we loaded a new class and need to initialize it
		if (!f->r_class->static_initialized) {
			pc -= 3; // we want to call putstatic again after the constructor
			clinit_c = f->r_class;
			f->r_class->static_initialized = 1;
			goto initialize_static;
		}

		f_block = f->r_field;

		if (!strcmp("D", f_block->signature)
			|| !strcmp("J", f_block->signature)) {

			f_block->byte2 = *optop--;
			f_block->byte1 = *optop--;
		} else if (!strncmp("[", f_block->signature, 1)
			|| !strcmp("B", f_block->signature)
			|| !strcmp("C", f_block->signature)
			|| !strcmp("F", f_block->signature)
			|| !strcmp("I", f_block->signature)
			|| !strcmp("S", f_block->signature)
			|| !strcmp("Z", f_block->signature)) {

			f_block->byte1 = *optop--;
		} else {

			ERROR("putstatic: Signature \"%s\" not supported",
				f_block->signature);
			exit(1);
		}
		NEXT();

	invokestatic:
		index = TWO_BYTE_INDEX(pc);
		pc += 2;
		if (!IS_RESOLVED(cp, index)) {
			resolve_methodref(c, index);
		}
		struct r_methodref_info *m = &cp[index].r_methodref_info;
		
		// in case we loaded a new class and need to initialize it
		if (!m->r_class->static_initialized) {
			pc -= 3; // we want to call invokestatic again after the constructor
			clinit_c = m->r_class;
			m->r_class->static_initialized = 1;
			goto initialize_static;
		}

		m_block = m->r_method;
		c_attr = m_block->c_attr;

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

	newarray:
		if (*optop < 0) {
			ERROR("NegativeArraySizeException\n");
			exit(1);
		}

		size_t type_size;
		switch (*pc) {
			case T_BOOLEAN:
				type_size = sizeof(u1);
				break;
			case T_CHAR:
				type_size = sizeof(char);
				break;
			case T_FLOAT:
				type_size = sizeof(float);
				break;
			case T_DOUBLE:
				type_size = sizeof(double);
				break;
			case T_BYTE:
				type_size = sizeof(u1);
				break;
			case T_SHORT:
				type_size = sizeof(u2);
				break;
			case T_INT:
				type_size = sizeof(u4);
				break;
			case T_LONG:
				type_size = sizeof(u8);
				break;
			default:
				ERROR("newarray: Invalid type %d\n", *pc);
				exit(1);
		}
		pc++;
		struct array *array_struct = malloc(sizeof(struct array));
		if (array_struct == NULL) {
			ERROR("newarray: Could not allocate memory: %s\n", strerror(errno));
			exit(1);
		}
		void *array = calloc(*optop, type_size);
		if (array == NULL) {
			ERROR("newarray: Could not allocate memory: %s\n", strerror(errno));
			exit(1);
		}
		array_struct->length = *optop;
		array_struct->arr = array;
		*optop = (u4) array_struct;

		NEXT();
	arraylength:
		if (*optop == 0) {
			ERROR("NullPointerException\n");
			exit(1);
		}
		*optop = ((struct array *) *optop)->length;
		NEXT();

	// custom method, clinit_c has to be set before
	initialize_static:
		m_block = get_static_constructor(clinit_c);
		if (m_block == NULL) {
			// class does not need to be initialized
			NEXT_AFTER_INITIALIZE_STATIC();
		}
 
		c_attr = m_block->c_attr;

		tmp_frame = optop + 1 - m_block->nargs;

		optop += c_attr->max_locals - m_block->nargs;
		*++optop = (u4) localc;
		*++optop = (u4) frame;
		*++optop = (u4) pc;
		*++optop = (u4) c;

		pc = c_attr->code;
		c = clinit_c;
		cp = clinit_c->constant_pool;
		frame = tmp_frame;
		localc = c_attr->max_locals;

		NEXT_AFTER_INITIALIZE_STATIC();

	impdep1:
		// opcode reserved for implementation dependent debugger operations
		// we use this as simple exit
		free_vm(c);
		return;

}

void free_vm(struct ClassFile *main_c)
{
	DEBUG("Entered %s\n", __func__);
	free_stack();
	free_all_linked_classes(main_c);
	free_string_pool();
} 

void run_main(struct ClassFile *c)
{
	DEBUG("Entered %s\n", __func__);
	run(c, get_main_method(c));
}

