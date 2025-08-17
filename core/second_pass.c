
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"
#include "sym_table.h"
#include "second_pass.h"

#include <ctype.h>

#include "parser.h"

/*builds new path for the new files we are going to create*/
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
static int ob_write_all(char *am_path,int code_count, int data_count,const unsigned *code_words,const unsigned *data_words)
{
    char *ob_path = make_out_path(am_path,FILE_EXT_OB);
    FILE *out;
    char buffer[BASE4_STR_MAX];
    unsigned addr;
    int i;
    unsigned w;
    int pad;
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
    addr=IC_START;
    for (i = 0 ;i<code_count; i++,addr++)
    {
        w = code_words[i] & ((1u << WORD_BITS) - 1u);
        make_unique_base4(addr,buffer);
        fputs(buffer,out);
        fputc(' ',out);
        make_unique_base4(w,buffer);
        pad = 5-(int)strlen(buffer);
        while (pad-- > 0 ) fputc('a',out);
        fputs(buffer,out);
        fputc('\n',out);
    }

    /*data words*/

    for (i = 0 ;i<data_count; i++,addr++)
    {
        w = data_words[i] & ((1u << WORD_BITS) - 1u);
        make_unique_base4(addr,buffer);
        fputs(buffer,out);
        fputc(' ',out);
        make_unique_base4(w,buffer);
        pad = 5-(int)strlen(buffer);
        while (pad-- > 0 ) fputc('a',out);
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

    unsigned ic = IC_START;
    unsigned add_words = 0;

    unsigned *code = NULL;
    int code_count = 0;
    unsigned code_idx = 0;
    int op_id;

    unsigned src_mode;
    unsigned dst_mode;
    unsigned first;




    int status = 1; /*start of as success, if error status = 0 and goto cleanup*/

    /*trying to open the file*/
    in =fopen(am_file,"r");
    if (!in)
    {
        printf("Error, cannot open '%s'\n",am_file);
        status = 0;
        goto cleanup;
    }

    /* allocate code buffer*/

    code_count = ctx->IC - IC_START;
    if (code_count<0) code_count =0;

    if (code_count>0)
    {
        code = (unsigned*)calloc((size_t)code_count,sizeof(unsigned));
        if (!code)
        {
            printf("Error, out of memory for code image\n");
            status = 0;
            goto cleanup;
        }
    }
    code_idx = 0;

    /*running through the lines in the .am file*/

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
            int k;
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
            /*checking for extra text after .entry*/
            if (!validate_no_extra(p, line_no, ctx)) continue;

            s = sym_table_lookup(name);
            if (!s)
            {
                printf("Error(line %d), .entry undefined\n",line_no);
                ctx->error_count++;
                continue;
            }
            /*checks for entry extern or dupe entry*/
            if (s->kind == SYM_EXTERN)
            {
                printf("Error(line %d), .entry on extern symbol '%s'\n",line_no,s->name);
                ctx->error_count++;
                continue;
            }
            sym_table_mark_entry(name);
            for (k=0;k<ent_count;k++)
            {
                if (strcmp(entries[k].name,s->name) == 0)
                {
                    break;
                }
            }
            if ( k == ent_count)
            {
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
            }
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
            long imm1 = 0;
            long imm2 = 0;
            int has_imm1 = 0;
            int has_imm2 = 0;
            char *start=NULL;
            char *endp= NULL;
            long v = 0;
            int r1 = -1;
            int r2 = -1;
            int base_idx = 0;
            int idx_reg = 0;
            int idx_op1=0;
            int idx_op2=0;
            int pay = 0;
            int val = 0;
            int m=0;
            int valid;
            unsigned pay_mask = PAYLOAD_MASK;



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
                        start = t + 1;
                        v = strtol(start,&endp,10);
                        if (endp==start)
                        {
                            ctx->error_count++;
                        }
                        else
                        {
                            a1 = OP_IMM;
                            imm1 = v;
                            has_imm1 = 1;
                            s=endp;

                            /*check for delimiter after immediate*/
                            if (!validate_operand_delim(s, line_no, ctx, 1)) continue;
                        }

                    }
                    else if (t[0] == 'r' && t[1] >= '0' && t[1] <= '7' && !is_alphanumeric(t[2]))
                    {
                        a1 = OP_REG;
                        r1 = t[1] - '0';
                        s = t + 2;
                        if (!validate_operand_delim(s, line_no, ctx, 1)) continue;
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
                        if (!validate_operand_delim(s, line_no, ctx, 1)) continue;

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
                            start = t + 1;
                            v = strtol(start,&endp,10);
                            if (endp==start)
                            {
                                ctx->error_count++;
                            }
                            else
                            {
                                a2 = OP_IMM;
                                imm2 = v;
                                has_imm2 = 1;
                                s = endp;
                                /*check for delimiter after immediate*/
                                if (!validate_operand_delim(s, line_no, ctx, 0)) continue;
                            }
                        }
                        else if (t[0] == 'r' && t[1] >= '0' && t[1] <= '7' && !is_alphanumeric(t[2]))
                        {
                            a2 = OP_REG;
                            r2 = t[1] - '0';
                            s = t + 2;
                            if (!validate_operand_delim(s, line_no, ctx, 0)) continue;
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
                            if (!validate_operand_delim(s, line_no, ctx, 0)) continue;
                        }
                        else
                        {
                            ctx->error_count++;
                        }
                    }
                }


                op_id = opcode_by_name(p,tok_len);
                /*legality check for the op_id*/
                if (op_id<0)
                {
                    printf("Error(line %d), unknown opcode\n",line_no);
                    ctx->error_count++;
                    continue;
                }

                valid = operands_legal(op_id,a1,a2);
                if (!valid)
                {
                    printf("Error(line %d), invalid addressing for opcode\n",line_no);
                    ctx->error_count++;
                    continue;
                }
                /*checks if there is extra text after the instruction operands*/
                if (!validate_no_extra(s, line_no, ctx)) continue;


                /*check if the call target is a valid code label*/
                if ((op_id == 9 || op_id == 10 || op_id == 13) && a1 == OP_DIR)
                {
                    sx = sym_table_lookup(sym1);
                    if (sx && sx ->kind ==SYM_DATA)
                    {
                        printf("Error(line %d), call target '%s' must be code label\n",line_no,sym1);
                        ctx->error_count++;
                        continue;
                    }
                }

                src_mode=0;
                dst_mode=0;
                first = 0;

                /*map addressing modes*/
                if (num_ops == 2)
                {
                    src_mode = (unsigned)a1;
                    dst_mode = (unsigned)a2;
                }
                else if (num_ops == 1)
                {
                    src_mode = (unsigned)OP_NONE;
                    dst_mode = (unsigned)a1;
                }
                else
                {
                    src_mode = (unsigned)OP_NONE;
                    dst_mode = (unsigned)OP_NONE;
                }

                first = ((unsigned)op_id << OPCODE_SHIFT);
                first |= (src_mode<<SRC_SHIFT);
                first |= (dst_mode<<DST_SHIFT);
                first |= ((unsigned)ARE_ABS<<ARE_SHIFT);

                if (code_idx < (unsigned)code_count)
                {
                    code[code_idx] = first;
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

                }
                if (a1 != OP_REG)
                {
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

                }
                if (num_ops == 2 && a2 !=OP_REG)
                {
                    off++;
                }
                /*immediate range checks*/
                if (has_imm1 && (imm1<IMM_MIN || imm1>IMM_MAX))
                {
                    printf("Error(line %d), immediate out of bounds\n",line_no);
                    ctx->error_count++;
                    continue;
                }
                if (has_imm2 && (imm2<IMM_MIN || imm2>IMM_MAX))
                {
                    printf("Error(line %d), immediate out of bounds\n",line_no);
                    ctx->error_count++;
                    continue;
                }
                /*register words*/
                {
                    base_idx = (int)code_idx;
                    idx_reg = base_idx +1;
                    pay = 0;

                    if (r1 >= 0) pay |= (r1 & 15) << 4;
                    if (r2 >= 0) pay |= (r2 & 15);

                    if (r1>= 0 || r2 >= 0)
                    {
                        val = ((pay & pay_mask)<<2)|ARE_ABS;
                        if ((int)idx_reg < code_count)
                        {
                            code[idx_reg] = (unsigned)val;
                        }
                    }
                }
                {/* immediate operands words, ARE=ABS*/
                    base_idx = (int)code_idx;
                    idx_op1 = base_idx+1;

                    if (regs>= 1)
                    {
                        idx_op1 += 1;
                    }
                    idx_op2 = idx_op1;

                    if (a1 != OP_REG)
                    {
                        idx_op2 += 1;
                    }
                    /*imm in op 1*/
                    if (has_imm1 && idx_op1 <code_count)
                    {
                        m = ((int)imm1) & pay_mask;
                        val = (m<<2) | ARE_ABS;
                        code[idx_op1] = (unsigned)val;
                    }

                    /*imm in op 2*/
                    if (has_imm2 && idx_op2 <code_count)
                    {
                        m = ((int)imm2) & pay_mask;
                        val = (m<<2) | ARE_ABS;
                        code[idx_op2] = (unsigned)val;
                    }
                }

                /*direct\extern operands*/
                {
                    base_idx = (int)code_idx;
                    idx_op1 = base_idx+1;
                    if (regs>= 1)
                    {
                        idx_op1 += 1;
                    }
                    idx_op2 = idx_op1;
                    if (a1 != OP_REG)
                    {
                        idx_op2 += 1;
                    }
                    /*op one*/
                    if (a1 == OP_DIR && idx_op1 <code_count)
                    {
                        if (!encode_direct_extern(sym1, code, idx_op1, line_no, ctx)) continue;
                    }
                    /* op two*/
                    if (num_ops == 2 && idx_op2 <code_count&& a2 == OP_DIR)
                    {
                        if (!encode_direct_extern(sym2, code, idx_op2, line_no, ctx)) continue;
                    }
                }
                add_words = 1;

                if (regs>=1) add_words+=1;

                add_words+=(unsigned)non_regs;

                ic+=add_words;
                code_idx+=add_words;
            }
        }


    }

    if (ctx->error_count > 0)
    {
        status = 0;
        goto cleanup;
    }
    /*writes .ob, if succeed printing .ent .ext*/
    if (code_count >0 || ctx->DC>0)
    {
        if (!ob_write_all(am_file,code_count,ctx->DC,code,(unsigned*)ctx->data_img))
        {
            printf("Error, failed writing .ob\n");
            status = 0;
            goto cleanup;
        }
    }

    if (ent_count > 0)
    {
        if (!ent_write_all(am_file, entries, ent_count))
        {
            printf("Error, failed writing .ent\n");
            status = 0;
            goto cleanup;
        }
    }
    if (ext_count > 0)
    {
        if (!ext_write_all(am_file,uses,ext_count))
        {
            printf("Error, failed writing .ext\n");
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
    free(code);

    return status;
}