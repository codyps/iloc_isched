#ifndef PARSE_TREE_H_
#define PARSE_TREE_H_


#include "list.h"
#include "warn.h"
#include <stdio.h>

typedef struct attr_t attr_t;
typedef struct arg_t arg_t;
typedef struct stmt_t stmt_t;
#include "lasm.tab.h" /* YYLTYPE */

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define ARR(...) { __VA_ARGS__ }
#define typeof_memeber(type, member) (typeof(((type *)NULL)->member))


#define OP_NM(n, l) OP(n, l, MEM_NONE)
#define OP_RD(n, l) OP(n, l, MEM_IN_RD)
#define OP_WR(n, l) OP(n, l, MEM_OUT_WR)

#define OP(name_, latency_, mem_) {	\
	.name = #name_,			\
	.latency = latency_,		\
	.mem_access = mem_		\
}

enum arg_type {
	ARG_NONE,
	ARG_IMMED,
	ARG_REG
};

enum dep_type {
	DEP_NONE,
	DEP_RAW,
	DEP_WAR
};

enum mem_access {
	MEM_NONE,
	MEM_IN_RD,
	MEM_OUT_WR
};

typedef struct instr_t {
	char *name;
	enum mem_access mem_access;
	unsigned latency;
} instr_t;

struct attr_t {
	struct list_head l;
	char *lbl;
};

typedef struct dep_t {
	stmt_t *dep;
	enum dep_type dep_type;
} dep_t;

struct arg_t {
	struct list_head l; /* elem in a list of args in a stmt */
	char *arg;
	enum arg_type type;
	int  ival;
	dep_t dep;
};

struct stmt_t {
	struct list_head l;
	char   *opcode;
	struct list_head arg_in_list;
	struct list_head arg_out_list;
	struct list_head attr_list;

	YYLTYPE location;
	instr_t *instr;
	int inum;
	dep_t mem_dep;
};

arg_t  *arg_mk(char *arg);
stmt_t *stmt_mk(char *opcode, arg_t *args_in, arg_t *args_out, attr_t *attrs, YYLTYPE location);
attr_t *attr_label_mk(char *label);

int  stmt_list_match_instrs(const struct list_head *head);

void stmt_list_print(const struct list_head *head, FILE *o);

void attr_list_free(struct list_head *head);
void arg_list_free(struct list_head *head);
void stmt_free(stmt_t *e);
void stmt_list_free(struct list_head *head);

#define stmt_list_for_each(pos, head) list_for_each_entry(pos, head, l)
#define arg_list_for_each(pos, head)  list_for_each_entry(pos, head, l)


#define WARN_STMT(stmt, ...) PARSE_ERR(stmt, __VA_ARGS__)
#define PARSE_ERR(stmt, ...) do {			\
	WARN_AT_POS("<stdio>", stmt->location.first_line, stmt->location.first_column, __VA_ARGS__);	\
} while(0)

#endif
