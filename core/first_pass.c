/*  Created by razsa on 8/8/2025  */

#include "first_pass.h"
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"
#include "parser.h"
#include "sym_table.h"


int run_first_pass(const char *am_file,CtxAsm *ctx)
{
    FILE *in=fopen(am_file,"r");/*open the am file recived by the pre-proc*/
    char line[LINE_BUFFER];
    int line_number=0;
    const char *tmp;
    int kind;
    int ok = 1;

    /*starting the context assembler struct*/
    ctx->IC = IC_START;
    ctx->DC = 0;
    ctx->error_count = 0;



    if(!in)
    {
        printf("Error opening am file\n");
        return -1;
    }
    while(fgets(line,LINE_BUFFER,in))
    {
        char *p=NULL;
        const char *pc;
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
                    if (*tmp != '\0')
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
            const char *t = pc;
            skip_spaces(&t);
            /*only label without any other chars after it, error*/
            if (*t == '\0' || *t == ';')
            {
                printf("Error (line %d), no command after label \n",line_number);
                ctx->error_count++;
                continue;
            }
            /* checking if what follows the label is valid instruction*/
            int tok_len = 0;
            if (!is_instruction(pc,&tok_len))
            {
                printf("Error (line %d), unkown opcode after label \n",line_number);
                ctx->error_count++;
                continue;
            }

        }



    }
    fclose(in);
    return ctx->error_count;

}
