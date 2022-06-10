#ifndef NXS_H
#define NXS_H

namespace niu2x::nxs {

struct token_t {
    enum {
        TK_ID = 256,
    };

    int type;
};

struct stream_t;

typedef void (*stream_read_more_t)(struct stream_t*, int size);

struct stream_t {
    const char* pos;
    const char* base;
    int size;
    stream_read_more_t read_more;
};

int stream_getchar(struct stream_t*);

struct token_t lexer(struct stream_t*);

}; // namespace niu2x::nxs

#endif