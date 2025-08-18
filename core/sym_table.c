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
    Symbol *s;
    size_t length;

    /*if symbol already exist*/
    s = sym_table_lookup(name);
    if (s)
    {
        /*can be repeated extern declration (2 files)*/
        if (s->kind == SYM_EXTERN && kind == SYM_EXTERN)
        {
            return 1;
        }
        return 0;
    }
    /*table is full?*/
    if (g_sym_count >= SYM_TABLE_MAX)
    {
        return 0;
    }

    /*if not full and not dupe, adding symbol*/
    length= strlen(name);
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

/*marking entry in the symbol table
 * searching for the given name and chage the is_entry field to 1
 */
int sym_table_mark_entry(char *name)
{
    Symbol *s = sym_table_lookup(name);
    if (!s) return 0;
    s->is_entry = 1;
    return 1;
}
