CC=gcc
FLEX=flex
YACC=bison

DEBUG ?= 0
VERBOSE ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS:=-DDEBUG -g $(CFLAGS)
endif

ifeq ($(VERBOSE), 1)
    CFLAGS:=-DVERBOSE $(CFLAGS)
endif

splc: syntax.y lex.l tree.c tree.h
	@mkdir -p bin/
	$(YACC) -t -d syntax.y
	$(FLEX) lex.l
	$(CC) syntax.tab.c tree.c -o bin/splc -ly -ll $(CFLAGS)
clean:
	@rm -rf bin/
	@rm -f *.yy.* *.tab.* splc
.PHONY: clean