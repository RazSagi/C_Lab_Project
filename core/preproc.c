/*  Created by razsa on 8/2/2025  */

#include "preproc.h"
#include "macro_list.h"
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include <stdlib.h>
#include "helpers.h"
/*This method gets a pointer to line and returns if there is "real" char there
 * this checks also if this line starts with ';'
 */

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
    MacroList *macros;
    char *end;
    const char *body;
    char saved;

    /*trying to open the file*/
    in=fopen(in_path,"r");
    if (!in)
    {
        return PREPROC_ERR_OPEN_IN;
    }
    /*trying to initialize the MacroList*/
    macros = ml_create();
    if (!macros)
    {
        fclose(in);
        return PREPROC_ERR_OUT_MEMORY;
    }
    out = fopen(out_path, "w");
    if (!out)
        {
            fclose(in);
            ml_delete(macros);
            return PREPROC_ERR_OPEN_OUT;
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
            fclose(out);
            return PREPROC_ERR_LINE_TOO_LONG;
        }

        token = first_letter(line);

        /*if line is empty\start with ; write the line to the out file */
        if (!token)
        {
            fputs(line,out);
            continue;
        }
        /*If the line is a already used macro, enroll the macro to the output file and move forward*/

        if (!in_macro)
        {
            char *name  = token;
            char *sep   = name;

            while (*sep && *sep!=' ' && *sep!='\t' && *sep!='\n' && *sep!='\r')
                sep++;

            saved = *sep;
            *sep = '\0';

            body = ml_lookup(macros, name);

            /* restore the original character */
            *sep = saved;
            if (body)
            {
                fputs(body, out);
                continue;
            }
        }

        /*Not in Macro, looking for starts of macro*/
        if (!in_macro && token)
        {
            char *firstword = token;
            char *restword = token;
            /*iterate throught the first word until its end*/
            while (*restword && *restword!=' '&&*restword!='\t' && *restword!='\n'&&*restword!='\r')
            {
                restword ++;
            }
            if (*restword)
            {
                saved = *restword;
                *restword = '\0';
                restword ++;


                if (strcmp(firstword,"mcro")==0)/*found start of mcro*/
                {
                    char *name = restword;
                    while (*name == ' '|| *name=='\t') name++;

                    /*if the macro name is empty, closing the file*/
                    if (*name=='\0')
                    {
                        free(cur_body);
                        ml_delete(macros);
                        fclose(in);
                        fclose(out);
                        return PREPROC_ERR_MACRO_NAME_INVALID;
                    }

                    /*closing the macro name (eliminate ws after the word)*/
                    end = name;
                    while (*end && *end!=' '&&*end!='\t'&&*end!='\n'&&*end!='\r')
                    {
                        end++;
                    }
                    *end='\0';

                    /*checking if the macro name is reserved word*/
                    if (is_reserved(name))
                    {
                        free(cur_body);
                        ml_delete(macros);
                        fclose(in);
                        fclose(out);
                        return PREPROC_ERR_MACRO_NAME_INVALID;
                    }

                    /*success, we found start of macro. proceeding to define it*/
                    strncpy(cur_name,name,MAX_LABEL_LENGTH-1);
                    cur_name[MAX_LABEL_LENGTH-1]='\0';
                    in_macro = 1;/*next loop we will just copy the rows until finding the "mcroend" line */
                    free(cur_body);/*reseting the cur_body to prevent leaking*/
                    cur_body = my_strdup("");
                    if (!cur_body)
                    {
                        ml_delete(macros);
                        fclose(in);
                        fclose(out);
                        return PREPROC_ERR_OUT_MEMORY;
                    }
                    restword--;
                    *restword = saved;
                    continue;
                }
                restword--;
                *restword = saved;
            }
        }
        /*Inside macro, checking if we reached to the end.if not, adding the curr line to the body of the macro*/
        if (in_macro)
            {
            if (strncmp(token, "mcroend", 7) == 0 && only_ws_after(token + 7))
            {
                ml_add(macros, cur_name, cur_body);
                in_macro = 0;
            }
            else
            {
                size_t old = strlen(cur_body);
                cur_body = realloc(cur_body, old + strlen(line) + 1);
                strcpy(cur_body + old, line);
            }



            continue;
            }
        /* regular line, no changes need to be dont. plain copy */
        fputs(line,out);
    }
    /*cleaning memory*/
    free(cur_body);
    ml_delete(macros);
    fclose(out);
    fclose(in);
    return PREPROC_OK;
}





