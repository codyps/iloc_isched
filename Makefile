CFLAGS = -g -Wall -MMD -std=gnu99
LDFLAGS=

CC     = gcc
CCLD   = gcc
LEX    = lex
YACC   = byacc
RM     = rm -rf

ifndef V
	QUIET_CC   = @ echo '    ' CC $@;
	QUIET_LD   = @ echo '    ' LD $@;
	QUIET_LEX  = @ echo '    ' LEX $@;
	QUIET_YACC = @ echo '    ' YACC $@;
endif

.SECONDARY:

lasm: lasm.yy.o lasm.tab.o lasm_main.o
lasm.yy.o : lasm.tab.h lasm.tab.c

# For fileno used by lex
lasm.yy.o : CFLAGS+=-D_POSIX_SOURCE
#lasm.tab.o: CFLAGS+=-DYYPARSE_PARAM=stmt_head

%.o : %.c
	$(QUIET_CC)$(CC) $(CFLAGS) -c -o $@ $<

%.yy.c : %.l
	$(QUIET_LEX)$(LEX) -t $< | sed 's/<stdout>/$@/g' > $@

%.tab.c %.tab.h : %.y
	$(QUIET_YACC)$(YACC) -dtvgi -b $(<:.y=) -p $(<:.y=) $<

.PHONY: clean
clean :
	$(RM) *.o *.tab.[ochd] *.yy.[ochd] lasm

% :
	$(QUIET_LD)$(CCLD) $(CFLAGS) $(LDFLAGS) -o $@ $^

-include $(wildcard *.d)
