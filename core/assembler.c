/*
// Created by Raz Sagi on 29/07/2025.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"
#include "preproc.h"

int pre_process(const char *file_name);
int first_pass(const char *file_name);
int second_pass(const char *file_name);

int main(int argc, char *argv[])
{
    int i, err=0;
    int rc;
    if (argc<2)
    {
        fprintf(stderr,"Using: assembler <basefile> [<basefile>...]\n");
        return 1;
    }
    for (i=1; i<argc; i++)
    {
        char *in = append_file_extension(argv[i], ".as");
        char *out = append_file_extension(argv[i], ".am");
        if (!in || !out)
        {
            fprintf(stderr,"Memory allocation failed\n");
            free(in);
            free(out);
            err=1;
            continue;
        }
        rc = run_preprocessor(in,out);
        if (rc != PREPROC_OK) {
            fprintf(stderr,"Error: preprocessing '%s' failed (code %d)\n",in, rc);
            err = 1;
        } else {
            printf(" good\n");
        }
        free(in);
        free(out);
    }
    return err;

}
