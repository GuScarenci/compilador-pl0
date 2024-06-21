/************************************************************
*             Trabalho 1 - SCC0605                          *
*                                                           *
*      Nome: Artur Brenner Weber                            *
*      nUSP: 12675451                                       *
*      Nome: Gabriel Franceschi Libardi                     *
*      nUSP: 11760739                                       *
*      Nome: Guilherme Castanon Silva Pereira               *
*      nUSP: 11801140                                       *
*      Nome: Gustavo Scarenci de Carvalho Pereira           *
*      nUSP: 12547792                                       *
*      Nome: Matheus Henrique Dias Cirillo                  *
*      nUSP: 12547750                                       *
*      Data de última atualizacao: 19/5/2024                *
*      Ambiente: VSCode 1.89.1                              *
*                                                           *
*                 Arquivo rdp.c                             *
************************************************************/
#include <string.h>

#include "lexer.h"
#include "rdp.h"

TokStream* token_stream;
Token* current_token = NULL;
size_t error_count = 0;

#define SUCCESS 0
#define IMMEDIATE 1
#define PARENT 2
#define SYNC_ERROR 3

#define FIELD_TYPE 0
#define FIELD_STR 1

typedef struct sync_tokens
{
    size_t num_tokens;
    char **token_types;
}SyncTokens;


void rdp(TokStream* b){
    token_stream = b;
    current_token = get_next_token(token_stream);
    programa();
    if(error_count == 0)
        printf("SUCCESS!\n");
}

int part_of(char* string, SyncTokens sync_tokens){
    for(size_t i = 0; i < sync_tokens.num_tokens; i++){
        if(!strcmp(sync_tokens.token_types[i], string)){
            return 1;
        }
    }
    return 0;
}

int match_function(int field, char* comp_type, SyncTokens immediate_tokens, SyncTokens parent_tokens){
    int match_expected = 0;
    if(field == FIELD_TYPE)
        match_expected = !strcmp(current_token->type, comp_type);
    else
        match_expected = !strcmp(current_token->token_str, comp_type);

    if(match_expected){
        current_token = get_next_token(token_stream);
        //TODO check for lexical errors here
        return SUCCESS;
    }
    else{
        if((immediate_tokens.num_tokens + parent_tokens.num_tokens) == 0)
            return SYNC_ERROR;

        while(current_token != NULL){
            //TODO somehow we should first check if we find anything in the immediate tokens with some depth
            if(part_of(current_token->type, immediate_tokens)) //SyncToken is always a list of types
                return IMMEDIATE;
            //TODO if we did not find anything in the immediate tokens, we should go back to the token where we found the error and check if it is in the parent_tokens
            if(part_of(current_token->type, parent_tokens))
                return PARENT;

            current_token = get_next_token(token_stream);
        }
        return SYNC_ERROR;
    }
}

#define match(field_type, str, error_message) \
do { \
    size_t error_line = token_stream->current_line;\
    int result = match_function(field_type, str, immediate_tokens, parent_tokens);\
    if(result != SUCCESS){\
        error_count++;\
        printf("(%ld): error: %s\n", error_line, error_message);\
        if(result == PARENT || result == SYNC_ERROR) return;\
    }\
} while(0) //this is a way to avoid result redefinition

void programa(){
    SyncTokens parent_tokens = {0, NULL};
    SyncTokens immediate_tokens = {0, NULL};
    bloco();
    match(FIELD_TYPE, "period", "Expected a period at the end of the program");
    //epsilon
}

void bloco(){
    //sync_tokens = {"period", "semicolon"};
    declaracao();
    comando();
    //epsilon
}

void declaracao(){
    //sync_tokens = {"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}
    constante();
    variavel();
    procedimento();
    //epsilon
}

