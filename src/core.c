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
	#define LOG_INSTR printf("- %s\n", opcode_strings[*pc])
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
#endif

/* =============================================================== */


#ifndef _TESTMODE_
	#define NEXT() LOG_INSTR;\
goto *table[*pc++];
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

#define TWO_BYTE_INDEX(pc) (((u2) *pc) << 8 | *(pc + 1))
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

	void *table[255];
	table[NOP] = &&nop;
	table[ICONST_M1] = &&iconst_m1;
	table[ICONST_0] = &&iconst_0;
	table[ICONST_1] = &&iconst_1;
	table[ICONST_2] = &&iconst_2;
	table[ICONST_3] = &&iconst_3;
	table[ICONST_4] = &&iconst_4;
	table[ICONST_5] = &&iconst_5;
	table[LCONST_0] = &&lconst_0;
	table[LCONST_1] = &&lconst_1;
	table[FCONST_0] = &&fconst_0;
	table[FCONST_1] = &&fconst_1;
	table[FCONST_2] = &&fconst_2;
	table[DCONST_0] = &&dconst_0;
	table[DCONST_1] = &&dconst_1;
	table[BIPUSH] = &&bipush;
	table[SIPUSH] = &&sipush;
	table[LDC1] = &&ldc1;
	table[LDC2] = &&ldc2;
	table[LDC2W] = &&ldc2w;
	table[ILOAD] = &&iload;
	table[LLOAD] = &&lload;
	table[FLOAD] = &&fload;
	table[DLOAD] = &&dload;
	table[ALOAD] = &&aload;
	table[ILOAD_0] = &&iload_0;
	table[ILOAD_1] = &&iload_1;
	table[ILOAD_2] = &&iload_2;
	table[ILOAD_3] = &&iload_3;
	table[LLOAD_0] = &&lload_0;
	table[LLOAD_1] = &&lload_1;
	table[LLOAD_2] = &&lload_2;
	table[LLOAD_3] = &&lload_3;
	table[FLOAD_0] = &&fload_0;
	table[FLOAD_1] = &&fload_1;
	table[FLOAD_2] = &&fload_2;
	table[FLOAD_3] = &&fload_3;
	table[DLOAD_0] = &&dload_0;
	table[DLOAD_1] = &&dload_1;
	table[DLOAD_2] = &&dload_2;
	table[DLOAD_3] = &&dload_3;
	table[ALOAD_0] = &&aload_0;
	table[ALOAD_1] = &&aload_1;
	table[ALOAD_2] = &&aload_2;
	table[ALOAD_3] = &&aload_3;
	table[ISTORE] = &&istore;
	table[LSTORE] = &&lstore;
	table[FSTORE] = &&fstore;
	table[DSTORE] = &&dstore;
	table[ISTORE_0] = &&istore_0;
	table[ISTORE_1] = &&istore_1;
	table[ISTORE_2] = &&istore_2;
	table[ISTORE_3] = &&istore_3;
	table[LSTORE_0] = &&lstore_0;
	table[LSTORE_1] = &&lstore_1;
	table[LSTORE_2] = &&lstore_2;
	table[LSTORE_3] = &&lstore_3;
	table[FSTORE_0] = &&fstore_0;
	table[FSTORE_1] = &&fstore_1;
	table[FSTORE_2] = &&fstore_2;
	table[FSTORE_3] = &&fstore_3;
	table[DSTORE_0] = &&dstore_0;
	table[DSTORE_1] = &&dstore_1;
	table[DSTORE_2] = &&dstore_2;
	table[DSTORE_3] = &&dstore_3;
	table[POP] = &&pop;
	table[POP2] = &&pop2;
	table[DUP] = &&dup;
	table[DUP_X1] = &&dup_x1;
	table[DUP_X2] = &&dup_x2;
	table[DUP2] = &&dup2;
	table[DUP2_X1] = &&dup2_x1;
	table[DUP2_X2] = &&dup2_x2;
	table[SWAP] = &&swap;
	table[IADD] = &&iadd;
	table[LADD] = &&ladd;
	table[FADD] = &&fadd;
	table[DADD] = &&dadd;
	table[ISUB] = &&isub;
	table[LSUB] = &&lsub;
	table[FSUB] = &&fsub;
	table[DSUB] = &&dsub;
	table[IMUL] = &&imul;
	table[LMUL] = &&lmul;
	table[FMUL] = &&fmul;
	table[DMUL] = &&dmul;
	table[IDIV] = &&idiv;
	table[LDIV] = &&ldiv;
	table[FDIV] = &&fdiv;
	table[DDIV] = &&ddiv;
	table[IREM] = &&irem;
	table[LREM] = &&lrem;
	table[FREM] = &&frem;
	table[DREM] = &&drem;
	table[INEG] = &&ineg;
	table[LNEG] = &&lneg;
	table[FNEG] = &&fneg;
	table[DNEG] = &&dneg;
	table[ISHL] = &&ishl;
	table[LSHL] = &&lshl;
	table[ISHR] = &&ishr;
	table[LSHR] = &&lshr;
	table[IUSHR] = &&iushr;
	table[LUSHR] = &&lushr;
	table[IAND] = &&iand;
	table[LAND] = &&land;
	table[IOR] = &&ior;
	table[LOR] = &&lor;
	table[IXOR] = &&ixor;
	table[LXOR] = &&lxor;
	table[IINC] = &&iinc;
	table[I2L] = &&i2l;
	table[I2F] = &&i2f;
	table[I2D] = &&i2d;
	table[L2I] = &&l2i;
	table[L2F] = &&l2f;
	table[L2D] = &&l2d;
	table[F2I] = &&f2i;
	table[F2L] = &&f2l;
	table[F2D] = &&f2d;
	table[D2I] = &&d2i;
	table[D2L] = &&d2l;
	table[D2F] = &&d2f;
	table[INT2BYTE] = &&int2byte;
	table[INT2CHAR] = &&int2char;
	table[INT2SHORT] = &&int2short;
	table[LCMP] = &&lcmp;
	table[FCMPL] = &&fcmpl;
	table[FCMPG] = &&fcmpg;
	table[DCMPL] = &&dcmpl;
	table[DCMPG] = &&dcmpg;
	table[IFEQ] = &&ifeq;
	table[IFNE] = &&ifne;
	table[IFLT] = &&iflt;
	table[IFGE] = &&ifge;
	table[IFGT] = &&ifgt;
	table[IFLE] = &&ifle;
	table[IF_ICMPEQ] = &&if_icmpeq;
	table[IF_ICMPNE] = &&if_icmpne;
	table[IF_ICMPLT] = &&if_icmplt;
	table[IF_ICMPGE] = &&if_icmpge;
	table[IF_ICMPGT] = &&if_icmpgt;
	table[IF_ICMPLE] = &&if_icmple;
	table[GOTO] = &&_goto;
	table[TABLESWITCH] = &&tableswitch;
	table[LOOKUPSWITCH] = &&lookupswitch;
	table[IRETURN] = &&ireturn;
	table[LRETURN] = &&lreturn;
	table[FRETURN] = &&freturn;
	table[DRETURN] = &&dreturn;
	table[RETURN] = &&_return;
	table[GETSTATIC] = &&getstatic;
	table[PUTSTATIC] = &&putstatic;
	table[INVOKENONVIRTUAL] = &&invokenonvirtual;
	table[INVOKESTATIC] = &&invokestatic;
	table[IMPDEP1] = &&impdep1;

	// initialize set of predefined native method
	init_natives();

	// set rounding mode to nearest number
	fesetround(FE_TONEAREST);

