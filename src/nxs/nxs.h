#ifndef NXS_H
#define NXS_H

extern char* token_name(int token);
extern int yyerror(char* message, ...);
extern int yylex();

#endif