#include <stdio.h>
#include <stdlib.h>
#include <byteswap.h>
#include <string.h>
#include "parser.h"

int little_endian = 0;

/*Read a byte from a file into a buffer.
 *
 * @param ptr the pointer to the buffer to fill
 * @param stream the file from which to read
 */
static size_t read_e8(u1 *ptr, FILE *stream)
{
	return fread(ptr, sizeof(u1), 1, stream);
}

/**
 * Read 2 bytes from a file into a buffer considering the endianness of the 
 * machine.
 *
 * @detail checks little_endian to determine the endianness
 *
 * @param ptr the pointer to the buffer to fill
 * @param stream the file from which to read
 */
static size_t read_e16(u2 *ptr, FILE *stream)
{
	size_t ret = fread(ptr, sizeof(u2), 1, stream);
	if (little_endian) {
		*ptr = bswap_16(*ptr);
	}
	return ret;
}

/**
 * Read 4 bytes from a file into a buffer considering the endianness of the 
 * machine.
 *
 * @detail checks little_endian to determine the endianness
 *
 * @param ptr the pointer to the buffer to fill
 * @param stream the file from which to read
 */
static size_t read_e32(u4 *ptr, FILE *stream)
{
	size_t ret = fread(ptr, sizeof(u4), 1, stream);
	if (little_endian) {
		*ptr = bswap_32(*ptr);
	}
	return ret;
}

static void parse_constant(struct cp_info *cp, FILE *fp)
{
	int i;
	read_e8(&cp->tag, fp);
	switch (cp->tag) {
		case CONSTANT_Class:
			read_e16(&cp->class_info.name_index, fp);
			break;
		case CONSTANT_Fieldref:
			read_e16(&cp->fieldref_info.class_index, fp);
			read_e16(&cp->fieldref_info.name_and_type_index, fp);
			break;
		case CONSTANT_Methodref:
			read_e16(&cp->methodref_info.class_index, fp);
			read_e16(&cp->methodref_info.name_and_type_index, fp);
			break;
		case CONSTANT_InterfaceMethodref:
			read_e16(&cp->interfaceMethodref_info.class_index, fp);
			read_e16(&cp->interfaceMethodref_info.name_and_type_index, fp);
			break;
		case CONSTANT_String:
			read_e16(&cp->string_info.string_index, fp);
			break;
		case CONSTANT_Integer:
			read_e32(&cp->integer_info.bytes, fp);
			break;
		case CONSTANT_Float:
			read_e32(&cp->float_info.bytes, fp);
			break;
		case CONSTANT_Long:
			read_e32(&cp->long_info.high_bytes, fp);
			read_e32(&cp->long_info.low_bytes, fp);
			break;
		case CONSTANT_NameAndType:
			read_e16(&cp->nameAndType_info.name_index, fp);
			read_e16(&cp->nameAndType_info.signature_index, fp);
			break;
		case CONSTANT_Utf8:
			read_e16(&cp->utf8_info.length, fp);
			cp->utf8_info.bytes = malloc(
				sizeof(u1) * cp->utf8_info.length);
			for (i = 0; i < cp->utf8_info.length; i++) {
				read_e8(&cp->utf8_info.bytes[i], fp);
			}
			break;
		case CONSTANT_Unicode:
			read_e16(&cp->unicode_info.length, fp);
			cp->unicode_info.bytes = malloc(
				sizeof(u1) * cp->unicode_info.length);
			for (i = 0; i < cp->unicode_info.length; i++) {
				read_e8(&cp->unicode_info.bytes[i], fp);
			}
			break;
		default:
			fprintf(stderr, "Wrong constant type tag %u\n", cp->tag);
			exit(1);
	}
}

static void parse_ConstantValue_attribute(struct ConstantValue_attribute *cv,
	FILE *fp)
{
	read_e16(&cv->sourcefile_index, fp);
}

// cyclic calls between parse_attribute and parse_Code_attribute
static void parse_attribute(struct attribute_info *att, struct cp_info *cp, 
	FILE *fp);

static void parse_Code_attribute(struct Code_attribute *code,struct cp_info *cp,
	FILE *fp)
{
	read_e16(&code->max_stack, fp);
	read_e16(&code->max_locals, fp);

	read_e32(&code->code_length, fp);
	code->code = malloc(sizeof(u1) * code->code_length);
	fread(&code->code, code->code_length, 1, fp);

	read_e16(&code->exception_table_length, fp);
	// exception table currently not supported
	fseek(fp, code->exception_table_length * 4 * 2, SEEK_CUR); // 4 u2 fields

	read_e16(&code->attributes_count, fp);
	code->attributes = malloc (
		sizeof(struct attribute_info) * code->attributes_count);
	int i;
	for (i = 0; i < code->attributes_count; i++) {
		parse_attribute(&code->attributes[i], cp, fp);
	}
}

