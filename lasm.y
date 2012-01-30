/* Hi */

%{
%}

%union {
	stmts_t *stmts;
	stmt_t  *stmt;
	op_t    *operation;
	args_t  *args;
	arg_t   *arg;
	char    *label;
}

%left  COLON
%token COMMA
%token ARROW ID NUM
%token COMMENT
%token EXPR_END

%type <stmts>     stmts
%type <stmt>      stmt
%type <operation> operation
%type <args>      args
%type <arg>       arg
%type <label>     label

%start program

%%

statments : /* empty */
	  | statements statment

statment : label operation
         | operation

label : IDENT COLON

operation : IDENT args EXPR_END

args : /* empty */
     | args arg

arg : IDENT
    | ARROW
    | NUM
    | COMMA

%%
