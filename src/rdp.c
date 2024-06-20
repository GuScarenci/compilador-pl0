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

void rdp(TokStream* b){
    current_token = get_next_token(b);
    programa(b);
    printf("SUCCESS!\n");
}

int match_str(TokStream* b, char* comp_token){
    if(!strcmp(current_token->token_str,comp_token)){
        current_token = get_next_token(b);
        return 1;
    }
    else{
        printf("ERROR!");
        exit(-1);
    }
}

int match_type(TokStream* b, char* comp_type, char* error_msg){
    if(!strcmp(current_token->type, comp_type)){
        current_token = get_next_token(b);
        return 1;
    }
    else{
        if(error_msg == NULL)
            printf("ERROR!\n");
        else
            printf("ERROR (line %ld): %s\n", b->current_line, error_msg);
        exit(-1);
    }
}

void programa(TokStream* b){
    bloco(b);
    match_str(b,".");
}

void bloco(TokStream* b){
    declaracao(b);
    comando(b);
}

void declaracao(TokStream* b){
    constante(b);
    variavel(b);
    procedimento(b);
    return;
}

void constante(TokStream* b){
    if(!strcmp(current_token->type, "keyword_const")){
        match_type(b,"keyword_const", NULL);
        match_type(b,"identifier", "Expected identifier in CONST declaration");
        match_str(b, "="); //type is rel_op
        match_type(b, "integer_literal", "Expected number after '=' in CONST declaration");
        mais_const(b);
        match_type(b,"semicolon", "Missing ';'");
    }
    return; //epsilon
}

void mais_const(TokStream* b){
    if(!strcmp(current_token->type,"comma")){
        match_type(b,"comma", NULL);
        match_type(b,"identifier", "Expected identifier after ',' in CONST declaration");
        match_str(b, "="); //type is rel_op
        match_type(b, "integer_literal", "Expected number after '=' in CONST declaration");
        mais_const(b);
    }
    return;
}

void variavel(TokStream* b){
    if(!strcmp(current_token->type, "keyword_var")){
        match_type(b,"keyword_var", NULL);
        match_type(b,"identifier", "Expected identifier in VAR declaration");
        mais_var(b);
        match_type(b,"semicolon", "Missing ';'");
    }
    return;
}

void mais_var(TokStream* b){
    if(!strcmp(current_token->type,"comma")){
        match_type(b, "comma", NULL);
        match_type(b,"identifier", "Expected identifier after ',' in VAR declaration");
        mais_var(b);
    }
    return;
}

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



