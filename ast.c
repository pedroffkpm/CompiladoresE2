#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

int freeDanglingUsed = FALSE;
int treePrinted = FALSE;
int createdNodes = 0;
int rootNodes = 0;

extern int parsingSucceded;
extern Node *danglingNodes;


/* Functions used by the scanner and parser 
Node* createNode(struct lexval* token);
Node* nglingNode(struct lexval* token);
void addChild(Node *father, Node *child);
void freeDanglingParser(Node* node);
*/

void checkTree(void* voidNode) {
	Node* node = (Node*)voidNode;
	int fullPass = 0;
	while(fullPass == 0) {
		fullPass = removeNullNode(node);
	}
	return;
}

void changeTokenType(Node* node) {
	node->token->tokenType = REMOVE;
	int i;
	for(i = 0; i < node->kidsNumber; ++i) {
		changeTokenType(node->kids[i]);
	}
	return;
}

Node* removeNullHead(Node* node) {
	Node* nodeAux;
	nodeAux = node;
	nodeAux->token = node->token;
	nodeAux->kids = node->kids;
	if (node->token == NULL) {
		nodeAux = node->kids[0];
		nodeAux->token = node->kids[0]->token;
		nodeAux->kids = node->kids[0]->kids;
			
		free(node->token);
    	free(node->tl);
    	free(node->fl);
    	freeList(node->instructions);
		free(node);
	} else {
		if (node->token->tokenType == REMOVE) {
			nodeAux = node->kids[node->kidsNumber-1];
			nodeAux->token = node->kids[node->kidsNumber-1]->token;
			nodeAux->kids = node->kids[node->kidsNumber-1]->kids;
			for(int i = 0; i < node->kidsNumber-1; ++i) {
				free(node->kids[i]->token);
      			free(node->kids[i]->tl);
      			free(node->kids[i]->fl);
      			freeList(node->kids[i]->instructions);
      //free instructions? mas se é NULL?
				free(node->kids[i]);
			}	
			free(node->token);
      		free(node->tl);
      		free(node->fl);
      		freeList(node->instructions);
      //free instructions? mas se é NULL?
			free(node);
		} else {
			return nodeAux;
		}
	}
	return removeNullHead(nodeAux);
}
int removeNullNode(Node* node) {
	int i = 0;
	int j,nullC;
	int nullNode = 1;
	Node* nodeAux;
	int removedNodes = 0;
	while(i < node->kidsNumber) {
		if(node->kids[i]->token == NULL) {
			nullNode = 0;
			if(node->kids[i]->kidsNumber == 0) {
				nodeAux = node->kids[i];	
				removedNodes++;
				for(j = i; j < node->kidsNumber-1; ++j) {
					node->kids[j]->token = node->kids[j+1]->token;
					node->kids[j] = node->kids[j+1];
				}
			} else {
				if(node->kids[i]->kidsNumber == 1) {
					nodeAux = node->kids[i]->kids[0];	
					node->kids[i] = nodeAux;
				}
			} 
		} else {
			
			if(node->kids[i]->token->tokenType == REMOVE) {
				nullNode = 0;
				if(node->kids[i]->kidsNumber == 0) {		
					nodeAux = node->kids[i];
					removedNodes++;
					for(j = i; j < node->kidsNumber-1; ++j) {
						node->kids[j]->token = node->kids[j+1]->token;
						node->kids[j] = node->kids[j+1];
					}
				} else {
					if(node->kids[i]->kidsNumber == 1) {
						nodeAux = node->kids[i]->kids[0];	
						node->kids[i] = nodeAux;
					}		
				}
			}
		}
		i++;			
	}
	for(i = 0; i < node->kidsNumber; ++i) {
		nullC = removeNullNode(node->kids[i]);
		if (nullC == 0) nullNode = 0;
	}
	for (i = 0; i < removedNodes; ++i) {
		free(node->kids[(node->kidsNumber-1)-i]->tl);
		free(node->kids[(node->kidsNumber-1)-i]->fl);
		free(node->kids[(node->kidsNumber-1)-i]->token);
		free(node->kids[(node->kidsNumber-1)-i]);
	}
	node->kidsNumber = node->kidsNumber - removedNodes;
	node->kids = (Node**)realloc(node->kids, node->kidsNumber*sizeof(Node**));
	return nullNode;
}
Node* createNode(struct lexval *token, int tokenType) {
	++createdNodes;
	if(token != NULL) {
		token->tokenInAst = TRUE;
	} //else {
		//token->tokenInAst = FALSE;
	//}
	Node* node = 	malloc(sizeof(Node));
	node->token = token;
	node->varType = -1;
	node->kidsNumber = 0;
	node->kids = (Node**)malloc(sizeof(Node**));
	node->kids[0] = NULL;
  node->instructions = createList();
  node->regTemp = -5; //ILOC.H -> -4 a -1 são reservados (RFP, RSP, RBSS, RPC)
  node->label = -1; // getLabel() gera a partir de 0
  node->trueNmr = 0;
  node->tl = NULL;

  node->falseNmr = 0;
  node->fl = NULL;

	if(tokenType != NONE)
		node->token->tokenType = tokenType;
	return node;
}


