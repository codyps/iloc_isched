/* Hi */
%pure-parser
%parse-param { struct list_head *stmt_head }

%{
#include <stddef.h>
#include <stdio.h> /* for fileno used by yacc */
#include "lasm.h"
%}

%union {
	struct list_head head;
	struct list_head *list;
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

%type <list>      program    /* a list of statements */
%type <stmt>      statements /* a list of statements */
%type <stmt>      statement  /* an operation with attrs */
%type <attr>      attr_list  /* a list of attributes */
%type <attr>      attr       /* currently, a synonym for label */
%type <attr>      label      /* presently the only attribute */
%type <arg>       args       /* a list of arguments */
%type <str>       arg

%start program


%%

program : statements
	{
		struct list_head *h = stmt_head;
		list_head_init(h);
		list_attach_head(h, &$1->l);
		$$ = h;
	}

statements : /* empty */
	   { $$ = NULL; }
           | statements statement
	   {
		if ($1) {
			list_add_prev(&$1->l, &$2->l);
			$$ = $1;
		} else {
			$$ = $2;
		}
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
		if ($1) {
			list_add_prev(&$1->l, &$2->l);
			$$ = $1;
		} else {
			$$ = $2;
		}
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
