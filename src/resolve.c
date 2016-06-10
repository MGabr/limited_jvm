#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "parser.h"

/**
 * Resolves a class. 
 *
 * @detail class may not be already resolved
 *
 * @param cp a pointer to the constant pool
 * @param index constant pool index of the class to resolve
 * @return a reference to the ClassFile of the resolved class
 */
struct ClassFile *resolve_class(struct ClassFile *curr_class, u2 index)
{
	struct cp_info *cp = curr_class->constant_pool;

	const char *name = cp[cp[index].class_info.name_index].r_utf8_info.str;
	const char *curr_name = curr_class->name;

	if (curr_name == name) {
		cp[index].tag = RESOLVED_Class;
		cp[index].r_class_info.r_class = curr_class;
		return curr_class;
	}

	struct ClassFile *clas = curr_class;
	while ((clas = clas->next) != curr_class) {
		if (clas->name == name) {
			cp[index].tag = RESOLVED_Class;
			cp[index].r_class_info.r_class = clas;
			return clas;
		}
	}

	fprintf(stderr, "Error while trying to resolve class %s: Class loading not supported yet.\n", name);
	exit(1);
	return NULL; // TODO: CLASS LOADING
}

/**
 * Resolves a nameAndType constant. 
 *
 * @detail nameAndType may not be already resolved
 *
 * @param cp a pointer to the constant pool
 * @param index constant pool index of the nameAndType to resolve
 */
void resolve_nameAndType(struct cp_info *cp, u2 index)
{
	const char *name_str
		= cp[cp[index].nameAndType_info.name_index].r_utf8_info.str;
	const char *signature_str
		= cp[cp[index].nameAndType_info.signature_index].r_utf8_info.str;

	cp[index].r_nameAndType_info.name_str = name_str; 
	cp[index].r_nameAndType_info.signature_str = signature_str;
}

/**
 * Resolves a methodref. 
 *
 * @detail methodref may not be already resolved
 *
 * @param c a pointer to the current class
 * @param index constant pool index of the methodref to resolve
 * @return a reference to the resolved methodref
 */
struct r_methodref_info *resolve_methodref(struct ClassFile *c, u2 index)
{
	struct cp_info *cp = c->constant_pool;

	struct ClassFile *m_class;

	if (!IS_RESOLVED(cp, cp[index].methodref_info.class_index)) {
		m_class = resolve_class(c, cp[index].methodref_info.class_index);
	}
	
	int signature_index = cp[index].methodref_info.name_and_type_index;
	if (!IS_RESOLVED(cp, signature_index)) {
		resolve_nameAndType(cp, signature_index);
	}

	const char *m_name = cp[signature_index].r_nameAndType_info.name_str;
	const char *m_signature 
		= cp[signature_index].r_nameAndType_info.signature_str;

	int i;
	struct r_method_info m;
	for (i = 0; i < m_class->methods_count; i++) {
		m = m_class->methods[i];
		if (m.name == m_name && m.signature == m_signature) {
			cp[index].r_methodref_info.r_class = m_class;
			cp[index].r_methodref_info.r_method = &m;
			return &cp[index].r_methodref_info;
		}
	}

	fprintf(stderr, "Error while trying to resolve method: Method %s%s not found in class.\n", m_name, m_signature);
	exit(1);

	return NULL;
}

/**
 * Resolves a constant - this means that index references to other constants
 * are resolved into actual references e.g. string_index is changed to a pointe
 * to the string, class_index is changed to a pointer to the class file.
 *
 * @detail constant may not be already resolved
 *
 * @param cp a pointer to the constant pool
 * @param index constant pool index of the constant to resolve
 */
void resolve_const(struct ClassFile *c, u2 index)
{
	struct cp_info *cp = c->constant_pool;

	switch (cp[index].tag) {
		case CONSTANT_Class:
			(void) resolve_class(c, index);
			break;
		case CONSTANT_Fieldref:
			// TODO
			break;
		case CONSTANT_Methodref:
			(void) resolve_methodref(c, index);
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
			resolve_nameAndType(cp, index);
			break;
		default:
			fprintf(stderr, "Error while trying to resolve constant: Can not resolve constant with tag %i.\n", cp->tag);
			exit(1);
	}
}

