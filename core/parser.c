/*  Created by razsa on 8/8/2025  */

#include "parser.h"

#include <stdio.h>

#include "helpers.h"
#include <string.h>

/*skip past spaces and tabs*/
void skip_spaces(const char **p)
{
    const char *p1 = *p;
    while (*p1 && (*p1 == ' '||*p1=='\t'))
    {
        p1++;
    }
    *p=p1;
}

/*trying to extract label XXXX:
 * return 1 if valid label extracted
 * return 0 if its not a label (not error)
 * return -1 if its a label but not valid(reserved name\length)
 */
int extract_label(const char **p,char out_label[MAX_LABEL_LENGTH])
{
    const char *s = *p;
    const char *start = s;
    size_t len;

    /*label must start with letter*/
    if (!is_letter(*s))
    {
        return 0;
    }
    s++;
    while (is_alphanumeric(*s))
    {
        s++;
    }
    if (*s != ':')
    {
        return 0; /*not a label*/
    }

    len = (size_t)(s-start);
    if (len >= MAX_LABEL_LENGTH)
    {
        printf("Error, label too long (max %d chars)\n",MAX_LABEL_LENGTH-1);
        return -1;
    }

    memcpy(out_label,start,len);
    out_label[len] = '\0';

    if (is_reserved(out_label))
    {
        printf("Error, label '%s' reserved not allowed\n",out_label);
        return -1;
    }
    s++;
    skip_spaces(&s);
    *p=s;
    return 1;
}
