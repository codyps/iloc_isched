#include "lasm.h"
#include "lasm.tab.h"

arg_t  *arg_mk(char *arg)
{
	arg_t *x = malloc(sizeof(*x));
	list_head_init(&x->l);
	x->arg = arg;
	return x;
}

stmt_t *stmt_mk(char *opcode, arg_t *args, attr_t *attrs)
{
	stmt_t *x = malloc(sizeof(*x));
	list_head_init(&x->l);
	x->opcode = opcode;
	x->args   = args;
	x->attrs  = attrs;
	return x;
}

attr_t *attr_label_mk(char *label)
{
	attr_t *x = malloc(sizeof(*x));
	list_head_init(&x->l);
	x->lbl = label;
	return x;
}

int main(int argc, char *argv[])
{
	yylex();
	return 0;
}
