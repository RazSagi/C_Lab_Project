/*
// Created by Raz Sagi on 29/07/2025.
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "helpers.h"
/* Array of Directives*/
static  char *directives[] = {
    ".data", ".string", ".entry", ".extern", NULL
};

/*Array of Instructions*/
static  char *instructions[] = {
    "mov","cmp","add","sub","not","clr","lea",
    "inc","dec","jmp","bne","red","prn","jsr","rts","stop",
    NULL
};

/*Array of Registers*/
static  char *registers[] = {
    "r0","r1","r2","r3","r4","r5","r6","r7", NULL
};
static  char *ops2[] = {"mov", "cmp", "add", "sub", "lea", NULL};
static  char *ops1[] = {"clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", NULL};
static  char *ops0[] = {"rts", "stop", NULL};


char *append_file_extension( char *base, char *ext)
{
    char *new_name = malloc(strlen(base) + strlen(ext) + 1);
    if (!new_name) return NULL;
    strcpy(new_name, base);
    strcat(new_name, ext);
    return new_name;
}
int is_reserved( char *name)
{
     char **p;
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
char *my_strdup( char *s)
{
    char *p = malloc(strlen(s)+1);
    if (p)
    {
        strcpy(p, s);
    }
    return p;
}
int is_instruction( char *p, int *tok_len)
{
     char *s = p;
     char **q;
     char *start;
    int len = 0;

    while (*s == ' ' || *s=='\t')
    {
        s++;
    }

    if (!is_letter(*s))
    {
        return 0;
    }

    start = s;
    while (is_letter(*s))
    {
        s++;
    }

    len = (int)(s-start);
    if (tok_len) *tok_len = len;

    for (q=instructions; *q; q++)
    {
        if ((int)strlen(*q) == len && strncmp(*q,start,(size_t)len) == 0)
        {
            return 1;
        }
    }
    return 0;

}

/*checking if the following opcode should have 0\1\2 operands*/
static int in_list( char *op,int len, char * *list)
{
     char * *p;
    for (p = list; *p; p++)
    {
        if ((int)strlen(*p)==len && strncmp(*p,op,(size_t)len) == 0)
        {
            return 1;
        }

    }
    return 0;
}

int opcode_count_operand( char *op,int len)
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

/*this method take number between 0 - 3 and retruns the right letter*/
static char base4_digit(unsigned v)
{
    switch (v & 3)
    {
        case 0: return 'a';
        case 1: return 'b';
        case 2: return 'c';
        default: return 'd';
    }
}

/*converting non-negative integer to base 4 string, return the length */
int make_unique_base4(unsigned value, char out[BASE4_STR_MAX])
{
    char tmp[BASE4_STR_MAX];
    int i = 0;
    int j;

    if (value == 0)
    {
        out[0] = 'a';
        out[1] = '\0';
        return 1;
    }

    while (value>0 && i<(BASE4_STR_MAX-1))
    {
        tmp[i++] = base4_digit(value);
        value >>=2;
    }
    /*reverse the output since we started with the less-importnet number*/
    for (j=0;j<i;j++)
    {
        out[j] = tmp[i-1-j];
    }
    out[i] = '\0';
    return i;
}

void print_unique_base4(FILE *f, unsigned value)
{
    char buffer[BASE4_STR_MAX];
    make_unique_base4(value, buffer);
    fputs(buffer, f);
}
/*this method gets opcode name and return the number of the opcode*/
int opcode_by_name(char *name,int len)
{
    int i;
    for (i = 0; instructions[i]; i++)
    {
        if ((int)strlen(instructions[i]) == len && strncmp(instructions[i],name,(size_t)len) == 0)
        {
            return i;
        }
    }
    return -1;
}

/*this method checks if a given opernad is legal*/
int operands_legal(int op_id, int a1, int a2)
{
    /*op_id is by the instruction array and by the booklet instructions*/
    switch (op_id)
    {
    case 0:/*mov*/
    case 2:/*add*/
    case 3:/*sub*/
        {
            if (a1 == OP_NONE) return 0;
            if ( a2 == OP_NONE) return 0;
            if (a2 == OP_IMM) return 0;
            return 1;
        }
    case 1:/*cmp*/
        {
            if (a1 == OP_NONE) return 0;
            if (a2 == OP_NONE) return 0;
            return 1;
        }
    case 6:/*lea*/
        {
            if (a1 == OP_NONE || a2 == OP_NONE) return 0;
            if (a1 != OP_DIR) return 0;
            if (a2 == OP_IMM) return 0;
            return 1;
        }
    case 4:/*not*/
    case 5:/*clr*/
    case 7:/*inc*/
    case 8:/*dec*/
        {
            if (a2!=OP_NONE) return 0;
            if (a1 == OP_IMM||a1 == OP_NONE) return 0;
            return 1;
        }
    case 9:/*jmp*/
    case 10:/*bne*/
    case 13:
        {
            if (a2 != OP_NONE) return 0;
            if (a1 != OP_DIR) return 0;
            return 1;
        }
    case 11:/*red*/
        {
            if (a2!=OP_NONE) return 0;
            if (a1!=OP_REG) return 0;
            return 1;
        }
    case 12:
        {
            if (a2 !=OP_NONE) return 0;
            if (a1 == OP_NONE)return 0;
            return 1;
        }
    case 14:
    case 15:
        {
            if (a1 != OP_NONE||a2!=OP_NONE) return 0;
            return 1;
        }

    }
    return 0;
}

/*checking for valid text in operand*/
int validate_operand_delim(const char *s, int line_no,CtxAsm *ctx,int allow_comma)
{
    char c;
    if (!s)return 1;
    c = *s;
    if (c=='\0' || c=='\n'||c=='\r'||c=='\t'||c== ' '||(allow_comma && c==','))
    {
        return 1;
    }
    printf("Error (line %d), Invalid char after operand\n", line_no);
    ctx->error_count++;
    return 0;
}

/*checking for no extra text after operand*/
int validate_no_extra(const char *s, int line_no,CtxAsm *ctx)
{
    const char *p = s;
    while (p&&(*p== ' '||*p == '\t')) p++;
    if (!p || *p=='\0'||*p=='\n'||*p=='\r'||*p==';')
    {
        return 1;
    }
    printf("Error (line %d), extra char after operand\n", line_no);
    ctx->error_count++;
    return 0;
}