#pragma once
#include "basic_io.h"
#define _CRT_SECURE_NO_WARNINGS

static struct
{
    int id;           // memory index
    int ln;           // line number
    double target;    // target value
    double step;
} stack_for[MEMORY_SIZE];

static int top_for = -1;

static int stack_if[MEMORY_SIZE];

static int top_if = -1;

static struct
{
	int isrun;
	int ln;
}stack_while[MEMORY_SIZE];

static int top_while = -1;

void exec_for(const STRING line);

void exec_next(const STRING line);

void exec_goto(const STRING line);

void exec_if(const STRING line);

void exec_else(const STRING line);

void exec_endif(const STRING line);

void exec_while(const STRING line);

void exec_wend(const STRING line);