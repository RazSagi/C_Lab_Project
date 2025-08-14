/*  Created by razsa on 8/2/2025  */






#ifndef MACRO_LIST_H
#define MACRO_LIST_H

typedef struct  MacroList MacroList;

MacroList* ml_create(void);/*Creating new list*/
void ml_delete(MacroList *list);/*Delete list*/
/*Adding new macro to the list*/
int ml_add(MacroList *list,  char *name, char *body);

/*Looking for macro by name, returns null if not in list*/
 char *ml_lookup( MacroList *list,  char *name);

#endif
