#************************************************************
#             Trabalho 2 - SCC0605                          *
#                                                           *
#      Nome: Artur Brenner Weber                            *
#      nUSP: 12675451                                       *
#      Nome: Gabriel Franceschi Libardi                     *
#      nUSP: 11760739                                       *
#      Nome: Guilherme Castanon Silva Pereira               *
#      nUSP: 11801140                                       *
#      Nome: Gustavo Scarenci de Carvalho Pereira           *
#      nUSP: 12547792                                       *
#      Nome: Matheus Henrique Dias Cirillo                  *
#      nUSP: 12547750                                       *
#      Data de última atualizacao: 22/06/2024               *
#      Ambiente: VSCode 1.89.1                              *
#                                                           *
#           Arquivo Makefile                                *
#***********************************************************/

CC ?= gcc
ZIP ?= zip
PDFLAGS += -march=native
CFLAGS += -Wall -Wextra -Wpedantic -std=gnu99
LDFLAGS += -lm
VDFLAGS += --leak-check=full --show-leak-kinds=all -s

SRCDIRS = $(sort $(dir $(wildcard ./*/)))
INC_PARAMS = $(addprefix -I ,$(SRCDIRS))

EXECUTABLE ?= build/program

ZIPFILE    ?= ../zipfile.zip
CFILES      = $(wildcard src/*.c) $(wildcard src/**/*.c)

.PHONY: all clean zip run test debug

all: PDFLAGS += -O3
all: $(EXECUTABLE)

clean:
	@rm -f $(ZIPFILE)
	@rm -rf build/
	@rm -rf out/*.csv

zip: clean
	$(ZIP) a $(ZIPFILE) ./include/* ./src/* ./res/*.csv ./res/*.pl0 ./res/*.drawio ./Makefile ./README.md ./*.pdf

run: $(EXECUTABLE)
	@./$(EXECUTABLE) $(ARGS)

debug: CFLAGS+= -g
debug: clean
debug: $(EXECUTABLE)

$(EXECUTABLE): $(CFILES)
	@mkdir -p build
	$(CC) $(CFLAGS) $(INC_PARAMS) -o $@ $^ $(LDFLAGS) $(PDFLAGS)

valgrind: debug
	valgrind $(VDFLAGS) ./$(EXECUTABLE) $(ARGS)
