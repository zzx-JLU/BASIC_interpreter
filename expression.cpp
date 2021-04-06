#include "expression.h"
#include <iostream>
#include <malloc.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS

TOKEN before;
const char* e = NULL;

static const OPERATOR operators[] = {
	/* 算数运算 */
	{2, 17, 1, left2right, oper_lparen},     // 左括号
	{2, 17, 17, left2right, oper_rparen},    // 右括号
	{2, 12, 12, left2right, oper_plus},      // 加
	{2, 12, 12, left2right, oper_minus},     // 减
	{2, 13, 13, left2right, oper_multiply},  // 乘
	{2, 13, 13, left2right, oper_divide},    // 除
	{2, 13, 13, left2right, oper_mod},       // 模
	{2, 14, 14, left2right, oper_power},     // 幂
	{1, 16, 15, right2left, oper_positive},  // 正号
	{1, 16, 15, right2left, oper_negative},  // 负号
	{1, 16, 15, left2right, oper_factorial}, // 阶乘
	/* 关系运算 */
	{2, 10, 10, left2right, oper_lt},        // 小于
	{2, 10, 10, left2right, oper_gt},        // 大于
	{2, 9, 9, left2right, oper_eq},          // 等于
	{2, 9, 9, left2right, oper_ne},          // 不等于
	{2, 10, 10, left2right, oper_le},        // 不大于
	{2, 10, 10, left2right, oper_ge},        // 不小于
	/* 逻辑运算 */
	{2, 5, 5, left2right, oper_and},         // 且
	{2, 4, 4, left2right, oper_or},          // 或
	{1, 15, 15, right2left, oper_not},       // 非
	/* 赋值 */
	// BASIC 中赋值语句不属于表达式！
	{2, 2, 2, right2left, oper_assignment},  // 赋值
	/* 最小优先级 */
	{2, 0, 0, right2left, oper_min}          // 栈底
};

PTLIST infix2postfix(const char expr[])
{
	PTLIST list = NULL, tail, p;
	PTLIST stack = NULL;
	e = expr;
	// 初始时在临时空间放一个优先级最低的操作符
	// 这样就不用判断是否为空了，方便编码
	stack = (PTLIST)calloc(1, sizeof(TOKEN_LIST));
	stack->next = NULL;
	stack->token.type = token_operator;
	stack->token.ator = operators[oper_min];
	// before 为全局变量，用于保存之前的操作符
	memset(&before, 0, sizeof(before));
	for (;;)
	{
		p = (PTLIST)calloc(1, sizeof(TOKEN_LIST));
		// calloc 自动初始化
		p->next = NULL;
		p->token = next_token();
		if (p->token.type == token_operand)
		{
			// 如果是操作数，就不用客气，直接输出
			if (!list)
			{
				list = tail = p;
			}
			else
			{
				tail->next = p;
				tail = p;
			}
		}
		else if (p->token.type == token_operator)
		{
			if (p->token.ator.oper == oper_rparen)
			{
				// 右括号
				free(p);
				while (stack->token.ator.oper != oper_lparen)
				{
					p = stack;
					stack = stack->next;
					tail->next = p;
					tail = p;
					tail->next = NULL;
				}
				p = stack;
				stack = stack->next;
				free(p);
			}
			else
			{
				while (stack->token.ator.isp >= p->token.ator.icp)
				{
					tail->next = stack;
					stack = stack->next;
					tail = tail->next;
					tail->next = NULL;
				}
				p->next = stack;
				stack = p;
			}
		}
		else
		{
			free(p);
			break;
		}
	}
	while (stack)
	{
		p = stack;
		stack = stack->next;
		if (p->token.ator.oper != oper_min)
		{
			p->next = NULL;
			tail->next = p;
			tail = p;
		}
		else
		{
			free(p);
		}
	}
	return list;
}

