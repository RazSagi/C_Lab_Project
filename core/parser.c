/*  Created by razsa on 8/8/2025  */

#include "parser.h"
#include <stdlib.h>
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
/*working with signded int*/
static int parse_signed_int(const char **p, long *out)
{
    const char *s = *p;
    const char *end;
    long v;
    skip_spaces(&s);
    /*checking if number*/
    if (!(*s == '-' || *s == '+' || (*s >= '0' && *s <= '9') ))
    {
        return 0;
    }
    v = strtol(s,(char **)&end,10);
    if (end ==s)
    {
        return 0;
    }
    *out = v;
    *p=end;
    return 1;
}
/*if possible, store the word and bump the counter*/
static int store_word(CtxAsm *ctx, int value, int line_number)
{
    if (ctx->DC >= MAX_DATA)
    {
        printf("Error (line %d), data overflow\n",line_number);
        ctx->error_count++;
        return 0;
    }
    ctx->data_img[ctx->DC++] = value;
    return 1;
}

/*handles directive data*/
int handle_data(const char **p,CtxAsm *ctx, int line_number)
{
    const char *s = *p;
    long v;

    if (!parse_signed_int(&s,&v))
    {
        printf("Error (line %d), data expects at least one integer\n",line_number);
        ctx->error_count++;
        return 0;
    }

    if (!store_word(ctx,(int)v,line_number))
    {
        return 0;
    }

    for (;;)
    {
        const char *q = s;
        skip_spaces(&q);
        /* if we arrived to comma*/
        if (*q != ',')
        {
            s=q;
            break;
        }
        q++; /* moving after the comma*/
        skip_spaces(&q);

        if (*q == ',' || *q == '\0')
        {
            printf("error (line %d), invalid using comma in .data\n",line_number);
            ctx->error_count++;
            return 0;
        }
        if (!parse_signed_int(&q,&v))
        {
            printf("Error (line %d), missing value after comma\n",line_number);
            ctx->error_count++;
            return 0;
        }
        if (!store_word(ctx,(int)v,line_number))
        {
            return 0;

        }
        s=q;
    }
    *p=s;
    return 1;

}
/*handles .string and update the dc*/
int handle_string(const char **p,CtxAsm *ctx, int line_number)
{
    const char *s = *p;

    if (*s != '"')
    {
        printf("Error (line %d), must sstart with quote\n",line_number);
        ctx->error_count++;
        return 0;
    }
    /*skip the quote*/
    s++;

    /*now we store each char until reaching quote or EOL*/
    while (*s && *s !='"')
    {
        if (!store_word(ctx,(int)(unsigned char)*s,line_number))
        {
            return 0;
        }
        s++;
    }
    /*must end with quote =)*/
    if (*s != '"')
    {
        printf("Error (line %d), must end with quote\n",line_number);
        ctx->error_count++;
        return 0;
    }
    s++;

    /*storing the null ender also*/
    if (!store_word(ctx,0,line_number))
    {
        return 0;
    }
    *p=s;
    return 1;
}
/*Helper method used to parse 1 dimension in brackets:[num]*/
static int parse_dim_brackets(const char **p, long *out, int line_number)
{
    const char *s = *p;
    long v;

    /*skipping white chars*/
    skip_spaces(&s);
    /*checking if its start of brackets*/
    if (*s!='[')
    {
        printf("Error (line %d), must start with '['\n",line_number);
        return 0;
    }
    /*move past the '['*/
    s++;
    //checking if what after the brackets is positive number(dimension cant be negative or char)*/
    if (!parse_signed_int(&s,&v)|| v<=0)
    {
        printf("Error (line %d), in .mat the dimension must be positive number '['\n",line_number);
        return 0;
    }

    skip_spaces(&s);
    /* checking if after the number we have closing brackets*/
    if (*s != ']')
    {
        printf("Error (line %d), must end with ']'\n",line_number);
        return 0;
    }
    s++;
    *out=v;
    *p=s;
    return 1;
}

/*same as handle string and handle data, handles mat
 *if we given less values that the dimensions we fill the rest of the matrix with zero
 *return an error if too many values given
 */
int handle_mat(const char **p,CtxAsm *ctx, int line_number)
{
    const char *s = *p;
    long rows;
    long cols;
    long v;
    int total = 0;
    int count = 0;

    /*start to check if rows are ok followed by col
     *only the dimension declartion */
    if (!parse_dim_brackets(&s,&rows,line_number))
    {
        ctx->error_count++;
        return 0;
    }
    if (!parse_dim_brackets(&s,&cols,line_number))
    {
        ctx->error_count++;
        return 0;
    }

    total = (int)(rows*cols);

    /*checking data section overflow*/
    if (ctx->DC + total > MAX_DATA)
    {
        printf("Error (line %d), data overflow\n",line_number);
        ctx->error_count++;
        return 0;
    }

    skip_spaces(&s);
    /* if there is no list to initialize, filling zeros*/
    if (*s == '\0')
    {
        int i;
        for (i=0; i<total; i++)
        {
            if (!store_word(ctx,0,line_number))
            {
                return 0;
            }
        }
        *p=s;
        return 1;
    }

    /*there is text we need to parse and handle*/
    if (!parse_signed_int(&s,&v))
    {
        printf("Error (line %d), expecting integer list after dimensions\n",line_number);
        ctx->error_count++;
        return 0;

    }
    if (!store_word(ctx,(int)v,line_number))
    {
        return 0;
    }
    count = 1;
    for ( ; ;)
    {
        const char *q = s;
        skip_spaces(&q);
        /*list ended*/
        if (*q != ',')
        {
            s=q;
            break;
        }
        /*moving past the comma*/
        q++;
        skip_spaces(&s);

        if (*q == ',' || *q == '\0')
        {
            printf("Error (line %d), double comma or comma without integer after\n",line_number)
            ctx->error_count++;
            return 0;
        }

        if (!parse_signed_int(&q,&v)
        {
            printf("Error (line %d), missing value after comma\n",line_number);
            ctx->error_count++;
            return 0;
        }

        /*too many values*/
        if (count >= total)
        {
            printf("Error (line %d), too many elements\n",line_number);
            ctx->error_count++;
            return 0;
        }

        if (!store_word(ctx,(int)v,line_number))
        {
            return 0;
        }
        count++;
        s=q;


    }

    /*if missing values, fill with zeros*/
    while (count< total)
    {
        if (!store_word(ctx,0,line_number))
        {
            return 0;
        }
        count++;
    }

    *p=s;
    return 1;


}
