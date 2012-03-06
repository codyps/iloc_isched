
#include "list.h"
#include "parse_tree.h"
#include "lasm.tab.h"
#include "lasm.yy.h"
#include "lasm_param.h"
#include "warn.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <unistd.h>

#define REG_READ  false
#define REG_WRITE true

typedef struct reg_access_t {
	struct list_head all; /* all */
	struct list_head act; /* all readers OR all writers */
	struct reg_access_t *prev_other;
	bool writer;
	stmt_t *stmt;
	arg_t  *arg;
} reg_access_t;

typedef struct reg_t {
	char *regname;
	struct list_head ra_all; /* reg_access_t */
	struct list_head ra_act[2]; /* index by writter, reg_access_t */
} reg_t;

#define ra_act_list_for_each(ra, head) list_for_each_entry(ra, head, act)

typedef struct reg_set {
	void *root;
	reg_t mem;
} reg_set_t;

static inline void reg_init(reg_t *r, char *name)
{
	r->regname = name;
	list_init(&r->ra_all);
	list_init(r->ra_act + 0);
	list_init(r->ra_act + 1);
}

static inline void reg_access_init(reg_access_t *sp, reg_t *r, stmt_t *stmt, arg_t *arg, bool writer)
{
	list_init(&sp->all);
	list_init(&sp->act);
	sp->writer     = writer;
	sp->stmt       = stmt;
	sp->arg        = arg;

	struct list_head *po = &r->ra_act[!writer];
	if (!list_is_empty(po)) {
		sp->prev_other = list_entry(po->prev, reg_access_t, act);
	} else {
		sp->prev_other = NULL;
	}
}

static inline void reg_set_init(reg_set_t *rs)
{
	rs->root = NULL;
	reg_init(&rs->mem, "memory");
}

int reg_cmpar_by_name(reg_t *to_insert, reg_t *exsisting)
{
	return strcmp(to_insert->regname, exsisting->regname);
}

reg_access_t *reg_add_access(reg_t *r, stmt_t *stmt, arg_t *arg, bool writer)
{
	reg_access_t *sp = malloc(sizeof(*sp));
	if (!sp)
		return NULL;

	reg_access_init(sp, r, stmt, arg, writer);

	list_add_prev(&r->ra_all, &sp->all);
	list_add_prev(&r->ra_act[writer], &sp->act);

	return sp;
}

reg_t *reg_find(struct reg_set *rs, char *regname)
{
	reg_t *r = malloc(sizeof(*r));
	if (!r)
		return NULL;

	reg_init(r, regname);

	reg_t **found_reg = tsearch(r, &rs->root, (__compar_fn_t)reg_cmpar_by_name);

	if (*found_reg != r)
		free(r);
	return *found_reg;
}

reg_access_t *ra_prev_opposite_access(reg_access_t *cur)
{
	return cur->prev_other;
}

bool stmt_has_fwd_dep(stmt_t *e)
{
	arg_t *a;
	if (!list_is_empty(&e->mem_dep_list))
		return true;

	arg_list_for_each(a, &e->arg_in_list) {
		if (a->dep.dep)
			return true;
	}

	arg_list_for_each(a, &e->arg_out_list) {
		if (a->dep.dep)
			return true;
	}

	return false;
}

bool stmt_has_rev_dep(stmt_t *e)
{
	return !list_is_empty(&e->rev_dep_list);
}

int stmt_add_rev_dep(stmt_t *dep_on, stmt_t *rev)
{
	rev_dep_t *rd = malloc(sizeof(*rd));
	if (!rd)
		return -1;

	rd->stmt = rev;

	list_add_prev(&dep_on->rev_dep_list, &rd->l);

	return 0;
}

enum dep_type ra_get_dep_type(reg_access_t *curr_ra)
{
	reg_access_t *prev_ra = ra_prev_opposite_access(curr_ra);
	if (!prev_ra)
		return DEP_NONE;

	if (!curr_ra->writer && prev_ra->writer)
		return DEP_RAW;

	if (curr_ra->writer && !prev_ra->writer)
		return DEP_WAR;

	DEBUG_PR("got a unhandled dep type: written: %d ; prev written: %d", curr_ra->writer, prev_ra->writer);
	return DEP_NONE;
}

