#ifndef NXS_AST_H
#define NXS_AST_H

#define AST_NAME_FIELD(name, _type) struct _type##_t* name;
#define AST_FIELD(_type)            AST_NAME_FIELD(_type, _type)
#define DEF_DESTROY(type)           void destroy_##type(type##_t* self)

enum {
    stat_type_print = 1,
};

typedef struct module_t {
    AST_FIELD(block)
} module_t;

typedef struct block_t {
    AST_FIELD(statlist)
} block_t;

typedef struct statlist_t {
    AST_FIELD(statlist)
    AST_FIELD(stat)
} statlist_t;

typedef struct stat_t {
    int type;

    struct {
        char* sz;
    };
} stat_t;

module_t* create_module(block_t*);
block_t* create_block(statlist_t*);
statlist_t* create_statlist(statlist_t*, stat_t*);

stat_t* create_stat_print(char*);

DEF_DESTROY(block);
DEF_DESTROY(stat);
DEF_DESTROY(module);
DEF_DESTROY(statlist);

module_t* nxs_parse(char*);

#endif