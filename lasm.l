
%{

#include "lasm.h"
#include "lasm.tab.h"
#include "lasm_param.h"
#define YY_NO_INPUT    /* hides warning about "input defined but not used" */

static inline void *memdup(void *s, size_t l)
{
	void *r = malloc(l);
	if (r)
		memcpy(r, s, l);
	return r;
}

// hides warning about unused yyunput.
static void yyunput (int c,char *buf_ptr, void *yyscanner) __attribute__((unused));

#define SAVE_TOKEN do { yylval->str = memdup(yytext, yyleng + 1); } while(0)
%}

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge
%pointer

%x IN_COMMENT

WT       [ \t]
SYM      [,:=>]
NUM_FST  [-0-9]
NUM      [0-9]
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
.*	{ // printf("Comment Line\n");
	}
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

