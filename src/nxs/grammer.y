%token <sz> T_STRING 
%token T_PRINT
%token T_SEMICOLON

%{

#include "nxs.h"
#include "ast.h"

extern module_t *root_module;

%}



%union {
	struct stat_t *stat;
	struct statlist_t *statlist;
	struct block_t *block;
	struct module_t *module;
	char *sz;
}

%nterm <stat> stat
%nterm <statlist> statlist
%nterm <block> block
%nterm <module> module

%%
module: block { $$ = create_module($1); root_module = $$;}
block: statlist { $$ = create_block($1);}

statlist: statlist T_SEMICOLON stat { $$ = create_statlist($1, $3);}
	| statlist stat { $$ = create_statlist($1, $2);}
	| { $$ = create_statlist(0, 0);}

stat: T_PRINT T_STRING { $$ = create_stat_print($2);}

%%

