/*  Created by razsa on 8/2/2025  */

#include "preproc.h"
#include "macro_list.h"
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include <stdlib.h>
/*This method gets a pointer to line and returns if there is "real" char there
 * this checks also if this line starts with ';'
 */
static char* first_letter(char *line)
{
    char *curr = line;
    while (*curr==' '||*curr=='\t')
    {
        curr++;
    }
    if (*curr == '\0'||*curr == '\n'||*curr == '\r'||*curr == ';')
    {
        return NULL;
    }
    return curr;

}
/*this method checks if there is chars after mcro\mcroend that isnt WS*/
static int only_ws_after(const char *p)
{
    while (*p!='\0'&&*p!='\n'&&*p!='\r')
    {
        if (*p!=' '&&*p!='\t')
        {
            return 0;
        }
        p++;
    }
    return 1;
}


int run_preprocessor(const char *in_path,char *out_path)
{
    /* starting all the variables */
    FILE *in=NULL;
    FILE *out=NULL;
    char line[LINE_BUFFER];
    int in_macro=0;
    char cur_name[MAX_LABEL_LENGTH];
    char *cur_body = NULL;
    char *token;

    /*trying to open the file*/
    in=fopen(in_path,"r");
    if (in==NULL)
    {
        return PREPROC_ERR_OPEN_IN;
    }
    /*trying to initialize the MacroList*/
    MacroList *macros = ml_create();
    if (!macros)
    {
        fclose(in);
        return PREPROC_ERR_OUT_MEMORY;
    }

    /*Start running on the give file*/
    while (fgets(line,sizeof(line),in))
    {
        /*Checking if the line is over 80 chars length, if so, closing the method and print error*/
        size_t len = strlen(line);
        size_t count;
        if (len>0 && line[len-1]=='\n')
        {
            count=len-1;
        }
        else
        {
            count=len;
        }
        if (count>MAX_LINE_LENGTH)
        {
            free(cur_body);
            ml_delete(macros);
            fclose(in);
            if (out) fclose(out);
            return PREPROC_ERR_LINE_TOO_LONG;
        }

        token = first_letter(line);

        /*if line is empty\start with ; write the line to the out file */
        if (!token)
        {
            if (out)
            {
                fputs(line,out);
            }
            continue;
        }
        /*If the line is a already used macro, enroll the macro to the output file and move forward*/
        const char *body = ml_lookup(macros,token);
        if (body !=NULL)
        {
            if (!out)
            {
                out = fopen(out_path,"w");
                if (!out)/*couldent open the output file*/
                {
                    free(cur_body);
                    ml_delete(macros);
                    fclose(in);
                    return PREPROC_ERR_OPEN_OUT;
                }
            }
            fputs(body,out);
            continue;
        }




    }
}
