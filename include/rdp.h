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
*                 Arquivo rdp.h                             *
************************************************************/

#ifndef __RDP_H__
#define __RDP_H__

#include <string.h>

#include "lexer.h"

void rdp(TokStream* b);

int match(TokStream* b, char* comp_token);

void programa(TokStream* b);

void bloco(TokStream* b);

void declaracao(TokStream* b);

void constante(TokStream* b);

void mais_const(TokStream* b);

void variavel(TokStream* b);

void mais_var(TokStream* b);

void procedimento();

void comando();

void mais_cmd();

void expressao();

void operador_unario();

void termo();

void mais_termos();

void fator();

void mais_fatores();

void condicao();

void relacional();

#endif
