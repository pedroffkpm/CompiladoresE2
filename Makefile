CC = gcc
LFL = lfl
CCFLAGS = -Wno-parentheses

#test if macos
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
	LFL = ll
endif


all: clean
	flex scanner.l
	$(CC) $(CCFLAGS) -o etapa1 main.c lex.yy.c -$(LFL)

clean:
	rm -rf lex.yy.c etapa1
