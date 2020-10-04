#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*............  TIPOS  ............*/

//cada tipo deveria ser um nó da AST??

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
    TER_OP,
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
    SHIFT_L,
    SHIFT_R,
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
    struct Node* nextNode; //vai "pro lado" na arvore (irmao)

} Node;

/*..............................*/


/*............  VALUES  ............*/
/*........................  HELPER  */

typedef struct
{
    Type type;
    bool _static;
    char* name;
    int array_size;
} GlobalVar_Value;

typedef struct ParamNode {
    bool _const;
    Type type;
    char* name;
    struct ParamNode* nextNode; //proximo item da lista encadeada (parametro)
} ParamNode;

typedef struct {
    Node* val; //lista precisa ser um nodo?
    //assim, tem que descer um nivel pra pegar o "next"
    //node->val->next

} ListValue;

typedef struct {
    bool _static;
    Type type;
    char* name;
    ParamNode* param;
    Node* body;

} GlobalFunc_Value;

typedef struct {
    bool _static;
    bool _const;
    Type type;
    char* name;
    Node* lit_ou_id;

} VarLocal_Value;

typedef struct {
    char* name;
    Node* index;

} IdExp_Value;

typedef struct {
    IdExp_Value* left;
    Node* right;

} Attrib_Value; //id_expr[index] (left) = expressao (right)

typedef struct {
    Node* cond;
    Node* bloco;
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
} Ter_Op_Value;


/*............  VALUES  ............*/
typedef union Value {
    int int_value;
    float float_value;
    bool bool_value;
    char char_value;
    char* string_value;

    Un_Op_Value un_op;
    Bin_Op_Value bin_op;
    Ter_Op_Value ter_op;

    GlobalVar_Value global_var;
    GlobalFunc_Value global_func;

    VarLocal_Value var_local;
    IdExp_Value identificador;
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
    char* bin_op;
    char* un_op;
    char* identificador;
    int int_lit;
    float float_lit;
    char char_lit;
    bool bool_lit;
    char* string_lit;
} TokenValue;

/*............  FUNCS  ............*/

Node* make_int(int value);
Node* make_float(float value);
Node* make_bool(bool value);
Node* make_char(char value);
Node* make_string(char* value);

Node* make_un_op(Un_Op_Type type, Node* value);
Node* make_bin_op(Bin_Op_Type type, Node* left, Node* right);
Node* make_ter_op(Node* cond, Node* left, Node* right);

ParamNode* make_param(Type type, bool _const, char* name);

Node* make_global_var(Type type, bool _static, char* name, int array_size);
Node* make_global_func(bool _static, Type type, char* name, ParamNode* param, Node* body);
Node* make_local_var(bool _static, bool _const, Type type, char* name, Node* value);
Node* make_identificador(char* name, Node* index);
Node* make_attrib(Node* left, Node* right);

Node* make_if(Node* cond, Node* bloco, Node* else_node);
Node* make_for(Node* init, Node* expr, Node* attrib, Node* bloco);
Node* make_while(Node* expr, Node* bloco);
Node* make_func_call(char* name, Node* arg);
Node* make_input(Node* value);
Node* make_output(Node* value);
Node* make_return(Node* value);

Node* make_block(Node* value);

Node* make_continue();
Node* make_break();
