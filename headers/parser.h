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


int extract_label(const char **p,char out_label[MAX_LABEL_LENGTH]);
int read_word(const char **p,char *out,size_t out_sz);
void skip_spaces(const char **p);
int check_directive_type(const char **p,int line_number,CtxAsm *ctx);
int handle_data(const char **p,CtxAsm *ctx, int line_number);
int handle_string(const char **p,CtxAsm *ctx, int line_number);

#endif
