#include "testmode.h"

struct state start_state;

struct state state;

int instrs_until_terminate;

int instrs_counter;

void init_testmode(void) {
	instrs_counter = 0;
}

