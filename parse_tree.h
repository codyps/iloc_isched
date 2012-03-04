#ifndef PARSE_TREE_H_
#define PARSE_TREE_H_

#include "list.h"
#include <stdio.h>

typedef struct attr_t {
	struct list_head l;
	char *lbl;
} attr_t;

typedef struct arg_t {
	struct list_head l;
	char *arg;
} arg_t;

typedef struct stmt_t {
	struct list_head l;
	char   *opcode;
	struct list_head arg_in_list;
	struct list_head arg_out_list;
	struct list_head attr_list;
} stmt_t;

arg_t  *arg_mk(char *arg);
stmt_t *stmt_mk(char *opcode, arg_t *args_in, arg_t *args_out, attr_t *attrs);
attr_t *attr_label_mk(char *label);

void stmt_list_print(struct list_head *head, FILE *o);

void attr_list_free(struct list_head *head);
void arg_list_free(struct list_head *head);
void stmt_free(stmt_t *e);
void stmt_list_free(struct list_head *head);

#endif
