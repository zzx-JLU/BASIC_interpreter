#include "basic_io.h"
#include <string.h>
#include "grammar.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "expression.h"
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS

void exec_for(const STRING line)//这是读入的指令
{
    STRING l;
    char* s, * t;
    int top = top_for + 1;

    if (strnicmp(line, "FOR ", 4))//后面有空格！ 相等=0
    {
        goto errorhandler;//如果不是合法的for 进入错误处理
    }
    else if (top >= MEMORY_SIZE)//判断层数
    {
        printf("FOR 循环嵌套过深！/n");
        exit(EXIT_FAILURE);
    }

    strcpy(l, line);

    s = l + 4;//去掉l的前4位 即for+空格
    while (*s && isspace(*s)) s++;//找到s不是空格的地方 for可能后面有多个空格
    if (isalpha(*s) && !isalnum(s[1]))//功能：如果参数是数字或字母字符，函数返回非零值，否则返回零值。 前两位必须是字母+字符 注意 结束符也是字符

    {
        stack_for[top].id = toupper(*s) - 'A';//转大写 -A
        stack_for[top].ln = cp;
    }
    else
    {
        goto errorhandler;
    }

    do
    {
        s++;
    } while (*s && isspace(*s));
    if (*s == '=')
    {
        s++;
    }
    else
    {
        goto errorhandler;
    }

    t = strstr(s, " TO ");//函数返回一个指针，它指向字符串str2 首次出现于字符串str1中的位置，如果没有找到，返回NULL。
    if (t != NULL)
    {
        *t = 0;
        memory[stack_for[top].id] = eval(s);//等于之后计算FOR J = x TO y中的x值
        s = t + 4;//跳转到y处
    }
    else
    {
        goto errorhandler;
    }

    t = strstr(s, " STEP ");//查看是否有step 没有的话默认为1
    if (t != NULL)
    {
        *t = 0;
        stack_for[top].target = eval(s).i;//eval返回的数据类型中的i
        s = t + 5;
        stack_for[top].step = eval(s).i;
        if (fabs(stack_for[top].step) < 1E-6)//如果step过大就error
        {
            goto errorhandler;
        }
    }
    else
    {
        stack_for[top].target = eval(s).i;
        stack_for[top].step = 1;
    }

    if ((stack_for[top].step > 0 && memory[stack_for[top].id].i > stack_for[top].target) || (stack_for[top].step < 0 && memory[stack_for[top].id].i < stack_for[top].target))//step肯定也要大于0 否则永远无法出循环
    {
        while (cp < code_size && strcmp(code[cp].line, "NEXT"))//next是basic中for循环体的标志符 strcmp相等返回值是0
        {
			//此处是for循环结束 cp找到for的结束位置
            cp++;
        }
        if (cp >= code_size)
        {
            goto errorhandler;
        }
    }
    else
    {
        top_for++;
    }

    return;
errorhandler:
	fprintf(stderr, "Line %d: 语法错误！/n", code[cp].ln);
	exit(EXIT_FAILURE);
}



void exec_next(const STRING line)
{
    if (stricmp(line, "NEXT"))//如果没有匹配到
    {
        fprintf(stderr,"Line %d: 语法错误！\n", code[cp].ln);
        exit(EXIT_FAILURE);
    }
    if (top_for < 0)
    {
        fprintf(stderr,"Line %d: NEXT 没有相匹配的 FOR！\n", code[cp].ln);
        exit(EXIT_FAILURE);
    }

    memory[stack_for[top_for].id].i += stack_for[top_for].step;
    if (stack_for[top_for].step > 0 && memory[stack_for[top_for].id].i > stack_for[top_for].target)
    {
        top_for--;//退出当前循环
    }
    else if (stack_for[top_for].step < 0 && memory[stack_for[top_for].id].i < stack_for[top_for].target)
    {
        top_for--;//退出当前循环
    }//考虑了+-两种情况
    else
    {
        cp = stack_for[top_for].ln;
    }
}

