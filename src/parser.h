#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;

#define MAGIC (0xCAFEBABE)

#define CONSTANT_Class (0x07)
#define CONSTANT_Fieldref (0x09)
#define CONSTANT_Methodref (0x0A)
#define CONSTANT_InterfaceMethodref (0x0B)
#define CONSTANT_String (0x08)
#define CONSTANT_Integer (0x03)
#define CONSTANT_Float (0x04)
#define CONSTANT_Long (0x05)
#define CONSTANT_Double (0x06)
#define CONSTANT_NameAndType (0x0C)
#define CONSTANT_Utf8 (0x01)
#define CONSTANT_Unicode (0x02)

#define RESOLVED_Class (0x17)
#define RESOLVED_Fieldref (0x19)
#define RESOLVED_Methodref (0x1A)
#define RESOLVED_InterfaceMethodref (0x1B)
#define RESOLVED_String (0x18)
#define RESOLVED_NameAndType (0x1C)
#define RESOLVED_Utf8 (0x11)
#define RESOLVED_Unicode (0x12)

#define IS_RESOLVED(cp, index) (cp[index].tag & 0x10)

#define ACC_PUBLIC (0x0001)
#define ACC_PRIVATE (0x0002)
#define ACC_PROTECTED (0x0004)
#define ACC_STATIC (0x0008)
#define ACC_FINAL (0x0010)
#define ACC_SYNCHRONIZED (0x0020)
#define ACC_VOLATILE (0x0040)
#define ACC_TRANSIENT (0x0080)
#define ACC_NATIVE (0x0100)
#define ACC_INTERFACE (0x0200)
#define ACC_ABSTRACT (0x0400)

struct r_methodref_info {
	struct ClassFile *r_class; // cp?
	struct r_method_info *r_method;
};

struct cp_info {
	u1 tag;
	union {
		// since all 'resolved' structs only have a pointer member,
		// this struct can be used to access this pointer without knowing
		// the type of resolved constant used.
		struct {
			void *ptr;
		} r_any_info;

		struct {
			u2 name_index;
		} class_info;
		struct {
			struct ClassFile *r_class;
		} r_class_info;

		struct {
			u2 class_index;
			u2 name_and_type_index;
		} fieldref_info;
		// TODO: RESOLVED_Fieldref_info

		struct {
			u2 class_index;
			u2 name_and_type_index;
		} methodref_info;
		struct r_methodref_info r_methodref_info;

		struct {
			u2 class_index;
			u2 name_and_type_index;
		} interfaceMethodref_info;
		// TODO: RESOLVED_InterfaceMethodref_info

		struct {
			u2 string_index;
		} string_info;
		struct {
			const char *str;
		} r_string_info;

		struct {
			u4 bytes;
		} integer_info;
		struct {
			u4 bytes;
		} float_info;

		// this struct can be used to access the bytes of a long or double  
		// without knowing the type (long or double).
		struct {
			u4 high_bytes;
			u4 low_bytes;
		} longOrDouble_info;

		struct {
			u4 high_bytes;
			u4 low_bytes;
		} long_info;
		struct {
			u4 high_bytes;
			u4 low_bytes;
		} double_info;

		struct {
			u2 name_index;
			u2 signature_index;
		} nameAndType_info;
		struct {
			const char *name_str;
			const char *signature_str;
		} r_nameAndType_info;

		// Utf8_info is resolved while parsing, so there is no utf8_info struct
		// Utf8 currently not supported, Strings interpreted as ASCII
		struct {
			const char *str;
		} r_utf8_info;

		/* Unicode currently not supported
		struct {
			u2 length;
			u1 *bytes;
		} unicode_info;
		*/
	};
};

struct field_info {
	u2 access_flags;
	u2 name_index;
	u2 signature_index;
	u2 attributes_count;
	struct attribute_info *attributes;
};

struct r_method_info {
	u2 access_flags;
	const char *name;
	const char *signature;
	u1 nargs;
	u2 attributes_count;
	struct attribute_info *attributes;
};

struct ConstantValue_attribute {
	u2 sourcefile_index;
};

struct Code_attribute {
	// extra field args_number, filled with signature
	u2 max_stack;
	u2 max_locals;
	u4 code_length;
	u1 *code;
	u2 exception_table_length;
	/* Currently not supported
	struct exception {
		u2 start_pc;
		u2 end_pc;
		u2 handler_pc;
		u2 catch_type;
	} *exception_table;
	*/
	u2 attributes_count;
	struct attribute_info *attributes;
};

/* Currently not supported
struct SourceFile_attribute {
	u2 sourcefile_index;
};

struct Exceptions_attribute {
	u2 number_of_exceptions;
	u2 *exception_index_table;
};

struct LineNumberTable_Attribute {
	u2 line_number_table_length;
	struct line_number {
		u2 start_pc;
		u2 line_number;
	} *line_number_table;
};

struct LocalVariableTable_attribute {
	u2 local_variable_table_length;
	struct local_variable {
		u2 start_pc;
		u2 length;
		u2 name_index;
		u2 signature_index;
		u2 slot;
	} *local_variable_table;
};
*/

struct attribute_info {
	u2 attribute_name_index;
	u4 attribute_length;
	union {
		struct ConstantValue_attribute constantValue_attribute;
		struct Code_attribute code_attribute;
	};
};

struct ClassFile {
	u4 magic;
	u2 minor_version;
	u2 major_version;
	u2 constant_pool_count;
	struct cp_info *constant_pool; //constant_pool_count - 1 entries;
	u2 access_flags;
	u2 this_class;
	u2 super_class;
	u2 interfaces_count;
	u2 *interfaces;
	u2 fields_count;
	struct field_info *fields;
	u2 methods_count;
	struct r_method_info *methods;
	u2 attributes_count;
	struct attribute_info *attributes;

	const char *name;
	struct ClassFile *next;
};

extern struct ClassFile *parse(const char *filename);

#endif