int dep_add(reg_access_t *prev, stmt_t *curr, dep_t *dep, enum dep_type dt)
{
	stmt_add_rev_dep(prev->stmt, curr);
	dep->dep      = prev->stmt;
	dep->dep_type = dt;

	return 0;
}

int stmt_add_dep(stmt_t *stmt, dep_t *dep, reg_access_t *curr_ra)
{
	enum dep_type dt = ra_get_dep_type(curr_ra);

	if (dt == DEP_NONE)
		return 0;

	reg_access_t *prev_ra = ra_prev_opposite_access(curr_ra);
	dep_add(prev_ra, stmt, dep, dt);

	return 0;
}

mem_dep_t *stmt_create_mem_dep(stmt_t *e)
{
	mem_dep_t *md = malloc(sizeof(*md));
	if (!md)
		return NULL;

	dep_init(&md->dep);

	list_add_prev(&e->mem_dep_list, &md->l);
	return md;
}

int stmt_add_mem_dep(reg_access_t *curr_ra, reg_t *reg)
{
	enum dep_type dt = ra_get_dep_type(curr_ra);
	if (dt == DEP_NONE)
		return 0;

	reg_access_t *prev_ra;
	stmt_t *e = curr_ra->stmt;
	ra_act_list_for_each(prev_ra, &reg->ra_act[!curr_ra->writer]) {
		mem_dep_t *md = stmt_create_mem_dep(e);
		dep_add(prev_ra, e, &md->dep, dt);
	}

	return 0;
}

int reg_accessed(struct reg_set *t, stmt_t *stmt, arg_t *arg, bool written)
{
	reg_t *found_reg = reg_find(t, arg->arg);
	if (!found_reg)
		return -1;

	reg_access_t *ra = reg_add_access(found_reg, stmt, arg, written);
	if (!ra)
		return -1;

	return stmt_add_dep(stmt, &arg->dep, ra);
}


int mem_accessed(reg_set_t *rs, stmt_t *e, bool written)
{
	reg_t *r   = &rs->mem;
	reg_access_t *ra = reg_add_access(r, e, NULL, written);
	if (!ra)
		return -1;

	return stmt_add_mem_dep(ra, r);
}

int stmt_populate_deps(stmt_t *e, struct reg_set *rs)
{
	arg_t *a;
	arg_list_for_each(a, &e->arg_in_list) {
		if (a->type == ARG_REG)
			reg_accessed(rs, e, a, REG_READ);
	}

	if (e->instr->mem_access == MEM_IN_RD) {
		mem_accessed(rs, e, REG_READ);
	}

	bool written = e->instr->mem_access != MEM_OUT_WR;
	arg_list_for_each(a, &e->arg_out_list) {
		if (a->type == ARG_REG)
			reg_accessed(rs, e, a, written);
	}

	if (e->instr->mem_access == MEM_OUT_WR) {
		mem_accessed(rs, e, REG_WRITE);
	}

	return 0;
}

int stmt_list_populate_deps(struct list_head *stmt_list, struct reg_set *rs)
{
	stmt_t *e;
	stmt_list_for_each(e, stmt_list) {
		//DEBUG_PR("processing stmt: %s", e->opcode);
		int r = stmt_populate_deps(e, rs);
		if (r < 0) {
			WARN_STMT(e, "could not generate deps");
			return -1;
		}
	}

	return 0;
}

void arg_list_emit(struct list_head *arg_list, FILE *o)
{
	arg_t *a;
	arg_list_for_each(a, arg_list) {
		if (a->l.next != arg_list)
			fprintf(o, "%s, ", a->arg);
		else
			fputs(a->arg, o);
	}
}

void stmt_emit_iloc(stmt_t *e, FILE *o)
{
	fputs(e->instr->name, o);
	fputc(' ', o);
	arg_list_emit(&e->arg_in_list, o);
	if (&e->arg_out_list != e->arg_out_list.next)
		fputs(" => ", o);
	arg_list_emit(&e->arg_out_list, o);
}

void stmt_list_emit_iloc(struct list_head *head, FILE *o)
{
	stmt_t *e;
	stmt_list_for_each(e, head) {
		stmt_emit_iloc(e, o);
		fputc('\n', o);
	}
}

void dep_print(dep_t *dep, int stmt_parent, FILE *o)
{
	if (dep->dep) {
		char *style;
		if (dep->dep_type == DEP_WAR) {
			style = "dashed";
		} else {
			style = "solid";
		}
		fprintf(o, "stmt_%d -> stmt_%d [style=\"%s\"]\n", dep->dep->inum, stmt_parent, style);
	}
}

