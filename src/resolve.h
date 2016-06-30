#ifndef RESOLVE_H
#define RESOLVE_H

#include "parser.h"

/**
 * Resolves a class. 
 *
 * @detail class may not be already resolved
 *
 * @param cp a pointer to the current class
 * @param index constant pool index of the class to resolve
 * @return a reference to the ClassFile of the resolved class
 */
struct ClassFile *resolve_class(struct ClassFile *curr_class, u2 index);

/**
 * Resolves a nameAndType constant. 
 *
 * @detail nameAndType may not be already resolved
 *
 * @param cp a pointer to the constant pool
 * @param index constant pool index of the nameAndType to resolve
 */
void resolve_nameAndType(struct cp_info *cp, u2 index);

/**
 * Resolves a methodref. 
 *
 * @detail methodref may not be already resolved
 *
 * @param c a pointer to the current class
 * @param index constant pool index of the methodref to resolve
 * @return a reference to the resolved methodref
 */
struct r_methodref_info *resolve_methodref(struct ClassFile *c, u2 index);

struct r_fieldref_info *resolve_fieldref(struct ClassFile *c, u2 index);

/**
 * Resolves a constant - this means that index references to other constants
 * are resolved into actual references e.g. string_index is changed to a pointe
 * to the string, class_index is changed to a pointer to the class file.
 *
 * @detail constant may not be already resolved
 *
 * @param cp a pointer to the current class
 * @param index constant pool index of the constant to resolve
 */
void resolve_const(struct ClassFile *c, u2 index);


#endif
