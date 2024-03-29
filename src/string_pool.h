/**
 * @file string_pool.h 
 * @author Markus Gabriel
 * @brief String pool implementation
 *
 */
#ifndef STRING_POOL_H
#define STRING_POOL_H

/**
 * Initialize the string pool.
 *
 * @detail Does nothing if the string pool has already been initialized
 */
extern void init_string_pool(void);

/**
 * Frees the string pool.
 *
 * @detail Does not free the strings in the string pool, since they might not be
 *		   allocated with malloc(), e.g. string literals
 */
extern void free_string_pool(void);

/**
 * Add a string to the string pool.
 *
 * @detail If there is no string with the same name, a reference to this 
 * 		   string is added to the string pool. In this case, DO NOT FREE the 
 *		   input string memory since the string will be used in the string pool
 *
 * @param str the string to add
 * @return the adress of a string with the same name or the address of the 
 *		   input string if there is no such string
 */
extern const char *add_string(const char *str);

/**
 * Find a string in the string pool.
 *
 * @param str the string to find
 * @return the adress of a string with the same name in the string pool or NULL
 *		   if there is no such string
 */
extern const char *find_string(const char *str);

#endif

