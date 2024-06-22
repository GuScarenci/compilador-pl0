#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "lexer.h"
#include "IO_utils.h"

#ifndef __ERROR_HANDLER_H__
#define __ERROR_HANDLER_H__

void print_error(FILE *out_file, char* error_message, Token error_line);

void print_warning(FILE *out_file, char *warning_message, Token warning_token);

void print_final_message(FILE* out_file);

#endif