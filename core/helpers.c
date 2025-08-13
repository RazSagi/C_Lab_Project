/*
// Created by Raz Sagi on 29/07/2025.
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "helpers.h"
/* Array of Directives*/
static const char *directives[] = {
    ".data", ".string", ".entry", ".extern", NULL
};

/*Array of Instructions*/
static const char *instructions[] = {
    "mov","cmp","add","sub","lea","clr","not",
    "inc","dec","jmp","bne","jsr","red","prn","rts","stop",
    NULL
};

/*Array of Registers*/
static const char *registers[] = {
    "r0","r1","r2","r3","r4","r5","r6","r7", NULL
};
static const char *ops2[] = {"mov", "cmp", "add", "sub", "lea", NULL};
static const char *ops1[] = {"clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", NULL};
static const char *ops0[] = {"rts", "stop", NULL};


char *append_file_extension(const char *base,const char *ext)
{
    char *new_name = malloc(strlen(base) + strlen(ext) + 1);
    if (!new_name) return NULL;
    strcpy(new_name, base);
    strcat(new_name, ext);
    return new_name;
}
int is_reserved(const char *name)
{
    const char **p;
    for (p = directives; *p; p++)
    {
        if (strcmp(name, *p)==0) return 1;
    }
    for (p = instructions; *p; p++)
    {
        if (strcmp(name, *p)==0) return 1;
    }
    for (p = registers; *p; p++)
    {
        if (strcmp(name, *p)==0) return 1;
    }
    return 0;
}
char* first_letter(char *line)
{
    char *curr = line;
    while (*curr==' '||*curr=='\t')
    {
        curr++;
    }
    if (*curr == '\0'||*curr == '\n'||*curr == '\r'||*curr == ';')
    {
        return NULL;
    }
    return curr;

}
int is_letter(char c)
{
    return (c>='A'&&c<='Z') || (c>='a'&&c<='z');
}
int is_digit(char c)
{
    return (c>='0'&&c<='9');
}
int is_alphanumeric(char c)
{
    return (is_digit(c)||is_letter(c));
}
char *my_strdup(const char *s)
{
    char *p = malloc(strlen(s)+1);
    if (p)
    {
        strcpy(p, s);
    }
    return p;
}
int is_instruction(const char *p, int *tok_len)
{
    const char *s = p;
    const char **q;
    const char *start;
    size_t len = 0;

    while (*s == ' ' || *s=='\t')
    {
        s++;
    }

    if (!is_letter(*s))
    {
        return 0;
    }

    start = s;
    while (!is_letter(*s))
    {
        s++;
    }

    len = (size_t)(s-start);
    if (len ==0)
    {
        return 0;
    }

    for (q=instructions; *q; q++)
    {
        if (strlen(*q == len) && strncmp(*q,start,len) == 0)
        {
            if (tok_len)
            {
                *tok_len = len;
            }
            return 1;
        }
    }
    return 0;

}

/*checking if the following opcode should have 0\1\2 operands*/
static int in_list(const char *op,int len,const char *const *list)
{
    for (const char *const *p = list; *p; p++)
    {
        if ((int)strlen(*p)==len && strncmp(*p,op,len) == 0)
        {
            return 1;
        }

    }
    return 0;
}

int opcode_count_op(const char *op,int len)
{
    if (in_list(op,len,ops2))
    {
        return 2;
    }
    if (in_list(op,len,ops1))
    {
        return 1;
    }
    if (in_list(op,len,ops0))
    {
        return 0;
    }
    return -1;

}