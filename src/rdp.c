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

#include "lexer.h"

Token* current_token = NULL;

void rdp(TokStream* b){
    programa(b);
    if(current_token->token_str == "$"){
        printf("SUCESS!");
    }
}

int match(TokStream* b, char* comp_token){
    if(current_token->token_str == comp_token){
        current_token = get_next_token(b)->token_str;
        return 1;
    }
    else{
        printf("ERROR!");
        exit(-1);
    }
}

void programa(TokStream* b){
    bloco(b);
    match(b,".");
}

void bloco(TokStream* b){
    declaracao(b);
    comando(b);
}

