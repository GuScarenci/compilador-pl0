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
*           Arquivo IO_util.c                               *
************************************************************/

#ifndef __IO_UTILS_H__
#define __IO_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define INIT_LEN 64 // Initial string size in readFirstLine
#define STR_GROWTH_FACTOR 2 // Rate at which string grows in readFirstLine

/*
 * Define codes for ANSI C standart colors.
 */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*
 * Returns the first argument passed to a variadic macro.
 */
#define _GET_FIRST_ARG(FIRST, ...) FIRST

/*
 * Returns the 17th argument passed to it. Its sole purpose is to
 * implement NUM_VA_ARGS.
 */
#define _GET_17TH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13,  \
    _14, _15, _16, _SEVENTEENTH, ...)                                          \
    _SEVENTEENTH

/*
 * Returns the number of arguments passed to a variadic macro. The
 * number of arguments must be greater than zero and less than or
 * equal to 16. The input is __VA_ARGS__, the output is the number
 * of arguments passed in __VA_ARGS__
 */
#define NUM_VA_ARGS(...)                                                       \
    _GET_17TH_ARG(                                                             \
        __VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

/*
 * ABORTPROGRAM aborts the program; showing the file, line, errno
 * and a message with printf-like formatting.
 */
#define ABORT_PROGRAM(...) fatalError(__LINE__, __FILE__, ##__VA_ARGS__);

/*
 * XALLOC allocates memory in the heap in the pointer p,
 * checking if the allocation failed and aborting the program if so
 */
#define XALLOC(type, p, size)                                                  \
    (p) = (type*)malloc(sizeof(type) * (size));                                \
    if (!(p)) {                                                                \
        ABORT_PROGRAM("malloc");                                               \
    }

// XREALLOC does the same as XALLOC, but uses realloc with a new size
#define XREALLOC(type, p, newsize)                                             \
    (p) = (type*)realloc(p, sizeof(type) * (newsize));                         \
    if (!(p)) {                                                                \
        ABORT_PROGRAM("realloc");                                              \
    }

// XCALLOC does the same as XALLOC, but uses calloc to set memory to '0's.
#define XCALLOC(type, p, size)                                                 \
    p = (type*)calloc(size, sizeof(type));                                     \
    if (!(p)) {                                                                \
        ABORT_PROGRAM("calloc");                                               \
    }

// OPEN_FILE does the same as fopen, but verifies if the specified file exists.
// usually, this would call ABORT_PROGRAM, however, because of runcodes test
// cases, a simple printf and a silent exit were used.
#define OPEN_FILE(file_pointer, filename, mode)                                \
    do {                                                                       \
        if (strcmp((filename), "stdout") == 0) {                               \
            (file_pointer) = stdout;                                           \
        } else {                                                               \
            (file_pointer) = fopen((filename), (mode));                        \
            if ((file_pointer) == NULL) {                                      \
                ABORT_PROGRAM("fopen: unable to open %s", (filename));          \
            }                                                                  \
        }                                                                      \
    } while (0);

// MEMSET_ALLOC does the same as XALLOC, but it initializes all bytes to chr
#define MEMSET_ALLOC(type, p, size, chr)                                       \
    XALLOC(type, p, size)                                                      \
    memset(p, chr, sizeof(type) * size);

/*
 * COLORFUL_PRINT prints a text in a color specified bby in user in file fp.
 */
#define COLORFUL_PRINT(fp, color, ...)                                         \
    fprintf(fp, color ##__VA_ARGS__);

/*
 * READ_INPUT does the same as scanf, but it verifies if the input
 * has a valid format and throws an appropriate error message.
 */
#define READ_INPUT(...)                                                        \
    do {                                                                       \
        int32_t matches = scanf(__VA_ARGS__);                                  \
        if (matches == EOF) {                                                  \
            ABORT_PROGRAM("Invalid input format");                             \
        }                                                                      \
        if (matches != NUM_VA_ARGS(__VA_ARGS__) - 1) {                         \
            ABORT_PROGRAM("Invalid input format: matched "                     \
                          "%d patterns of %d required. Use %s",                \
                matches, NUM_VA_ARGS(__VA_ARGS__) - 1,                         \
                _GET_FIRST_ARG(__VA_ARGS__))                                   \
        }                                                                      \
    } while (false)


/*
 * fatalError aborts the program with the file and line of the error, and a
 * printf-like formatted message
 */
void fatalError(int line, const char* file, const char* fmt, ...);

/*
 * readLine reads the first line of fp (relative to current position)
 * and stores it in line. The function deals with memory allocation, do not
 * pass a malloced pointer to it.
 */
void readLine(char** line, FILE* fp);

/*
 * min takes two ssize_ts and returns the smallest one.
 */
ssize_t min(ssize_t a, ssize_t b);

/*
 * strStrip trims all the blanck characters (as defined in ctype.h)
 * from the beggining and the end of a string.
 */
void strStrip(char** str_ptr);

#endif