VARIANT eval(const char expr[])
{
	// ...
	// 一些变量的定义和声明
	PTLIST p;
	PTLIST op1, op2;
	char* s1, * s2;
	PTLIST stack = (PTLIST)calloc(1, sizeof(TOKEN_LIST));
	stack->next = NULL;
	stack->token.type = token_operator;
	stack->token.ator = operators[oper_min];
	// 将中缀表达式转换成后缀表达式
	PTLIST list = infix2postfix(expr);
	while (list)
	{
		// 取出一个 token
		p = list;
		list = list->next;

		// 如果是操作数就保存到 stack 中
		if (p->token.type == token_operand)
		{
			p->next = stack;
			stack = p;
			continue;
		}

		// 如果是操作符...
		switch (p->token.ator.oper)
		{
		// 加法运算
		case oper_plus:
			// 取出 stack 中最末两个操作数
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.i += op2->token.var.i;
			}
			else
			{
				// 字符串的加法即合并两个字符串
				// 如果其中一个是数字则需要先转换为字符串
				if (op1->token.var.type == var_double)
				{
					sprintf(s1, "%g", op1->token.var.i);
				}
				else
				{
					strcpy(s1, op1->token.var.s);
				}
				if (op2->token.var.type == var_double)
				{
					sprintf(s2, "%g", op2->token.var.i);
				}
				else
				{
					strcpy(s2, op2->token.var.s);
				}
				op1->token.var.type = var_string;
				strcat(s1, s2);
				strcpy(op1->token.var.s, s1);
			}
			free(op2);
			break;
		// 减法运算
		case oper_minus:
			// 取出 stack 中最末两个操作数
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.i -= op2->token.var.i;
			}
			else
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
			free(op2);
			break;
		// 乘法运算
		case oper_multiply:
			// 取出 stack 中最末两个操作数
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.i *= op2->token.var.i;
			}
			else
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
			free(op2);
			break;
		// 除法运算
		case oper_divide:
			// 取出 stack 中最末两个操作数
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double &&
				op2->token.var.i != 0)
			{
				op1->token.var.i *= op2->token.var.i;
			}
			else if (op2->token.var.i == 0)
			{
				fprintf(stderr, "除数为0!\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
			free(op2);
			break;
		// 取余运算
		case oper_mod:
			// 取出 stack 中最末两个操作数
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type != var_double ||
				op2->token.var.type != var_double)
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
			else if (op2->token.var.i == 0)
			{
				fprintf(stderr, "除数为0!\n");
				exit(EXIT_FAILURE);
			}
			else if (op1->token.var.i - (int)op1->token.var.i != 0 ||
				op2->token.var.i - (int)op2->token.var.i != 0)
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				op1->token.var.i = (int)op1->token.var.i % (int)op2->token.var.i;
			}
			free(op2);
			break;
		// 幂运算
		case oper_power:
			// 取出 stack 中最末两个操作数
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.i = pow(op1->token.var.i, op2->token.var.i);
			}
			else
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
			free(op2);
			break;
		// 正号
		case oper_positive:
			op2 = stack;
			if (op2->token.var.type != var_double)
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
		// 负号
		case oper_negative:
			op2 = stack;
			if (op2->token.var.type == var_double)
			{
				op2->token.var.i = -op2->token.var.i;
			}
			else
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
		// 阶乘运算
		case oper_factorial:
			op2 = stack;
			if (op2->token.var.type == var_double)
			{
				if (op2->token.var.i - (int)op2->token.var.i != 0)
				{
					fprintf(stderr, "类型错误!\n");
					exit(EXIT_FAILURE);
				}
				else
				{
					int res = 1;
					for (int i = 2; i <= (int)op2->token.var.i; i++)
					{
						res *= i;
					}
					op2->token.var.i = res;
				}
			}
			else
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
		// 小于
		case oper_lt:
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.type = var_string;
				if (op1->token.var.i < op2->token.var.i)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			else
			{
				if (op1->token.var.type == var_double)
				{
					sprintf(s1, "%g", op1->token.var.i);
				}
				else
				{
					strcpy(s1, op1->token.var.s);
				}
				if (op2->token.var.type == var_double)
				{
					sprintf(s2, "%g", op2->token.var.i);
				}
				else
				{
					strcpy(s2, op2->token.var.s);
				}
				if (strcmp(s1, s2) < 0)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			free(op2);
			break;
		// 大于
		case oper_gt:
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.type = var_string;
				if (op1->token.var.i > op2->token.var.i)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			else
			{
				if (op1->token.var.type == var_double)
				{
					sprintf(s1, "%g", op1->token.var.i);
				}
				else
				{
					strcpy(s1, op1->token.var.s);
				}
				if (op2->token.var.type == var_double)
				{
					sprintf(s2, "%g", op2->token.var.i);
				}
				else
				{
					strcpy(s2, op2->token.var.s);
				}
				if (strcmp(s1, s2) > 0)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			free(op2);
			break;
		// 等于
		case oper_eq:
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.type = var_string;
				if (op1->token.var.i == op2->token.var.i)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			else
			{
				if (op1->token.var.type == var_double)
				{
					sprintf(s1, "%g", op1->token.var.i);
				}
				else
				{
					strcpy(s1, op1->token.var.s);
				}
				if (op2->token.var.type == var_double)
				{
					sprintf(s2, "%g", op2->token.var.i);
				}
				else
				{
					strcpy(s2, op2->token.var.s);
				}
				if (strcmp(s1, s2) == 0)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			free(op2);
			break;
		// 不等于
		case oper_ne:
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.type = var_string;
				if (op1->token.var.i != op2->token.var.i)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			else
			{
				if (op1->token.var.type == var_double)
				{
					sprintf(s1, "%g", op1->token.var.i);
				}
				else
				{
					strcpy(s1, op1->token.var.s);
				}
				if (op2->token.var.type == var_double)
				{
					sprintf(s2, "%g", op2->token.var.i);
				}
				else
				{
					strcpy(s2, op2->token.var.s);
				}
				if (strcmp(s1, s2) != 0)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			free(op2);
			break;
		// 小于等于
		case oper_le:
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.type = var_string;
				if (op1->token.var.i <= op2->token.var.i)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			else
			{
				if (op1->token.var.type == var_double)
				{
					sprintf(s1, "%g", op1->token.var.i);
				}
				else
				{
					strcpy(s1, op1->token.var.s);
				}
				if (op2->token.var.type == var_double)
				{
					sprintf(s2, "%g", op2->token.var.i);
				}
				else
				{
					strcpy(s2, op2->token.var.s);
				}
				if (strcmp(s1, s2) <= 0)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			free(op2);
			break;
		// 大于等于
		case oper_ge:
			op2 = stack;
			op1 = stack = stack->next;

			if (op1->token.var.type == var_double &&
				op2->token.var.type == var_double)
			{
				op1->token.var.type = var_string;
				if (op1->token.var.i >= op2->token.var.i)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			else
			{
				if (op1->token.var.type == var_double)
				{
					sprintf(s1, "%g", op1->token.var.i);
				}
				else
				{
					strcpy(s1, op1->token.var.s);
				}
				if (op2->token.var.type == var_double)
				{
					sprintf(s2, "%g", op2->token.var.i);
				}
				else
				{
					strcpy(s2, op2->token.var.s);
				}
				if (strcmp(s1, s2) >= 0)
				{
					strcpy(op1->token.var.s, "true");
				}
				else
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			free(op2);
			break;
		// 逻辑与
		case oper_and:
			op2 = stack;
			op1 = stack = stack->next;
			if (op1->token.var.type == var_double ||
				op2->token.var.type == var_double)
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				if (strcmp(op1->token.var.s, "true") ||
					strcmp(op1->token.var.s, "false"))
				{
					fprintf(stderr, "类型错误!\n");
					exit(EXIT_FAILURE);
				}
				if (strcmp(op2->token.var.s, "true") ||
					strcmp(op2->token.var.s, "false"))
				{
					fprintf(stderr, "类型错误!\n");
					exit(EXIT_FAILURE);
				}
				if (!strcmp(op1->token.var.s, "false") ||
					!strcmp(op2->token.var.s, "false"))
				{
					strcpy(op1->token.var.s, "false");
				}
			}
			free(op2);
			break;
		// 逻辑或
		case oper_or:
			op2 = stack;
			op1 = stack = stack->next;
			if (op1->token.var.type == var_double ||
				op2->token.var.type == var_double)
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				if (strcmp(op1->token.var.s, "true") ||
					strcmp(op1->token.var.s, "false"))
				{
					fprintf(stderr, "类型错误!\n");
					exit(EXIT_FAILURE);
				}
				if (strcmp(op2->token.var.s, "true") ||
					strcmp(op2->token.var.s, "false"))
				{
					fprintf(stderr, "类型错误!\n");
					exit(EXIT_FAILURE);
				}
				if (!strcmp(op1->token.var.s, "true") ||
					!strcmp(op2->token.var.s, "true"))
				{
					strcpy(op1->token.var.s, "true");
				}
			}
			free(op2);
			break;
		// 逻辑非
		case oper_not:
			op2 = stack;
			if (op2->token.var.type == var_double)
			{
				fprintf(stderr, "类型错误!\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				if (strcmp(op2->token.var.s, "true") ||
					strcmp(op2->token.var.s, "false"))
				{
					fprintf(stderr, "类型错误!\n");
					exit(EXIT_FAILURE);
				}
				if (!strcmp(op2->token.var.s, "true"))
				{
					strcpy(op2->token.var.s, "false");
				}
				else if (!strcmp(op2->token.var.s, "false"))
				{
					strcpy(op2->token.var.s, "true");
				}
			}
			break;
		default:
			// 无效操作符处理
			fprintf(stderr, "无效的操作符!\n");
			exit(EXIT_FAILURE);
			break;
		}
		free(p);
	}

	VARIANT value = stack->token.var;
	free(stack);

	// 最后一个元素即表达式的值
	return value;
}

static TOKEN next_token()
{
	TOKEN token = { token_unknown };
	STRING s;
	int i;
	if (e == NULL)
	{
		return token;
	}
	// 去掉前导空格
	while (*e && isspace(*e))
	{
		e++;
	}
	if (*e == 0)
	{
		return token;
	}
	if (*e == '"')
	{
		// 字符串
		token.type = token_operand;
		token.var.type = var_string;
		e++;
		for (i = 0; *e && *e != '"'; i++) {
			token.var.s[i] = *e;
			e++;
		}
		e++;
	}
	else if (isalpha(*e))
	{
		// 如果首字符为字母则有两种情况
		// 1. 变量
		// 2. 逻辑操作符
		token.type = token_operator;
		for (i = 0; isalnum(*e); i++)
		{
			s[i] = toupper(*e);
			e++;
		}
		s[i] = 0;
		if (!strcmp(s, "AND"))
		{
			token.ator = operators[oper_and];
		}
		else if (!strcmp(s, "OR"))
		{
			token.ator = operators[oper_or];
		}
		else if (!strcmp(s, "NOT"))
		{
			token.ator = operators[oper_not];
		}
		else if (i == 1)
		{
			token.type = token_operand;
			token.var = memory[s[0] - 'A'];
			if (token.var.type == var_null)
			{
				memset(&token, 0, sizeof(token));
				fprintf(stderr, "变量%c未赋值！\n", s[0]);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			fprintf(stderr, "%s:无法识别的字符序列!\n", s);
			exit(EXIT_FAILURE);
		}
	}
	else if (isdigit(*e) || *e == '.')
	{
		// 数字
		token.type = token_operand;
		token.var.type = var_double;
		for (i = 0; isdigit(*e) || *e == '.'; i++)
		{
			s[i] = *e;
			e++;
		}
		s[i] = 0;
		if (sscanf(s, "%lf", &token.var.i) != 1)
		{
			// 读取数字失败！
			// 错误处理
			fprintf(stderr, "%s:读取数字失败!\n", s);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		// 剩下算数运算符和关系运算符
		token.type = token_operator;
		switch (*e)
		{
		case '(':
			token.ator = operators[oper_lparen];
			break;
		case ')':
			token.ator = operators[oper_rparen];
			break;
		case '+':
			if (before.type == token_operand)
			{
				token.ator = operators[oper_plus];
			}
			else
			{
				token.ator = operators[oper_positive];
			}
			break;
		case '-':
			if (before.type == token_operand)
			{
				token.ator = operators[oper_minus];
			}
			else
			{
				token.ator = operators[oper_negative];
			}
			break;
		case '*':
			token.ator = operators[oper_multiply];
			break;
		case '/':
			token.ator = operators[oper_divide];
			break;
		case '%':
			token.ator = operators[oper_mod];
			break;
		case '^':
			token.ator = operators[oper_power];
			break;
		case '!':
			if (*(e + 1) == '=')
			{
				token.ator = operators[oper_ne];
			}
			else
			{
				token.ator = operators[oper_factorial];
			}
			break;
		case '<':
			if (*(e + 1) == '=')
			{
				token.ator = operators[oper_le];
			}
			else
			{
				token.ator = operators[oper_lt];
			}
			break;
		case '>':
			if (*(e + 1) == '=')
			{
				token.ator = operators[oper_ge];
			}
			else
			{
				token.ator = operators[oper_gt];
			}
			break;
		case '=':
			token.ator = operators[oper_eq];
			break;
		default:
			// 不可识别的操作符
			fprintf(stderr, "%c:不可识别的操作符!\n", *e);
			exit(EXIT_FAILURE);
			break;
		}
		e++;
	}
	before = token;
	return token;
}