void arg_list_deps_print(struct list_head *arg_list, int stmt_parent, FILE *o)
{
	arg_t *a;
	arg_list_for_each(a, arg_list) {
		dep_print(&a->dep, stmt_parent, o);
	}
}

void stmt_deps_print(stmt_t *e, FILE *o)
{
	fprintf(o, "stmt_%d [label=\"(%u) ", e->inum, e->cum_latency);
	stmt_emit_iloc(e, o);
	fprintf(o, "\"]\n");

	arg_list_deps_print(&e->arg_in_list,  e->inum, o);
	arg_list_deps_print(&e->arg_out_list, e->inum, o);

	mem_dep_t *md;
	mem_dep_list_for_each(md, &e->mem_dep_list) {
		dep_print(&md->dep, e->inum, o);
	}
}

void stmt_list_deps_print(struct list_head *stmt_list, FILE *o)
{
	stmt_t *e;
	fprintf(o, "digraph g {\n");
	stmt_list_for_each(e, stmt_list) {
		stmt_deps_print(e, o);
	}
	fprintf(o, "}\n");
}

void stmt_calc_cum_latency(stmt_t *e, unsigned prev_latency)
{
	if (!e)
		return;

	unsigned ilate = e->instr->latency;
	unsigned curlate = ilate + prev_latency;
	if (e->cum_latency < curlate) {
		DEBUG_PR("curlate: %u, cum_lat: %u (stmt %d: %s) ", curlate, e->cum_latency, e->inum, e->opcode);
		e->cum_latency = curlate;

		arg_t *a;
		arg_list_for_each(a, &e->arg_in_list) {
			stmt_calc_cum_latency(a->dep.dep, curlate);
		}

		arg_list_for_each(a, &e->arg_out_list) {
			stmt_calc_cum_latency(a->dep.dep, curlate);
		}

		mem_dep_t *md;
		mem_dep_list_for_each(md, &e->mem_dep_list) {
			stmt_calc_cum_latency(md->dep.dep, curlate);
		}
	}
}

void stmt_list_calc_cum_latency(struct list_head *stmt_list)
{
	stmt_t *e;
	stmt_list_for_each(e, stmt_list) {
		if (stmt_has_rev_dep(e))
			continue;

		stmt_calc_cum_latency(e, 0);
	}
}


void populate_ready(struct list_head *ready, struct list_head *stmts)
{
	stmt_t *e;
	stmt_list_for_each(e, stmts) {
		if (stmt_has_fwd_dep(e))
			continue;
		/* operate only on the leaves */
		list_add_prev(ready, &e->ready_list);
	}
}

bool stmt_ready(stmt_t *e)
{
	arg_t *a;
	arg_list_for_each(a, &e->arg_in_list) {
		if (a->dep.dep && !a->dep.dep->completed)
			return false;
	}

	arg_list_for_each(a, &e->arg_out_list) {
		if (a->dep.dep && !a->dep.dep->completed)
			return false;
	}

	mem_dep_t *md;
	mem_dep_list_for_each(md, &e->mem_dep_list) {
		if (!md->dep.dep->completed)
			return false;
	}

	return true;
}

void emit_nop(FILE *o)
{
	fprintf(o, "nop\n");
}

void print_ready_list(struct list_head *rdy, FILE *o)
{
	stmt_t *e;
	fprintf(o, "[");
	stmt_rdy_list_for_each(e, rdy) {
		fprintf(o, "%s, ", e->opcode);
	}
	fprintf(o, "]\n");
}

void print_active_list(struct list_head *act, FILE *o)
{
	stmt_t *e;
	fprintf(o, "[");
	stmt_active_list_for_each(e, act) {
		fprintf(o, "%s, ", e->opcode);
	}
	fprintf(o, "]\n");
}

typedef stmt_t *(*heur_fn_t)(struct list_head *ready_list);

stmt_t *heur_first(struct list_head *ready_list)
{
	return list_entry(ready_list->next, stmt_t, ready_list);
}

stmt_t *heur_longest_path(struct list_head *ready_list)
{
	stmt_t *m = heur_first(ready_list);
	stmt_t *e;
	stmt_rdy_list_for_each(e, ready_list) {
		if (e->cum_latency > m->cum_latency)
			m = e;
	}

	return m;
}

