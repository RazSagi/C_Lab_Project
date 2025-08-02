/*  Created by razsa on 8/2/2025  */

#include <stdlib.h>
#include <string.h>
#include "macro_list.h"

typedef struct MacroNode
{
    char *name;
    char *body;
    struct MacroNode *next;
} MacroNode;
struct MacroList
{
    MacroNode *head;
};

/*Creating new Macro List*/
MacroList *ml_create(void)
{
    MacroList *L=malloc(sizeof *L);
    if (L) L->head = NULL;
    return L;
}