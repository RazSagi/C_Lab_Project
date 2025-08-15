
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"
#include "sym_table.h"
#include "second_pass.h"

#include <ctype.h>

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
    char line[1024];
    int line_no = 0;
    char *q;

    EntItem *entries = NULL;
    int ent_count = 0;
    EntItem *tmp;

    ExtUse *uses = NULL;
    int ext_count = 0;
    ExtUse *utmp;

    unsigned ic = 100;
    unsigned add_words = 0;

    int status = 1; /*start of as success, if error status = 0 and goto cleanup*/

    /*trying to open the file*/
    in =fopen(am_file,"r");
    if (!in)
    {
        printf("Error, cannot open '%s'\n",am_file);
        status = 0;
        goto cleanup;
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
        q = p;
        if (extract_label(&q,lbl) == 1)
        {
            p = q;
        }

        /*directive*/
        dir_type = check_directive_type(&p,line_no,ctx);
        if (dir_type == DIR_ENTRY)
        {
            char name[MAX_LABEL_LENGTH];
            int n=0;
            Symbol *s;

            skip_spaces(&p);

            while (is_alphanumeric(*p)&& n<MAX_LABEL_LENGTH-1)
            {
                name[n++] = *p;
                p++;
            }
            name[n] = '\0';

            if (n==0)
            {
                printf("Error(line %d), missing name after .entry\n",line_no);
                ctx->error_count++;
                continue;
            }
            s = sym_table_lookup(name);
            if (!s)
            {
                printf("Error(line %d), .entry undfined\n",line_no);
                ctx->error_count++;
                continue;
            }
            sym_table_mark_entry(name);


            tmp = (EntItem*)realloc(entries,sizeof(EntItem)*(ent_count+1));
            if (!tmp)
            {
                status = 0;
                goto cleanup;
            }
            entries = tmp;
            entries[ent_count].name = s->name;
            entries[ent_count].address = (unsigned)s->value;
            ent_count++;
            continue;

        }
        /*instruction, looking for extern uses and advance IC*/
        if (dir_type == DIR_NONE)
        {
            int tok_len = 0;
            char *s = NULL;
            int num_ops = 0;
            int a1 = OP_NONE;
            int a2 = OP_NONE;
            char sym1[MAX_LABEL_LENGTH];
            char sym2[MAX_LABEL_LENGTH];
            int regs = 0;
            int non_regs = 0;
            int off = 1; /*first word after opcode*/
            char *t = NULL;
            int n = 0;
            Symbol *sx = NULL;

            sym1[0] = '\0';
            sym2[0] = '\0';
            if (is_instruction(p,&tok_len)&& tok_len>0)
            {
                s = p + tok_len;
                num_ops = opcode_count_operand(p,tok_len);

                /*operand #1 */
                if (num_ops >=1)
                {
                    t = s;
                    skip_spaces(&t);

                    if (*t == '#')
                    {
                        t++;
                        if (*t == '+' || *t == '-') t++;
                        while (isdigit(*t)) t++;
                        a1 = OP_IMM;
                        s = t;

                    }
                    else if (t[0] == 'r' && t[1] >= '0' && t[1] <= '7' && !is_alphanumeric(t[2]))
                    {
                        a1 = OP_REG;
                        s = t + 2;
                    }
                    else if (is_letter(*t))
                    {
                        n=0;
                        while (is_alphanumeric(*t) && n<MAX_LABEL_LENGTH-1)
                        {
                            sym1[n++] = *t;
                            t++;
                        }
                        sym1[n] = '\0';
                        a1 = OP_DIR;
                        s = t;
                    }
                    else
                    {
                        ctx->error_count++;
                    }
                }
                /*operand #2*/
                if (num_ops == 2)
                {
                    t=s;
                    skip_spaces(&t);
                    if (*t != ',')
                    {
                        ctx->error_count++;
                    }
                    else
                    {
                        /*found comma, moving past it*/
                        t++;
                        skip_spaces(&t);

                        if (*t == '#')
                        {
                            t++;
                            if (*t == '+' || *t == '-') t++;
                            while (isdigit(*t)) t++;
                            a2 = OP_IMM;
                            s = t;
                        }
                        else if (t[0] == 'r' && t[1] >= '0' && t[1] <= '7' && !is_alphanumeric(t[2]))
                        {
                            a2 = OP_REG;
                            s = t + 2;
                        }
                        else if (is_letter(*t))
                        {
                            n=0;
                            while (is_alphanumeric(*t) && n<MAX_LABEL_LENGTH-1)
                            {
                                sym2[n++] = *t;
                                t++;
                            }
                            sym2[n] = '\0';
                            a2 = OP_DIR;
                            s = t;
                        }
                        else
                        {
                            ctx->error_count++;
                        }
                    }
                }

                regs = (a1 == OP_REG) + (a2 == OP_REG);
                non_regs = (a1 != OP_REG) + (a2 != OP_REG);

                if (regs>= 1)
                {
                    off++; /*register before non-reg*/
                }

                /*extern for operand 1*/
                if (a1 == OP_DIR && sym1[0] !='\0')
                {
                    sx = sym_table_lookup(sym1);
                    if (sx&&sx->kind == SYM_EXTERN)
                    {
                        utmp = (ExtUse*)realloc(uses,sizeof(ExtUse)*(ext_count+1));
                        if (!utmp)
                        {
                            status = 0;
                            goto cleanup;
                        }
                        uses = utmp;
                        uses[ext_count].name = sx->name;
                        uses[ext_count].address = ic+(unsigned)off;
                        ext_count++;
                    }
                    off++;/* took one non reg operand word*/
                }
                /*extern for operand 2 */
                if (num_ops == 2 && a2 == OP_DIR && sym2[0] !='\0')
                {
                    sx = sym_table_lookup(sym2);
                    if (sx&&sx->kind == SYM_EXTERN)
                    {
                        utmp = (ExtUse*)realloc(uses,sizeof(ExtUse)*(ext_count+1));
                        if (!utmp)
                        {
                            status = 0;
                            goto cleanup;
                        }
                        uses = utmp;
                        uses[ext_count].name = sx->name;
                        uses[ext_count].address = ic+(unsigned)off;
                        ext_count++;
                    }
                    off++;
                }
                add_words = 1;

                if (regs>=1) add_words+=1;

                add_words+=(unsigned)non_regs;

                ic+=add_words;
            }
        }


    }
    fclose(in);
    if (ctx->error_count > 0)
    {
        free(entries);
        free(uses);
        return 0;
    }
    if (ent_count > 0)
    {
        if (!ent_write_all(am_file, entries, ent_count))
        {
            printf("Error, failed writing .ent");
            status = 0;
            goto cleanup;
        }
    }
    if (ext_count > 0)
    {
        if (!ext_write_all(am_file,uses,ext_count))
        {
            printf("Error, failed writing .ext");
            status = 0;
            goto cleanup;
        }
    }
   cleanup:
    if (in)
    {
        fclose(in);
    }
    free(entries);
    free(uses);
    return status;
}