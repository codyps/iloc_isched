
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "parse_tree.h"
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

stmt_t *stmt_mk(char *opcode, arg_t *args_in, arg_t *args_out, attr_t *attrs)
{
	stmt_t *x = malloc(sizeof(*x));
	if (x) {
		list_head_init(&x->l);
		x->opcode = opcode;
		list_init(&x->arg_in_list);
		if (args_in)
			list_add(&args_in->l, &x->arg_in_list);

		list_init(&x->arg_out_list);
		if (args_out)
			list_add(&args_out->l, &x->arg_out_list);

		list_init(&x->attr_list);
		if (attrs)
			list_add(&attrs->l, &x->attr_list);
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


void stmt_list_print(struct list_head *head, FILE *o)
{
	stmt_t *e;
	list_for_each_entry(e, head, l) {
		fprintf(o, "op: %s\n", e->opcode);
	}
}

void attr_list_free(struct list_head *head)
{
	attr_t *e, *tmp;
	list_for_each_entry_safe(e, tmp, head, l) {
		free(e->lbl);
		free(e);
	}
}

void arg_list_free(struct list_head *head)
{
	arg_t *e, *tmp;
	list_for_each_entry_safe(e, tmp, head, l) {
		free(e->arg);
		free(e);
	}
}

void stmt_free(stmt_t *e)
{
	free(e->opcode);
	arg_list_free(&e->arg_in_list);
	arg_list_free(&e->arg_out_list);
	attr_list_free(&e->attr_list);
	free(e);
}

void stmt_list_free(struct list_head *head)
{
	stmt_t *e, *tmp;
	list_for_each_entry_safe(e, tmp, head, l) {
		stmt_free(e);
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
	lasm_lex_destroy(s);

	stmt_list_print(&lh, stdout);
	stmt_list_free(&lh);

	return 0;
}
