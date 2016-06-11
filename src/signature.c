#include <stdio.h>
#include <stdlib.h>

#include "signature.h"

/**
 * Calculate the number of arguments of a method signature.
 *
 * @param signature the method signature whose number of arguments should be
 *        calculated
 * @return the number of arguments
 */
u1 calculate_nargs(const char *signature)
{
    u1 nargs = 0;

	int inClassPart = 0;
	int inArrayPart = 0;
    int i = 0;

	if (signature[i++] != '(') {
		fprintf(stderr, "Error during calculation of nargs: Invalid (not starting with '(') signature %s\n", signature);
		exit(1);
	}

	char signature_char;
    while ((signature_char = signature[i++]) != ')') {

		if (inClassPart) {
			if (signature_char == ';') {
				inClassPart = 0;
			}
			continue;
		}

		if (inArrayPart) {
			if (signature_char == SIGNATURE_ARRAY_START) { // nested array
				continue;
			}

			inArrayPart = 0;
			if (signature_char == SIGNATURE_CLASS_START) {
				inClassPart = 1;
			}
			continue;
		}

        switch (signature_char) {
            case SIGNATURE_BYTE:
			case SIGNATURE_CHAR:
			case SIGNATURE_FLOAT:
			case SIGNATURE_INT:
			case SIGNATURE_SHORT:
			case SIGNATURE_BOOLEAN:
				nargs++;
				break;
			case SIGNATURE_LONG:
			case SIGNATURE_DOUBLE:
				nargs += 2;
				break;
			case SIGNATURE_CLASS_START:
				nargs += 1;
				inClassPart = 1;
				break;
			case SIGNATURE_ARRAY_START:
				nargs += 1;
				inArrayPart = 1;
				break;
			default:
				fprintf(stderr, "Error during calculation of nargs: Invalid signature %s\n", signature);
				exit(1);
        }
    }

	return nargs; 
}


