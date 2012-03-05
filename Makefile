all::

CFLAGS = -ggdb -O0
LDFLAGS=


ALL_CFLAGS = $(CFLAGS) -std=gnu99 -MMD -Wall -Wextra
ALL_LDFLAGS = $(LDFLAGS)


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

TRACK-CFLAGS: FORCE
	@FLAGS='$(TRACK_CFLAGS)'; \
	    if test x"$$FLAGS" != x"`cat TRACK-CFLAGS 2>/dev/null`" ; then \
		echo 1>&2 "    * new build flags or prefix"; \
		echo "$$FLAGS" >TRACK-CFLAGS; \
            fi

TRACK_LDFLAGS = $(subst ','\'',$(ALL_LDFLAGS)) #')

TRACK-LDFLAGS: FORCE
	@FLAGS='$(TRACK_LDFLAGS)'; \
	    if test x"$$FLAGS" != x"`cat TRACK-LDFLAGS 2>/dev/null`" ; then \
		echo 1>&2 "    * new link flags"; \
		echo "$$FLAGS" >TRACK-LDFLAGS; \
            fi
.SECONDARY:

all:: lasm

OBJ = lasm.yy.o lasm.tab.o lasm_main.o parse_tree.o
lasm.yy.o: lasm.tab.h
lasm.tab.o lasm.yy.o: ALL_CFLAGS:=$(filter-out -Wextra,$(ALL_CFLAGS))

lasm : $(OBJ) TRACK-LDFLAGS TRACK-CFLAGS
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) $(ALL_LDFLAGS) -o $@ $(OBJ)

%.o : %.c TRACK-CFLAGS
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<

%.dot.png: %.dot
	dot -Tpng $< -o $@

%.yy.c : %.l
	$(QUIET_LEX)$(LEX) --yylineno -P "$(<:.l=)_" -o $@ --header-file=$(@:.c=.h) $<

%.tab.c %.tab.h : %.y
	$(QUIET_YACC)$(YACC) -d -b $(<:.y=) -p "$(<:.y=)_" $<

.PHONY: clean
clean :
	$(RM) *.[od] *.tab.[ch] *.yy.[ch] *.output lasm

%.dot.png : %.dot
	dot -Tpng -Grankdir=BT -O $<

%.dot : %.iloc lasm
	./lasm < $< > $@

-include $(wildcard *.d)
