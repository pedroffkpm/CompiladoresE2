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

	$(CC) $(CCFLAGS) main.c lex.yy.o parser.tab.o -$(LFL) -o etapa2

lex.yy.o: parser.y scanner.l
	bison -d parser.y --report=all
	flex --header-file=lex.yy.h scanner.l
	$(CC) -c lex.yy.c parser.tab.c

clean:
	rm -f etapa2 lex.yy.* parser.tab.* *.o