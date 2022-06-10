#include <nxs/nxs.h>

namespace niu2x::nxs {

static int stream_ensure_available(struct stream_t* self, int size)
{
    int avaliable = self->size - (self->pos - self->base);
    if (avaliable >= size)
        return 0;
    int n = self->read_more(self, size - avaliable);
    return n - (size - avaliable);
}

int stream_getchar(struct stream_t* self)
{
    if (stream_ensure_available(self, 1) < 0)
        return EOF;

    return self->base[self->pos++];
}

} // namespace niu2x::nxs