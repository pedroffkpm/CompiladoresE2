/*
  Função principal para realização da E3.
  Não modifique este arquivo.
*/
#include <stdio.h>
#include "iloc.h"

extern int yyparse(void);
extern int yylex_destroy(void);

void *arvore = NULL;
void exporta (void *arvore);
void libera (void *arvore);
void validateProgram(void *arvore);

int main (int argc, char **argv)
{
  int ret = yyparse(); 
  //exporta(arvore); //essa chamada vai ser comentada!
	Node* tree = (Node*)arvore;
  iloc_init(tree); //arvore precisa virar do tipo Node*
  printCode(tree); //arvore precisa ser Node* e precisa ser a raiz
  // freeInstructions(tree);
  libera(arvore);
  arvore = NULL;
  yylex_destroy();
  return ret;
}
