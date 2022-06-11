#include <nxs/nxs.h>

namespace niu2x::nxs {

static int stream_ensure_available(struct stream_t* self, int size)
{
    int avaliable = self->size - self->pos;
    if (avaliable >= size)
        return avaliable - size;

    int n = self->read_more(self, size - avaliable);
    return n - (size - avaliable);
}

int stream_getchar(struct stream_t* self)
{
    NXS_CHECK_RESULT(stream_ensure_available(self, 1),
        "stream_getchar no more avaliable char");

    return self->base[self->pos++];
}

int stream_ungetchar(struct stream_t* self)
{
    if (self->pos > 0) {
        return --self->pos;
    }
    return -1;
}

} // namespace niu2x::nxs