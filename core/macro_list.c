/*  Created by razsa on 8/2/2025  */

#include <stdlib.h>
#include <string.h>
#include "macro_list.h"

#include "helpers.h"

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
/*Deleting Macro list and freeing memory*/
void ml_delete(MacroList *L)
{
    MacroNode *p=L->head;
    while (p)
    {
        MacroNode *n = p->next;
        free(p->name);
        free(p->body);
        free(p);
        p=n;
    }
    free(L);
}
typedef enum
{
    ML_OK = 0,
    ML_ERROR_NODE = 1,
    ML_ERROR_NAMED = 2
}ML_Error;
/*Adding new Node(Macro) to the list*/
int ml_add(MacroList *list, const char *name,const char *body)
{
    MacroNode *node = malloc(sizeof *node);
    if (!node) return ML_ERROR_NODE;
    node->name = my_strdup(name);
    node->body = my_strdup(body);
    if (!node->name || !node->body)
    {
        free(node->name);
        free(node->body);
        free(node);
        return ML_ERROR_NAMED;
    }
    node->next = list->head;
    list->head = node;
    return ML_OK;
}

/*looking for Macro by name*/
const char *ml_lookup(const MacroList *list, const char *name)
{
    MacroNode *p=list->head;
    for (p = list->head; p; p=p->next)
    {
        if (strcmp(p->name, name) == 0)/*if exist, return the macro*/
        {
            return p->body;
        }
    }
    return NULL;
}