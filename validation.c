#include "validation.h"

void validateProgram(void* arvore) {
    Node* node = (Node*) arvore;

    SymbolTable* scope = createTable();

    validateTable(scope, node); //já printa erro? ou retorna string?

    deleteTables();

    free(scope);

}

void validateTable(SymbolTable* table, Node* node) {

}
