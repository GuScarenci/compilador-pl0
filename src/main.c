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
*           Arquivo Main.c                                  *
************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "rdp.h"
#include "str_utils.h"
#include "IO_utils.h"

int main(int argc, char** argv){
    // If arguments are used incorrectly, display this 
    if(argc != 3){
        ABORT_PROGRAM("Usage: make run ARGS=\"<source_file> <output_file>\"\n");
    }

    // Initializes token stream, calls lexer analyzer and output file 
    TokStream* b = token_stream_init(argv[1]);
    FILE* out_file;

    OPEN_FILE(out_file, argv[2], "w")
    rdp(b, out_file);


    //FUNCAO GENERICA TRAB 1
    // while ((t = get_next_token(b))) {
    //     fprintf(out_file, "%s , %s\n", t->token_str, t->type);
    //     free(t->token_str);
    //     free(t->type);
    //     free(t);
    // }

    // Frees memory, closes files and returns correct
    token_stream_free(&b);
    fclose(out_file);
    return 0;
}