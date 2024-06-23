# Compilador de PL-0
Esse projeto foi feito para a disciplina SCC0605 - Teoria da Computação e Compiladores. Os integrantes do grupo são:
- Artur Brenner Weber. NUSP: 12675451.
- Gabriel Franceschi Libardi. NUSP: 11760739.
- Guilherme Castanon Silva Pereira. NUSP: 11801140.
- Gustavo Moura Scarenci de Carvalho Ferreira. NUSP: 12547792.
- Matheus Henrique Dias Cirillo. NUSP: 12547750.

Por enquanto, apenas o analisador sintático (a segunda etapa do compilador) está implementada.
## Como Compilar e Executar
Para compilar o código, basta chamar a diretiva `all` do `Makefile`:
```
make all
```
Para executar o código, basta chamar a diretiva `run` do `Makefile` e passar o arquivo de entrada e de saída:
```
make run ARGS="<source_file> <output_file>"
```

em que <source_file> é o caminho para o código fonte (que é um arquivo de texto), e <output_file> é a saída pedida com as mensagens de sucesso ou de erro de compilação. A execução do programa sobrescreve todo o conteúdo de <output_file> com a saída do *parser*. A diretiva `run` também compila automaticamente o código se o executável não foi encontrado. Procurou-se imprimir erros de compilação amigáveis e informativos caso haja erro no programa PL/0 de entrada.

Para a melhor vizualização da saída de erros, é recomendado que a saída seja o terminal `stdout`. De forma alternativa, se deseja-se jogar a saída para um arquivo de texto, recomenda-se fazer um `cat`do arquivo para o terminal para a vizualização da saída. Visto que a saída do programa faz destaque de erros e warnings com cores.

## #TODO List
- [x] Analisador Léxico.
- [x] Analisador Sintático.
- [ ] Analisador Semântico.
- [ ] Gerador de IR.
- [ ] Otimizador de código.
- [ ] Gerador de código de máquina.

#### Estrutura de Pastas
Todo o seu código deve ser colocado dentro da pasta _src/_ e você pode criar qualquer número de diretórios necessários dentro dela.
Você também pode criar pastas como _res/_, _scripts/_ na pasta raiz para auxiliar. Exemplos de código para teste estão no diretório _code/_, e os seus nomes seguem a convenção de _code/r\[numero\].pl0_ serem códigos válidos, e _code\[numero\].pl0_ serem programas inválidos.

#### Depuração
Para usar os recursos de depuração que este espaço de trabalho fornece, você precisa instalar a [extensão C/C++ da Microsoft](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools).

Para executar seu código sem depuração, você pode usar ``` Ctrl + F5 ``` e para executar seu código com as ferramentas de depuração e pontos de interrupção, use ``` F5 ```.

Para testar rapidamente seu código sem usar os recursos de depuração do vscode, você pode executar a tarefa **C: gcc build and run project** ou pode configurar um atalho em seu _keybindings.json_ usando:

```
    {
        "key": "alt+q",
        "command": "workbench.action.tasks.runTask",
        "args": "C: gcc build and run project",
        "when": "editorTextFocus"
    },
```

Há também uma tarefa para criar arquivos _.c_ e _.h_ com um código base. Você pode executá-la com **C: new .h and .c file** ou pode configurar um atalho em seu _keybindings.json_ usando:

```
    {
        "key": "alt+d",
        "command": "workbench.action.tasks.runTask",
        "args": "C: new .h and .c file",
        "when": "editorTextFocus"
    },
```