void constante(){
    SyncTokens parent_tokens = {9, (char*[]){"keyword_var", "keyword_proc", "keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};
    
    if(!strcmp(current_token->type, "keyword_const")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        match(FIELD_TYPE, "keyword_const", "Expected CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"rel_op"}};
        match(FIELD_TYPE, "identifier", "Expected identifier in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"integer_literal"}};
        match(FIELD_STR, "=", "Use '=' for CONST assignment");

        immediate_tokens = (SyncTokens){2, (char*[]){"semicolon", "comma"}}; //!as identifier is a parent_token, if we have one here where we are expecting a number, AVALANCHE DE ERROS
        match(FIELD_TYPE, "integer_literal", "Expected number after '=' in CONST declaration");

        mais_const();

        immediate_tokens = (SyncTokens){0, NULL};
        match(FIELD_TYPE, "semicolon", "Missing ';'");
    }
    //epsilon
}

void mais_const(){
        SyncTokens parent_tokens = {1, (char*[]){"semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type,"comma")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        match(FIELD_TYPE, "comma", "Expected ',' in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"rel_op"}};
        match(FIELD_TYPE, "identifier", "Expected identifier after ',' in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"integer_literal"}};
        match(FIELD_STR, "=", "Use '=' for CONST assignment");

        immediate_tokens = (SyncTokens){2, (char*[]){"semicolon", "comma"}};
        match(FIELD_TYPE, "integer_literal", "Expected number after '=' in CONST declaration");

        mais_const();
    }
    //epsilon
}

void variavel(){
    SyncTokens parent_tokens = {8, (char*[]){"keyword_proc", "keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "keyword_var")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        match(FIELD_TYPE, "keyword_var", "Expected VAR declaration");

        immediate_tokens = (SyncTokens){2, (char*[]){"comma", "semicolon"}};
        match(FIELD_TYPE, "identifier", "Expected identifier in VAR declaration");

        mais_var();

        immediate_tokens = (SyncTokens){0, NULL};
        match(FIELD_TYPE, "semicolon", "Missing ';'");
    }
    //epsilon
}

void mais_var(){
    SyncTokens parent_tokens = {1, (char*[]){"semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type,"comma")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        match(FIELD_TYPE, "comma", "Expected ',' in VAR declaration");

        immediate_tokens = (SyncTokens){2, (char*[]){"comma", "semicolon"}};
        match(FIELD_TYPE, "identifier", "Expected identifier after ',' in VAR declaration");

        mais_var();
    }
    //epsilon
}


void procedimento(){
    SyncTokens parent_tokens = {8, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon", "keyword_proc"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "keyword_proc")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        match(FIELD_TYPE, "keyword_proc", "Expected PROCEDURE declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"semicolon"}};
        match(FIELD_TYPE, "identifier", "Expected identifier in PROCEDURE declaration");

        immediate_tokens = (SyncTokens){9, (char*[]){"keyword_const", "keyword_var", "keyword_proc", "identifier", "keyword_call", "keyword_begin", "keyword_if", "keyword_while", "semicolon"}};
        match(FIELD_TYPE, "semicolon", "Missing ';'");

        bloco();

        immediate_tokens = (SyncTokens){8, (char*[]){"keyword_proc","keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
        match(FIELD_TYPE, "semicolon", "Missing ';'");

        procedimento();
    }
    //epsilon
}

void comando(){
    SyncTokens parent_tokens = {3, (char*[]){"keyword_end", "period", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "identifier")){
        immediate_tokens = (SyncTokens){1, (char*[]){"assign_op"}};
        match(FIELD_TYPE, "identifier", "Expected identifier in COMMAND");

        immediate_tokens = (SyncTokens){5, (char*[]){"identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        match(FIELD_STR, ":=", "Use ':=' for assignment");

        expressao();
    } else if (!strcmp(current_token->type, "keyword_call")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        match(FIELD_TYPE, "keyword_call", "Expected CALL");

        immediate_tokens = (SyncTokens){0, NULL};
        match(FIELD_TYPE, "identifier", "Expected identifier after CALL");
    } else if (!strcmp(current_token->type, "keyword_begin")){ //BEGIN
        immediate_tokens = (SyncTokens){5, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while"}};
        match(FIELD_TYPE, "keyword_begin", "Expected BEGIN");

        comando();

        mais_cmd();

        immediate_tokens = (SyncTokens){0, NULL};
        match(FIELD_TYPE, "keyword_end", "Expected END in BEGIN");
    } else if (!strcmp(current_token->type, "keyword_if")){ //IF
        immediate_tokens = (SyncTokens){6, (char*[]){"keyword_odd", "identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        match(FIELD_TYPE, "keyword_if", "Expected IF");

        condicao();

        immediate_tokens = (SyncTokens){5, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while"}};
        match(FIELD_TYPE, "keyword_then", "Expected a THEN in IF condition");

        comando();
    } else if (!strcmp(current_token->type, "keyword_while")){
        immediate_tokens = (SyncTokens){6, (char*[]){"keyword_odd", "identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        match(FIELD_TYPE, "keyword_while", "Expected WHILE");

        condicao();

        immediate_tokens = (SyncTokens){5, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while"}};
        match(FIELD_TYPE, "keyword_do", "Expected a DO in WHILE condition");

        comando();
    }
    //epsilon
}

void mais_cmd(){
    SyncTokens parent_tokens = {2, (char*[]){"keyword_end", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "semicolon")){
        immediate_tokens = (SyncTokens){5, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while"}};
        match(FIELD_TYPE, "semicolon", "Missing ';");

        comando();

        mais_cmd();
    }
    //epsilon
}

void expressao(){
    operador_unario();
    termo();
    mais_termos();
}

void operador_unario(){
    SyncTokens parent_tokens = {5, (char*[]){"identifier", "integer_literal", "left_par"}};
    SyncTokens immediate_tokens = {0, NULL};
    
    if(!strcmp(current_token->type, "simb_plus")){
        match(FIELD_TYPE, "simb_plus", "Expected '+'");
    }else if(!strcmp(current_token->type, "simb_minus")){
        match(FIELD_TYPE, "simb_minus", "Expected '-'");
    }
    //epsilon
}

void termo(){
    fator();
    mais_fatores();
}

void mais_termos(){
    SyncTokens parent_tokens = {7, (char*[]){"semicolon", "keyword_end", "period", "right_par", "rel_op", "keyword_then", "keyword_do"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "simb_plus")){
        immediate_tokens = (SyncTokens){3, (char*[]){"identifier", "integer_literal", "left_par"}};
        match(FIELD_TYPE, "simb_plus", "Expected '+'");

        termo();
        
        mais_termos();
    } else if(!strcmp(current_token->type, "simb_minus")){
        immediate_tokens = (SyncTokens){3, (char*[]){"identifier", "integer_literal", "left_par"}};
        match(FIELD_TYPE, "simb_minus", "Expected '-'");

        termo();
        mais_termos();
    }
    //epsilon
}

void fator(){
    SyncTokens parent_tokens = {11, (char*[]){"simb_mult", "simb_div", "simb_minus", "simb_plus", "semicolon", "keyword_end", "period", "right_par", "rel_op", "keyword_then", "keyword_do"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "identifier")){
        match(FIELD_TYPE, "identifier", "Expected identifier in factor");
    }else if(!strcmp(current_token->type, "integer_literal")){
        match(FIELD_TYPE, "integer_literal", "Expected number in factor");
    }else if(!strcmp(current_token->type, "left_par")){
        immediate_tokens = (SyncTokens){5, (char*[]){"identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        match(FIELD_TYPE, "left_par", "Expected opening parenthesis in expression");

        expressao();

        immediate_tokens = (SyncTokens){0, NULL};
        match(FIELD_TYPE, "right_par", "Missing closing parenthesis in expression");
    }else{
        //TODO: libardi should add an error here
        error_count++;
        printf("(%ld): error: %s\n", token_stream->current_line, "Expected identifier, number or expression");
    }
}

void mais_fatores(){
    SyncTokens parent_tokens = {9, (char*[]){"simb_minus", "simb_plus", "semicolon", "keyword_end", "period", "right_par", "rel_op", "keyword_then", "keyword_do"}};
    SyncTokens immediate_tokens = {0, NULL};
    if(!strcmp(current_token->type, "simb_mult")){
        immediate_tokens = (SyncTokens){5, (char*[]){"identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        match(FIELD_TYPE, "simb_mult", "Expected multiply operator");
        fator();
        mais_fatores();
    }else if(!strcmp(current_token->type, "simb_div")){
        immediate_tokens = (SyncTokens){5, (char*[]){"identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        match(FIELD_TYPE, "simb_div", "Expected divide operator");
        fator();
        mais_fatores();
    }
    //epsilon
}

void condicao(){
    SyncTokens parent_tokens = {3, (char*[]){"keyword_then", "keyword_do"}};
    SyncTokens immediate_tokens = {0, NULL};
    if(!strcmp(current_token->type, "keyword_odd")){
        match(FIELD_TYPE, "keyword_odd", "Expected ODD keyword");
    }else{
        expressao();
        relacional();
        expressao();
    }
    //epsilon
}

void relacional(){
    SyncTokens parent_tokens = {3, (char*[]){"simb_plus", "simb_minus", "identifier", "integer_literal","left_par"}};
    SyncTokens immediate_tokens = {0, NULL};
    if(!strcmp(current_token->type, "rel_op")){
        match(FIELD_TYPE, "rel_op", "Expected a relational operator");
    }else{
        //TODO: libardi should add an error here
        error_count++;
        printf("(%ld): error: %s\n", token_stream->current_line, "Expected a relational operator");
    }
}


