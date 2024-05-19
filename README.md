# Compilador de PL-0
Esse projeto foi feito para a disciplina SCC0605 - Teoria da Computação e Compiladores. Os integrantes do grupo são:
- Artur Brenner Weber. NUSP: 12675451.
- Gabriel Franceschi Libardi. NUSP: 11760739.
- Guilherme Castanon Silva Pereira. NUSP: 11801140.
- Gustavo Moura Scarenci de Carvalho Ferreira. NUSP: 12547792.
- Matheus Henrique Dias Cirillo. NUSP: 12547750.

Por enquanto, apenas o analisador léxico (a primeira etapa do compilador) está implementada.
## Como Compilar e Executar
Para compilar o código, basta chamar a diretiva `all` do `Makefile`:
```
make all
```
Para executar o código, basta chamar a diretiva `run` do `Makefile` e passar o arquivo de entrada e de saída:
```
make run ARGS="<source_file> <output_file>"
```
em que <source_file> é o caminho para o código fonte (que é um arquivo de texto) e <output_file> é a saída pedida com a tabela de tokens e suas respectivas classes. A execução do programa sobrescreve todo o conteúdo de <output_file> com a saída do analisado léxico. A diretiva `run` também compila automaticamente o código se o executável não foi encontrado.

## #TODO List
- [x] Analisador Léxico.
- [ ] Analisador Sintático.
- [ ] Analisador Semântico.
- [ ] Gerador de IR.
- [ ] Otimizador de código.
- [ ] Gerador de código de máquina.


#### Folder structure
All your code should be placed inside _src/_ and you can create any numbers of directories needed inside it.
You can also create folders such as _res/_, _scripts/_ in the root folder to aid you.

#### Debugging

In order to use the debugging features this workspace provides you need to install the [C/C++ extension from Microsoft](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools).

To run your code without debugging you can use ``` Ctrl + F5 ``` and to run your code with the debugging tools and breakpoints use ``` F5 ```.


To quickly test run your code without using vscode's debug features you can run the task **C: gcc build and run project** or you can configure a keybind in your _keybindings.json_ using:

```
    {
        "key": "alt+q",
        "command": "workbench.action.tasks.runTask",
        "args": "C: gcc build and run project",
        "when": "editorTextFocus"
    },
```

There's also a task to create a _.c_ and _.h_ files with a base code, you can run it with **C: new .h and .c file**  or you can configure a keybind in your _keybindings.json_ using:
```
    {
        "key": "alt+d",
        "command": "workbench.action.tasks.runTask",
        "args": "C: new .h and .c file",
        "when": "editorTextFocus"
    },
```

