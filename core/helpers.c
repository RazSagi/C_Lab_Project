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
