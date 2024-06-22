/************************************************************
*             Trabalho 2 - SCC0605                          *
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
*      Data de última atualizacao: 22/06/2024               *
*      Ambiente: VSCode 1.89.1                              *
*                                                           *
*                 Arquivo rdp.c                             *
************************************************************/
#include <string.h>

#include "lexer.h"
#include "rdp.h"
#include "error_handler.h"
#include "token_labels.h"

static TokStream* token_stream;
static Token* current_token = NULL;
static FILE* out_file = NULL;

void rdp(TokStream* b, FILE* out_fp){
    token_stream = b;
    out_file = out_fp;
    current_token = get_next_token(token_stream);
    if(current_token == NULL){
        print_message(out_file, "Empty program", ANSI_COLOR_RED);
    }else{
        programa();
        if(current_token != NULL){
            print_warning(out_file, "Unexpected token after program ending.", *current_token);
            FREE_TOKEN(current_token);
        }
    }
    print_final_message(out_file);
}

int part_of(char* string, SyncTokens sync_tokens){
    for(size_t i = 0; i < sync_tokens.num_tokens; i++){
        if(!strcmp(sync_tokens.token_types[i], string)){
            return 1;
        }
    }
    return 0;
}

int match_function(int field, char* comp_type, char *error_msg, SyncTokens immediate_tokens, SyncTokens parent_tokens){
    int match_expected = 0;
    if(field == FIELD_TYPE)
        match_expected = !strcmp(current_token->type, comp_type);
    else
        match_expected = !strcmp(current_token->token_str, comp_type);

    if(match_expected){
        Token* tok_buff = current_token;
        current_token = get_next_token(token_stream);

        if(current_token != NULL && current_token->is_error){
            print_error(out_file, current_token->type, *current_token);
            while(current_token != NULL){
                FREE_TOKEN(tok_buff);
                if(part_of(current_token->type, immediate_tokens)) {
                    return IMMEDIATE;
                }
                if(part_of(current_token->type, parent_tokens)) {
                    return PARENT;
                }

                tok_buff = current_token;
                current_token = get_next_token(token_stream);
            }
        }

        if (strcmp(comp_type, PERIOD) && current_token == NULL) {
            print_error(out_file, "Program ended unexpectedly", *tok_buff);
            print_final_message(out_file);
            FREE_TOKEN(tok_buff);
            exit(EXIT_FAILURE);
        }

        FREE_TOKEN(tok_buff);
        return SUCCESS;
    } else{
        print_error(out_file, error_msg, *current_token);

        if((immediate_tokens.num_tokens + parent_tokens.num_tokens) == 0)
            return SYNC_ERROR;

        while(current_token != NULL){
            if(part_of(current_token->type, immediate_tokens))
                return IMMEDIATE;

            if(part_of(current_token->type, parent_tokens))
                return PARENT;

            FREE_TOKEN(current_token);
            current_token = get_next_token(token_stream);
            if(current_token != NULL && current_token->is_error)
                print_error(out_file, current_token->type, *current_token);
        }
        return SYNC_ERROR;
    }
}

void programa(){
    SyncTokens parent_tokens = {0, NULL};
    SyncTokens immediate_tokens = {0, NULL};
    bloco();
    MATCH(FIELD_TYPE, PERIOD, "Expected a period at the end of the program");
    //epsilon
}

void bloco(){
    //sync_tokens = {PERIOD, SEMICOLON};
    declaracao();
    comando();
    //epsilon
}

void declaracao(){
    //sync_tokens = {KW_CALL, KW_BEGIN, KW_IF, IDENT, KW_WHILE, PERIOD, SEMICOLON}
    constante();
    variavel();
    procedimento();
    //epsilon
}

void constante(){
    SyncTokens parent_tokens = {9, (char*[]){KW_VAR, KW_PROC, KW_CALL, KW_BEGIN, KW_IF, IDENT, KW_WHILE, PERIOD, SEMICOLON}};
    SyncTokens immediate_tokens = {0, NULL};
    
    if(!strcmp(current_token->type, KW_CONST)){
        immediate_tokens = (SyncTokens){1, (char*[]){IDENT}};
        MATCH(FIELD_TYPE, KW_CONST, "Expected CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){RELOP}};
        MATCH(FIELD_TYPE, IDENT, "Expected identifier in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){INT}};
        MATCH(FIELD_STR, "=", "Use '=' for CONST assignment");

        immediate_tokens = (SyncTokens){2, (char*[]){SEMICOLON, COMMA}};
        MATCH(FIELD_TYPE, INT, "Expected number after '=' in CONST declaration");

        mais_const();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, SEMICOLON, "Missing ';'");
    }
    //epsilon
}

void mais_const(){
    SyncTokens parent_tokens = {1, (char*[]){SEMICOLON}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type,COMMA)){
        immediate_tokens = (SyncTokens){1, (char*[]){IDENT}};
        MATCH(FIELD_TYPE, COMMA, "Expected ',' in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){RELOP}};
        MATCH(FIELD_TYPE, IDENT, "Expected identifier after ',' in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){INT}};
        MATCH(FIELD_STR, "=", "Use '=' for CONST assignment");

        immediate_tokens = (SyncTokens){2, (char*[]){SEMICOLON, COMMA}};
        MATCH(FIELD_TYPE, INT, "Expected number after '=' in CONST declaration");

        mais_const();
    }
    //epsilon
}

void variavel(){
    SyncTokens parent_tokens = {8, (char*[]){KW_PROC, KW_CALL, KW_BEGIN, KW_IF, IDENT, KW_WHILE, PERIOD, SEMICOLON}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, KW_VAR)){
        immediate_tokens = (SyncTokens){1, (char*[]){IDENT}};
        MATCH(FIELD_TYPE, KW_VAR, "Expected VAR declaration");

        immediate_tokens = (SyncTokens){2, (char*[]){COMMA, SEMICOLON}};
        MATCH(FIELD_TYPE, IDENT, "Expected identifier in VAR declaration");

        mais_var();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, SEMICOLON, "Missing ';'");
    }
    //epsilon
}