void exec_goto(const STRING line)
{
    int ln;

    if (strnicmp(line, "GOTO ", 5))
    {
        fprintf(stderr,"Line %d: 语法错误！\n", code[cp].ln);
        exit(EXIT_FAILURE);
    }

    ln = (int)eval(line + 5).i;//goto 的目标代码行数
    if (ln > code[cp].ln)
    {
        // 往下跳转
        while (cp < code_size && ln != code[cp].ln)
        {
            if (!strnicmp(code[cp].line, "IF ", 3))
            {
                top_if++;
                stack_if[top_if] = 1;
            }
            else if (!stricmp(code[cp].line, "ELSE"))
            {
                stack_if[top_if] = 1;
            }
            else if (!stricmp(code[cp].line, "END IF"))
            {
                top_if--;
            }
            else if (!strnicmp(code[cp].line, "WHILE ", 6))
            {
                top_while++;
                stack_while[top_while].isrun = 1;
                stack_while[top_while].ln = cp;
            }
            else if (!stricmp(code[cp].line, "WEND"))
            {
                top_while--;
            }
            else if (!strnicmp(code[cp].line, "FOR ", 4))
            {
                int i = 4;
                VARIANT v;
                while (isspace(code[cp].line[i]))
					i++;
                v = memory[toupper(code[cp].line[i]) - 'A'];//找到for后面跟随的变量地址
                exec_for(code[cp].line);
                memory[toupper(code[cp].line[i]) - 'A'] = v;
            }
            else if (!stricmp(code[cp].line, "NEXT"))
            {
                top_for--;
            }
            cp++;
        }
    }
    else if (ln < code[cp].ln)
    {
        // 往上跳转
        // 代码类似，此处省略
    }
    else
    {
        // 我不希望出现死循环，你可能有其他处理方式
        fprintf(stderr,"Line %d: 死循环！/n", code[cp].ln);
        exit(EXIT_FAILURE);
    }

    if (ln == code[cp].ln)
    {
        cp--;
    }
    else
    {
        fprintf(stderr,"标号 %d 不存在！/n", ln);
        exit(EXIT_FAILURE);
    }
}
/*
exec_while,√
exec_wend,√
exec_if,√
exec_else,√
exec_endif,√
*/

