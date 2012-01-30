
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
%}

WT       [ \t\n\v]
SYM      [,:=>]
NUM      [-0-9]
ID_FST   [^-/ \t\n\v,:=>0-9]
ID       [^-/ \t\n\v,:=>]


%%

{WT}+   printf("W\n");
--[^\n]*   printf("Haskel Comment\n");
\/\/[^\n]* printf("C Comment\n");
\/\*.*\*\/ printf("Big comment\n");

:          printf("Label marker\n");
{ID_FST}{ID}*  printf("Identifier %s\n", yytext);
{NUM}+     printf("Num: %s\n", yytext);
=>         printf("Arrow\n");


.	   printf("Unk %s\n", yytext);

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
