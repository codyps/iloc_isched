#ifndef LASM_H_
#define LASM_H_

struct list_head { struct list_head *prev, *next; };

typedef struct attr_s {
	struct list_head l;
} attr_t;
typedef struct arg_s {
	struct list_head l;
} arg_t;
typedef struct stmt_s {
	struct list_head l;
	char   *opcode;
	arg_t  *args;
	attr_t *attrs;
} stmt_t;

void yyerror(char *);
int yylex(void);

#endif
