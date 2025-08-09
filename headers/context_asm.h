/*  Created by razsa on 8/8/2025  */






#ifndef CONTEXT_ASM_H
#define CONTEXT_ASM_H
#include "globals.h"

typedef struct
{
    int IC;
    int DC;
    int error_count;
    int data_img[MAX_DATA];

}CtxAsm;
#endif
