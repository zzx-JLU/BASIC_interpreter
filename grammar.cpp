#include "basic_io.h"
#include <string.h>
#include "grammar.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "expression.h"
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS

void exec_for(const STRING line)//���Ƕ����ָ��
{
    STRING l;
    char* s, * t;
    int top = top_for + 1;

    if (strnicmp(line, "FOR ", 4))//�����пո� ���=0
    {
        goto errorhandler;//������ǺϷ���for ���������
    }
    else if (top >= MEMORY_SIZE)//�жϲ���
    {
        printf("FOR ѭ��Ƕ�׹��/n");
        exit(EXIT_FAILURE);
    }

    strcpy(l, line);

    s = l + 4;//ȥ��l��ǰ4λ ��for+�ո�
    while (*s && isspace(*s)) s++;//�ҵ�s���ǿո�ĵط� for���ܺ����ж���ո�
    if (isalpha(*s) && !isalnum(s[1]))//���ܣ�������������ֻ���ĸ�ַ����������ط���ֵ�����򷵻���ֵ�� ǰ��λ��������ĸ+�ַ� ע�� ������Ҳ���ַ�

    {
        stack_for[top].id = toupper(*s) - 'A';//ת��д -A
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

    t = strstr(s, " TO ");//��������һ��ָ�룬��ָ���ַ���str2 �״γ������ַ���str1�е�λ�ã����û���ҵ�������NULL��
    if (t != NULL)
    {
        *t = 0;
        memory[stack_for[top].id] = eval(s);//����֮�����FOR J = x TO y�е�xֵ
        s = t + 4;//��ת��y��
    }
    else
    {
        goto errorhandler;
    }

    t = strstr(s, " STEP ");//�鿴�Ƿ���step û�еĻ�Ĭ��Ϊ1
    if (t != NULL)
    {
        *t = 0;
        stack_for[top].target = eval(s).i;//eval���ص����������е�i
        s = t + 5;
        stack_for[top].step = eval(s).i;
        if (fabs(stack_for[top].step) < 1E-6)//���step�����error
        {
            goto errorhandler;
        }
    }
    else
    {
        stack_for[top].target = eval(s).i;
        stack_for[top].step = 1;
    }

    if ((stack_for[top].step > 0 && memory[stack_for[top].id].i > stack_for[top].target) || (stack_for[top].step < 0 && memory[stack_for[top].id].i < stack_for[top].target))//step�϶�ҲҪ����0 ������Զ�޷���ѭ��
    {
        while (cp < code_size && strcmp(code[cp].line, "NEXT"))//next��basic��forѭ����ı�־�� strcmp��ȷ���ֵ��0
        {
			//�˴���forѭ������ cp�ҵ�for�Ľ���λ��
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
	fprintf(stderr, "Line %d: �﷨����/n", code[cp].ln);
	exit(EXIT_FAILURE);
}



void exec_next(const STRING line)
{
    if (stricmp(line, "NEXT"))//���û��ƥ�䵽
    {
        fprintf(stderr,"Line %d: �﷨����\n", code[cp].ln);
        exit(EXIT_FAILURE);
    }
    if (top_for < 0)
    {
        fprintf(stderr,"Line %d: NEXT û����ƥ��� FOR��\n", code[cp].ln);
        exit(EXIT_FAILURE);
    }

    memory[stack_for[top_for].id].i += stack_for[top_for].step;
    if (stack_for[top_for].step > 0 && memory[stack_for[top_for].id].i > stack_for[top_for].target)
    {
        top_for--;//�˳���ǰѭ��
    }
    else if (stack_for[top_for].step < 0 && memory[stack_for[top_for].id].i < stack_for[top_for].target)
    {
        top_for--;//�˳���ǰѭ��
    }//������+-�������
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
        fprintf(stderr,"Line %d: �﷨����\n", code[cp].ln);
        exit(EXIT_FAILURE);
    }

    ln = (int)eval(line + 5).i;//goto ��Ŀ���������
    if (ln > code[cp].ln)
    {
        // ������ת
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
                v = memory[toupper(code[cp].line[i]) - 'A'];//�ҵ�for�������ı�����ַ
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
        // ������ת
        // �������ƣ��˴�ʡ��
    }
    else
    {
        // �Ҳ�ϣ��������ѭ�������������������ʽ
        fprintf(stderr,"Line %d: ��ѭ����/n", code[cp].ln);
        exit(EXIT_FAILURE);
    }

    if (ln == code[cp].ln)
    {
        cp--;
    }
    else
    {
        fprintf(stderr,"��� %d �����ڣ�/n", ln);
        exit(EXIT_FAILURE);
    }
}
/*
exec_while,��
exec_wend,��
exec_if,��
exec_else,��
exec_endif,��
*/

void exec_if(const STRING line)
{
	STRING l;
	char* s, * t,*k;
	int top = top_if + 1;

	if (strnicmp(line, "IF ", 3))//�����пո� ���=0
	{
		goto errorhandler;//������ǺϷ���for ���������
	}
	else if (top >= MEMORY_SIZE)//�жϲ���
	{
		fprintf(stderr, "IF ѭ��Ƕ�׹��/n");
		exit(EXIT_FAILURE);
	}
	strcpy(l, line);
	s = l + 3;//ȥ��l��ǰ3λ ��while+�ո�
	while (*s && isspace(*s))
		s++;//�ҵ�s���ǿո�ĵط� if���ܺ����ж���ո�
	//if������ʽ����Ҫ���� ����eval����

	t = strstr(s, "THEN");//�ҵ�THEN
	if (t != NULL)
	{
		strncpy(k, s, t - s);//����֤ Ŀ����ȡ������ʽ�Ӹ�eval����
	}
	else
	{
		goto errorhandler;
	}
	if (!strcmp(eval(k).s, "true"))//step�϶�ҲҪ����0 ������Զ�޷���ѭ��
	{
		top_if++;
		stack_if[top_if] = 1;//���һ�����if��ִ�еĲ�ִ�к����else
		//����Ҫ�κ���� ��Ϊ���غ�cp��++ ִ��if������һ��
	}
	else
	{
		stack_if[top_if] = -1;//���һ�����if�ǲ�ִ�е� ��Ҫִ��else
		while (cp < code_size && (strcmp(code[cp].line, "ENDIF")|| strcmp(code[cp].line, "ELSE")))//�ҵ�endif����else ����if��� תȥִ��������
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
	fprintf(stderr, "Line %d: �﷨����\n", code[cp].ln);
	exit(EXIT_FAILURE);
}
void exec_else(const STRING line)
{
	if (strnicmp(line, "ELSE", 4))//�����пո� ���=0
	{
		goto errorhandler;//������ǺϷ���else ���������
	}
	else if (top_if < 0)//�жϲ���
	{
		fprintf(stderr, "ELSE û�ж�Ӧ��IF/n");
		exit(EXIT_FAILURE);
	}
	if (stack_if[top_if] == 1)//if���Ѿ�ִ�� ����Ҫִ��else
	{
		while (cp < code_size && strcmp(code[cp].line, "ENDIF") )//�ҵ�endif����else ����if��� תȥִ��������
		{
			cp++;
		}
		if (cp >= code_size)
		{
			goto errorhandler;
		}
	}
	else
	{//ִ�д˴��� ����Ҫcp++ ���غ��++
		return;
	}
errorhandler:
	fprintf(stderr, "Line %d: �﷨����\n", code[cp].ln);
	exit(EXIT_FAILURE);
}
void exec_endif(const STRING line)
{
	if (strnicmp(line, "ENDIF", 5))//�����пո� ���=0
	{
		goto errorhandler;//������ǺϷ���for ���������
	}
	else if (top_if <0)//�жϲ���
	{
		fprintf(stderr, "ENDIF û�ж�Ӧ��IF/n");
		exit(EXIT_FAILURE);
	}
	else
	{
		top_if--;//if������
	}
errorhandler:
	fprintf(stderr, "Line %d: �﷨����\n", code[cp].ln);
	exit(EXIT_FAILURE);
}
void exec_while(const STRING line)//���Ƕ����ָ��
{
	STRING l;
	char* s, * t;
	int top = top_while + 1;

	if (strnicmp(line, "WHILE ", 6))//�����пո� ���=0
	{
		goto errorhandler;//������ǺϷ���for ���������
	}
	else if (top >= MEMORY_SIZE)//�жϲ���
	{
		fprintf(stderr,"WHILE ѭ��Ƕ�׹��/n");
		exit(EXIT_FAILURE);
	}

	strcpy(l, line);
	s = l + 6;//ȥ��l��ǰ4λ ��while+�ո�
	while (*s && isspace(*s))
		s++;//�ҵ�s���ǿո�ĵط� while���ܺ����ж���ո�
	//while������ʽ����Ҫ���� ����eval����


	if (!strcmp(eval(s).s,"true"))//step�϶�ҲҪ����0 ������Զ�޷���ѭ��
	{
		while (cp < code_size && strcmp(code[cp].line, "WEND"))//�ҵ�wend
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
	fprintf(stderr,"Line %d: �﷨����\n", code[cp].ln);
	exit(EXIT_FAILURE);
}

void exec_wend(const STRING line)
{
	if (stricmp(line, "WEND"))//���û��ƥ�䵽
	{
		fprintf(stderr,"Line %d: �﷨����\n", code[cp].ln);
		exit(EXIT_FAILURE);
	}
	if (top_while < 0)
	{
		fprintf(stderr,"Line %d: NEXT û����ƥ��� FOR��\n", code[cp].ln);
		exit(EXIT_FAILURE);
	}
	//֮��Ҫ�ж�����
	memory[stack_for[top_for].id].i += stack_for[top_for].step;
	if (stack_for[top_for].step > 0 && memory[stack_for[top_for].id].i > stack_for[top_for].target)
	{
		top_for--;//�˳���ǰѭ��
	}
	else if (stack_for[top_for].step < 0 && memory[stack_for[top_for].id].i < stack_for[top_for].target)
	{
		top_for--;//�˳���ǰѭ��
	}//������+-�������
	else
	{
		cp = stack_for[top_for].ln;
	}
}