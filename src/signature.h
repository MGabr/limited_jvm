#include "parser.h"

#define SIGNATURE_BYTE ('B')
#define SIGNATURE_CHAR ('C')
#define SIGNATURE_FLOAT ('F')
#define SIGNATURE_INT ('I')
#define SIGNATURE_SHORT ('S')
#define SIGNATURE_BOOLEAN ('Z')
#define SIGNATURE_LONG ('J')
#define SIGNATURE_DOUBLE ('D')
#define SIGNATURE_CLASS_START ('L')
#define SIGNATURE_ARRAY_START ('[')

/**
 * Calculate the number of arguments of a method signature.
 *
 * @param signature the method signature whose number of arguments should be
 *        calculated
 * @return the number of arguments
 */
u1 calculate_nargs(const char *signature);

