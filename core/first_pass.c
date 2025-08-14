/*  Created by razsa on 8/8/2025  */

#include "first_pass.h"
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"
#include "parser.h"
#include "sym_table.h"


int run_first_pass(char *am_file,CtxAsm *ctx)
{
    FILE *in=fopen(am_file,"r");/*open the am file recived by the pre-proc*/
    char line[LINE_BUFFER];
    int line_number=0;
    char *tmp;
    int kind;
    int ok = 1;
    char *t;
    int base_ic;
    char *t2;
    char *s;
    char name[MAX_LABEL_LENGTH];
    char *start;
    int len;


    /*starting the context assembler struct*/
    ctx->IC = IC_START;
    ctx->DC = 0;
    ctx->error_count = 0;
    ctx->entry_count = 0;



    if(!in)
    {
        printf("Error opening am file\n");
        return -1;
    }
    while(fgets(line,LINE_BUFFER,in))
    {
        char *p=NULL;
        char *pc;
        char label[MAX_LABEL_LENGTH];
        int lbl_stat;

        line_number++;

        line[strcspn(line,"\r\n")]='\0';/*removing excess char at the end of the line*/
        p = first_letter(line);
        if (p==NULL)
        {
            continue;
        }
        pc = p;
        /* checking if the current line starts with label*/
        lbl_stat = extract_label(&pc,label);
        if (lbl_stat==-1)
        {
            ctx->error_count++;
            continue;
        }
        if (lbl_stat==1)
        {
            tmp = pc;
            kind = check_directive_type(&tmp,line_number,ctx);
            if (*pc== '.' && kind == DIR_NONE)
            {
                continue;
            }

            /*found label followed by directive*/
            if (*pc == '.' && kind !=DIR_NONE)
            {
                /*checking if data like directives*/
                if (kind == DIR_DATA || kind == DIR_MAT || kind == DIR_STRING)
                {
                    if (!sym_table_add(label,ctx->DC,SYM_DATA))
                    {
                        printf("Error (line %d), duplicate for symbol '%s' \n",line_number,label);
                        ctx->error_count++;
                        continue;
                    }
                    ok = 1;
                    if (kind == DIR_DATA)
                    {
                        if (!handle_data(&tmp,ctx,line_number))
                        {
                            ok = 0;
                        }
                    }
                    else if (kind == DIR_STRING)
                    {
                        if (!handle_string(&tmp,ctx,line_number))
                        {
                            ok = 0;
                        }
                    }
                    else /*dir_mat*/
                    {
                        if (!handle_mat(&tmp,ctx,line_number))
                        {
                            ok = 0;
                        }

                    }
                    /*handler failed and return an error already, mobing to next line*/
                    if (!ok)
                    {
                        continue;
                    }

                    skip_spaces(&tmp);
                    if (*tmp != '\0' && *tmp !=';')
                    {
                        printf("Error (line %d), invalid text after directive \n",line_number);
                        ctx->error_count++;
                    }
                    continue;
                }

                else
                {
                    printf("Error (line %d), label before .entry or .extern is invalid '%s' \n",line_number,label);
                    ctx->error_count++;
                    continue;
                }
            }
            /* there is no directive after the label, hence continue checking what after the label*/

            t = pc;
            skip_spaces(&t);
            /*only label without any other chars after it, error*/
            if (*t == '\0' || *t == ';')
            {
                printf("Error (line %d), no command after label \n",line_number);
                ctx->error_count++;
                continue;
            }

            base_ic= ctx->IC;
            tmp=pc;

            if (!handle_instruct(&tmp,ctx,line_number))
            {
                continue;
            }

            /*checking if there is trainling chars after the instruction*/
            skip_spaces(&tmp);
            if (*tmp!='\0' && *tmp !=';')
            {
                printf("Error (line %d), invalid text after directive \n",line_number);
                ctx->error_count++;
            }

            if (!sym_table_add(label,base_ic,SYM_CODE))
            {
                printf("Error (line %d), duplicate for symbol '%s' \n",line_number,label);
                ctx->error_count++;
            }
            continue;

        }
        /*its not a label, moving forward*/
        /*checking if the first char is '.', if not its probably instruction*/
        t2 = pc;
        skip_spaces(&t2);

        if (*t2 != '.')
        {
            tmp=pc;
            if (!handle_instruct(&tmp,ctx,line_number))
            {
                /*if failes, already printed the error*/
                continue;
            }
            /*checking for excecive char after the instruction*/
            skip_spaces(&tmp );
            if (*tmp!='\0' && *tmp !=';')
            {
                printf("Error (line %d), invalid text after directive \n",line_number);
                ctx->error_count++;
            }
            continue;
        }
        else
        {
            tmp=pc;
            kind = check_directive_type(&tmp,line_number,ctx);
            if (kind == DIR_NONE)
            {
                continue;
            }
            ok = 1;
            if (kind == DIR_DATA)
            {
                if (!handle_data(&tmp,ctx,line_number))
                {
                    ok = 0;
                }
            }
            else if (kind == DIR_STRING)
            {
                if (!handle_string(&tmp,ctx,line_number))
                {
                    ok = 0;
                }
            }
            else if (kind == DIR_MAT)
            {
                if (!handle_mat(&tmp,ctx,line_number))
                {
                    ok = 0;
                }
            }
            else if (kind == DIR_EXTERN)
            {
                s = tmp;
                skip_spaces(&s);
                if (!is_letter(*s))
                {
                    printf("Error (line %d), Symbols is expected after .extern \n",line_number);
                    ctx->error_count++;
                    continue;
                }
                start = s++;

                while (is_alphanumeric(*s))
                {
                    s++;
                }
                len = (int)(s-start);
                if (len >= MAX_LABEL_LENGTH)
                {
                    printf("Error (line %d), Symbols too long after .extern \n",line_number);
                    ctx->error_count++;
                    continue;
                }

                memcpy(name,start,(size_t)len);
                name[len] = '\0';

                if (is_reserved(name))
                {
                    printf("Error (line %d), Invalid extern \n",line_number);
                    ctx->error_count++;
                    continue;
                }

                if (!sym_table_add(name,0,SYM_EXTERN))
                {
                    printf("Error (line %d), duplicate for symbol '%s' \n",line_number,name);
                    ctx->error_count++;
                    continue;
                }
                /*checking for text after the directive*/
                skip_spaces(&s);
                if (*s!='\0' && *s !=';')
                {
                    printf("Error (line %d), invalid text after directive \n",line_number);
                    ctx->error_count++;
                }
                continue;
            }
            /*almost the same as the DIR_EXTERN*/
            else if (kind == DIR_ENTRY)
            {
                s=tmp;
                skip_spaces(&s);
                if (!is_letter(*s))
                {
                    printf("Error (line %d), Symbols is expected after .entry \n",line_number);
                    ctx->error_count++;
                    continue;
                }
                start = s++;
                while (is_alphanumeric(*s))
                {
                    s++;
                }

                len = (int)(s-start);
                if (len >= MAX_LABEL_LENGTH)
                {
                    printf("Error (line %d), Symbols too long after .entry \n",line_number);
                    ctx->error_count++;
                    continue;
                }
                memcpy(name,start,(size_t)len);
                name[len] = '\0';

                if (is_reserved(name))
                {
                    printf("Error (line %d), Invalid entry name \n",line_number);
                    ctx->error_count++;
                    continue;
                }

                if (ctx->entry_count >= MAX_ENTRIES)
                {
                    printf("Error (line %d), too many entries \n",line_number);
                    ctx->error_count++;
                    continue;
                }
                strcpy(ctx->entries[ctx->entry_count++],name);

                /*checking for unwanted chars after entry*/
                skip_spaces(&s);
                if (*s!='\0' && *s !=';')
                {
                    printf("Error (line %d), invalid text after .directive \n",line_number);
                    ctx->error_count++;
                }
                continue;


            }
            /*checking the status of the ok variable*/
            if (!ok)
            {
                continue;
            }
            skip_spaces(&tmp);
            if (*tmp!='\0' && *tmp !=';')
            {
                printf("Error (line %d), invalid text after directive \n",line_number);
                ctx->error_count++;

            }
            continue;
        }




    }
    fclose(in);
    sym_table_relocate(ctx->IC);
    return ctx->error_count;

}