#ifdef _TESTMODE_
	init_testmode();
#endif

	// ------------- execute instructions ---------------------------

	NEXT();

	// some local variables used in the goto blocks
	// NOT used for side effects over different goto blocks
	u2 index;
	u4 *tmp_frame;
	u4 def_offset_addr_i;
	i4 *def_offset_addr;
	i4 low;
	struct r_fieldref_info *f;
	struct r_field_info *f_block;

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
		NEXT();
	lconst_1:
		*((u8 *) ++optop) = 1l;
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
		NEXT();
	dconst_1:
		*((double *) ++optop) = 1.0; 
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
	aload_1:
	aload_2:
	aload_3:

	istore:
		*(frame + *pc++) = *optop--;
		NEXT();
	lstore:
		*((u8 *) (frame + *pc++)) = *((u8 *) (optop - 1));
		NEXT();
	fstore:
		*(frame + *pc++) = *optop--;
		NEXT();
	dstore:
		*((double *) (frame + *pc++)) = *((double *) (optop - 1));
		optop -= 2;
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

	pop:
		optop--;
		NEXT();
	pop2:
		optop -= 2;
		NEXT();
	dup:
		// TODO: Implement
	dup_x1:
		// TODO: Implement
	dup_x2:
		// TODO: Implement
	dup2:
		// TODO: Implement
	dup2_x1:
		// TODO: Implement
	dup2_x2:
		// TODO: Implement
	swap:
		// TODO: Implement
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
		*(frame + *pc) += (i1) *(pc + 1);
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
		pc += TWO_BYTE_INDEX(pc);
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
		if (!IS_RESOLVED(cp, index)) {
			resolve_fieldref(c, index);
		}
		f = &cp[index].r_fieldref_info;
		f_block = f->r_field;
		
		switch (*f_block->signature) {
			case 'B':
			case 'C':
			case 'F':
			case 'I':
			case 'S':
			case 'Z':
				*++optop = f_block->byte1;
				break;

			case 'D':
			case 'L':
				*++optop = f_block->byte2;
				*++optop = f_block->byte1;
				break;

			// arrays and objects currently not supported
		}
		NEXT();
	putstatic:
		if (!IS_RESOLVED(cp, index)) {
			resolve_fieldref(c, index);
		}
		f = &cp[index].r_fieldref_info;
		f_block = f->r_field;

		switch (*f_block->signature) {
			case 'B':
			case 'C':
			case 'F':
			case 'I':
			case 'S':
			case 'Z':
				f_block->byte1 = *optop--;
				break;

			case 'D':
			case 'L':
				f_block->byte2 = *optop--;
				f_block->byte1 = *optop--;
				break;

			// arrays and objects currently not supported
		}
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

