#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

void resolve_const(struct cp_info *cp, u2 index)
{
	switch (cp[index].tag) {
		case CONSTANT_Class:
			// TODO
			break;
		case CONSTANT_Fieldref:
			// TODO
			break;
		case CONSTANT_Methodref:
			// TODO
			break;
		case CONSTANT_InterfaceMethodref:
			// TODO
			break;
		case CONSTANT_String:
			cp[index].tag = RESOLVED_String;
			cp[index].r_string_info.str 
				= cp[cp[index].string_info.string_index].r_utf8_info.str;
			break;
		case CONSTANT_NameAndType:
			// TODO
			break;
		default:
			fprintf(stderr, "Error while trying to resolve constant: Can not resolve constant with tag %i.\n", cp->tag);
			exit(1);
	}
}

