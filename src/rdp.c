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

Token* current_token = NULL;
size_t error_count = 0;

typedef struct sync_tokens
{
    char **token_types;
    size_t num_tokens;
}SyncTokens;


void rdp(TokStream* b){
    current_token = get_next_token(b);
    programa(b);
    if(error_count == 0)
        printf("SUCCESS!\n");
}

int is_in_sync(SyncTokens sync_tokens, char* token_type){
    for(size_t i = 0; i < sync_tokens.num_tokens; i++){
        if(!strcmp(sync_tokens.token_types[i], token_type)){
            return 1;
        }
    }
    return 0;
}

int match_str(TokStream* b, char* comp_token, char* error_msg, SyncTokens sync_tokens){
    if(!strcmp(current_token->token_str,comp_token)){
        current_token = get_next_token(b);
        return 1;
    }
    else{
        error_count++;
        if(error_msg != NULL){
            printf("Error at line %ld: %s\n", b->current_line , error_msg);
        } else {
            printf("match_type ERROR!\n");
        }

        if(sync_tokens.num_tokens == 0){
            return 0;
        }

        while(!is_in_sync(sync_tokens, current_token->type)){
            current_token = get_next_token(b);
        }
        return 0;
    }
}

int match_type(TokStream* b, char* comp_type, char* error_msg, SyncTokens sync_tokens){
    if(!strcmp(current_token->type, comp_type)){
        current_token = get_next_token(b);
        return 1;
    }
    else{
        error_count++;
        if(error_msg != NULL){
            printf("Error at line %ld: %s\n", b->current_line , error_msg);
        } else {
            printf("match_type ERROR!\n");
        }

        if(sync_tokens.num_tokens == 0){
            return 0;
        }

        while(!is_in_sync(sync_tokens, current_token->type)){
            current_token = get_next_token(b);
        }
        return 0;
    }
}

void programa(TokStream* b){
    SyncTokens sync_tokens = { .num_tokens = 0,
                            .token_types = NULL};
    bloco(b);
    if(!match_str(b,".", "Expected a period at the end of the program", sync_tokens)) return;
    //epsilon
}

void bloco(TokStream* b){
    SyncTokens sync_tokens = { .num_tokens = 2,
                                .token_types = (char*[]){"period", "semicolon"}};
    declaracao(b);
    //comando(b);
    //epsilon
}

void declaracao(TokStream* b){
    SyncTokens sync_tokens = { .num_tokens = 7,
                            .token_types = (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
    constante(b);
    variavel(b);
    //procedimento(b);
    //epsilon
}

void constante(TokStream* b){
    SyncTokens sync_tokens = { .num_tokens = 9,
                        .token_types = (char*[]){"keyword_var", "keyword_proc", "keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
    if(!strcmp(current_token->type, "keyword_const")){
        if(!match_type(b,"keyword_const", NULL, sync_tokens)) return;
        if(!match_type(b,"identifier", "Expected identifier in CONST declaration", sync_tokens)) return;
        if(!match_str(b, "=", "Use '=' for CONST assignment", sync_tokens)) return;
        if(!match_type(b, "integer_literal", "Expected number after '=' in CONST declaration", sync_tokens)) return;
        mais_const(b);
        if(!match_type(b,"semicolon", "Missing ';'", sync_tokens)) return;
    }
    //epsilon
}

void mais_const(TokStream* b){
        SyncTokens sync_tokens = { .num_tokens = 1,
                            .token_types = (char*[]){"semicolon"}};
    if(!strcmp(current_token->type,"comma")){
        if(!match_type(b,"comma", NULL, sync_tokens)) return;
        if(!match_type(b,"identifier", "Expected identifier after ',' in CONST declaration", sync_tokens)) return;
        if(!match_str(b, "=", "Use '=' for CONST assignment",sync_tokens)) return; //type is rel_op
        if(!match_type(b, "integer_literal", "Expected number after '=' in CONST declaration", sync_tokens)) return;
        mais_const(b);
    }
}

void variavel(TokStream* b){
    SyncTokens sync_tokens = { .num_tokens = 8,
                        .token_types = (char*[]){"keyword_proc", "keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
    if(!strcmp(current_token->type, "keyword_var")){
        if(!match_type(b,"keyword_var", NULL, sync_tokens)) return;
        if(!match_type(b,"identifier", "Expected identifier in VAR declaration", sync_tokens)) return;
        mais_var(b);
        if(!match_type(b,"semicolon", "Missing ';'", sync_tokens)) return;
    }
}

void mais_var(TokStream* b){
    SyncTokens sync_tokens = { .num_tokens = 1,
                    .token_types = (char*[]){"semicolon"}};
    if(!strcmp(current_token->type,"comma")){
        if(!match_type(b, "comma", NULL, sync_tokens)) return;
        if(!match_type(b,"identifier", "Expected identifier after ',' in VAR declaration", sync_tokens)) return;
        mais_var(b);
    }
}


/*
void procedimento(TokStream* b){
    if(!strcmp(current_token->type, "keyword_proc")){
        match_type(b,"keyword_proc", NULL);
        match_type(b,"identifier", "Expected identifier in PROCEDURE declaration");
        match_type(b,"semicolon", "Missing ';'");
        bloco(b);
        match_type(b,"semicolon", "Missing ';'");
        procedimento(b);
    }
    return;
}

void comando(TokStream* b){
    if(!strcmp(current_token->type, "identifier")){
        match_type(b, "identifier", NULL);
        match_str(b, ":="); //type is rel_op
        expressao(b);
    } else if (!strcmp(current_token->type, "keyword_call")){
        match_type(b, "keyword_call", NULL);
        match_type(b, "identifier", "Expected identifier after CALL");
    } else if (!strcmp(current_token->type, "keyword_begin")){
        match_type(b, "keyword_begin", NULL);
        comando(b);
        mais_cmd(b);
        match_type(b, "keyword_end", "Expected END after BEGIN");
    } else if (!strcmp(current_token->type, "keyword_if")){
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


