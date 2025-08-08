/*  Created by razsa on 8/8/2025  */

#include "first_pass.h"
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"
#include "parser.h"


int run_first_pass(const char *am_file,CtxAsm *ctx)
{
    FILE *in=fopen(am_file,"r");/*open the am file recived by the pre-proc*/
    char line[LINE_BUFFER];
    int line_number=0;

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
            printf("Debugging line %d label=%s\n",line_number,label);
        }
        printf("debugging line %d rest=%s\n",line_number,pc);


    }
    fclose(in);
    return ctx->error_count;

}
