#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <math.h>

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
	table[RETURN] = &&_return;
	table[INVOKENONVIRTUAL] = &&invokenonvirtual;
	table[INVOKESTATIC] = &&invokestatic;
	table[IMPDEP1] = &&impdep1;

	
	// initialize set of predefined native method
	init_natives();

#ifdef _TESTMODE_
	init_testmode();
#endif

	// ------------- execute instructions ---------------------------

	NEXT();

	// some local variables used in the goto blocks
	// NOT used for side effects over different goto blocks
	u2 index;
	u4 *tmp_frame;

	iconst_m1:
		// TODO: test
		*++optop = -1;
		NEXT();
	iconst_0:
		*++optop = 0;
		NEXT();
	iconst_1:
		*++optop = 1;
		NEXT();
	iconst_2:
		*++optop = 2;
		NEXT();
	iconst_3:
		*++optop = 3;
		NEXT();
	iconst_4:
		*++optop = 4;
		NEXT();
	iconst_5:
		*++optop = 5;
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
		// TODO: TEST
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
		NEXT();
	fload:
		*++optop = *(frame + *pc++);
		NEXT();	
	dload:
		*((double *) ++optop) = *((double *) (frame + *pc++));
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
			fprintf(stderr, "ArithmeticException: / by zero\n");
			exit(1);
		}
		optop--;
		*optop = (i4) *optop / (i4) *(optop + 1);
		NEXT();
	ldiv:
		if (*((i8 *) (optop - 1)) == 0) {
			fprintf(stderr, "ArithmeticException: / by zero\n");
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
			fprintf(stderr, "ArithmeticException: %% by zero\n");
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
			fprintf(stderr, "ArithmeticException: %% by zero\n");
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
		// TODO: Implement
		NEXT();
	ishr:
		optop--;
		// only the 5 lowest-order bits are used for shift
		// TODO: arithmetic shift not portable
		*((i4 *) optop) >>= (*(optop + 1) & 0x1f);
		NEXT();
	lshr:
		// TODO: Implement
		NEXT();
	iushr:
		optop--;
		// only the 5 lowest-order bits are used for shift
		*optop >>= (*(optop + 1) & 0x1f);
		NEXT();
	lushr:
		// TODO: Implement
		NEXT();
	iand:
		optop--;
		*optop &= *(optop + 1);
		NEXT();
	land:
		// TODO: Implement
		NEXT();
	ior:
		optop--;
		*optop |= *(optop + 1);
		NEXT();
	lor:
		// TODO: Implement
		NEXT();
	ixor:
		optop--;
		*optop ^= *(optop + 1);
		NEXT();
	lxor:
		// TODO: Implement
		NEXT();
	iinc:
		*(frame + *pc) += (i1) *(pc + 1);
		pc += 2;
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

