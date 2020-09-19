# CompiladoresE2
Segunda Etapa do trabalho de Compiladores INF01147

### To-Do:

___

- Definir a gramática da linguagem:
    - no arquivo `parser.y`
    - ~~Declarações de variáveis globais~~
    - ~~Definição de Funções~~
    - ~~Bloco de Comandos~~
    - ~~Comandos Simples:~~
        - ~~declaração de variável~~
        - ~~comando de atribuição~~
        - ~~comandos de entrada e saída~~
        - ~~chamada de função~~
        - ~~comandos de shift~~
        - ~~comando de retorno, break, continue~~
        - ~~comandos de controle de fluxo~~
    - Expressões aritméticas, lógicas:
        - Operadores:
            - Unários: `+`, `-`, `!`, `&`, `*`, `?`, `#`
                - (modificar etapa 1: TK_INT_LIT não pode mais considerar os sinais)
                - Alterar também regras como a TK_FLOAT_LIT que usassem isso
            - Binários: `+`, `-`, `*`, `/`, `%`, `|`, `&`, `^`, comparadores relacionais, lógicos (`&&`, `||`)
            - Ternários: `expressão` `?` `expressão` `:` `expressão`
            - Associativos à direita: `&`, `*` (acesso a ponteiros), `#`

- Relatório de Erro Sintático
    - ~~mensagem de erro com a linha de código~~
    - retornar um valor diferente de 0

- Remoção de conflitos gramaticais
    - verificar arquivo `parser.output` com a opção `--report-file`
        - conflitos Reduce/Reduce
        - conflitos Shift/Reduce


### Entrega:

___

- Arquivos *`parser.y`*, *`main.c`*, *`scanner.l`* e *`Makefile`* compactados como `Etapa2.tgz`.




    
