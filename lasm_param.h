#ifndef LASM_PARAM_H_
#define LASM_PARAM_H_

#include "list.h"

typedef struct lasm_parse_t {
	void *scanner;
	struct list_head stmt_list;
} lasm_parse_t;

#define YYPARSE_PARAM data
#define YYLEX_PARAM   (((lasm_parse_t *)data)->scanner)


#endif