void mais_var(){
    SyncTokens parent_tokens = {1, (char*[]){SEMICOLON}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type,COMMA)){
        immediate_tokens = (SyncTokens){1, (char*[]){IDENT}};
        MATCH(FIELD_TYPE, COMMA, "Expected ',' in VAR declaration");

        immediate_tokens = (SyncTokens){2, (char*[]){COMMA, SEMICOLON}};
        MATCH(FIELD_TYPE, IDENT, "Expected identifier after ',' in VAR declaration");

        mais_var();
    }
    //epsilon
}


void procedimento(){
    SyncTokens parent_tokens = {8, (char*[]){KW_CALL, KW_BEGIN, KW_IF, IDENT, KW_WHILE, PERIOD, SEMICOLON, KW_PROC}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, KW_PROC)){
        immediate_tokens = (SyncTokens){1, (char*[]){IDENT}};
        MATCH(FIELD_TYPE, KW_PROC, "Expected PROCEDURE declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){SEMICOLON}};
        MATCH(FIELD_TYPE, IDENT, "Expected identifier in PROCEDURE declaration");

        immediate_tokens = (SyncTokens){8, (char*[]){KW_CONST, KW_VAR, KW_PROC, IDENT, KW_CALL, KW_BEGIN, KW_IF, KW_WHILE}}; //should have a SEMICOLON here, but we opted not to include it
        MATCH(FIELD_TYPE, SEMICOLON, "Missing ';'");

        bloco();

        immediate_tokens = (SyncTokens){8, (char*[]){KW_PROC,KW_CALL, KW_BEGIN, KW_IF, IDENT, KW_WHILE, PERIOD, SEMICOLON}};
        MATCH(FIELD_TYPE, SEMICOLON, "Missing ';'");

        procedimento();
    }
    //epsilon
}

void comando(){
    SyncTokens parent_tokens = {3, (char*[]){KW_END, PERIOD, SEMICOLON}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, IDENT)){
        immediate_tokens = (SyncTokens){1, (char*[]){"assign_op"}};
        MATCH(FIELD_TYPE, IDENT, "Expected identifier in COMMAND");

        immediate_tokens = (SyncTokens){5, (char*[]){IDENT, INT, LEFTPAR, PLUS, MINUS}};
        MATCH(FIELD_STR, ":=", "Use ':=' for assignment");

        expressao();
    } else if (!strcmp(current_token->type, KW_CALL)){
        immediate_tokens = (SyncTokens){1, (char*[]){IDENT}};
        MATCH(FIELD_TYPE, KW_CALL, "Expected CALL");

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, IDENT, "Expected identifier after CALL");
    } else if (!strcmp(current_token->type, KW_BEGIN)){ //BEGIN
        immediate_tokens = (SyncTokens){5, (char*[]){KW_CALL, KW_BEGIN, KW_IF, IDENT, KW_WHILE}};
        MATCH(FIELD_TYPE, KW_BEGIN, "Expected BEGIN");

        comando();
        mais_cmd();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, KW_END, "Expected END in BEGIN");
    } else if (!strcmp(current_token->type, KW_IF)){ //IF
        immediate_tokens = (SyncTokens){6, (char*[]){KW_ODD, IDENT, INT, LEFTPAR, PLUS, MINUS}};
        MATCH(FIELD_TYPE, KW_IF, "Expected IF");

        condicao();

        immediate_tokens = (SyncTokens){5, (char*[]){KW_CALL, KW_BEGIN, KW_IF, IDENT, KW_WHILE}};
        MATCH(FIELD_TYPE, KW_THEN, "Expected a THEN in IF condition");

        comando();
    } else if (!strcmp(current_token->type, KW_WHILE)){
        immediate_tokens = (SyncTokens){6, (char*[]){KW_ODD, IDENT, INT, LEFTPAR, PLUS, MINUS}};
        MATCH(FIELD_TYPE, KW_WHILE, "Expected KW_WHILE");

        condicao();

        immediate_tokens = (SyncTokens){5, (char*[]){KW_CALL, KW_BEGIN, KW_IF, IDENT, KW_WHILE}};
        MATCH(FIELD_TYPE, KW_DO, "Expected a DO in KW_WHILE condition");

        comando();
    }
    //epsilon
}

