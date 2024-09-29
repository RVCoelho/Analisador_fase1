/*
Autores:
    - Ricardo Veiga Coelho - 10409972
    - Nicolas Carmona Rondon - 10401903

Programa: compilador.c
Descricao: implementacao de analisador lexico e sintatico para Pascal 
*/

//bibliotecas
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// definicao dos atomos

typedef enum {
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    EOS,
    PROGRAM,
    PONTO,
    PONTO_VIRGULA,
    INTEGER,
    BOOLEAN,
    BEGIN,
    END,
    IF,
    FOR,
    READ,
    WRITE,
    SET,
    TO,
    DOIS_PONTOS,
    ELIF,
    DO,
    ABRE_PARENTESES,
    FECHA_PARENTESES,
    VIRGULA,
    OR,
    AND,
    MENOR_IGUAL,
    MENOR,
    DIFERENTE,
    MAIOR,
    MAIOR_IGUAL,
    IGUAL,
    MAIS,
    MENOS,
    VEZES,
    DIVIDIDO,
    TRUE,
    FALSE,
    NOT,
    COMENTARIO
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    float atributoNumero;
    char atributoID[16];
} TInfoAtomo;

char *msgAtomo[] = {
    "ERRO LEXICO",
    "IDENTIFICADOR",
    "NUMERO",
    "EOS",
    "PROGRAM",
    "PONTO",
    "PONTO_VIRGULA",
    "INTEGER",
    "BOOLEAN",
    "BEGIN",
    "END",
    "IF",
    "FOR",
    "READ",
    "WRITE",
    "SET",
    "TO",
    "DOIS_PONTOS",
    "ELIF",
    "DO",
    "ABRE_PARENTESES",
    "FECHA_PARENTESES",
    "VIRGULA",
    "OR",
    "AND",
    "MENOR_IGUAL",
    "MENOR",
    "DIFERENTE",
    "MAIOR",
    "MAIOR_IGUAL",
    "IGUAL",
    "MAIS",
    "MENOS",
    "VEZES",
    "DIVIDIDO",
    "TRUE",
    "FALSE",
    "NOT",
    "COMENTARIO"
};

// variaveis globais
char *buffer = NULL;
int contaLinha = 1;
TAtomo lookahead;
TInfoAtomo infoAtomo;

// Declaracao das funcoes
void lerArquivo(FILE *arquivo);
void consome(TAtomo atomo);
void programa();
void bloco();
void declaracao_de_variaveis();
void tipo();
void lista_variavel();
void comando_composto();
void comando();
void comando_atribuicao();
void comando_condicional();
void comando_repeticao();
void comando_entrada();
void comando_saida();
void expressao();
void expressao_logica();
void expressao_relacional();
void expressao_simples();
void termo();
void fator();
TInfoAtomo obterAtomo();
TInfoAtomo reconheceID();
TInfoAtomo reconheceNum();
TInfoAtomo reconheceComentario();

int main() {
    FILE *arquivo;

    arquivo = fopen("entrada.txt", "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    
    lerArquivo(arquivo);

    //inicializa o lookahead
    infoAtomo = obterAtomo();
    lookahead = infoAtomo.atomo;

    programa();

    consome(EOS);

    printf("%d linhas analisadas, programa sintaticamente correto\n", contaLinha);

    fclose(arquivo);
    // free(buffer);
    return 0;
}

void lerArquivo(FILE *arquivo) {
    char linha[256];
    size_t bufferSize = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        size_t linhaLen = strlen(linha);
        buffer = realloc(buffer, bufferSize + linhaLen + 1);
        if (buffer == NULL) {
            perror("Erro ao realocar memória");
            exit(1);
        }
        strcpy(buffer + bufferSize, linha);
        bufferSize += linhaLen;
    }
}

