#ifndef LASM_PARAM_H_
#define LASM_PARAM_H_

#include "list.h"

//#define YYLEX_PARAM   scanner
/* should be lasm_parse_t */

int iloc_parse(struct list_head *stmt_list);
int iloc_lex();

#endif