void exec_if(const STRING line)
{
	STRING l;
	char* s, * t,*k;
	int top = top_if + 1;

	if (strnicmp(line, "IF ", 3))//后面有空格！ 相等=0
	{
		goto errorhandler;//如果不是合法的for 进入错误处理
	}
	else if (top >= MEMORY_SIZE)//判断层数
	{
		fprintf(stderr, "IF 循环嵌套过深！/n");
		exit(EXIT_FAILURE);
	}
	strcpy(l, line);
	s = l + 3;//去掉l的前3位 即while+空格
	while (*s && isspace(*s))
		s++;//找到s不是空格的地方 if可能后面有多个空格
	//if后面表达式不需要处理 交给eval处理

	t = strstr(s, "THEN");//找到THEN
	if (t != NULL)
	{
		strncpy(k, s, t - s);//待验证 目的是取出计算式子给eval计算
	}
	else
	{
		goto errorhandler;
	}
	if (!strcmp(eval(k).s, "true"))//step肯定也要大于0 否则永远无法出循环
	{
		top_if++;
		stack_if[top_if] = 1;//标记一下这个if是执行的不执行后面的else
		//不需要任何语句 因为返回后cp会++ 执行if语句的下一条
	}
	else
	{
		stack_if[top_if] = -1;//标记一下这个if是不执行的 需要执行else
		while (cp < code_size && (strcmp(code[cp].line, "ENDIF")|| strcmp(code[cp].line, "ELSE")))//找到endif或者else 结束if语句 转去执行这两个
		{
			cp++;
		}
		if (cp >= code_size)
		{
			goto errorhandler;
		}
	}

	return;

errorhandler:
	fprintf(stderr, "Line %d: 语法错误！\n", code[cp].ln);
	exit(EXIT_FAILURE);
}
void exec_else(const STRING line)
{
	if (strnicmp(line, "ELSE", 4))//后面有空格！ 相等=0
	{
		goto errorhandler;//如果不是合法的else 进入错误处理
	}
	else if (top_if < 0)//判断层数
	{
		fprintf(stderr, "ELSE 没有对应的IF/n");
		exit(EXIT_FAILURE);
	}
	if (stack_if[top_if] == 1)//if处已经执行 不需要执行else
	{
		while (cp < code_size && strcmp(code[cp].line, "ENDIF") )//找到endif或者else 结束if语句 转去执行这两个
		{
			cp++;
		}
		if (cp >= code_size)
		{
			goto errorhandler;
		}
	}
	else
	{//执行此代码 不需要cp++ 返回后会++
		return;
	}
errorhandler:
	fprintf(stderr, "Line %d: 语法错误！\n", code[cp].ln);
	exit(EXIT_FAILURE);
}
void exec_endif(const STRING line)
{
	if (strnicmp(line, "ENDIF", 5))//后面有空格！ 相等=0
	{
		goto errorhandler;//如果不是合法的for 进入错误处理
	}
	else if (top_if <0)//判断层数
	{
		fprintf(stderr, "ENDIF 没有对应的IF/n");
		exit(EXIT_FAILURE);
	}
	else
	{
		top_if--;//if语句结束
	}
errorhandler:
	fprintf(stderr, "Line %d: 语法错误！\n", code[cp].ln);
	exit(EXIT_FAILURE);
}
void exec_while(const STRING line)//这是读入的指令
{
	STRING l;
	char* s, * t;
	int top = top_while + 1;

	if (strnicmp(line, "WHILE ", 6))//后面有空格！ 相等=0
	{
		goto errorhandler;//如果不是合法的for 进入错误处理
	}
	else if (top >= MEMORY_SIZE)//判断层数
	{
		fprintf(stderr,"WHILE 循环嵌套过深！/n");
		exit(EXIT_FAILURE);
	}

	strcpy(l, line);
	s = l + 6;//去掉l的前4位 即while+空格
	while (*s && isspace(*s))
		s++;//找到s不是空格的地方 while可能后面有多个空格
	//while后面表达式不需要处理 交给eval处理


	if (!strcmp(eval(s).s,"true"))//step肯定也要大于0 否则永远无法出循环
	{
		while (cp < code_size && strcmp(code[cp].line, "WEND"))//找到wend
		{
			cp++;
		}
		if (cp >= code_size)
		{
			goto errorhandler;
		}
	}
	else
	{
		top_while++;
	}

	return;

errorhandler:
	fprintf(stderr,"Line %d: 语法错误！\n", code[cp].ln);
	exit(EXIT_FAILURE);
}

void exec_wend(const STRING line)
{
	if (stricmp(line, "WEND"))//如果没有匹配到
	{
		fprintf(stderr,"Line %d: 语法错误！\n", code[cp].ln);
		exit(EXIT_FAILURE);
	}
	if (top_while < 0)
	{
		fprintf(stderr,"Line %d: NEXT 没有相匹配的 FOR！\n", code[cp].ln);
		exit(EXIT_FAILURE);
	}
	//之后要判断里面
	memory[stack_for[top_for].id].i += stack_for[top_for].step;
	if (stack_for[top_for].step > 0 && memory[stack_for[top_for].id].i > stack_for[top_for].target)
	{
		top_for--;//退出当前循环
	}
	else if (stack_for[top_for].step < 0 && memory[stack_for[top_for].id].i < stack_for[top_for].target)
	{
		top_for--;//退出当前循环
	}//考虑了+-两种情况
	else
	{
		cp = stack_for[top_for].ln;
	}
}