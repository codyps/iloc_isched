/* Hi */

%{
#include <stddef.h>
#include <stdio.h> /* for fileno used by yacc */
#include "lasm.h"
%}

%union {
	stmt_t *stmt;
	arg_t  *arg;
	attr_t *attr;
	char   *str;
}

%left  COLON
%token <str> COMMA
%token <str> ARROW
%token <str> NUM
%token <str> IDENT
%token STMT_END

%type <stmt>      statements /* a list of statements */
%type <stmt>      statement  /* an operation with attrs */
%type <attr>      attr_list  /* a list of attributes */
%type <attr>      attr       /* currently, a synonym for label */
%type <attr>      label      /* presently the only attribute */
%type <arg>       args       /* a list of arguments */
%type <str>       arg

%start statements

%%

statements : /* empty */
	   { $$ = NULL; }
           | statements statement
	   {
		$2->l.prev = &$1->l;
		$$ = $2;
		if (!first_stmt)
			first_stmt = $$;
           }

statement : attr_list IDENT args STMT_END
	  {
		$$ = stmt_mk($2, $3, $1);
	  }

attr_list : /* empty */
	  { $$ = NULL; }
	  | attr_list STMT_END /* eat STMT_ENDs in the attr_list */
	  | attr_list attr
	  {
		$2->l.prev = &$1->l;
		$$ = $2;
	  }

attr : label

label : IDENT COLON
      { $$ = attr_label_mk($1); }

args : /* empty */
     { $$ = NULL; }
     | args arg
     {
	$$ = arg_mk($2);
	$$->l.prev = &$1->l;
     }

arg : IDENT
    | ARROW
    | NUM
    | COMMA

%%
