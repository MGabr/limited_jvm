/**
 * @file parser.h
 * @author Markus Gabriel
 * @brief Definitions for class file and internal VM structures
 *
 * Definitions of class file structures and internal custom structures used 
 * throughout the whole virtual machine.
 * Methods for parsing class files into these structures and loading classes
 * in general.
 */
#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

typedef int8_t i1;
typedef int16_t i2;
typedef int32_t i4;
typedef int64_t i8;

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

// resolved by default
#define RESOLVED_Integer (0x13)
#define RESOLVED_Float (0x14)
#define RESOLVED_Long (0x15)
#define RESOLVED_Double (0x16)

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

#define IS_NATIVE(method) (method->access_flags & ACC_NATIVE)

// array types
#define T_BOOLEAN (4)
#define T_CHAR (5)
#define T_FLOAT (6)
#define T_DOUBLE (7)
#define T_BYTE (8)
#define T_SHORT (9)
#define T_INT (10)
#define T_LONG (11)


struct r_methodref_info {
	struct ClassFile *r_class;
	struct r_method_info *r_method;
};

struct r_fieldref_info {
	struct ClassFile *r_class;
	struct r_field_info *r_field;
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
		struct r_fieldref_info r_fieldref_info;

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
			u4 first_bytes;
			// u4 low_bytes; this field is saved right after the first byte 
			// overwriting parts of the next cp entry
		} longOrDouble_info;

		struct {
			u4 first_bytes;
			// u4 low_bytes; this field is saved right after the first byte 
			// overwriting parts of the next cp entry
		} long_info;
		struct {
			u4 first_bytes;
			// u4 low_bytes; this field is saved right after the first byte
			// overwriting parts of the next cp entry
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

struct r_field_info {
	u2 access_flags;
	const char *name;
	const char *signature;
	u2 attributes_count;
	struct attribute_info *attributes;

	u4 byte1;
	u4 byte2;
};

struct r_method_info {
	u2 access_flags;
	const char *name;
	const char *signature;
	u1 nargs;
	struct Code_attribute *c_attr; // shortcut to code attribute
	u2 attributes_count;
	struct attribute_info *attributes;
};

struct ConstantValue_attribute {
	u2 sourcefile_index;
};

struct Code_attribute {
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
	struct r_field_info *fields;
	u2 methods_count;
	struct r_method_info *methods;
	u2 attributes_count;
	struct attribute_info *attributes;

	const char *name;
	struct ClassFile *next;
	u1 static_initialized;
};

/**
 * Parses the file with the given filename into a class file structure.
 *
 * @detail This is DEPRECATED, use load_class() instead
 *
 * @param filename the name of the file to parse into a class file structure
 * @return the created class file
 */
extern struct ClassFile *parse(const char *filename);

/**
 * Loads the class with the given class name considering the classpath settings.
 *
 * @param classname the name of the class to load, can contain packages/paths
 *                  separated by '/'
 * @return the class structure
 */
extern struct ClassFile *load_class(const char *classname);

/**
 * Links a new, not already linked class to the existing classes.
 *
 * @param any_c any already linked class
 * @param new_c the new class to link to the other existing classes
 */
extern void link_class(struct ClassFile *any_c, struct ClassFile *new_c);

/**
 * Frees the given class structure.
 *
 * @detail Does not free any strings from the string pool and does currently
 *		   also NOT UNLINK the class, so don't free classes linked to other
 *		   classes still used by the VM. In most cases free_all_linked_classes()
 *		   will be the more suitable method.
 *
 * @param cf the class structure to free
 */
extern void free_class(struct ClassFile *cf);

/**
 * Frees the given class structure and all classes linked to it.
 *
 * @detail Does not free any strings from the string pool.
 *
 * @param cf the class structure to free 
 */
extern void free_all_linked_classes(struct ClassFile *cf);

#endif

