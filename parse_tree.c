#include "parse_tree.h"
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <errno.h>

static const instr_t ops [] = {
	OP_NM(nop,     1),
	OP_NM(addI,    1),
	OP_NM(add,     1),
	OP_NM(subI,    1),
	OP_NM(sub,     1),
	OP_NM(mult,    3),
	OP_NM(div,     3),
	OP_RD(load,    5),
	OP_NM(loadI,   1),
	OP_RD(loadAO,  5),
	OP_RD(loadAI,  5),
	OP_WR(store,   5),
	OP_WR(storeAO, 5),
	OP_WR(storeAI, 5),
	OP_RD(output,  1)
};

#define OPS_CT ARRAY_SIZE(ops)


arg_t  *arg_mk(char *arg)
{
	arg_t *x = malloc(sizeof(*x));
	if (x) {
		list_head_init(&x->l);
		x->arg  = arg;

		int v = 0;
		int r = sscanf(arg, "%d", &v);
		if (r != 1) {
			/* not a number, must be a reg */
			x->type = ARG_REG;
			x->ival = 0;
		} else {
			x->type = ARG_IMMED;
			x->ival = v;
		}
		errno = 0;
		dep_init(&x->dep);
	}
	return x;
}

stmt_t *stmt_mk(char *opcode, arg_t *args_in, arg_t *args_out, attr_t *attrs, YYLTYPE location)
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

		x->instr = 0;
		x->location = location;
		list_init(&x->rev_dep_list);
		list_init(&x->mem_dep_list);
		list_init(&x->ready_list);
		list_init(&x->active_set);
		x->cum_latency = 0;

		x->start_cycle = 0;
		x->completed = false;

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

int instr_cmp_name(const char *name, const instr_t *instr)
{
	return strcmp(name, instr->name);
}


instr_t *lookup_instr(const instr_t *tbl, size_t tbl_sz, const char *name)
{
	return lfind(name, tbl, &tbl_sz, sizeof(*tbl), (__compar_fn_t)instr_cmp_name);
}

int stmt_list_match_instrs(const struct list_head *head)
{
	stmt_t *e;
	int err = 0;
	int inum = 0;
	stmt_list_for_each(e, head) {
		instr_t *i = lookup_instr(ops, OPS_CT, e->opcode);
		if (!i) {
			PARSE_ERR(e, "no such instruction: '%s'", e->opcode);
			err++;
		} else {
			e->instr = i;
		}
		e->inum = inum;
		inum++;
	}
	return -err;
}

static void stmt_mem_print(const stmt_t *s, FILE *o)
{
	fprintf(o, "mem:");
	switch(s->instr->mem_access) {
	case MEM_NONE:
		fprintf(o, "none");
		break;
	case MEM_IN_RD:
		fprintf(o, "inputs spec read location");
		break;
	case MEM_OUT_WR:
		fprintf(o, "outputs spec write location");
		break;
	default:
		fprintf(o, "unk");
	}
}

void stmt_print(const stmt_t *e, FILE *o)
{
	fprintf(o, "op: %s,\t", e->instr->name);
	stmt_mem_print(e, o);
	fprintf(o, "\n");
}

void stmt_list_print(const struct list_head *head, FILE *o)
{
	stmt_t *e;
	stmt_list_for_each(e, head) {
		stmt_print(e, o);
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
