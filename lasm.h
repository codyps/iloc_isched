#ifndef LASM_H_
#define LASM_H_

#include "list.h"

typedef struct attr_s {
	struct list_head l;
	char *lbl;
} attr_t;
typedef struct arg_s {
	struct list_head l;
	char *arg;
} arg_t;
typedef struct stmt_s {
	struct list_head l;
	char   *opcode;
	arg_t  *args;
	attr_t *attrs;
} stmt_t;

arg_t  *arg_mk(char *arg);
stmt_t *stmt_mk(char *opcode, arg_t *args, attr_t *attrs);
attr_t *attr_label_mk(char *label);

/*
void yyerror(const char *);
int yylex(void);
int yyparse(void *);
*/

#endif
