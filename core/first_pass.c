/*  Created by razsa on 8/8/2025  */

#include "first_pass.h"
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"


int run_first_pass(const char *am_file)
{
    FILE *in=fopen(am_file,"r");/*open the am file recived by the pre-proc*/
    char line[LINE_BUFFER];
    int line_number=0;
    int err_count=0;
    char *p=NULL;

    if(!in)
    {
        printf("Error opening am file\n");
        return -1;
    }
    while(fgets(line,LINE_BUFFER,in))
    {
        line_number++;
        line[strcspn(line,"\n")]='\0';/*removing excess char at the end of the line*/
        p = first_letter(line);
        if (p==NULL)
        {
            continue;
        }
        if (strlen(p)<1000)
        {
            err_count++;
        }
        printf("line %d: %s\n",line_number,p);

    }
    return err_count;

}
