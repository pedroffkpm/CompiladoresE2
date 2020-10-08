#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*............  TIPOS  ............*/

//cada tipo deveria ser um nó da AST??

//     var_global
//   /         | 
//tipo       identificador
// |           |
//int          x 

//???

typedef enum Type {
    //literais
    INT,
    FLOAT,
    BOOL,
    CHAR,
    STRING,
    IDENTIFICADOR,
    //operacoes
    UN_OP,
    BIN_OP,
    TERN_OP,
    SHIFT_L, //shift fora das operacoes binarias reforca melhor que soh pode vir lit_int depois do operador
    SHIFT_R,
    //declaracoes
    VARIABLE,
    GLOBAL_VAR_DEC,
    FUNC_DEC,
    BLOCK,
    //comando
    ATRIB,
    FUNC_CALL,
    RETURN,
    BREAK,
    CONTINUE,
    INPUT,
    OUTPUT,
    IF,
    FOR,
    WHILE_DO,
} Type;

typedef enum Bin_Op_Type {
    ADD,
    SUB,
    MUL,
    DIV,
    DIV_REST,
    BIT_OR,
    BIT_AND,
    POW,
    LESS_EQ,
    GREATER_EQ,
    EQUAL,
    N_EQUAL,
    AND,
    OR,
    BASH_PIPE, //pipe nao esta no parser ainda
    FORWARD_PIPE,
} Bin_Op_Type;

typedef enum Un_Op_Type {
    PLUS,
    MINUS,
    NOT,
    ADDRESS,
    POINTER_VALUE,
    QUESTION_MARK,
    HASH,
} Un_Op_Type;


/*............  NO  ............*/

union Value;

typedef struct Node {
    Type type;
    union Value* val; //vai "pra baixo" na arvore (filho/valor)
    struct Node* nextNode; //vai "pro lado" na arvore (irmao/prox. da lista)

} Node;

/*..............................*/


/*............  VALUES  ............*/
/*........................  HELPER  */

typedef struct
{
    char* name; //deveria ser um nodo?? node de identificador??
    int array_size;
} GlobalVar_Value; //declaracao de var global mesmo sem init faz parte sim da ast

typedef struct ParamNode {
    char* name;
    struct ParamNode* nextNode; //proximo item da lista encadeada (parametro)
} ParamNode;

typedef struct {
    char* name;
    ParamNode* param; //argumento(s)
    Node* body; //primeiro comando/bloco

} GlobalFunc_Value;

typedef struct {
    Node* val; //lista precisa ser um nodo?
    //assim, tem que descer um nivel pra pegar o "next"
    //node->val->next

} ListValue;

typedef struct {
    char* name;
    Node* index;

} Variable;

typedef struct {
    Variable* identificador;
    Node* init;

} VarLocal_Value;

typedef struct {
    Variable* left;
    Node* right;

} Attrib_Value; //id_expr[index] (left) = expressao (right)

typedef struct {
    Node* cond;
    Node* then;
    Node* else_node;

} If_Value;

typedef struct {
    Node* init;
    Node* expr;
    Node* attrib;
    Node* bloco;

} For_Value;

typedef struct {
    Node* expr;
    Node* bloco;

} WhileDo_Value;

//INPUT: ListValue (o pdf diz "tem pelo menos um filho")
//OUTPUT: ListValue (o pdf diz "tem pelo menos um filho")

typedef struct {
    char* name;
    Node* arg;

} FuncCall_Value;

//SHIFT_L / R: BinOp com left << right

//retorno: ListValue de operacao->next = operacao

//bloco: ListValue de operacoes

typedef struct {
    Un_Op_Type type;
    Node* node;
} Un_Op_Value; // *a

typedef struct {
    Bin_Op_Type type;
    Node* left;
    Node* right;
} Bin_Op_Value; // a + b

typedef struct {
    Node* cond;
    Node* left;
    Node* right;
} Tern_Op_Value;


/*............  VALUES  ............*/
typedef union Value {
    int int_value;
    float float_value;
    bool bool_value;
    char char_value;
    char* string_value;

    Un_Op_Value un_op;
    Bin_Op_Value bin_op;
    Tern_Op_Value tern_op;

    Attrib_Value shift_l; //distincao entre expressao e comando
    Attrib_Value shift_r; //shift eh comando; n pode ser representado como op binaria

    GlobalVar_Value global_var;
    GlobalFunc_Value global_func;

    VarLocal_Value var_local;
    Variable identificador;
    Attrib_Value attrib;

    If_Value if_op;
    For_Value for_op;
    WhileDo_Value while_do_op;
    FuncCall_Value func_call;
    
    ListValue input;
    ListValue output;
    ListValue retorno;
    ListValue block;

} Value;

/*............  yylval TOKEN  ............*/

typedef enum {
  CHAR_ESPECIAL,
  OP_COMPOSTO,
  ID,
  INT_LITERAL,
  FLOAT_LITERAL,
  CHAR_LITERAL,
  BOOL_LITERAL,
  STRING_LITERAL,
} TokenType;

typedef union {
    char char_especial;
    Bin_Op_Type bin_op;
    Un_Op_Type un_op;
    char* identificador;
    int int_lit;
    float float_lit;
    char char_lit;
    bool bool_lit;
    char* string_lit;
} TokenValue;

/*............  FUNCS  ............*/

void liberaParam(ParamNode* pnode);
void liberaVariable(Variable* idnode);

Node* make_int(int value);
Node* make_float(float value);
Node* make_bool(bool value);
Node* make_char(char value);
Node* make_string(char* value);

Node* make_un_op(Un_Op_Type type, Node* value);
Node* make_bin_op(Bin_Op_Type type, Node* left, Node* right);
Node* make_tern_op(Node* cond, Node* left, Node* right);

Node* make_shift_l(Node* left, Node* right);
Node* make_shift_r(Node* left, Node* right);

ParamNode* make_param(char* name);

Node* make_global_var(char* name, int array_size);
Node* make_global_func(char* name, ParamNode* param, Node* body);
Node* make_local_var(Node* identificador, Node* value);
Node* make_identificador(char* name, Node* index);
Node* make_attrib(Node* left, Node* right);

Node* make_if(Node* cond, Node* then, Node* else_node);
Node* make_for(Node* init, Node* expr, Node* attrib, Node* bloco);
Node* make_while(Node* expr, Node* bloco);
Node* make_func_call(char* name, Node* arg);
Node* make_input(Node* value);
Node* make_output(Node* value);
Node* make_return(Node* value);

Node* make_block(Node* value);

Node* make_continue();
Node* make_break();
