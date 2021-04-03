#pragma once
#include "basic_io.h"

static struct
{
    int id;           // memory index
    int ln;           // line number
    double target;    // target value
    double step;
} stack_for[MEMORY_SIZE];

static int top_for = -1;