static void parse_attribute(struct attribute_info *att, struct cp_info *cp, 
	FILE *fp)
{
	if (cp[att->attribute_name_index].tag != CONSTANT_Utf8) {
		fprintf(stderr, "Corrupted class file: Wrong constant type tag %u for attribute_name_index, %u required.\n", att->attribute_name_index, CONSTANT_Utf8);
		exit(1);
	}

	char *att_name
		= (char *) cp[att->attribute_name_index].utf8_info.bytes;
	if (!strcmp(att_name, "ConstantValue")) {
		parse_ConstantValue_attribute(&att->constantValue_attribute, fp);
	} else if (!strcmp(att_name, "Code")) {
		parse_Code_attribute(&att->code_attribute, cp, fp);
	} else {
		// other attributes currently not supported 
		fseek(fp, att->attribute_length, SEEK_CUR);
	}
}

static void parse_field(struct field_info *f, struct cp_info *cp, FILE *fp)
{
	read_e16(&f->access_flags, fp);
	read_e16(&f->name_index, fp);
	read_e16(&f->signature_index, fp);

	read_e16(&f->attributes_count, fp);
	f->attributes = malloc(sizeof(struct attribute_info) * f->attributes_count);
	int i;
	for (i = 0; i < f->attributes_count; i++) {
		parse_attribute(&f->attributes[i], cp, fp);
	}
}

static void parse_method(struct method_info *m, struct cp_info *cp, FILE *fp)
{
	read_e16(&m->access_flags, fp);
	read_e16(&m->name_index, fp);
	read_e16(&m->signature_index, fp);
	
	read_e16(&m->attributes_count, fp);
	m->attributes = malloc(
		sizeof(struct attribute_info) * m->attributes_count);
	int i;
	for (i = 0; i < m->attributes_count; i++) {
		parse_attribute(&m->attributes[i], cp, fp);
	}
}

static void parse_constant_pool(struct ClassFile *cf, FILE *fp)
{
	read_e16(&cf->constant_pool_count, fp);
	cf->constant_pool = malloc(
		sizeof(struct cp_info) * (cf->constant_pool_count - 1));
	int i;
	for (i = 1; i < cf->constant_pool_count; i++) {
		parse_constant(&cf->constant_pool[i], fp);
	}
}

static void parse_interfaces(struct ClassFile *cf, FILE *fp)
{
	read_e16(&cf->interfaces_count, fp);
	cf->interfaces = malloc(sizeof(u2) * cf->interfaces_count);
	int i;
	for (i = 0; i < cf->interfaces_count; i++) {
		read_e16(&cf->interfaces[i], fp);
	}
}

static void parse_fields(struct ClassFile *cf, FILE *fp)
{
	read_e16(&cf->fields_count, fp);
	cf->fields = malloc(sizeof(struct field_info) * cf->fields_count);
	int i;
	for (i = 0; i < cf->fields_count; i++) {
		parse_field(&cf->fields[i], cf->constant_pool, fp);
	}
}

static void parse_methods(struct ClassFile *cf, FILE *fp)
{
	read_e16(&cf->methods_count, fp);
	cf->methods = malloc(sizeof(struct method_info) * cf->methods_count);
	int i;
	for (i = 0; i < cf->methods_count; i++) {
		parse_method(&cf->methods[i], cf->constant_pool, fp);
	}
}

static void parse_attributes(struct ClassFile *cf, FILE *fp)
{
	read_e16(&cf->attributes_count, fp);
	cf->attributes = malloc(
		sizeof(struct attribute_info) * cf->attributes_count);
	int i;
	for (i = 0; i < cf->attributes_count; i++) {
		parse_attribute(&cf->attributes[i], cf->constant_pool, fp);
	}
}

struct ClassFile *parse(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	struct ClassFile *cf = malloc(sizeof(struct ClassFile));


	fread(&cf->magic, sizeof(u4), 1, fp);
	if (cf->magic != MAGIC) {
		little_endian = 1; 
		cf->magic = bswap_32(cf->magic);
		if (cf->magic != MAGIC) {
			fprintf(stderr, "Not a class file: File does not begin with the magic number, but with %04x",  cf->magic);
			exit(1);
		}
	}

	read_e16(&cf->minor_version, fp);
	read_e16(&cf->major_version, fp);

	parse_constant_pool(cf, fp);	

	read_e16(&cf->access_flags, fp);
	read_e16(&cf->this_class, fp);
	read_e16(&cf->super_class, fp);

	parse_interfaces(cf, fp);
	parse_fields(cf, fp);
	parse_methods(cf, fp);
	parse_attributes(cf, fp);

	return cf;
}

/*
int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <classfile name>\n", argv[0]);
	}
	parse(argv[1]);
}
*/
