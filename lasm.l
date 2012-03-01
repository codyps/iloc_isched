
%{
#include <stdio.h>
#include "lasm.h"
#include "lasm.tab.h"
//#define YY_NO_INPUT    /* hides warning about "input defined but not used" */
//#define YY_SKIP_YYWRAP /* allows me to make it static */

#if 0
static int yywrap(void)
{
	return 1;
}
#endif

static void *memdup(void *s, size_t l)
{
	void *r = malloc(l);
	if (r)
		memcpy(r, s, l);
	return r;
}

// hides warning about unused yyunput.
//static void yyunput (int c,char *buf_ptr) __attribute__((unused));

#define SAVE_TOKEN do { lasm_lval.str = memdup(yytext, yyleng + 1); } while(0)

%}

%pointer

%x IN_COMMENT

WT       [ \t]
SYM      [,:=>]
NUM      [-0-9]
ID_FST   [[:alpha:]]
ID       [[:alnum:]]

%%

{WT}+      /* ignore whitespace */;
--[^\n]*   /* ignore comments */;
\/\/[^\n]* /* ignore comments */;

"/*"	{
	//printf("Start comment\n");
	BEGIN(IN_COMMENT);
}

<IN_COMMENT>{
"*/"    {
	//printf("End comment\n");
	BEGIN(0);
	}
.*	{ // printf("Comment Line\n"); }
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
