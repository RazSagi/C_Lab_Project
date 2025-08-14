/*  Created by razsa on 8/8/2025  */


#include "sym_table.h"
#include<string.h>
#include<stdio.h>
#include "globals.h"

static Symbol g_symbols[SYM_TABLE_MAX];
static int g_sym_count = 0;

void sym_table_reset(void)
{
    g_sym_count = 0;
}

static int name_equals( char *name1,  char *name2)
{
    return strcmp(name1, name2) == 0;
}

int sym_table_contains( char *name)
{
    int i;
    for (i = 0; i < g_sym_count; i++)
    {
        if (name_equals(g_symbols[i].name, name))
        {
            return 1;
        }
    }
    return 0;
}
/*adding to the symbols table*/
int sym_table_add( char *name, int value, SymKind kind)
{
    size_t length;
    /*dupe symbol*/
    if (sym_table_contains(name))
    {
        return 0;
    }
    /*too many symbols*/
    if (g_sym_count >= SYM_TABLE_MAX)
    {
        return 0;
    }

    length = strlen(name);
    memcpy(g_symbols[g_sym_count].name, name, length+1);

    g_symbols[g_sym_count].value = value;
    g_symbols[g_sym_count].kind = kind;
    g_symbols[g_sym_count].is_entry = 0;

    g_sym_count++;
    return 1;
}

void sym_table_relocate(int del)
{
    int i;
    for (i = 0; i < g_sym_count; i++)
    {
        if (g_symbols[i].kind == SYM_DATA)
        {
            g_symbols[i].value += del;
        }
    }
}
Symbol* sym_table_lookup( char *name)
{
    int i;
    for (i = 0; i < g_sym_count; i++)
    {
        if (strcmp(g_symbols[i].name, name) == 0)
        {
            return &g_symbols[i];
        }
    }
    return 0;
}