/*
Autores:
    - Ricardo Veiga Coelho - 10409972
    - Nicolas Carmona Rondon -

Programa: compilador.c
Descricao: implementacao de analisador lexico e sintatico para Pascal 
*/

//bibliotecas
#include <stdio.h>
#include <stdbool.h>

//definicao de tipos
typedef enum{
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    EOS
}TAtomo;

typedef struct{
    TAtomo atomo;
    int linha;
    float atributoNumero;
    char atributoID[16];
}TInfoAtomo;

char *buffer;
char lookahead;

// declaracao de funcoes
void consome(char atomo);


int main(){
    char *fileName = "entrada.txt";
    FILE *fp = fopen(fileName, "r");

    if(fp == NULL){
        printf("Erro ao abrir o arquivo %s\n", fileName);
        return 1;
    }

    while(buffer = fgetc(fp) != EOF) putchar(buffer);

    return 0;
}

void consome(char atomo){
    if(lookahead == atomo){
        lookahead = buffer++; //obterAtomo();
    } else {
        printf("erro sintatico: esperado [%c] encontrado [%c]\n",atomo,lookahead);
        exit(1);
    }
}

//parte sintatica
/*
<programa> ::= program identificador “;” <bloco> “.”
<bloco>::= <declaracao_de_variaveis> <comando_composto>
<declaracao_de_variaveis> ::= {<tipo> <lista_variavel> “;”}
<tipo> ::= integer | boolean
<lista_variavel> ::= identificador { “,” identificador }
<comando_composto> ::= begin <comando> {“;”<comando>} end
<comando> ::= <comando_atribuicao> |
<comando_condicional> |
<comando_repeticao> |
<comando_entrada> |
<comando_saida> |
<comando_composto>
<comando_atribuicao> ::= set identificador to <expressao>
<comando_condicional> ::= if <expressao> “:”
<comando> [elif <comando>]
TInfoAtomo
solicita átomo
Analisador sintático
char lookahead
consome()
soliciata
Arquivo fonte
...
if a > b :
a = a + 1;
write(a);
.....
Analisador léxico
char *buffer
obter_atomo()
<comando_repeticao> ::= for identificador of <expressão> to <expressão> “:” <comando>
<comando_entrada> ::= read “(“ <lista_variavel> “)”
<comando_saida> ::= write “(“ <expressao> { “,” <expressao> } “)”
<expressao> ::= <expressao_logica> { or <expressao_logica> }
<expressao_logica>::= <expressao_relacional> { and <expressao_relacional> }
<expressão_relacional> ::= <expressao_simples> [<op_relacional> <expressao_simples>]
<op_relacional> ::= “<” | “<=” | “=” | “/=” | “>” | “>=”
<expressao_simples> ::= <termo> { (“+” | “−” ) <termo> }
<termo> ::= <fator> { ( “*” | “/” ) <fator> }
<fator> ::= identificador |
numero |
true |
false |
not <fator> |
“(“ <expressao> “)”
*/


//parte lexica

/*
Caracteres Delimitadores: Os caracteres delimitadores: espaços em branco, quebra de linhas,
tabulação e retorno de carro (‘ ’, ‘\n’, ‘\t’, ‘\r’) deverão ser eliminados (ignorados) pelo
analisador léxico, mas o controle de linha (contagem de linha) deverá ser mantido.
• Comentários: Existem dois tipos de comentário, um começando com ‘#’ e indo até o final da linha
(1 linha) com o finalizador do comentário o caractere ‘\n’. O outro começando com “{-” e
terminando com “-}” (várias linhas), nesse comentário é importante que a contagem de linha seja
mantida: Importante: Os comentários devem ser repassados para o analisador sintático para serem
reportados e descartados.
• Identificadores: Os identificadores começam com uma letra em minúsculo, em seguida pode vir
zero ou mais letras minúsculas, underline ‘_’ ou dígitos, limitados a 15 caracteres. Caso seja
encontrado um identificador com mais de 15 caracteres deve ser retornado ERRO pelo analisador
léxico. A seguir a definição regular para identificadores.
letra → a|b|...|z|
digito → 0|1|...|9
identificador → letra(letra|digito|_)*
Importante: Na saída do compilador, para átomo identificador, deverá ser impresso o
lexema que gerou o átomo, ou seja, a sequência de caracteres reconhecida.
• Palavras reservadas: As palavras reservadas na linguagem Pascal+- são lexemas em minúsculo:
and, begin, boolean, elif, end, false, for, if, integer, not, of, or, program, read, set,
to, true, write
Importante: Uma sugestão é que as palavras reservadas sejam reconhecidas na mesma função que
reconhece os identificadores e deve ser retornado um átomo específico para cada palavra
reservada reconhecida.
• Números: No compilador teremos somente números inteiros na notação binária, com seguinte
definição regular abaixo:
numero → 0b(0|1)+
Importante: Na saída do compilador, para átomo numero, deverá ser impresso o valor numérico
na notação decimal do atributo do átomo, ou seja, o lexema que gerou o átomo convertido para
notação decimal.
*/



//letra → a|b|...|z|
TInfoAtomo reconheceLetra(char *entrada){
    TInfoAtomo infoAtomo;
    infoAtomo.atomo = ERRO;
    char *iniID = buffer;
    buffer++;

}

//identificador → letra(letra|digito|_)*
TInfoAtomo reconheceID(char *entrada){
    TInfoAtomo infoAtomo;
    infoAtomo.atomo = ERRO;
    char *iniID = buffer;
q0:
    // if(ehLetra(*entrada)){
    //     entrada++;
    //     goto q1;
    // }
    // if()
q1:
}
