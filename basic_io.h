#pragma once
#define MEMORY_SIZE (26)
#define PROGRAM_SIZE (10000)
#define _CRT_SECURE_NO_WARNINGS

extern int cp;
extern int code_size;

typedef enum
{
    var_null = 0,
    var_double,
    var_string
} variant_type;

typedef char STRING[128];

typedef struct
{
    variant_type type;
    union
    {
        double i;
        STRING s;
    };
} VARIANT;

typedef struct
{
    int ln;     // line number
    STRING line;
} CODE;

extern VARIANT memory[MEMORY_SIZE]; // ֻ֧��a~z��26�������������ִ�Сд
extern CODE code[PROGRAM_SIZE];

void load_program(STRING filename);

void exec_input(const STRING line);

void exec_print(const STRING line);

void exec_assignment(const STRING line);