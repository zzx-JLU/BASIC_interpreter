#include "basic_io.h"
#include <stdio.h>
#include <cassert>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "expression.h"
#define _CRT_SECURE_NO_WARNINGS

int cp = 1;
int code_size = 0;

void load_program(STRING filename)
{
    FILE* fp = fopen(filename, "r");
    int bg, ed;

    if (fp == NULL)
    {
        fprintf(stderr, "文件 %s 无法打开！\n", filename);
        exit(EXIT_FAILURE);
    }

    while (fscanf(fp, "%d", &code[cp].ln) != EOF)
    {
        if (code[cp].ln <= code[cp - 1].ln)
        {
            fprintf(stderr, "Line %d: 标号错误！\n", cp);
            exit(EXIT_FAILURE);
        }

        fgets(code[cp].line, sizeof(code[cp].line), fp);
        for (bg = 0; isspace(code[cp].line[bg]); bg++); // 过滤前面的空白符
        ed = (int)strlen(code[cp].line + bg) - 1;
        while (ed >= 0 && isspace(code[cp].line[ed + bg])) // 过滤后面的空白符
        {
            ed--;
        }
        if (ed >= 0)
        {
            memmove(code[cp].line, code[cp].line + bg, ed + 1);
            code[cp].line[ed + 1] = 0;
        }
        else
        {
            code[cp].line[0] = 0;
        }

        cp++;
        if (cp >= PROGRAM_SIZE)
        {
            fprintf(stderr, "程序%s太大，代码空间不足！\n", filename);
            exit(EXIT_FAILURE);
        }
    }

    code_size = cp;
    cp = 1;
}

void exec_input(const STRING line)
{
    const char* s = line;
    int n;

    assert(s != NULL);
    s += 5;

    while (*s)
    {
        while (*s && isspace(*s))
        {
            s++;
        }
        if (!isalpha(*s) || isalnum(*(s + 1)))
        {
            perror("变量名错误！\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            n = toupper(*s) - 'A';
        }

        if (!scanf("%lf", &memory[n].i))
        {
            int i;
            // 用户输入的是一个字符串
            memory[n].type = var_string;
            if ((memory[n].s[0] = getchar()) == '"')
            {
                for (i = 0; (memory[n].s[i] = getchar()) != '"'; i++);
            }
            else
            {
                for (i = 1; !isspace(memory[n].s[i] = getchar()); i++);
            }
            memory[n].s[i] = 0;
        }
        else
        {
            memory[n].type = var_double;
        }

        do
        {
            s++;
        } while (*s && isspace(*s));
        if (*s && *s != ',')
        {
            perror("INPUT 表达式语法错误！\n");
            exit(EXIT_FAILURE);
        }
        else if (*s)
        {
            s++;
        }
    }
}

void exec_print(const STRING line)
{
    STRING l;
    char* s, * e;
    VARIANT v;
    int c = 0;

    strcpy(l, line);
    s = l;

    assert(s != NULL);
    s += 5;

    for (;;)
    {
        for (e = s; *e && *e != ','; e++)
        {
            // 去除字符串
            if (*e == '"')
            {
                do
                {
                    e++;
                } while (*e && *e != '"');
            }
        }
        if (*e)
        {
            *e = 0;
        }
        else
        {
            e = NULL;
        }

        if (c++) putchar('\t');
        v = eval(s);
        if (v.type == var_double)
        {
            printf("%g", v.i);
        }
        else if (v.type == var_string)
        {
            printf(v.s);
        }

        if (e)
        {
            s = e + 1;
        }
        else
        {
            putchar('\n');
            break;
        }
    }
}

void exec_assignment(const STRING line)
{
    const char* s = line;
    int n;

    if (!strnicmp(s, "LET ", 4))
    {
        s += 4;
    }
    while (*s && isspace(*s))
    {
        s++;
    }
    if (!isalpha(*s) || isalnum(*(s + 1)))
    {
        perror("变量名错误！\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        n = toupper(*s) - 'A';
    }

    do
    {
        s++;
    } while (*s && isspace(*s));
    if (*s != '=')
    {
        fprintf(stderr, "赋值表达式 %s 语法错误！\n", line);
        exit(EXIT_FAILURE);
    }
    else
    {
        memory[n] = eval(s + 1);
    }
}