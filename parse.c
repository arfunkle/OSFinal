/*
 * This parser recognizes valid pipelines using a top-down technique
 * called recursive descent.
 *
 * The valid forms of pipelines supported by this parser corresponds
 * to this grammar:
 *
 * pipeline -> ε
 * pipeline -> command
 * pipeline -> pipeline '|' command
 * command -> simple_command
 * command -> simple_command '>' WORD
 * simple_command -> WORD
 * simple_command -> simple_command WORD
 *
 * To built a corresponding recursive descent parser, we must
 * transform the above grammar to an LL(1) equivalent grammar:
 *
 * pipeline -> ε
 * pipeline -> command pipeline'
 * pipeline' -> '|' command pipeline'
 * pipeline' -> ε
 * command -> simple_command
 * command -> simple_command '>' WORD
 * simple_command -> WORD simple_command'
 * simple_command' -> WORD simple_command'
 * simple_command' -> ε
 *
 * For details of this transformation, see, for example, Introduction
 * to Compilers and Language Design, Douglas Thain, Second edition,
 * Chapter 4 -- Parsing, http://compilerbook.org.
 *
 * We can then write the parser rather effortlessly:
 *
 * int parse_pipeline() {
 *     token_t t = get_token();
 *     if (t.type == TOKEN_EOF) return 1;
 *     putback_token(t);
 *     return parse_command() && parse_pipeline_prime() && expect_token(TOKEN_EOF);
 * }
 *
 * int parse_pipeline_prime() {
 *     token_t t = get_token();
 *     if (t.type == TOKEN_PIPE)
 *         return parse_command() && parse_pipeline_prime();
 *     else {
 *         putback_token(t);
 *         return 1;
 *     }
 * }
 *
 * int parse_command() {
 *    if (parse_simple_command()) {
 *        token_t t = get_token();
 *        if (t.type == TOKEN_OUT_REDIRECT) return expect_token(TOKEN_WORD);
 *        putback_token(t);
 *        return 1;
 *    }
 *    return 0;
 * }
 *
 * int parse_simple_command() {
 *     return expect_token(TOKEN_WORD) && parse_simple_command_prime();
 * }
 *
 * int parse_simple_command_prime() {
 *     token_t t = get_token();
 *     if (t.type == TOKEN_WORD) return parse_simple_command_prime();
 *     putback_token(t);
 *     return 1;
 * }
 *
 * This code outline only checks that the input conforms to the
 * grammar.  A fully-fleshed code must also return a representation
 * (combination of struct root and struct commands) of the input that
 * can be then be used to perform the associated actions.  Filling
 * this representation requires at times to break some single
 * expressions into multiple steps.  For details compare the outline
 * above with the actual code below.
 */

#include "parse.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "alloc.h"
#include "error.h"

/**
 * A list of the different types of token observed during parsing.
 */
typedef enum {
      TOKEN_NONE,
      TOKEN_EOF,            ///< end-of-file
      TOKEN_PIPE,           ///< pipeline operator, i.e., '|'
      TOKEN_OUT_REDIRECT,   ///< output redirection, i.e., '<'
      TOKEN_WORD,           ///< a WORD
} token_type_t;

/**
 * This structure represents a token.
 */
typedef struct {
    token_type_t type;      ///< token type
    char *begin;            ///< beginning of token in input (if type is WORD)
} token_t;

/**
 * This structure represents the state of the parser.
 */
typedef struct {
    char *input;                ///< pointer to the input
    token_t prev_token;         ///< putback token if type is not NONE
    root_t *root;               ///< pointer to the parsing state
    command_t *current_command; ///< command currently being parsed
} parser_t;

// Forward declaration of local functions.
int parse_pipeline(parser_t *p);
int parse_pipeline_prime(parser_t *p);
int parse_command(parser_t *p);
int parse_simple_command(parser_t *p);
int parse_simple_command_prime(parser_t *p);
token_t get_token(parser_t *p);
void putback_token(parser_t *p, token_t t);
int expect_token(parser_t *p, token_type_t type);
void add_root(parser_t *p);
void add_command(parser_t *p);
void add_word_to_command(parser_t *p, token_t t);
void add_NULL_to_command(parser_t *p);
void check_capacity(parser_t *p);
void add_outfile(parser_t *p, token_t t);
void free_command(struct command *c);


root_t *parse(char *input) {
    parser_t parser;

    parser.input = input;
    parser.prev_token.type = TOKEN_NONE;
    add_root(&parser);
    parser.current_command = NULL;

    parser.root->valid = parse_pipeline(&parser);

    return parser.root;
}

void parse_end(struct root *r) {
    if (r == NULL) return;
    if (r->first_command != NULL) {
        free_command(r->first_command);
    }
    free(r);
}


