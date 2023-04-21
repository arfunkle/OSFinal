/**
 * Handle errors.
 */

#include "error.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"

void err_with_message(const char *message) {
    fprintf(stderr, "shell: %s\n", message);
}

void die_with_message(const char *message) {
    err_with_message(message);
    exit(1);
}

void err_with_errno(const char *s) {
    if (s == NULL || strlen(s) == 0)
        perror("shell: ");
    else {
        char *prefix = alloc(strlen("shell: ") + strlen(s) + 1);
        prefix[0] = 0;
        perror(strcat(strcat(prefix, "shell: "), s));
    }
}

void die_with_errno(const char *s) {
    err_with_errno(s);
    exit(1);
}
