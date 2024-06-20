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
    printf("SUCESS!\n");
}

int match(TokStream* b, char* comp_token){
    fflush(stdout);
    if(!strcmp(current_token->token_str,comp_token)){
        current_token = get_next_token(b);
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

void declaracao(TokStream* b){
    return;
    constante(b);
    variavel(b);
    procedimento(b);
}

void constante(TokStream* b){
    return;
    if(!strcmp(current_token->token_str,"CONST")){
        match(b,"CONST");
        match(b,"ident");
        match(b,"=");
        match(b,"numero");
        mais_const(b);
    }
    else{
        return; //epsilon
    }
}

void mais_const(TokStream* b){return;}

void variavel(TokStream* b){return;}

void mais_var(TokStream* b){return;}

void procedimento(TokStream* b){return;}

void comando(TokStream* b){return;}

void mais_cmd(TokStream* b){return;}

void expressao(TokStream* b){return;}

void operador_unario(TokStream* b){return;}

void termo(TokStream* b){return;}

void mais_termos(TokStream* b){return;}

void fator(TokStream* b){return;}

void mais_fatores(TokStream* b){return;}

void condicao(TokStream* b){return;}

void relacional(TokStream* b){return;}



