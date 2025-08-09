/*  Created by razsa on 8/8/2025  */

#include "parser.h"

#include <stdio.h>

#include "helpers.h"
#include <string.h>

#include "context_asm.h"




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
/*checking if the text after the label is a directive and if so advancing *p after the directive+space*/
int check_directive_type(const char **p,int line_number,CtxAsm *ctx)
{
    const char *s = *p;

    skip_spaces(&s);

    /*check if its even directive*/
    if (*s != '.') return DIR_NONE;

    /*skip the dot*/
    s++;

    /*checking to if its fit one of our directives*/
    /*data*/
    if (strncmp(s,"data",4)==0 && (s[4] == ' '|| s[4] == '\t' || s[4] == '\0'))
    {
        s+=4;/*passing it to the first char after "data"*/
        skip_spaces(&s);
        if (*s == '\0')
        {
            printf("Error (line %d), missing arguments after .data\n",line_number);
            ctx->error_count++;
            return DIR_NONE;
        }
        *p=s;
        return DIR_DATA;
    }

    /*string*/
    if (strncmp(s,"string",6)==0 && (s[6] == ' '|| s[6] == '\t' || s[6] == '\0'))
    {
        s+=6;/*passing it to the first char after "string"*/
        skip_spaces(&s);
        if (*s == '\0')
        {
            printf("Error (line %d), missing arguments after .string\n",line_number);
            ctx->error_count++;
            return DIR_NONE;
        }
        *p=s;
        return DIR_STRING;
    }

    /*mat*/
    if (strncmp(s,"mat",3)==0 && (s[3] == ' '|| s[3] == '\t' || s[3] == '\0'))
    {
        s+=3;/*passing it to the first char after "string"*/
        skip_spaces(&s);
        if (*s == '\0')
        {
            printf("Error (line %d), missing arguments after .mat\n",line_number);
            ctx->error_count++;
            return DIR_NONE;
        }
        *p=s;
        return DIR_MAT;
    }

    /*entry*/
    if (strncmp(s,"entry",5)==0 && (s[5] == ' '|| s[5] == '\t' || s[5] == '\0'))
    {
        s+=5;/*passing it to the first char after "string"*/
        skip_spaces(&s);
        if (*s == '\0')
        {
            printf("Error (line %d), missing arguments after .entry\n",line_number);
            ctx->error_count++;
            return DIR_NONE;
        }
        *p=s;
        return DIR_ENTRY;
    }

    /*extern*/
    if (strncmp(s,"extern",6)==0 && (s[6] == ' '|| s[6] == '\t' || s[6] == '\0'))
    {
        s+=6;/*passing it to the first char after "string"*/
        skip_spaces(&s);
        if (*s == '\0')
        {
            printf("Error (line %d), missing arguments after .extern\n",line_number);
            ctx->error_count++;
            return DIR_NONE;
        }
        *p=s;
        return DIR_EXTERN;
    }

    /*if its aint fit any one of our saved words its means it started with . and didnt match = error , unkown directive*/

    printf("Error (line %d), unknown directive\n",line_number);
    ctx->error_count++;
    return DIR_NONE;






}
