#pragma once

#include <stddef.h>

/**
 * Allocates some memory.
 *
 * This function prints an error message and exits with status 1 on
 * failures.
 *
 * @param size  number of bytes to allocate
 * @return a pointer to the allocated region
 */
void *alloc(size_t size);


/**
 * Changes the size of the memory block pointed to by ptr to be large
 * enough for nmemb elements, each of which is size bytes.
 *
 * This function prints an error message and exits with status 1 on
 * failures.
 *
 * @param ptr  pointer to the original memory block
 * @param nmemb  number of elements
 * @param size  size of each elements
 * @return a pointer to the reallocated array
 */
void *realloc_array(void *ptr, size_t nmemb, size_t size);
