
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "lasm.h"
#include "lasm.tab.h"

int lasm_parse(struct list_head *stmt_head);






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

void yyerror(const char *str)
{
	fprintf(stderr, "error: %s\n", str);
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
	lasm_parse(&lh);

	stmt_print(&lh, stdout);

	return 0;
}