Node* createDanglingNode(struct lexval* token) {
	Node* node = malloc(sizeof(Node));
	node->token = token;
	node->kidsNumber = 0;
	node->kids = (Node**)malloc(sizeof(Node**));
  node->instructions = NULL;
  node->regTemp = -5;
  node->label = -1;

  node->trueNmr = 0;
  node->tl = NULL;
  node->falseNmr = 0;
  node->fl = NULL;

	return node;
}

void addType(Node *node, Type type){
	node->varType = type;
	return;
}

void addTypeRec(Node *node, Type type) {
	int i = 0;
	if(node != NULL) {
		if(node->varType == -1)
			addType(node, type);
		while(i < node->kidsNumber) {
			addTypeRec(node->kids[i], type);
			++i;
		}
	}
	return;
}
/*
void addNature(Node *node, Nature nature) {
	node->nature = nature;
	return;
}
*/

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
			freeDanglingParser(node->kids[i]);
			++i;
		}
		free(node->kids);
		if(node->token != NULL) {
			if(node->token->tokenType == KEYWORD || node->token->tokenType == COMP_OPER || node->token->tokenType == IDS || (node->token->tokenType == LITERAL && node->token->literalType == STRING)) {
				free(node->token->value.str);			
			}
			free(node->token);
      free(node->tl);
      free(node->fl);
		}
		
      	freeList(node->instructions);
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
					printf("[label=%s];\n", node->token->value.str);
					break;
			break;
			}
			break;
		case VEC_INDEX:
			printf("[label=\"[]\"];\n");
			break;
		case FUNCTION:
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
			if(node->token->tokenInAst == FALSE) {
				if(node->token->tokenType == KEYWORD || node->token->tokenType == COMP_OPER || node->token->tokenType == IDS || (node->token->tokenType == LITERAL && node->token->literalType == STRING)) {
					free(node->token->value.str);
				}
				free(node->token);
        		//free(node->tl);
        		//free(node->fl);
        		//freeList(node->instructions);
			}
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
	if(parsingSucceded == FALSE)
		return;
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
			if(node->token != NULL)
				free(node->token);
    		free(node->tl);
    		free(node->fl);
    		freeList(node->instructions);
		free(node);	
	} else {
		
	}
}

void addTrueList(Node* node, int label) {
  node->trueNmr++;

  if(node->tl == NULL) {
    node->tl = (int*) malloc(sizeof(int));
  } else { //append no final da lista
    node->tl = (int*) realloc(node->tl, sizeof(int) * node->trueNmr);
  }
  node->tl[node->trueNmr-1] = label;
}

void addFalseList(Node* node, int label) {
  node->falseNmr++;

  if(node->fl == NULL) {
    node->fl = (int*) malloc(sizeof(int));
  } else { //append no final da lista
    node->fl = (int*) realloc(node->fl, sizeof(int) * node->falseNmr);
  }
  node->fl[node->falseNmr-1] = label;
}

void concatTrueL(Node* node1, Node* node2) {
  for(int i = 0; i < node2->trueNmr; i++) {
    addTrueList(node1, node2->tl[i]);
  }
}

void concatFalseL(Node* node1, Node* node2) {
  for(int i = 0; i < node2->falseNmr; i++) {
    addFalseList(node1, node2->fl[i]);
  }
}

