%option noyywrap
%option nodefault
%option yylineno

%{

#include <string.h>
#include <stdarg.h>
#include "grammer.tab.h"
#include "nxs.h"
#include "ast.h"

#define MY_MAX(x, y) (x) > (y) ? (x) : (y)

struct buffer_t {
	char *base;
	int capacity;
	int size;
};

static void buffer_init(struct buffer_t *buffer) {
	buffer->base = 0;
	buffer->capacity = 0;
	buffer->size = 0;
}

static void buffer_free(struct buffer_t *buffer) {
	if(buffer->base) free(buffer->base);
	buffer->base = 0;
	buffer->capacity = 0;
	buffer->size = 0;
}

static void buffer_resize(struct buffer_t *buffer, int new_capacity) {
	char *new_base = (char *) malloc(new_capacity);
	if(buffer->size) {
		memcpy(new_base, buffer->base, buffer->size);
	}
	if(buffer->base)
		free(buffer->base);
	buffer->base = new_base;
	buffer->capacity = new_capacity;
}

static void buffer_push(struct buffer_t *buffer, char c) {
	int new_size = 1 + buffer->size;
	while(new_size > buffer->capacity) {
		buffer_resize(buffer, MY_MAX(buffer->capacity * 2, new_size));
	}
	buffer->base[buffer->size ++ ] = c;
}


	
%}

%%

";" {return T_SEMICOLON;}
"print" {return T_PRINT;}
"\"" {
	struct buffer_t sb;
	buffer_init(&sb);

	char c;
	int state = 0; 	
	// 0: running , is not escaping
	// 2: running , is escaping
	// 1: fail
	// 3: success
	while(state != 1 && state != 3){
		switch(state) {
			case 0:{
				c = input();
				if(!c)
					state = 1;
				else if(c == '\\') {
					state = 2;
				}
				else if(c == '"') {
					state = 3;
				}
				else{
					buffer_push(&sb, c);
				}
				break;
			}
			case 2:{
				c = input();
				if(!c)
					state = 1;
				else{
					buffer_push(&sb, c);
				}
				break;
			}
		}
	}

	if(state == 3){
		yylval.sz = strdup(sb.base == 0 ? "": sb.base);
		buffer_free(&sb);
		return T_STRING;
	}
	else{
		yyerror("string is not end: %s line: %d\n", sb.base == 0 ? "": sb.base, yylineno);
		buffer_free(&sb);
		yyterminate();
	}
}

\r {}
\n {}
" " {}

. {yyerror("unexpected: %s line: %d\n", yytext, yylineno);}

<<EOF>> {yyterminate();}

%%

static int had_error = 0;

int yyerror(char *message, ...) {
	had_error = 1;
	va_list ap;
	va_start(ap, message);
	int n = vfprintf(stderr, message, ap);
	va_end(ap);
	return n;
} 

block_t *root_block = 0;

block_t *nxs_parse(char *sz) {
	yylineno = 1;
	root_block = 0;
	had_error = 0;
	YY_BUFFER_STATE bs = yy_scan_string(sz);
	yyparse();
	yy_delete_buffer(bs);

	if(had_error){
		destroy_block(root_block);
		root_block = 0;
	}
	return root_block;
}