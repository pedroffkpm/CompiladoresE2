#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

int freeDanglingUsed = FALSE;
int treePrinted = FALSE;
int createdNodes = 0;
int rootNodes = 0;

extern Node *danglingNodes;


/* Functions used by the scanner and parser 
Node* createNode(struct lexval* token);
Node* nglingNode(struct lexval* token);
void addChild(Node *father, Node *child);
void freeDanglingParser(Node* node);
*/

Node* createNode(struct lexval *token, int tokenType) {
	++createdNodes;
	if(token != NULL) {
		token->tokenInAst = TRUE;
	} //else {
		//token->tokenInAst = FALSE;
	//}
	Node* node = malloc(sizeof(Node));
	node->token = token;
	node->kidsNumber = 0;
	node->kids = (Node**)malloc(sizeof(Node**));
	if(tokenType != NONE)
		node->token->tokenType = tokenType;
	return node;
}

Node* createDanglingNode(struct lexval* token) {
	Node* node = malloc(sizeof(Node));
	node->token = token;
	node->kidsNumber = 0;
	node->kids = (Node**)malloc(sizeof(Node**));
	return node;
}

void addChild(Node* father, Node* child) {
	father->kidsNumber++;
	father->kids = (Node**)realloc(father->kids, father->kidsNumber*sizeof(Node**));
	father->kids[father->kidsNumber-1] = child;
}

void freeDanglingParser(Node* node) {
	int i = 0;
	if(node != NULL) {
		nullifyPointer(node->token, danglingNodes);
		while(i < node->kidsNumber) {
			freeDanglingScanner(node->kids[i]);
			++i;
		}
		free(node->kids);
		if(node->token != NULL) {
			if(node->token->tokenType == KEYWORD || node->token->tokenType == COMP_OPER || node->token->tokenType == IDS || (node->token->tokenType == LITERAL && node->token->literalType == STRING))
				free(node->token->value.str);
			free(node->token);
		}
		free(node);
	}
}


/*Functions used by other ast functions
void printTree(union Value value, int tokenType, int literalType);
void printTokenLabel(Node* node);
void freeDanglingScanner(Node* node);
*/

void printTree(Node* node) {
	int i = 0;
	if(node != NULL) {
		while(i < node->kidsNumber) {
			if(node->kids[i]->token != NULL) {
				//if(node->kids[i]->token->tokenInAst) {
				printf("%p, %p\n", node, node->kids[i]);
				//}
			}		
			i++;
		}
	}
	i = 0;
	while(i < node->kidsNumber) {
		printTree(node->kids[i]);		
		i++;
	}
}

void printTokenLabel(Node* node) {
	int tokenType = node->token->tokenType;
	printf("%p ", node);
	switch(tokenType) {
		case KEYWORD:
			printf("[label=\"%s\"];\n", node->token->value.str);
			break;
		case SPEC_CHAR:
			printf("[label=\"%c\"];\n", node->token->value.c);
			break;
		case COMP_OPER:
			printf("[label=\"%s\"];\n", node->token->value.str);
			break;
		case IDS:
			printf("[label=\"%s\"];\n", node->token->value.str);
			break;
		case LITERAL:
			switch(node->token->literalType) {
				case INT:
					printf("[label=\"%d\"];\n", node->token->value.i);
					break;
				case FLOAT:
					printf("[label=\"%f\"];\n", node->token->value.f);
					break;
				case CHAR:
					printf("[label=\"%c\"];\n", node->token->value.c);
					break;
				case BOOL:
					if(node->token->value.b)
						printf("[label=\"true\"];\n");
					else
						printf("[label=\"false\"];\n");
					break;
				case STRING:
					printf("%s ", node->token->value.str);
					break;
			break;
			}
			break;
		case VEC_INDEX:
			printf("[label=\"[]\"];\n");
			break;
		case F_CALL:
			printf("[label=\"call %s\"];\n", node->token->value.str);
			break;
		case TERN_OP:
			printf("[label=\"?:\"];\n");
			break;
		break;
	}
}

void freeDanglingScanner(Node* node) {
	int i = 0;
	if(node != NULL) {
		while(i < node->kidsNumber) {
			freeDanglingScanner(node->kids[i]);
			++i;
		}
		free(node->kids);
		if(node->token != NULL) {
			if(!node->token->tokenInAst) {
				if(node->token->tokenType == KEYWORD || node->token->tokenType == COMP_OPER || node->token->tokenType == IDS || (node->token->tokenType == LITERAL && node->token->literalType == STRING))
					free(node->token->value.str);
			}
			free(node->token);
		}
		free(node);
	}
}

void nullifyPointer(struct lexval* token, Node* node) {
	int i = 0;
	if(node != NULL) {
		while(i < node->kidsNumber) {
			nullifyPointer(token, node->kids[i]);
			++i;
		}
		if(node->token == token) {
			node->token = NULL;
		}
	}
}



/*Functions used by main
void exporta(void *voidNode);
void libera(void* voidNode);
*/

void exporta(void* voidNode) {
	Node* node = (Node*)voidNode;
	int i = 0;
	if(!treePrinted) {
		treePrinted = TRUE;
		printTree(node);
	}

	if(node != NULL) {
		if(node->token != NULL) {
			printTokenLabel(node);
		}
		while(i < node->kidsNumber) {
		exporta(node->kids[i]);		
		++i;
		}
	}
}


void libera(void* voidNode) {
	if(freeDanglingUsed == FALSE) {
		freeDanglingUsed = TRUE;
		freeDanglingScanner(danglingNodes);
	}
	Node* node = (Node*)voidNode;
	int i = 0;
	if(node!=NULL) {
		while(i < node->kidsNumber) {
			libera(node->kids[i]);
			++i;
		}
		free(node->kids);
		if(node->token != NULL) {
			if(node->token->tokenType == KEYWORD || node->token->tokenType == COMP_OPER || node->token->tokenType == IDS || (node->token->tokenType == LITERAL && node->token->literalType == STRING))
				free(node->token->value.str);		
		}
		free(node->token);
		free(node);	
	}
}



