#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "parser.h"
#include "log.h"

struct ClassFile *resolve_class(struct ClassFile *curr_class, u2 index)
{
	DEBUG("Entered %s\n", __func__);

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

	struct ClassFile *loaded_clas = load_class(name);
	loaded_clas->next = clas->next;
	clas->next = loaded_clas;
	cp[index].tag = RESOLVED_Class;
	cp[index].r_class_info.r_class = loaded_clas;
	return loaded_clas; 
}

void resolve_nameAndType(struct cp_info *cp, u2 index)
{
	DEBUG("Entered %s\n", __func__);

	const char *name_str
		= cp[cp[index].nameAndType_info.name_index].r_utf8_info.str;
	const char *signature_str
		= cp[cp[index].nameAndType_info.signature_index].r_utf8_info.str;

	cp[index].r_nameAndType_info.name_str = name_str; 
	cp[index].r_nameAndType_info.signature_str = signature_str;
	cp[index].tag = RESOLVED_NameAndType;
}

struct r_methodref_info *resolve_methodref(struct ClassFile *c, u2 index)
{
	DEBUG("Entered %s\n", __func__);

	struct cp_info *cp = c->constant_pool;

	struct ClassFile *m_class = c;

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
			cp[index].tag = RESOLVED_Methodref;
			return &cp[index].r_methodref_info;
		}
	}

	ERROR(
		"Error while trying to resolve method: Method %s%s not found in class.\n", 
		m_name, m_signature);
	exit(1);

	return NULL;
}

struct r_fieldref_info *resolve_fieldref(struct ClassFile *c, u2 index)
{
	DEBUG("Entered %s\n", __func__);

	struct cp_info *cp = c->constant_pool;

	struct ClassFile *f_class = c;

	if (!IS_RESOLVED(cp, cp[index].fieldref_info.class_index)) {
		f_class = resolve_class(c, cp[index].fieldref_info.class_index); 
	}

	int signature_index = cp[index].fieldref_info.name_and_type_index;
	if (!IS_RESOLVED(cp, signature_index)) {
		resolve_nameAndType(cp, signature_index);
	}

	const char *f_name = cp[signature_index].r_nameAndType_info.name_str;
	const char *f_signature
		= cp[signature_index].r_nameAndType_info.signature_str;

	int i;
	struct r_field_info f;
	for (i = 0; i < f_class->fields_count; i++) {
		f = f_class->fields[i];
		if (f.name == f_name && f.signature == f_signature) {
			cp[index].r_fieldref_info.r_class = f_class;
			cp[index].r_fieldref_info.r_field = &f;
			cp[index].tag = RESOLVED_Fieldref;
			return &cp[index].r_fieldref_info;
		}
	}

	ERROR(
		"Error while trying to resolve field: Field %s %s not found in class.\n",
		f_signature, f_name);
	exit(1);

	return NULL;
}

void resolve_const(struct ClassFile *c, u2 index)
{
	DEBUG("Entered %s\n", __func__);

	struct cp_info *cp = c->constant_pool;

	switch (cp[index].tag) {
		case CONSTANT_Class:
			(void) resolve_class(c, index);
			break;
		case CONSTANT_Fieldref:
			(void) resolve_fieldref(c, index);
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
			ERROR("Error while trying to resolve constant: Can not resolve constant with tag %i.\n", cp->tag);
			exit(1);
	}
}

