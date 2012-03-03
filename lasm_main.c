
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "lasm.h"
#include "lasm.tab.h"
#include "lasm.yy.h"
#include "lasm_param.h"


arg_t  *arg_mk(char *arg)
{
	arg_t *x = malloc(sizeof(*x));
	if (x) {
		list_head_init(&x->l);
		x->arg = arg;
	}
	return x;
}

stmt_t *stmt_mk(char *opcode, arg_t *args, attr_t *attrs)
{
	stmt_t *x = malloc(sizeof(*x));
	if (x) {
		list_head_init(&x->l);
		x->opcode = opcode;
		x->args   = args;
		x->attrs  = attrs;
	}
	return x;
}

attr_t *attr_label_mk(char *label)
{
	attr_t *x = malloc(sizeof(*x));
	if (x) {
		list_head_init(&x->l);
		x->lbl = label;
	}
	return x;
}


void stmt_print(struct list_head *head, FILE *o)
{
	stmt_t *e;
	list_for_each_entry_prev(e, head, l) {
		fprintf(o, "op: %s\n", e->opcode);
	}
}

int main(int argc, char *argv[])
{
	LIST_HEAD(lh);
	yyscan_t s = NULL;
	int r = lasm_lex_init(&s);
	if (r != 0)
		fprintf(stderr, "bleh.");

	lasm_parse(&lh, s);

	stmt_print(&lh, stdout);

	return 0;
}
