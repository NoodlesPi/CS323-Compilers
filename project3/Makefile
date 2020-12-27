CC=g++
FLEX=flex
YACC=bison

splc: clean
	@mkdir bin
	touch bin/splc
	@chmod +x bin/splc
	$(FLEX) lex.l
	$(YACC) -t -d syntax.y
	$(CC) -lfl -ly tree.cpp semantic.cpp tac_ir.cpp syntax.tab.c -o ./bin/splc
clean:
	@rm -rf bin/
	@rm -f test/*.out test-ex/*.out sample/*.out
	@rm -f lex.yy.* syntax.tab.* syntax.output