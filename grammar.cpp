#include "basic_io.h"
#include <string.h>
#include "grammar.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "expression.cpp"
#define _CRT_SECURE_NO_WARNINGS

void exec_for(const STRING line)
{
    STRING l;
    char* s, * t;
    int top = top_for + 1;

    if (strnicmp(line, "FOR ", 4)) {
        goto errorhandler;
    }
    else if (top >= MEMORY_SIZE) {
        fprintf(stderr, "FOR 循环嵌套过深！/n");
        exit(EXIT_FAILURE);
    }

    strcpy(l, line);

    s = l + 4;
    while (*s && isspace(*s)) s++;
    if (isalpha(*s) && !isalnum(s[1])) {
        stack_for[top].id = toupper(*s) - 'A';
        stack_for[top].ln = cp;
    }
    else {
        goto errorhandler;
    }

    do {
        s++;
    } while (*s && isspace(*s));
    if (*s == '=') {
        s++;
    }
    else {
        goto errorhandler;
    }

    t = strstr(s, " TO ");
    if (t != NULL) {
        *t = 0;
        memory[stack_for[top].id] = eval(s);
        s = t + 4;
    }
    else {
        goto errorhandler;
    }

    t = strstr(s, " STEP ");
    if (t != NULL) {
        *t = 0;
        stack_for[top].target = eval(s).i;
        s = t + 5;
        stack_for[top].step = eval(s).i;
        if (fabs(stack_for[top].step) < 1E-6) {
            goto errorhandler;
        }
    }
    else {
        stack_for[top].target = eval(s).i;
        stack_for[top].step = 1;
    }

    if ((stack_for[top].step > 0 &&
        memory[stack_for[top].id].i > stack_for[top].target) ||
        (stack_for[top].step < 0 &&
            memory[stack_for[top].id].i < stack_for[top].target)) {
        while (cp < code_size && strcmp(code[cp].line, "NEXT")) {
            cp++;
        }
        if (cp >= code_size) {
            goto errorhandler;
        }
    }
    else {
        top_for++;
    }

    return;

errorhandler:
    fprintf(stderr, "Line %d: 语法错误！/n", code[cp].ln);
    exit(EXIT_FAILURE);
}

void exec_next(const STRING line)
{
    if (stricmp(line, "NEXT")) {
        fprintf(stderr, "Line %d: 语法错误！/n", code[cp].ln);
        exit(EXIT_FAILURE);
    }
    if (top_for < 0) {
        fprintf(stderr, "Line %d: NEXT 没有相匹配的 FOR！/n", code[cp].ln);
        exit(EXIT_FAILURE);
    }

    memory[stack_for[top_for].id].i += stack_for[top_for].step;
    if (stack_for[top_for].step > 0 &&
        memory[stack_for[top_for].id].i > stack_for[top_for].target) {
        top_for--;
    }
    else if (stack_for[top_for].step < 0 &&
        memory[stack_for[top_for].id].i < stack_for[top_for].target) {
        top_for--;
    }
    else {
        cp = stack_for[top_for].ln;
    }
}

void exec_goto(const STRING line)
{
    int ln;

    if (strnicmp(line, "GOTO ", 5)) {
        fprintf(stderr, "Line %d: 语法错误！/n", code[cp].ln);
        exit(EXIT_FAILURE);
    }

    ln = (int)eval(line + 5).i;
    if (ln > code[cp].ln) {
        // 往下跳转
        while (cp < code_size && ln != code[cp].ln) {
            if (!strnicmp(code[cp].line, "IF ", 3)) {
                top_if++;
                stack_if[top_if] = 1;
            }
            else if (!stricmp(code[cp].line, "ELSE")) {
                stack_if[top_if] = 1;
            }
            else if (!stricmp(code[cp].line, "END IF")) {
                top_if--;
            }
            else if (!strnicmp(code[cp].line, "WHILE ", 6)) {
                top_while++;
                stack_while[top_while].isrun = 1;
                stack_while[top_while].ln = cp;
            }
            else if (!stricmp(code[cp].line, "WEND")) {
                top_while--;
            }
            else if (!strnicmp(code[cp].line, "FOR ", 4)) {
                int i = 4;
                VARIANT v;
                while (isspace(code[cp].line[i])) i++;
                v = memory[toupper(code[cp].line[i]) - 'A'];
                exec_for(code[cp].line);
                memory[toupper(code[cp].line[i]) - 'A'] = v;
            }
            else if (!stricmp(code[cp].line, "NEXT")) {
                top_for--;
            }
            cp++;
        }
    }
    else if (ln < code[cp].ln) {
        // 往上跳转
        // 代码类似，此处省略
    }
    else {
        // 我不希望出现死循环，你可能有其他处理方式
        fprintf(stderr, "Line %d: 死循环！/n", code[cp].ln);
        exit(EXIT_FAILURE);
    }

    if (ln == code[cp].ln) {
        cp--;
    }
    else {
        fprintf(stderr, "标号 %d 不存在！/n", ln);
        exit(EXIT_FAILURE);
    }
}