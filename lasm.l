
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
#include "lasm.tab.h"
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


\n         printf("NL\n");

:          printf("Label marker\n");
{ID_FST}{ID}*  printf("Identifier %s\n", yytext);
{NUM}+     printf("Num: %s\n", yytext);
=>         printf("Arrow\n");
,          printf("Comma\n");
.	   printf("*** Unk %s\n", yytext);

%%

int main(int argc, char *argv[])
{
	yylex();
	return 0;
}

int yywrap(void)
{
	return 1;
}
