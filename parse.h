#pragma once

/**
 * This is a parser for pipelined commands, or pipelines for short,
 * pioneered by the Unix operating system.  This parser analyzes a
 * simplified form of pipelines compared to what traditional shells
 * (e.g., /bin/sh) typically support.  Following are the supported
 * pipeline forms expressed with the convention used in the synopsis
 * section of man pages (see "man man"):
 *
 *    COMMAND [ > FILE ] [ | COMMAND [ > FILE ] ] ...
 *
 * Each of the strings or tokens comprising the pipeline must be
 * separated by whitespaces and contained in a single line of
 * characters.
 */

struct root; // forward declaration
struct command; // forward declaration

/**
 * Breaks a character string into a linked list of commands suited for
 * the execv family of system calls.
 *
 * The caller must supply a null-terminated character string.  This
 * function returns a pointer to a root structure summarizing the
 * results of the parsing: (1) whether the input string is a valid
 * pipeline and, (2) if valid, a pointer (possibly NULL) to a linked
 * list of commands.  See the comments associated with the root and
 * command structures for more details.
 *
 * This function allocates the various returned structures but the
 * caller is responsible for freeing the memory (e.g., by calling
 * parse_end()).  Also, the function assumes that it can modify the
 * content pointed to by input and that this content will remain live
 * until the returned structures are freed.
 *
 * On unrecoverable errors this function calls die_with_errno(NULL).
 *
 * @param input  a null-terminated character string
 * @return a pointer to a root structure summarizing the parsing results
 */
struct root *parse(char *input);

/**
 * Free all the strutures allocated by a call to parse().
 *
 * @param r  pointer to a root structure
 */
void parse_end(struct root *r);

/**
 * The parser returns a pointer this structure.
 *
 * The structure contains two fields: valid and first_command.
 *
 * If parsing fails (i.e., the pipeline is not well-formed), then
 * valid is set to zero; first_command may or may not be set to NULL.
 *
 * If parsing succeeds, then valid is set to a non-zero value.  If the
 * pipeline is empty (i.e., no commands at all), then first_command is
 * set to NULL.  If the pipeline is non-empty, first_command points to
 * the first command of the pipeline.
 */
typedef struct root {
    int valid;                      ///< non-zero if pipeline is valid
    struct command *first_command;  ///< pointer to first command of pipeline
} root_t;

/**
 * A structure to represent a command in a pipeline.
 *
 * The structure contains five fields: argv, argc, capacity, outfile,
 * and next.
 *
 * The argv field points to an array of pointers to null-terminated
 * strings.  The array is terminated with a NULL pointer.  This array
 * is suitable for calling the execv family of system calls.
 *
 * The capacity and argc fields are for internal use only; do not
 * access them.
 *
 * If the command has its output redirected to a file, outfile points
 * to the name of that file (a null-terminated string), else it
 * contains NULL.
 */
typedef struct command {
    char **argv;            ///< pointer to a simple command
    int argc;               ///< actual number of WORDs in argv
    int capacity;           ///< number of WORDs that can fit in argv
    char *outfile;          ///< if non-NULL, out redirect target
    struct command *next;   ///< if non-NULL, next command in pipeline
} command_t;
