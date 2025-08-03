/*  Created by razsa on 8/2/2025  */

#include "preproc.h"
#include "macro_list.h"
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include <stdlib.h>
/*This method gets a pointer to line and returns if there is "real" char there*/
static char* first_letter(char *line)
{
    char *curr = line;
    while (*curr==' '||*curr=='\t')
    {
        curr++;
    }
    if (*curr == '\0'||*curr == '\n'||*curr == '\r')
    {
        return NULL;
    }
    return curr;

}


int run_preprocessor(const char *in_path,char *out_path)
{
    FILE *in;
    FILE *out;
    MacroList *macros = ml_create();
    char line[MAX_LINE_LENGTH]
    (void)in_path;
    (void)out_path;
    return 0;
}
