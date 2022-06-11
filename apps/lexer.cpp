#include <nxs/nxs.h>
#include <string.h>
#include <cmath>

namespace nxs = niu2x::nxs;

#define BUF_SIZE 1024

static int stream_read_more(nxs::stream_t* self, int size)
{
    size = std::max(size, BUF_SIZE / 2);

    memmove(self->base, self->base + BUF_SIZE - size, size);
    self->pos -= size;
    self->size -= size;

    int n = fread(self->base + self->size, 1, size, stdin);
    if (n < size) {
        if (feof(stdin))
            self->base[self->size + (n++)] = EOF;
    }

    self->size += n;
    return n;
}

int main()
{

    nxs::stream_t stream;
    stream.base = (char*)malloc(BUF_SIZE);
    stream.size = BUF_SIZE;
    stream.pos = BUF_SIZE;
    stream.read_more = stream_read_more;

    nxs::token_t token;

    do {
        token = nxs::lexer(&stream);
        printf("type: %d\n", token.type);
    } while (token.type != nxs::TK_EOF);
    return 0;
}