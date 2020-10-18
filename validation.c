#include "validation.h"

void validateProgram(void* arvore) {
    Node* node = (Node*) arvore;

    SymbolTable* scope = createTable();

    validateTable(scope, node); //já printa erro? ou retorna string?

    popTable(); //como ter certeza que foram todas? aqui precisa de garantia que todas foram popped e só sobrou o primeiro escopo

    free(scope);

}
