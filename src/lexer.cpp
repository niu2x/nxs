#include <nxs/nxs.h>
#include <stdlib.h>
#include <ctype.h>

namespace niu2x::nxs {

struct lexer_context_t {
    struct stream_t* stream;
    struct buffer_t buffer;
    int peek;
};

static void lexer_context_getchar(struct lexer_context_t* self)
{
    if (self->peek != EOF) {
        buffer_append(&self->buffer, self->peek);
    }

    self->peek = stream_getchar(self->stream);
}

static void lexer_context_skipchar(struct lexer_context_t* self)
{
    self->peek = stream_getchar(self->stream);
}

static void lexer_context_free(struct lexer_context_t* self)
{
    buffer_free(&self->buffer);
}

static bool valid_for_id(int c) { return isdigit(c) || isalpha(c) || c == '_'; }

struct token_t lexer(struct stream_t* stream)
{
    lexer_context_t lexer_context = { stream, NXS_BUFFER_INIT, EOF };

    struct token_t result;
    result.type = TK_UNKNOWN;
    lexer_context_getchar(&lexer_context);

    while (result.type == TK_UNKNOWN) {

        if (lexer_context.peek == EOF) {
            result.type = TK_EOF;
            lexer_context_getchar(&lexer_context);
        }

        else if (isdigit(lexer_context.peek)) {
            do {
                lexer_context_getchar(&lexer_context);
            } while (isdigit(lexer_context.peek));

            if (lexer_context.peek == '.') {
                do {
                    lexer_context_getchar(&lexer_context);
                } while (isdigit(lexer_context.peek));
                result.type = TK_DOUBLE;
                result.value.d = buffer_to_double(&lexer_context.buffer);
            } else {
                result.type = TK_INTEGER;
                result.value.i = buffer_to_integer(&lexer_context.buffer);
            }
        } else if (isspace(lexer_context.peek)) {
            lexer_context_skipchar(&lexer_context);
        }

        else if (isalpha(lexer_context.peek)) {
            do {
                lexer_context_getchar(&lexer_context);
            } while (valid_for_id(lexer_context.peek));

            result.type = TK_ID;
            result.value.s = buffer_to_string(&lexer_context.buffer);

            if (!strcmp(result.value.s, "import")) {
                result.type = TK_IMPORT;
            } else if (!strcmp(result.value.s, "function")) {
                result.type = TK_FUNCTION;
            }

        }

        else {
            result.type = lexer_context.peek;
            lexer_context_getchar(&lexer_context);
        }
    }

    if (lexer_context.peek != EOF)
        NXS_CHECK_RESULT(stream_ungetchar(stream), "stream ungetchar fail");
    lexer_context_free(&lexer_context);
    return result;
}

const char* token_name(int k)
{
#define CASE(item)                                                             \
    case TK_##item: {                                                          \
        return #item;                                                          \
    }

    static char buf[2] = { 0, 0 };

    switch (k) {
        CASE(ID)
        CASE(IMPORT)
        CASE(FUNCTION)
        CASE(INTEGER)
        CASE(DOUBLE)
        CASE(STRING)
        CASE(EOF)
        CASE(UNKNOWN)
        default: {
            buf[0] = k;
            return buf;
        }
    }
#undef CASE
}

void token_value_snprintf(struct token_t* tk, char* buf, int size)
{
    if (size > 0)
        buf[0] = 0;
    switch (tk->type) {
        case TK_ID:
            snprintf(buf, size, "%s", tk->value.s);
            break;
        case TK_IMPORT:
            break;
        case TK_FUNCTION:
            break;
        case TK_INTEGER:
            snprintf(buf, size, "%d", tk->value.i);
            break;
        case TK_DOUBLE:
            snprintf(buf, size, "%lf", tk->value.d);
            break;
        case TK_STRING:
            break;
        case TK_EOF:
            break;
        case TK_UNKNOWN:
            break;
        default: {
            break;
        }
    }
}

void token_free(struct token_t* tk)
{
    switch (tk->type) {
        case TK_ID: {
            free(tk->value.s);
            break;
        }
    }
}

} // namespace niu2x::nxs