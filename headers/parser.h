/*  Created by razsa on 8/8/2025  */






#ifndef PARSER_H
#define PARSER_H
#include "globals.h"
#include <string.h>

#include "context_asm.h"

#define DIR_NONE 0
#define DIR_DATA 1
#define DIR_STRING 2
#define DIR_MAT 3
#define DIR_ENTRY 4
#define DIR_EXTERN 5
#define MAX_DIRECTIVE 10


int extract_label( char **p,char out_label[MAX_LABEL_LENGTH]);
int read_word( char **p,char *out,size_t out_sz);
void skip_spaces( char **p);
int check_directive_type( char **p,int line_number,CtxAsm *ctx);
int handle_data( char **p,CtxAsm *ctx, int line_number);
int handle_string( char **p,CtxAsm *ctx, int line_number);
int handle_mat( char **p,CtxAsm *ctx, int line_number);
int handle_instruct( char **p, CtxAsm *ctx, int line_number);

#endif
