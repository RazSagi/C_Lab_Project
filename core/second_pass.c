
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"
#include "sym_table.h"
#include "second_pass.h"

#include "parser.h"

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
        fputc('\n',out);
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

/*handles .ent and .ent file*/
typedef struct
{
    char *name;
    unsigned address;
}EntItem;

/*write the .ent file same as we did with the .ob*/

static int ent_write_all(char *am_path, EntItem *items, int count)
{
    char *ent_path;
    FILE *out;
    int i;
    char buffer[BASE4_STR_MAX];

    if (count <= 0) return 1;

    ent_path = make_out_path(am_path,FILE_EXT_ENT);
    if (!ent_path) return 0;

    out = fopen(ent_path,"w");
    if (!out)
    {
        free(ent_path);
        return 0;
    }

    for (i = 0; i<count; i++)
    {
        make_unique_base4(items[i].address,buffer);
        fputs(items[i].name,out);
        fputc(' ',out);
        fputs(buffer,out);
        fputc('\n',out);
    }
    fclose(out);
    free(ent_path);
    return 1;
}

/*same as the .ent but for .extern files*/
typedef struct
{
    char *name;
    unsigned address;
}ExtUse;

/*doing the same as the .ent and writing the .ext file in 1 time*/

static int ext_write_all(char *am_path, ExtUse *uses, int count)
{
    char *ext_path;
    FILE *out;
    int i;
    char buffer[BASE4_STR_MAX];

    if (count <= 0) return 1;

    ext_path = make_out_path(am_path,FILE_EXT_EXT);
    if (!ext_path) return 0;

    out = fopen(ext_path,"w");
    if (!out)
    {
        free(ext_path);
        return 0;
    }

    for (i = 0; i<count; i++)
    {
        make_unique_base4(uses[i].address,buffer);
        fputs(uses[i].name,out);
        fputc(' ',out);
        fputs(buffer,out);
        fputc('\n',out);
    }
    fclose(out);
    free(ext_path);
    return 1;
}

int run_second_pass(char *am_file,CtxAsm *ctx)
{
    FILE *in;
    char line[SYM_TABLE_MAX];
    int line_no = 0;

    /*trying to open the file*/
    in =fopen(am_file,"r");
    if (!in)
    {
        printf("Error, cannot open '%s'\n",am_file);
        return 0;
    }
    /*running throught the lines in the .am file*/

    while (fgets(line,sizeof(line),in))
    {
        char *p = first_letter(line);
        char lbl[MAX_LABEL_LENGTH];
        int dir_type;

        line_no++;
        /*line is empty or comment*/
        if (!p) continue;

        /*label */
        char *q = p;
        if (extract_label(&q,lbl) == 1)
        {
            p = q;
        }

        /*directive*/
        dir_type = check_directive_type(&q,line_no,ctx);
        if (dir_type == DIR_ENTRY)
        {
            char name[MAX_LABEL_LENGTH];
            int n=0;

            while (is_alphanumeric(*p)&& n<MAX_LABEL_LENGTH-1)
            {
                name[n++] = *p;
                p++;
            }
            name[n] = '\0';

            if (n==0)
            {
                printf("Error(line %d), missing name after .entry\n",line_no);
            }
            else if (!sym_table_mark_entry(name))
            {
                printf("Error(line %d), .entry undifined\n",line_no);
            }
            continue;
        }

    }
    fclose(in);
    return 1;
}