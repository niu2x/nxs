#include <nxs/nxs.h>

#include <stdlib.h>

namespace niu2x::nxs {

const char* empty_str = "";

static int buffer_ensure_available(struct buffer_t* self, int size)
{
    int avaliable = self->capacity - self->size;

    if (avaliable >= size) {
        return avaliable - size;
    }

    int expire_capacity = NXS_CAPACITY_EXPAND(self->capacity, size);

    if (self->base == empty_str) {
        char* base = (char*)malloc(expire_capacity);
        if (base) {
            self->base = base;
            self->size = 0;
            self->capacity = expire_capacity;
        }
    } else {
        char* base = (char*)realloc((void*)(self->base), expire_capacity);
        if (base) {
            self->base = base;
            self->capacity = expire_capacity;
        }
    }
    avaliable = self->capacity - self->size;
    return avaliable - size;
}

void buffer_free(struct buffer_t* self)
{
    if (self->base != empty_str) {
        free(self->base);
        self->base = (char*)empty_str;
        self->size = self->capacity = 0;
    }
}

void buffer_append(struct buffer_t* self, char c)
{
    NXS_CHECK_RESULT(
        buffer_ensure_available(self, 1), "buffer_append alloc memory fail");

    self->base[self->size++] = c;
}

int buffer_atoi(struct buffer_t* self) { return atoi(self->base, self->size); }

} // namespace niu2x::nxs