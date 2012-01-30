CFLAGS = -g -Wall -MMD
LDFLAGS=

CC     = gcc
CCLD   = gcc
LEX    = lex
YACC   = yacc
RM     = rm -rf


lasm: lasm.yy.o

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.yy.c: %.l
	$(LEX) -t $< | sed 's/<stdout>/$@/g' > $@

%.tab.c: %.y
	$(YACC) -dtv $< -b $(<:.y=)

clean:
	$(RM)

%:
	$(CCLD) $(CFLAGS) $(LDFLAGS) -o $@ $<

-include $(wildcard *.d)
