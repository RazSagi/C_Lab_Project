
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"
#include "sym_table.h"
#include "second_pass.h"

/*builds new path for the new files we gonna create*/
static char *make_out_path (char *am_path, char *ext)
{
    size_t n = strlen(am_path);
    size_t last_sep = (size_t)-1;
    size_t dot = n;
    char *out;
    size_t i;

    for (i = 0; i<n; i++)
    {
        char c = am_path[i];
        if (c =='/' || c == '\\')
        {
            last_sep = i;
        }
    }

    for (i=n;i>(last_sep+1);i--)
    {
       if (am_path[i-1] == '.')
       {
           dot = i-1;
           break;
       }
    }

    out = (char*)malloc(dot+strlen(ext)+1);
    if (!out) return NULL;
    memcpy(out,am_path,dot);
    strcpy(out+dot,ext);
    return out;
}

/*the first line of the .ob in a b c d*/
static int ob_write_all(char *am_path,int code_count, int data_count,unsigned *code_words,unsigned *data_words)
{
    char *ob_path = make_out_path(am_path,FILE_EXT_OB);
    FILE *out;
    char buffer[BASE4_STR_MAX];
    unsigned addr;
    int i;
    if (!ob_path) return 0;

    out = fopen(ob_path,"w");
    if (!out)
    {
        free(ob_path);
        return 0;
    }
    /*header*/
    make_unique_base4((unsigned)code_count,buffer);
    fputs(buffer,out);
    fputc(' ',out);
    make_unique_base4((unsigned)data_count,buffer);
    fputs(buffer,out);
    fputc('\n',out);

    /*code words*/
    addr=100;
    for (i = 0 ;i<code_count; i++,addr++)
    {
        make_unique_base4(addr,buffer);
        fputs(buffer,out);
        fputc(' ',out);
        make_unique_base4(code_words[i],buffer);
        fputs(buffer,out);
        fputc("/n",out);
    }

    /*data words*/

    for (i = 0 ;i<data_count; i++,addr++)
    {
        make_unique_base4(addr,buffer);
        fputs(buffer,out);
        fputc(' ',out);
        make_unique_base4(data_words[i],buffer);
        fputs(buffer,out);
        fputc('\n',out);
    }


    fclose(out);
    free(ob_path);
    return 1;
}