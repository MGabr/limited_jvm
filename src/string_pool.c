#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "log.h"


struct bucket *hashtable;

struct bucket {
	struct bucket *next;
	const char *str;
};

/*
 * djb2 hash function by Dan Bernstein
 * http://stackoverflow.com/a/7666577
 */
unsigned long hash(unsigned const char *str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}


void init_string_pool(void)
{
	DEBUG("Entered %s\n", __func__);

	// zero initialize, so all next pointers and all strings are NULL
	hashtable = calloc(nr_string_buckets, sizeof(struct bucket));
}

const char *add_string(const char *str)
{
	DEBUG("Entered %s\n", __func__);

	long h = hash((unsigned const char *) str) % nr_string_buckets;
	struct bucket *b = &hashtable[h];

	if (b->str == NULL) { // free bucket
		b->str = str;
		return b->str;
	}

	do {
		if (!strcmp(b->str, str)) { // string already in hashtable
			return b->str;
		}
	} while (b->next != NULL && (b = b->next));

	b->next = malloc(sizeof(struct bucket));
	b->next->next = NULL;
	b->next->str = str;
	return b->next->str;
}

const char *find_string(const char *str)
{
	DEBUG("Entered %s\n", __func__);

	long h = hash((unsigned const char *) str) % nr_string_buckets;
	struct bucket *b = &hashtable[h];

	if (b->str == NULL) {
		return NULL;
	}

	do {
		if (!strcmp(b->str, str)) {
			return b->str;
		}
	} while ((b = b->next) != NULL);

	return NULL;
}

