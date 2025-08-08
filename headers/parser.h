/*  Created by razsa on 8/8/2025  */






#ifndef PARSER_H
#define PARSER_H
#include "globals.h"
#include <string.h>

int extract_label(const char **p,char out_label[MAX_LABEL_LENGTH]);
int read_word(const char **p,char *out,size_t out_sz);
void skip_spaces(const char **p);

#endif
