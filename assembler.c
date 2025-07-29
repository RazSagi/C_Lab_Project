//
// Created by Raz Sagi on 29/07/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "helpers.h"

int pre_process(const char *file_name);
int first_pass(const char *file_name);
int second_pass(const char *file_name);

int main(int argc, char *argv[]){
    int i;
    if (argc <2) {
        printf("Missing files");
        return 1;
    }
    for (i = 1;i<argc;i++){
        char *name = append_file_extension(argv[i], ".as");
        FILE *file = fopen(name, "r");
        if (!file) {
            printf("Couldent open file: %s\n", name);
        }
        else {
            printf("Assembeling:%s\n", name);
            fclose(file);
        }
        free(name);
    }
    return 0;
}
