/*  Created by razsa on 8/8/2025  */

#include "first_pass.h"
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"


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
        line_number++;
        line[strcspn(line,"\n")]='\0';/*removing excess char at the end of the line*/
        p = first_letter(line);
        if (p==NULL)
        {
            continue;
        }
        if (strlen(p)<1000)
        {
            ctx->error_count++;
        }
        printf("line %d: %s\n",line_number,p);

    }
    fclose(in);
    return ctx->error_count;

}
