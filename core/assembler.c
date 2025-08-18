/*
// Created by Raz Sagi on 29/07/2025.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "first_pass.h"
#include "globals.h"
#include "helpers.h"
#include "preproc.h"
#include "context_asm.h"
#include "second_pass.h"
#include "sym_table.h"


int main(int argc, char *argv[])
{
    int i;
    int all_err=0;
    int fp_err=0;
    int rc;
    CtxAsm ctx;
    if (argc<2)
    {
        printf("Using: assembler <basefile> [<basefile>...]\n");
        return 1;
    }
    for (i=1; i<argc; i++)
    {
        char *in = append_file_extension(argv[i], ".as");
        char *out = append_file_extension(argv[i], ".am");
        fp_err=0;
        if (!in || !out)
        {
            printf("Memory allocation failed\n");
            free(in);
            free(out);
            all_err=1;
            continue;
        }
        rc = run_preprocessor(in,out);
        memset(&ctx, 0, sizeof(ctx));
        ctx.IC = IC_START;
        ctx.DC = 0;
        ctx.error_count = 0;
        sym_table_reset();
        if (rc != PREPROC_OK)
        {
            printf("Error: preprocessing '%s' failed (code %d)\n",in, rc);
            all_err = 1;
            free(in);
            free(out);
            continue;
        }
        fp_err=run_first_pass(out, &ctx);
        if (fp_err<0)
        {
            printf("Error, couldent open file");
            all_err = 1;
        }
        else
        {
            if (fp_err>0)
            {
                printf("First pass found %d errors in '%s'\n",fp_err,argv[i]);
                all_err = 1;
            }
            else
            {
                printf("First pass success\n");

                if (!run_second_pass(out, &ctx))
                {
                    printf("Second pass found errors in '%s'\n",argv[i]);
                    all_err = 1;
                }
                else
                {
                    printf("Second pass success \n");
                }
            }
        }
        free(in);
        free(out);

    }

    return all_err;

}
