#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
    char *path;

    if (argc == 1) {
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "%s: HOME not set\n", argv[0]);
            exit(1);
        }
    }
    else {
        path = argv[1];
    }

    if (chdir(path) == -1) {
        char *prefix = malloc(strlen(argv[0]) + 3);
        if (prefix == NULL) {
            fprintf(stderr, "%s: Memory exhausted\n", argv[0]);
            exit(1);
        }
        sprintf(prefix, "%s: ", argv[0]);
        perror(prefix);
    }
}
