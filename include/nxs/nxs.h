#ifndef NXS_H
#define NXS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace niu2x::nxs {

enum {
    TK_ID = 256,
    TK_IMPORT,
    TK_FUNCTION,
    TK_INTEGER,
    TK_DOUBLE,
    TK_STRING,
    TK_EOF,
    TK_UNKNOWN,
};

const char* token_name(int tk);

struct token_t {
    int type;
    union {
        int i;
        double d;
        char* s;
    } value;
};
void token_value_snprintf(struct token_t* tk, char* buf, int size);
void token_free(struct token_t* tk);

struct stream_t;

typedef int (*stream_read_more_t)(struct stream_t*, int size);

struct stream_t {
    char* base;
    int pos;
    int size;
    stream_read_more_t read_more;
};

int stream_getchar(struct stream_t*);
int stream_ungetchar(struct stream_t*);

struct token_t lexer(struct stream_t*);

struct buffer_t {
    char* base;
    int size;
    int capacity;
};
const extern char* empty_str;
#define NXS_BUFFER_INIT                                                        \
    {                                                                          \
        (char*)empty_str, 0, 0                                                 \
    }

void buffer_append(struct buffer_t* self, char c);
void buffer_free(struct buffer_t* self);
int buffer_to_integer(struct buffer_t* self);
double buffer_to_double(struct buffer_t* self);
char* buffer_to_string(struct buffer_t* self);

#define NXS_CAPACITY_EXPAND(capacity, delta) ((capacity) + (delta)) * 3 / 2
#define NXS_CHECK_RESULT(exp, message)                                         \
    if ((exp) < 0) {                                                           \
        fprintf(stderr, "%s\n", message);                                      \
        exit(1);                                                               \
    }

int atoi(const char* base, int size);
double stod(const char* base, int size);

}; // namespace niu2x::nxs

#endif