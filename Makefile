CFLAGS = -g -Wall -MMD
LDFLAGS= -lfl

CC     = c99
CCLD   = c99
LEX    = lex
YACC   = yacc
RM     = rm -rf

lasm: lasm.yy.o lasm.tab.o lasm.o
lasm.yy.o : lasm.tab.h lasm.tab.c

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.yy.c: %.l
	$(LEX) -t $< | sed 's/<stdout>/$@/g' > $@

%.tab.h %.tab.c: %.y
	$(YACC) -dtv $< -b $(<:.y=)

clean:
	$(RM) *.o *.tab.[ochd] *.yy.[ochd] lasm

%:
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o $@ $^

-include $(wildcard *.d)
