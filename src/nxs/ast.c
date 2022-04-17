#include "ast.h"

#include <stdlib.h>

#define ALLOC(type) (type*)malloc(sizeof(type))
#define FREE(ptr)   free(ptr)

DEF_DESTROY(statlist)
{
    if (!self)
        return;
    destroy_statlist(self->statlist);
    destroy_stat(self->stat);
    FREE(self);
}

DEF_DESTROY(stat)
{
    if (!self)
        return;
    FREE(self->sz);
    FREE(self);
}

module_t* create_module(block_t* block)
{
    module_t* self = ALLOC(module_t);
    self->block = block;
    return self;
}

DEF_DESTROY(module)
{
    if (!self)
        return;
    destroy_block(self->block);
    FREE(self);
}

block_t* create_block(statlist_t* statlist)
{
    block_t* self = ALLOC(block_t);
    self->statlist = statlist;
    return self;
}

DEF_DESTROY(block)
{
    if (!self)
        return;
    destroy_statlist(self->statlist);
    FREE(self);
}

statlist_t* create_statlist(statlist_t* statlist, stat_t* stat)
{
    statlist_t* self = ALLOC(statlist_t);
    self->statlist = statlist;
    self->stat = stat;
    return self;
}

stat_t* create_stat_print(char* sz)
{
    stat_t* self = ALLOC(stat_t);
    self->sz = sz;
    self->type = stat_type_print;
    return self;
}
