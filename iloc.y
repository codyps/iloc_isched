%{
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "parse_tree.h"
#include "lasm_param.h"
%}

%parse-param { struct list_head *stmt_list }

%union {
	struct list_head head;
	struct list_head *list;
	stmt_t *stmt;
	arg_t  *arg;
	attr_t *attr;
	char   *str;
	int    token;
}
%{
void iloc_error(struct list_head *data, char *msg)
{
	fprintf(stderr, "parse error: %s", msg);
	exit(1);
}
%}

%left  COLON
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
%type <arg>       output_args
%type <str>       arg

%start program

%%

program : statements
	{
		struct list_head *h = stmt_list;
		list_head_init(h);
		if ($1)
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

output_args : /* empty */
	    { $$ = NULL; }
	    | ARROW args
	    { $$ = $2; }

statement : attr_list IDENT args output_args STMT_END
	  {
		$$ = stmt_mk($2, $3, $4, $1, @$);
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
	arg_t *a = arg_mk($2);
	if ($1) {
		list_add_prev(&$1->l, &a->l);
		$$ = $1;
	} else {
		$$ = a;
	}
     }

arg : IDENT
    | NUM

%%


