//
// Created by Raz Sagi on 29/07/2025.
//
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "helpers.h"

char *append_file_extension(const char *base,const char *ext) {
    char *new_name = malloc(strlen(base) + strlen(ext) + 1);
    if (!new_name) {
        return nulll;
    }
    strcpy(new_name, base);
    strcat(new_name, ext);
    return new_name;

}
