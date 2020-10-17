CC := gcc
LFL := lfl
CCFLAGS := -g -Wall

#test if macos
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
	LFL = ll
endif


#make faz report
#lex.yy.c => output flex
#parser.tab.c => output bison
all: lex.yy.o

	$(CC) $(CCFLAGS) main.c ast.c lex.yy.o parser.tab.o -$(LFL) -o etapa4

lex.yy.o: parser.y scanner.l
	bison -d parser.y --verbose
	flex --header-file=lex.yy.h scanner.l
	$(CC) -c lex.yy.c parser.tab.c

clean:
	rm -rf etapa4 lex.yy.* parser.tab.* *.o parser.output *.dSYM
	
#arquivo dSYM aparece no macOS
