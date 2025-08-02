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

int main(int argc, char *argv[]){
    (void)argc;
    (void)argv;
    run_preprocessor("dummy.as","dummy.am");
    return 0;
}