stmt_t *heur_highest_instr_latency(struct list_head *ready_list)
{
	stmt_t *m = heur_first(ready_list);
	stmt_t *e;
	stmt_rdy_list_for_each(e, ready_list) {
		if (e->instr->latency > m->instr->latency)
			m = e;
	}

	return m;
}

void stmt_list_schedule(struct list_head *stmt_list, heur_fn_t choose_next_stmt, bool emit_nops, FILE *o)
{
	unsigned cycle = 1;
	LIST_HEAD(ready);
	LIST_HEAD(active);
	populate_ready(&ready, stmt_list);

	while(!list_is_empty(&ready) || !list_is_empty(&active)) {
		if (!list_is_empty(&ready)) {
			stmt_t *e = choose_next_stmt(&ready);
			list_del(&e->ready_list);
			e->start_cycle = cycle;
			list_add_prev(&active, &e->active_set);
			stmt_emit_iloc(e, o);
			fputc('\n', o);
		} else if (emit_nops) {
			emit_nop(o);
		}

		cycle += 1;

		stmt_t *e = NULL, *tmp = NULL;
		stmt_active_list_for_each(e, &active) {
			if ((e->start_cycle  + e->instr->latency) <= cycle)
				e->completed = true;
		}

		stmt_active_list_for_each_safe(e, tmp, &active) {
			if ((e->start_cycle  + e->instr->latency) <= cycle) {
				list_del(&e->active_set);

				/* for each successor s of op in P */
				rev_dep_t *rd;
				rev_dep_list_for_each(rd, &e->rev_dep_list) {
					/* if s is ready */
					if (stmt_ready(rd->stmt)) {
						/* ready <- ready U s */
						list_del(&rd->stmt->ready_list);
						list_add_prev(&ready, &rd->stmt->ready_list);
					}
				}
			}
		}
	}
}

void help(int argc, char *argv[])
{
	WARN("usage: %s [options]", argc?argv[0]:"scheduler");
	WARN("	accepts iloc input on STDIN");
	WARN("	prints output (iloc or dot) on STDOUT");
	WARN("options:");
	WARN("	-a,-b,-c	use one of the schedulers");
	WARN("	-D		emit dot instead of iloc");
	WARN("	-N		emit nops in the iloc code");
	WARN("	-h		show this help");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{

	int sched_type = 'z';
	bool emit_dot  = false;
	bool emit_nops = false;
	int opt;

	while ((opt = getopt(argc, argv, "abcDNh")) != EOF) {
		switch(opt) {
		case 'a':
		case 'b':
		case 'c':
			sched_type = opt;
			break;

		case 'D':
			emit_dot = true;
			break;

		case 'N':
			emit_nops = true;
			break;
		default:
			WARN("unknown option '%c'", opt);
		case 'h':
			help(argc, argv);
		}
	}

	if (emit_dot && emit_nops) {
		WARN("cannot emit nops in dot code");
	}

	if (sched_type == 'z') {
		WARN("the null scheduler is selected, not scheduling instructions");
	}


	LIST_HEAD(lh);
	yyscan_t s = NULL;
	int r = lasm_lex_init(&s);
	if (r != 0)
		fprintf(stderr, "bleh.");

	lasm_parse(&lh, s);
	lasm_lex_destroy(s);

	r = stmt_list_match_instrs(&lh);
	if (r < 0)
		return -1;

	reg_set_t rs;
	reg_set_init(&rs);

	r = stmt_list_populate_deps(&lh, &rs);
	if (r < 0) {
		WARN("error in populating dependencies.");
		return -1;
	}


	if (sched_type != 'z' && !emit_dot) {
		heur_fn_t heur = NULL;
		switch(sched_type) {
		case 'a':
			heur = heur_longest_path;
			break;
		case 'b':
			heur = heur_highest_instr_latency;
			stmt_list_calc_cum_latency(&lh);
			break;
		case 'c':
			heur = heur_first;
			break;
		default:
			WARN("unknown heuristic '%c'", sched_type);
			return -1;
		}
		stmt_list_schedule(&lh, heur, emit_nops, stdout);
	} else if (sched_type == 'z' && !emit_dot) {
		stmt_list_emit_iloc(&lh, stdout);
	} else if (emit_dot) {
		stmt_list_deps_print(&lh, stdout);
	}

	stmt_list_free(&lh);

	return 0;
}
