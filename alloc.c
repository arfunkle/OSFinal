/**
 * Support for simple memory allocation.
 */

#include "alloc.h"

#include <stddef.h>
#include <stdlib.h>

#include "error.h"

void *alloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) die_with_message("shell: Memory exhausted\n");
    return p;
}

void *realloc_array(void *ptr, size_t nmemb, size_t size) {
    void *p = realloc(ptr, nmemb*size);
    if (p == NULL) die_with_message("shell: Memory exhausted\n");
    return p;
}
