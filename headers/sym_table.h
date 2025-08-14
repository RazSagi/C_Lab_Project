/*  Created by razsa on 8/8/2025  */


#ifndef SYM_TABLE_H
#define SYM_TABLE_H

#include "globals.h"

typedef enum
{
    SYM_CODE = 1,
    SYM_DATA = 2,
    SYM_EXTERN = 3,
    SYM_ENTRY = 4
}SymKind;

typedef struct
{
    char name[MAX_LABEL_LENGTH];
    int value;
    SymKind kind;
    int is_entry;
}Symbol;

/*reset the table between files*/
void sym_table_reset(void);

/*return 1 if the label is laready exists*/
int sym_table_contains( char *name);

/*return 1 on success adding, 0 if dupe or table full*/
int sym_table_add( char *name,int value,SymKind kind);

/*add the IC to all SYM_DTA symbols by the end of the first pass*/
void sym_table_relocate(int del);

/*returns pointer to the symbol*/
Symbol* sym_table_lookup( char *name);

#endif
