#ifndef _LASM_PARAM_H
#define _LASM_PARAM_H

#include "list.h"

typedef struct lasm_parse_t {
	yylex_t scanner;
	struct list_head stmt_list;
} lasm_parse_t;

#define YYPARSE_PARAM 
#define YYLEX_PARAM


#endif
