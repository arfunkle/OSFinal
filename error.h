#pragma once

/**
 * Prints an error message.
 *
 * The string "shell: " is prepended to the error message.
 *
 * @param message  a non-empty error message
 */
void err_with_message(const char *message);

/**
 * Prints an error message and exits with status 1.
 *
 * The string "shell: " is prepended to the error message.
 *
 * @param message  a non-empty error message
 */
void die_with_message(const char *message);

/**
 * Calls perror() with a prefix.
 *
 * The string "shell: " is prepended to the error message.  If s is
 * not empty, s, followed by ": ", is placed between "shell: " and the
 * error mesage corresponding to the current value of the global
 * variable errno.
 *
 * This function should only be called after a library call that sets
 * errno.
 *
 * @param s  a string or NULL
 */
void err_with_errno(const char *s);

/**
 * Calls perror() with a prefix and exits with status 1.
 *
 * The string "shell: " is prepended to the error message.  If s is
 * not NULL, s, followed by ": ", is placed between "shell: " and the
 * error mesage corresponding to the current value of the global
 * variable errno.
 *
 * This function should only be called after a library call that sets
 * errno.
 *
 * @param s  a string or NULL
 */
void die_with_errno(const char *s);
