CC=g++
FLEX=flex
YACC=bison
splc: clean
	@mkdir bin
	touch bin/splc
	@chmod +x bin/splc
	$(YACC) -t -d syntax.y
	$(FLEX) lex.l
	$(CC) -lfl -ly -o ./bin/splc
clean:
	@rm -rf bin/
	@rm -f test/*.out test-ex/*.out
	@rm -f lex.yy.* syntax.tab.* syntax.output