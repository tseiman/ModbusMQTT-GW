/*
 * my_alloc.c
 *
 *  Created on: Mar 30, 2022
 *      Author: tseiman
 */
#include <stdlib.h>
#include "log.h"

void *my_malloc(size_t size) {
	void *ref = malloc(size);
	logger(LOG_DEBUG, "Allocating %d bytes at addr %p", size , ref);
	return ref;
}

void *my_realloc(void *ptr, size_t size) {
	void *ref = realloc(ptr, size);
	logger(LOG_DEBUG, "Re-Allocating %d bytes at addr %p", size , ref);
	return ref;
}

void my_free(void * ref) {
	logger(LOG_DEBUG, "Freeing at addr %p",  ref);
	free(ref);
}
