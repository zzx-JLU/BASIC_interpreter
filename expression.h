#pragma once
#include "basic_io.h"

typedef VARIANT OPERAND;

typedef enum
{
    /* �������� */
    oper_lparen = 0,    // ������
    oper_rparen,        // ������
    oper_plus,          // ��
    oper_minus,         // ��
    oper_multiply,      // ��
    oper_divide,        // ��
    oper_mod,           // ģ
    oper_power,         // ��
    oper_positive,      // ����
    oper_negative,      // ����
    oper_factorial,     // �׳�
    /* ��ϵ���� */
    oper_lt,            // С��
    oper_gt,            // ����
    oper_eq,            // ����
    oper_ne,            // ������
    oper_le,            // ������
    oper_ge,            // ��С��
    /* �߼����� */
    oper_and,           // ��
    oper_or,            // ��
    oper_not,           // ��
    /* ��ֵ */
    oper_assignment,    // ��ֵ
    oper_min            // ջ��
} operator_type;

typedef enum
{
    left2right,
    right2left
} associativity;

typedef struct
{
    int numbers;        // ������
    int icp;            // ���ȼ�
    int isp;            // ���ȼ�
    associativity ass;  // �����
    operator_type oper; // ������
} OPERATOR;

typedef enum
{
    token_operand = 1,
    token_operator
} token_type;

typedef struct
{
    token_type type;
    union
    {
        OPERAND var;
        OPERATOR ator;
    };
} TOKEN;

typedef struct tlist
{
    TOKEN token;
    struct tlist* next;
} TOKEN_LIST, * PTLIST;