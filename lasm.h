#ifndef LASM_H_
#define LASM_H_

struct list_head { struct list_head *prev, *next; };

typedef struct stmt_s {
	struct list_head l;
} stmt_t;
typedef struct attr_s {
	struct list_head l;
} attr_t;
typedef struct arg_s {
	struct list_head l;
} arg_t;

void yyerror(char *);
int yylex(void);

#endif
