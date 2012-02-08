
%{
//Definitions
//name subsitute
%}

%pointer
%{
// or %array, refers to type of 'yytext'
%}
%{
#include <stdio.h>
#include "lasm.h"
#include "lasm.tab.h"
#define YY_NO_INPUT
#define YY_NO_YYUNINPUT
#define yylex lasmlex
static void yyunput (int c,char *buf_ptr) __attribute__((unused));
%}


%x IN_COMMENT

WT       [ \t\v]
SYM      [,:=>]
NUM      [-0-9]
ID_FST   [^-/ \t\n\v,:=>0-9]
ID       [^-/ \t\n\v,:=>]

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
{ID_FST}{ID}*  return IDENT;
{NUM}+     return NUM;
=>         return ARROW;
,          return COMMA;
.	   printf("*** Unk %s\n", yytext);

%%

int yywrap(void)
{
	return 1;
}
