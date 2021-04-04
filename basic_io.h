#pragma once
#define MEMORY_SIZE (26)
#define PROGRAM_SIZE (10000)

extern VARIANT memory[MEMORY_SIZE]; // 只支持a~z这26个变量，不区分大小写
extern CODE code[PROGRAM_SIZE];
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