// consome o atomo esperado
void consome(TAtomo atomo) {
    // ignorar comentários
    while (lookahead == COMENTARIO) {
        printf("# %d:comentario\n", infoAtomo.linha);
        infoAtomo = obterAtomo();
        lookahead = infoAtomo.atomo;
    }

    if (lookahead == atomo) { // atomo esperado
        if (atomo == IDENTIFICADOR || atomo == NUMERO) printf("# %d:%s | %s\n", infoAtomo.linha, msgAtomo[atomo], infoAtomo.atributoID);
        else printf("# %d:%s\n", infoAtomo.linha, msgAtomo[atomo]);
        
        infoAtomo = obterAtomo();
        lookahead = infoAtomo.atomo;
    } else {
        printf("Funcao consome: #%d:Erro sintatico:esperado [%s] encontrado [%s] \n", infoAtomo.linha, msgAtomo[atomo], msgAtomo[lookahead]);
        exit(0);
    }
}

//############################################################################################################
//parte sintatica
//############################################################################################################

// <programa> ::= program identificador “;” <bloco> “.”
void programa() {
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

// <bloco>::= <declaracao_de_variaveis> <comando_composto>
void bloco(){
    declaracao_de_variaveis();
    comando_composto();
}

// <declaracao_de_variaveis> ::= {<tipo> <lista_variavel> “;”}
void declaracao_de_variaveis(){
    while(lookahead == INTEGER || lookahead == BOOLEAN){
        tipo();
        lista_variavel();
        consome(PONTO_VIRGULA);
    }
}

// <tipo> ::= integer | boolean
void tipo(){
    if(lookahead == INTEGER) consome(INTEGER);
    else if(lookahead == BOOLEAN) consome(BOOLEAN);
    else printf("Funcao tipo: #%d:Erro sintatico:esperado [INTEGER] ou [BOOLEAN] encontrado [%s] \n",infoAtomo.linha,msgAtomo[lookahead]);
}

// <lista_variavel> ::= identificador { “,” identificador }
void lista_variavel(){
    consome(IDENTIFICADOR);
    while(lookahead == VIRGULA){
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
}

// <comando_composto> ::= begin <comando> {“;”<comando>} end
void comando_composto(){
    consome(BEGIN);
    comando();
    while(lookahead == PONTO_VIRGULA){
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(END);
}

// <comando> ::= <comando_atribuicao> | <comando_condicional> | <comando_repeticao> | <comando_entrada> | <comando_saida> | <comando_composto>
void comando(){
    switch(lookahead){
        case IDENTIFICADOR:
            comando_atribuicao();
            break;
        case IF:
            comando_condicional();
            break;
        case FOR:
            comando_repeticao();
            break;
        case READ:
            comando_entrada();
            break;
        case WRITE:
            comando_saida();
            break;
        case BEGIN:
            comando_composto();
            break;
        default:
            printf("Funcao comando: #%d:Erro sintatico:esperado [IDENTIFICADOR] ou [IF] ou [FOR] ou [READ] ou [WRITE] ou [BEGIN] encontrado [%s] \n",infoAtomo.linha,msgAtomo[lookahead]);
    }
}

// <comando_atribuicao> ::= identificador “=” <expressao>
void comando_atribuicao(){
    consome(IDENTIFICADOR);
    consome(IGUAL);
    expressao();
}

// <comando_condicional> ::= if <expressao> “:” <comando> [elif <comando>]
void comando_condicional(){
    consome(IF);
    expressao();
    consome(DOIS_PONTOS);
    comando();
    if(lookahead == ELIF){
        consome(ELIF);
        comando();
    }
}

// <comando_repeticao> ::= for identificador to <expressao> do <comando>
void comando_repeticao(){
    consome(FOR);
    consome(IDENTIFICADOR);
    consome(TO);
    expressao();
    consome(DO);
    comando();
}

// <comando_entrada> ::= read “(“ <lista_variavel> “)”
void comando_entrada(){
    consome(READ);
    consome(ABRE_PARENTESES);
    lista_variavel();
    consome(FECHA_PARENTESES);
}

// <comando_saida> ::= write “(“ <expressao> { “,” <expressao> } “)”
void comando_saida(){
    consome(WRITE);
    consome(ABRE_PARENTESES);
    expressao();
    while(lookahead == VIRGULA){
        consome(VIRGULA);
        expressao();
    }
    consome(FECHA_PARENTESES);
}

// <expressao> ::= <expressao_logica> { or <expressao_logica> }
void expressao(){
    expressao_logica();
    while(lookahead == OR){
        consome(OR);
        expressao_logica();
    }
}

// <expressao_logica>::= <expressao_relacional> { and <expressao_relacional> }
void expressao_logica(){
    expressao_relacional();
    while(lookahead == AND){
        consome(AND);
        expressao_relacional();
    }
}

// <expressão_relacional> ::= <expressao_simples> [<op_relacional> <expressao_simples>]
void expressao_relacional(){
    expressao_simples();
    if(lookahead == MENOR || lookahead == MENOR_IGUAL || lookahead == IGUAL || lookahead == DIFERENTE || lookahead == MAIOR || lookahead == MAIOR_IGUAL){
        TAtomo op = lookahead;
        consome(op); // Consume the relational operator
        expressao_simples();
    }
}

// <op_relacional> ::= “<” | “<=” | “=” | “/=” | “>” | “>=”
void op_relacional(){
    if(lookahead == MENOR)
        consome(MENOR);
    else if(lookahead == MENOR_IGUAL)
        consome(MENOR_IGUAL);
    else if(lookahead == IGUAL)
        consome(IGUAL);
    else if(lookahead == DIFERENTE)
        consome(DIFERENTE);
    else if(lookahead == MAIOR)
        consome(MAIOR);
    else if(lookahead == MAIOR_IGUAL)
        consome(MAIOR_IGUAL);
    else
        printf("Funcao comando: #%d:Erro sintatico:esperado [MENOR] ou [MENOR_IGUAL] ou [IGUAL] ou [DIFERENTE] ou [MAIOR] ou [MAIOR_IGUAL] encontrado [%s] \n",infoAtomo.linha,msgAtomo[lookahead]);
}

// <expressao_simples> ::= <termo> { (“+” | “−” ) <termo> }
void expressao_simples(){
    termo();
    while(lookahead == MAIS || lookahead == MENOS){
        if(lookahead == MAIS)
            consome(MAIS);
        else
            consome(MENOS);
        termo();
    }
}

// <termo> ::= <fator> { ( “*” | “/” ) <fator> }
void termo(){
    fator();
    while(lookahead == VEZES || lookahead == DIVIDIDO){
        if(lookahead == VEZES)
            consome(VEZES);
        else
            consome(DIVIDIDO);
        fator();
    }
}

// <fator> ::= identificador | numero | true | false | not <fator> | “(“ <expressao> “)”
void fator(){
    if(lookahead == IDENTIFICADOR)
        consome(IDENTIFICADOR);
    else if(lookahead == NUMERO)
        consome(NUMERO);
    else if(lookahead == TRUE)
        consome(TRUE);
    else if(lookahead == FALSE)
        consome(FALSE);
    else if(lookahead == NOT){
        consome(NOT);
        fator();
    } else if(lookahead == ABRE_PARENTESES){
        consome(ABRE_PARENTESES);
        expressao();
        consome(FECHA_PARENTESES);
    } else {
        printf("Funcao fator: #%d:Erro sintatico:esperado [IDENTIFICADOR] ou [NUMERO] ou [TRUE] ou [FALSE] ou [NOT] ou [ABRE_PARENTESES] encontrado [%s] \n",infoAtomo.linha,msgAtomo[lookahead]);
        exit(0);
    }
}

//############################################################################################################
//parte lexica
//############################################################################################################

TInfoAtomo obterAtomo() {
    TInfoAtomo info_atomo;

    //consumir espaços em branco e novas linhas
    while (*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r') {
        if (*buffer == '\n') contaLinha++;
        buffer++;
    }

    // reconhece comentários
    if (*buffer == '#') {
        char *inicioComentario = buffer;
        buffer++;
        while (*buffer != '\n' && *buffer != '\0') buffer++;
        if (*buffer == '\n') {
            contaLinha++;
            buffer++;
        }
        // copia o comentário para o atributoID
        size_t length = buffer - inicioComentario;
        if (length > 15) length = 15; // limita o tamanho do comentário
        strncpy(info_atomo.atributoID, inicioComentario, length);
        info_atomo.atributoID[length] = '\0'; // finaliza a string
        info_atomo.atomo = COMENTARIO;
        info_atomo.linha = contaLinha;
        return info_atomo;
    } else if (*buffer == '{' && *(buffer + 1) == '-') {
        char *inicioComentario = buffer;
        buffer += 2;
        while (!(*buffer == '-' && *(buffer + 1) == '}') && *buffer != '\0') {
            if (*buffer == '\n') contaLinha++;
            buffer++;
        }
        if (*buffer == '-' && *(buffer + 1) == '}') {
            buffer += 2;
        }
        // copia o comentário para o atributoID
        size_t length = buffer - inicioComentario;
        if (length > 15) length = 15; // limita o tamanho do comentário
        strncpy(info_atomo.atributoID, inicioComentario, length);
        info_atomo.atributoID[length] = '\0'; // finaliza a string
        info_atomo.atomo = COMENTARIO;
        info_atomo.linha = contaLinha;
        return info_atomo;
    }

    // reconhece operadores e pontuação
    switch (*buffer) {
        case ';':
            info_atomo.atomo = PONTO_VIRGULA;
            buffer++;
            break;
        case '.':
            info_atomo.atomo = PONTO;
            buffer++;
            break;
        case ':':
            info_atomo.atomo = DOIS_PONTOS;
            buffer++;
            break;
        case '(':
            info_atomo.atomo = ABRE_PARENTESES;
            buffer++;
            break;
        case ')':
            info_atomo.atomo = FECHA_PARENTESES;
            buffer++;
            break;
        case ',':
            info_atomo.atomo = VIRGULA;
            buffer++;
            break;
        case '+':
            info_atomo.atomo = MAIS;
            buffer++;
            break;
        case '-':
            info_atomo.atomo = MENOS;
            buffer++;
            break;
        case '*':
            info_atomo.atomo = VEZES;
            buffer++;
            break;
        case '/':
            info_atomo.atomo = DIVIDIDO;
            buffer++;
            break;
        case '<':
            if (*(buffer + 1) == '=') {
                info_atomo.atomo = MENOR_IGUAL;
                buffer += 2;
            } else {
                info_atomo.atomo = MENOR;
                buffer++;
            }
            break;
        case '>':
            if (*(buffer + 1) == '=') {
                info_atomo.atomo = MAIOR_IGUAL;
                buffer += 2;
            } else {
                info_atomo.atomo = MAIOR;
                buffer++;
            }
            break;
        case '=':
            info_atomo.atomo = IGUAL;
            buffer++;
            break;
        case '!':
            if (*(buffer + 1) == '=') {
                info_atomo.atomo = DIFERENTE;
                buffer += 2;
            } else {
                info_atomo.atomo = ERRO;
            }
            break;
        default:
            // reconhece identificadores e números
            if (islower(*buffer))
                info_atomo = reconheceID();
            else if (isdigit(*buffer))
                info_atomo = reconheceNum();
            else if (*buffer == 0)
                info_atomo.atomo = EOS;
            else
                info_atomo.atomo = ERRO;
            break;
    }

    info_atomo.linha = contaLinha;
    return info_atomo;
}

//identificador → letra(letra|digito|_)*
TInfoAtomo reconheceID(){
    TInfoAtomo infoAtomo;
    infoAtomo.atomo = ERRO;
    char *iniID = buffer;
    // ja temos uma letra minuscula
    buffer++;
q0:
    if(islower(*buffer) || isdigit(*buffer) || *buffer == '_'){
        buffer++;
        goto q0;
    }
    if(isupper(*buffer)) return infoAtomo;
    
    strncpy(infoAtomo.atributoID,iniID,buffer-iniID);
    infoAtomo.atributoID[buffer-iniID] = 0; // finaliza a string

    // checa palavras reservadas
    if (strcmp(infoAtomo.atributoID, "program") == 0)
        infoAtomo.atomo = PROGRAM;
    else if (strcmp(infoAtomo.atributoID, "integer") == 0)
        infoAtomo.atomo = INTEGER;
    else if (strcmp(infoAtomo.atributoID, "boolean") == 0)
        infoAtomo.atomo = BOOLEAN;
    else if (strcmp(infoAtomo.atributoID, "begin") == 0)
        infoAtomo.atomo = BEGIN;
    else if (strcmp(infoAtomo.atributoID, "end") == 0)
        infoAtomo.atomo = END;
    else if (strcmp(infoAtomo.atributoID, "if") == 0)
        infoAtomo.atomo = IF;
    else if (strcmp(infoAtomo.atributoID, "for") == 0)
        infoAtomo.atomo = FOR;
    else if (strcmp(infoAtomo.atributoID, "read") == 0)
        infoAtomo.atomo = READ;
    else if (strcmp(infoAtomo.atributoID, "write") == 0)
        infoAtomo.atomo = WRITE;
    else if (strcmp(infoAtomo.atributoID, "set") == 0)
        infoAtomo.atomo = SET;
    else if (strcmp(infoAtomo.atributoID, "to") == 0)
        infoAtomo.atomo = TO;
    else if (strcmp(infoAtomo.atributoID, "elif") == 0)
        infoAtomo.atomo = ELIF;
    else if (strcmp(infoAtomo.atributoID, "do") == 0)
        infoAtomo.atomo = DO;
    else if (strcmp(infoAtomo.atributoID, "or") == 0)
        infoAtomo.atomo = OR;
    else if (strcmp(infoAtomo.atributoID, "and") == 0)
        infoAtomo.atomo = AND;
    else if (strcmp(infoAtomo.atributoID, "true") == 0)
        infoAtomo.atomo = TRUE;
    else if (strcmp(infoAtomo.atributoID, "false") == 0)
        infoAtomo.atomo = FALSE;
    else if (strcmp(infoAtomo.atributoID, "not") == 0)
        infoAtomo.atomo = NOT;
    else
        infoAtomo.atomo = IDENTIFICADOR;

    return infoAtomo;
}

// numero -> 0b(0|1)+
TInfoAtomo reconheceNum(){
    TInfoAtomo infoAtomo;
    infoAtomo.atomo = ERRO;
    char *iniNum = buffer;

    if (*buffer == '0') {
        buffer++;
        if (*buffer == 'b') {
            buffer++;
            if (*buffer == '0' || *buffer == '1') {
                buffer++;
                while (*buffer == '0' || *buffer == '1') {
                    buffer++;
                }
                if (!isalpha(*buffer)) {
                    char binStr[buffer - iniNum + 1];
                    strncpy(binStr, iniNum + 2, buffer - iniNum - 2);
                    binStr[buffer - iniNum - 2] = '\0';
                    infoAtomo.atributoNumero = strtol(binStr, NULL, 2);
                    infoAtomo.atomo = NUMERO;
                    return infoAtomo;
                }
            }
        }
    }
    return infoAtomo;
}

// reconhece comentários
TInfoAtomo reconheceComentario() {
    TInfoAtomo infoAtomo;
    infoAtomo.atomo = COMENTARIO;
    char *inicioComentario = buffer;

    if (*buffer == '#') { // comentário de linha
        buffer++;
        while (*buffer != '\n' && *buffer != '\0') {
            buffer++;
        }
        if (*buffer == '\n') {
            contaLinha++;
            buffer++;
        }
        infoAtomo.atomo = COMENTARIO;
    } else if (*buffer == '{' && *(buffer + 1) == '-') { // comentário de bloco
        buffer += 2;
        while (!(*buffer == '-' && *(buffer + 1) == '}') && *buffer != '\0') { // procura pelo fim do comentário
            if (*buffer == '\n')
                contaLinha++;
            buffer++;
        }
        if (*buffer == '-' && *(buffer + 1) == '}') {
            buffer += 2;
        }
        infoAtomo.atomo = COMENTARIO;
    }

    // copia o comentário para o atributoID
    size_t length = buffer - inicioComentario;
    strncpy(infoAtomo.atributoID, inicioComentario, length);
    infoAtomo.atributoID[length] = '\0'; // finaliza a string

    infoAtomo.linha = contaLinha;
    return infoAtomo;
}