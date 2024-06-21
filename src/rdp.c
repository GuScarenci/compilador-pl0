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

static TokStream* token_stream;
static Token* current_token = NULL;
static FILE* out_file = NULL;
static size_t error_count = 0;


void rdp(TokStream* b, FILE* out_fp){
    token_stream = b;
    current_token = get_next_token(token_stream);
    out_file = out_fp;
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

    } else{
        if((immediate_tokens.num_tokens + parent_tokens.num_tokens) == 0)
            return SYNC_ERROR;

        current_token = get_next_token(token_stream);
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

void programa(){
    SyncTokens parent_tokens = {0, NULL};
    SyncTokens immediate_tokens = {0, NULL};
    bloco();
    MATCH(FIELD_TYPE, "period", "Expected a period at the end of the program");
    //epsilon
}

void bloco(){
    //sync_tokens = {"period", "semicolon"};
    declaracao();
    //comando();
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
        MATCH(FIELD_TYPE, "keyword_const", "Expected CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"rel_op"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"integer_literal"}};
        MATCH(FIELD_STR, "=", "Use '=' for CONST assignment");

        immediate_tokens = (SyncTokens){2, (char*[]){"semicolon", "comma"}};
        MATCH(FIELD_TYPE, "integer_literal", "Expected number after '=' in CONST declaration");

        mais_const();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, "semicolon", "Missing ';'");
    }
}

void mais_const(){
        SyncTokens parent_tokens = {1, (char*[]){"semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type,"comma")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "comma", "Expected ',' in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"rel_op"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier after ',' in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"integer_literal"}};
        MATCH(FIELD_STR, "=", "Use '=' for CONST assignment");

        immediate_tokens = (SyncTokens){2, (char*[]){"semicolon", "comma"}};
        MATCH(FIELD_TYPE, "integer_literal", "Expected number after '=' in CONST declaration");

        mais_const();
    }
}

void variavel(){
    SyncTokens parent_tokens = {8, (char*[]){"keyword_proc", "keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "keyword_var")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "keyword_var", "Expected VAR declaration");

        immediate_tokens = (SyncTokens){2, (char*[]){"comma", "semicolon"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier in VAR declaration");

        mais_var();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, "semicolon", "Missing ';'");
    }
}

void mais_var(){
    SyncTokens parent_tokens = {1, (char*[]){"semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type,"comma")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "comma", "Expected ',' in VAR declaration");

        immediate_tokens = (SyncTokens){2, (char*[]){"comma", "semicolon"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier after ',' in VAR declaration");

        mais_var();
    }
}


void procedimento(){
    SyncTokens parent_tokens = {7, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "keyword_proc")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "keyword_proc", "Expected PROCEDURE declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"semicolon"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier in PROCEDURE declaration");

        immediate_tokens = (SyncTokens){9, (char*[]){"keyword_const", "keyword_var", "keyword_proc", "identifier", "keyword_call", "keyword_begin", "keyword_if", "keyword_while", "semicolon"}};
        MATCH(FIELD_TYPE, "semicolon", "Missing ';'");

        bloco();

        immediate_tokens = (SyncTokens){7, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
        MATCH(FIELD_TYPE, "semicolon", "Missing ';'");

        procedimento();
    }
    return;
}

/*void comando(){
    SyncTokens parent_tokens = {3, (char*[]){"keyword_end", "period", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "identifier")){
        immediate_tokens = (SyncTokens){1, (char*[]){"rel_op"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier in COMMAND");

        immediate_tokens = (SyncTokens){5, (char*[]){"identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        MATCH(FIELD_STR, ":=", "Use ':=' for assignment");

        expressao();
    } else if (!strcmp(current_token->type, "keyword_call")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "keyword_call", "Expected CALL");

        immediate_tokens = (SyncTokens){2, (char*[]){"semicolon", "keyword_end"}}; //TODO we need to improve this
        MATCH(FIELD_TYPE, "identifier", "Expected identifier after CALL");
    } else if (!strcmp(current_token->type, "keyword_begin")){
        immediate_tokens = (SyncTokens){8, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon", "keyword_end"}}; //TODO define followers
        MATCH(FIELD_TYPE, "keyword_begin", "Expected BEGIN");

        comando();

        mais_cmd();

        immediate_tokens = (SyncTokens){1, (char*[]){"keyword_end"}}; //TODO define followers
        MATCH(FIELD_TYPE, "keyword_end", "Expected END in BEGIN");
    } else if (!strcmp(current_token->type, "keyword_if")){
        immediate_tokens = (SyncTokens){1, (char*[]){"rel_op"}};
        MATCH(FIELD_TYPE, "keyword_if", "Expected IF");

        match_type(b, "keyword_if", NULL);
        condicao(b);
        match_type(b, "keyword_then", "Expected THEN after IF condition");
        comando(b);
    } else if (!strcmp(current_token->type, "keyword_while")){
        match_type(b, "keyword_while", NULL);
        condicao(b);
        match_type(b, "keyword_do", "Expected DO after WHILE condition");
        comando(b);
    }
    return;
}

void mais_cmd(TokStream* b){
    if(!strcmp(current_token->type, "semicolon")){
        match_type(b, "semicolon", NULL);
        comando(b);
        mais_cmd(b);
    }
    return;
}

void expressao(TokStream* b){
    operador_unario(b);
    termo(b);
    mais_termos(b);
    return;
}

void operador_unario(TokStream* b){
    if(!strcmp(current_token->type, "simb_plus")){
        match_type(b, "simb_plus", NULL);
    }else if(!strcmp(current_token->type, "simb_minus")){
        match_type(b, "simb_minus", NULL);
    }
    return;
}

void termo(TokStream* b){
    fator(b);
    mais_fatores(b);
    return;
}

void mais_termos(TokStream* b){
    if(!strcmp(current_token->type, "simb_plus")){ //!se simbolos fossem unificados dava pra simplificar isso
        match_type(b, "simb_plus", NULL);
        termo(b);
        mais_termos(b);
    } else if(!strcmp(current_token->type, "simb_minus")){
        match_type(b, "simb_minus", NULL);
        termo(b);
        mais_termos(b);
    }
    return;
}

void fator(TokStream* b){
    if(!strcmp(current_token->type, "identifier")){
        match_type(b, "identifier", NULL);
    }else if(!strcmp(current_token->type, "integer_literal")){
        match_type(b, "integer_literal", NULL);
    }else if(!strcmp(current_token->type, "left_par")){
        match_type(b, "left_par", NULL);
        expressao(b);
        match_type(b, "right_par", "Missing closing parenthesis in expression");
    }
    //TODO handle error
}

void mais_fatores(TokStream* b){
    if(!strcmp(current_token->type, "simb_mult")){
        match_type(b, "simb_mult", NULL);
        fator(b);
        mais_fatores(b);
    }else if(!strcmp(current_token->type, "simb_div")){
        match_type(b, "simb_div", NULL);
        fator(b);
        mais_fatores(b);
    }
    return;
}

void condicao(TokStream* b){
    if(!strcmp(current_token->type, "keyword_odd")){
        match_type(b, "keyword_odd", NULL);
    }else{
        expressao(b);
        relacional(b);
        expressao(b);
    }
}

void relacional(TokStream* b){
    if(!strcmp(current_token->type, "rel_op")){
        match_type(b, "rel_op", NULL);
    }
    //TODO: should have an error here
}
*/