int parse_pipeline(parser_t *p) {
    token_t t = get_token(p);
    if (t.type == TOKEN_EOF) return 1;
    putback_token(p, t);
    if (!parse_command(p) || !parse_pipeline_prime(p))
        return 0;
    if (expect_token(p, TOKEN_EOF)) {
        add_NULL_to_command(p);
        return 1;
    }
    return 0;
}

int parse_pipeline_prime(parser_t *p) {
    token_t t = get_token(p);
    if (t.type == TOKEN_PIPE) {
        add_NULL_to_command(p);
        return parse_command(p) && parse_pipeline_prime(p);
    }
    putback_token(p, t);
    return 1;
}

int parse_command(parser_t *p) {
    add_command(p);
    if (parse_simple_command(p)) {
        token_t t = get_token(p);
        if (t.type == TOKEN_OUT_REDIRECT) {
            token_t t = get_token(p);
            if (t.type == TOKEN_WORD) {
                add_outfile(p, t);
                return 1;
            }
            return 0;
        }
        putback_token(p, t);
        return 1;
    }
    return 0;
}

int parse_simple_command(parser_t *p) {
    token_t t = get_token(p);
    if (t.type == TOKEN_WORD) {
        add_word_to_command(p, t);
        return parse_simple_command_prime(p);
    }
    return 0;
}

int parse_simple_command_prime(parser_t *p) {
    token_t t = get_token(p);
    if (t.type == TOKEN_WORD) {
        add_word_to_command(p, t);
        return parse_simple_command_prime(p);
    }
    putback_token(p, t);
    return 1;
}

token_t get_token(parser_t *p) {
    token_t t;

    // Check for a recently putback token.
    if (p->prev_token.type != TOKEN_NONE) {
        t = p->prev_token;
        p->prev_token.type = TOKEN_NONE;
        return t;
    }

    // Skip whitespace.
    while (*p->input && isspace(*p->input)) ++p->input;
    assert(!*p->input || !isspace(*p->input));

    // Check for EOF.
    // Notes:
    //   . has higher precedence than *
    //   ! and * have same precedence but have right-to-left associativity
    //   !*p->input evaluates to true if p->input points to null ASCII char.
    if (!*p->input) {
        t.type = TOKEN_EOF;
        return t;
    }

    // Find a token.
    t.begin = p->input;
    ++p->input;
    while (*p->input && !isspace(*p->input)) ++p->input;
    char *end = p->input;
    assert(end > t.begin && (!*p->input || isspace(*p->input)));

    // Set token type.
    if (strncmp(t.begin, "|", end - t.begin) == 0)
        t.type = TOKEN_PIPE;
    else if (strncmp(t.begin, ">", end - t.begin) == 0)
        t.type = TOKEN_OUT_REDIRECT;
    else
        t.type = TOKEN_WORD;

    // Null-terminate token.
    if (*p->input) *p->input++ = '\0';

    return t;
}

void putback_token(parser_t *p, token_t t) {
    assert(p->prev_token.type == TOKEN_NONE && t.type != TOKEN_NONE);
    p->prev_token = t;
}

int expect_token(parser_t *p, token_type_t type) {
    token_t t = get_token(p);
    if (t.type == type) return 1;
    putback_token(p, t);
    return 0;
}

void add_root(parser_t *p) {
    p->root = alloc(sizeof(root_t));
    p->root->valid = 0;
    p->root->first_command = NULL;
}

void add_command(parser_t *p) {
    command_t *c = alloc(sizeof(command_t));
    c->argv = alloc(sizeof(char *));
    c->argc = 0;
    c->capacity = 1;
    c->next = NULL;
    c->outfile = NULL;
    if (p->current_command == NULL) p->root->first_command = c;
    else                            p->current_command->next = c;
    p->current_command = c;
}

void add_word_to_command(parser_t *p, token_t t) {
    check_capacity(p);
    p->current_command->argv[p->current_command->argc] = t.begin;
    ++p->current_command->argc;
}

void add_NULL_to_command(parser_t *p) {
    check_capacity(p);
    p->current_command->argv[p->current_command->argc] = NULL;
    ++p->current_command->argc;
}

void check_capacity(parser_t *p) {
    int index = p->current_command->argc;
    assert(index <= p->current_command->capacity);
    if (index == p->current_command->capacity) {
        p->current_command->argv = realloc_array(p->current_command->argv,
                                                 2*p->current_command->capacity,
                                                 sizeof(char *));
        if (p->current_command->argv == NULL) die_with_errno(NULL);
        p->current_command->capacity *= 2;
    }
}

void add_outfile(parser_t *p, token_t t) {
    p->current_command->outfile = t.begin;
}

void free_command(struct command *c) {
    if (c->next != NULL) free_command(c->next);
    if (c->argv != NULL) free(c->argv);
    free(c);
}
