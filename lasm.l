
%{
//Definitions
//name subsitute
// or %array, refers to type of 'yytext'
#include <stdio.h>
#include "lasm.h"
#include "lasm.tab.h"
//#include "lasm.tab.i"
#define YY_NO_INPUT
#define YY_NO_YYUNINPUT
#define YY_SKIP_YYWRAP
static int yywrap(void)
{
	return 1;
}

static void *memdup(void *s, size_t l)
{
	void *r = malloc(l);
	if (r)
		memcpy(r, s, l);
	return r;
}

//#define yylex lasmlex
static void yyunput (int c,char *buf_ptr) __attribute__((unused));
#define SAVE_TOKEN do { lasm_lval.str = memdup(yytext, yyleng + 1); } while(0)

%}

%pointer

%x IN_COMMENT

WT       [ \t]
SYM      [,:=>]
NUM      [-0-9]
%{ // ID_FST   [^-/ \t\n\v,:=>0-9]
%}
%{ // ID       [^/ \t\n\v,:=>]
%}

ID_FST [[:alpha:]]
ID     [[:alnum:]]

%%

{WT}+      /* ignore whitespace */;
--[^\n]*   /* ignore comments */;
\/\/[^\n]* /* ignore comments */;

"/*"	{
	printf("Start comment\n");
	BEGIN(IN_COMMENT);
}

<IN_COMMENT>{
"*/"    {
	printf("End comment\n");
	BEGIN(0);
	}
.*	{ printf("Comment Line\n"); }
}

\/\*.*\*\/ printf("Big comment\n");

\n         return STMT_END;
:          return COLON;

{ID_FST}{ID}* { SAVE_TOKEN; return IDENT; }

{NUM}+     { SAVE_TOKEN; return NUM;   }
=>         { SAVE_TOKEN; return ARROW; }
,          { SAVE_TOKEN; return COMMA; }
.	   printf("*** Unk '%s'\n", yytext);

%%