void mais_cmd(){
    SyncTokens parent_tokens = {2, (char*[]){KW_END, SEMICOLON}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, SEMICOLON)){
        immediate_tokens = (SyncTokens){5, (char*[]){KW_CALL, KW_BEGIN, KW_IF, IDENT, KW_WHILE}};
        MATCH(FIELD_TYPE, SEMICOLON, "Missing ';");

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
    SyncTokens parent_tokens = {5, (char*[]){IDENT, INT, LEFTPAR}};
    SyncTokens immediate_tokens = {0, NULL};
    
    if(!strcmp(current_token->type, PLUS)){
        MATCH(FIELD_TYPE, PLUS, "Expected '+'");
    }else if(!strcmp(current_token->type, MINUS)){
        MATCH(FIELD_TYPE, MINUS, "Expected '-'");
    }
    //epsilon
}

void termo(){
    fator();
    mais_fatores();
}

void mais_termos(){
    SyncTokens parent_tokens = {7, (char*[]){SEMICOLON, KW_END, PERIOD, RIGHTPAR, RELOP, KW_THEN, KW_DO}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, PLUS)){
        immediate_tokens = (SyncTokens){3, (char*[]){IDENT, INT, LEFTPAR}};
        MATCH(FIELD_TYPE, PLUS, "Expected '+'");

        termo();
        
        mais_termos();
    } else if(!strcmp(current_token->type, MINUS)){
        immediate_tokens = (SyncTokens){3, (char*[]){IDENT, INT, LEFTPAR}};
        MATCH(FIELD_TYPE, MINUS, "Expected '-'");

        termo();
        mais_termos();
    }
    //epsilon
}

void fator(){
    SyncTokens parent_tokens = {11, (char*[]){MULT, DIV, MINUS, PLUS, SEMICOLON, KW_END, PERIOD, RIGHTPAR, RELOP, KW_THEN, KW_DO}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, IDENT)){
        MATCH(FIELD_TYPE, IDENT, "Expected identifier in factor");
    }else if(!strcmp(current_token->type, INT)){
        MATCH(FIELD_TYPE, INT, "Expected number in factor");
    }else if(!strcmp(current_token->type, LEFTPAR)){
        immediate_tokens = (SyncTokens){5, (char*[]){IDENT, INT, LEFTPAR, PLUS, MINUS}};
        MATCH(FIELD_TYPE, LEFTPAR, "Expected opening parenthesis in expression");

        expressao();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, RIGHTPAR, "Missing closing parenthesis in expression");
    }else{
        print_error(out_file, "Expected identifier, number or expression", *current_token);
    }
}

void mais_fatores(){
    SyncTokens parent_tokens = {9, (char*[]){MINUS, PLUS, SEMICOLON, KW_END, PERIOD, RIGHTPAR, RELOP, KW_THEN, KW_DO}};
    SyncTokens immediate_tokens = {0, NULL};
    if(!strcmp(current_token->type, MULT)){
        immediate_tokens = (SyncTokens){5, (char*[]){IDENT, INT, LEFTPAR, PLUS, MINUS}};
        MATCH(FIELD_TYPE, MULT, "Expected multiply operator");
        fator();
        mais_fatores();
    }else if(!strcmp(current_token->type, DIV)){
        immediate_tokens = (SyncTokens){5, (char*[]){IDENT, INT, LEFTPAR, PLUS, MINUS}};
        MATCH(FIELD_TYPE, DIV, "Expected divide operator");
        fator();
        mais_fatores();
    }
    //epsilon
}

void condicao(){
    SyncTokens parent_tokens = {3, (char*[]){KW_THEN, KW_DO}};
    SyncTokens immediate_tokens = {0, NULL};
    if(!strcmp(current_token->type, KW_ODD)){
        immediate_tokens = (SyncTokens){5, (char*[]){IDENT, INT, LEFTPAR, PLUS, MINUS}};
        MATCH(FIELD_TYPE, KW_ODD, "Expected ODD keyword");

        expressao();
    }else{
        expressao();
        relacional();
        expressao();
    }
    //epsilon
}

void relacional(){
    SyncTokens parent_tokens = {3, (char*[]){PLUS, MINUS, IDENT, INT,LEFTPAR}};
    SyncTokens immediate_tokens = {0, NULL};
    if(!strcmp(current_token->type, RELOP)){
        MATCH(FIELD_TYPE, RELOP, "Expected a relational operator");
    }else{
        print_error(out_file, "Expected a relational operator", *current_token);
    }
}


