/* Hi */

%{
%}

%union {
	expr_list_t *expr_list;
	expr_t      *expr;
	op_t        *operation;
	arg_list_t  *arg_list;
	arg_t       *arg;
	comment_t   *comment;
	char        *label;
}

%left  COLON
%token COMMA
%token ARROW ID NUM
%token COMMENT
%token EXPR_END

%type <expr_list> expr_list
%type <expr>      expr
%type <operation> operation
%type <arg_list>  arg_list
%type <arg>       arg
%type <comment>   comment
%type <label>     label

%start program

%%

expr_list : expr
	  | expr expr

expr : label operation
     | operation

label : IDENT COLON

operation : IDENT arg_list EXPR_END

arg_list : arg
         | arg arg

arg : IDENT
    | ARROW
    | NUM
    | COMMA

%%
