CC=g++
FLEX=flex
YACC=bison
splc:
	@mkdir bin
	$(YACC) -t -d syntax.y
	$(FLEX) lex.l
	$(CC) -lfl -ly tree.cpp semantic.cpp syntax.tab.c -o ./bin/splc
clean:
	@rm -rf bin/
	@rm -f test/*.out test-ex/*.out
	@rm -f lex.yy.* syntax.tab.* syntax.output
