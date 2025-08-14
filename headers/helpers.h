/*
// Created by Raz Sagi on 29/07/2025.
*/

#ifndef HELPERS_H
#define HELPERS_H
#include "globals.h"
#include <stdio.h>

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




#endif
