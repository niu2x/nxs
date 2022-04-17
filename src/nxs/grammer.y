%token <sz> T_STRING 
%token T_PRINT
%token T_SEMICOLON

%{

#include "nxs.h"
#include "ast.h"

extern block_t *root_block;

%}



%union {
	struct stat_t *stat;
	struct statlist_t *statlist;
	struct block_t *block;
	char *sz;
}

%nterm <stat> stat
%nterm <statlist> statlist
%nterm <block> block

%%
block: statlist { $$ = create_block($1); root_block = $$;}

statlist: statlist T_SEMICOLON stat { $$ = create_statlist($1, $3);}
	| statlist stat { $$ = create_statlist($1, $2);}
	| stat { $$ = create_statlist(0, $1);}

stat: T_PRINT T_STRING { $$ = create_stat_print($2);}

%%

