all::

CFLAGS = -g -Wall -MMD -std=gnu99
LDFLAGS=

CC     = gcc
LEX    = flex
YACC   = bison
RM     = rm -rf

ifndef V
	QUIET_CC   = @ echo '    ' CC $@;
	QUIET_LINK = @ echo '    ' LD $@;
	QUIET_LEX  = @ echo '    ' LEX $@;
	QUIET_YACC = @ echo '    ' YACC $@;
endif


.PHONY: FORCE

### Detect prefix changes
## Use "#')" to hack around vim highlighting.
TRACK_CFLAGS = $(CC):$(subst ','\'',$(ALL_CFLAGS)) #')

GIT-CFLAGS: FORCE
	@FLAGS='$(TRACK_CFLAGS)'; \
	    if test x"$$FLAGS" != x"`cat GIT-CFLAGS 2>/dev/null`" ; then \
		echo 1>&2 "    * new build flags or prefix"; \
		echo "$$FLAGS" >GIT-CFLAGS; \
            fi

TRACK_LDFLAGS = $(subst ','\'',$(ALL_LDFLAGS)) #')

GIT-LDFLAGS: FORCE
	@FLAGS='$(TRACK_LDFLAGS)'; \
	    if test x"$$FLAGS" != x"`cat GIT-LDFLAGS 2>/dev/null`" ; then \
		echo 1>&2 "    * new link flags"; \
		echo "$$FLAGS" >GIT-LDFLAGS; \
            fi
.SECONDARY:

all:: lasm

OBJ = lasm.yy.o lasm.tab.o lasm_main.o parse_tree.o
lasm.yy.o: lasm.tab.h
lasm : $(OBJ) GIT-LDFLAGS GIT-CFLAGS
	$(QUIET_LINK)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ)

%.o : %.c GIT-CFLAGS
	$(QUIET_CC)$(CC) $(CFLAGS) -c -o $@ $<

%.dot.png: %.dot
	dot -Tpng $< -o $@

%.yy.c : %.l
	$(QUIET_LEX)$(LEX) --yylineno -P "$(<:.l=)_" -o $@ --header-file=$(@:.c=.h) $<

%.tab.c %.tab.h : %.y
	$(QUIET_YACC)$(YACC) -d -b $(<:.y=) -p "$(<:.y=)_" $<

.PHONY: clean
clean :
	$(RM) *.[od] *.tab.[ch] *.yy.[ch] *.output lasm


-include $(wildcard *.d)
