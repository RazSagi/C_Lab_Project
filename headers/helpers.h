/*
// Created by Raz Sagi on 29/07/2025.
*/

#ifndef HELPERS_H
#define HELPERS_H
#include "globals.h"
#include <stdio.h>
#include "context_asm.h"


char *append_file_extension( char *base, char *ext);
int is_reserved( char *name);
char* first_letter(char *line);
int is_letter(char c);
int is_digit(char c);
int is_alphanumeric(char c);
char *my_strdup( char *s);
int is_instruction( char *p, int *tok_len);
int opcode_count_operand( char *op, int len);
int make_unique_base4(unsigned value, char out[BASE4_STR_MAX]);
void print_unique_base4(FILE *f,unsigned value);
int opcode_by_name(char *name,int len);
int operands_legal(int op_id, int a1, int a2);
int validate_operand_delim(const char *s, int line_no,CtxAsm *ctx, int allow_comma);
int validate_no_extra(const char *s, int line_no,CtxAsm *ctx);
int encode_direct_extern(const char *symname,unsigned *code, int idx,int line_no,CtxAsm *ctx);





#endif
