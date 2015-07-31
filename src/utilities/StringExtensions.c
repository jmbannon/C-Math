/* 
 =======================================================================
 StringExtensions.c

 Jesse Bannon

 TODO
 =======================================================================
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char * substring(
        char * string,
        const int beg_idx,
        const int end_idx
) {
    if (beg_idx < 0 || end_idx >= strlen(string))
        return NULL;

    int i, j = 0, size = end_idx - beg_idx + 1;
    char * substring = malloc(size);
    for (i = beg_idx; i <= end_idx; i++)
    {
        substring[j++] = string[i];
    }
    substring[j] = '\0';
    return substring;
}

char * remove_ends(
        char* functionPart_str,
        const size_t amount
) {
    size_t len = strlen(functionPart_str);
    char * endRemoved = malloc(len-(2*amount)+1);

    strncpy(endRemoved, functionPart_str+amount, len-(2*amount));
    endRemoved[len-(2*amount)] = '\0';

    return endRemoved;
} 

void append_char(
        char* string, const char c
) {

    if (strlen(string)+1 >= 1024) {
        printf("* ERROR [appendChar]:\n"
               "* Not enough memory to store function builder >= 1024 "
               "characters.\n");
        return;
    }

    sprintf(string, "%s%c", string, c);
}

void append_str(
        char* string, 
        const char* toCopy, 
        const int charAmount
) {
    if (charAmount < 0) {
        printf("* ERROR [appendStr]:\n"
               "* Character amount must be greater than 0.\n");
        return;
    }

    else if (strlen(string) + charAmount >= 1024)
    {
        printf("* ERROR [appendStr]:\n"
               "* Not enough memory to store function builder >= 1024 "
               "characters.\n");
        return;
    }

    if (string[0] != '\0')
        sprintf(string, "%s%.*s", string, charAmount, toCopy);
    else
        sprintf(string, "%.*s", charAmount, toCopy);
}

void remove_char(
        char* string, 
        const int index
) {
    size_t len = strlen(string);
    if (index < 0 || index > len) 
    {
        printf(
        "* ERROR [removeChar]:\n"
        "* Index out of range.\n"
        "*   String: %s\n*   Index: %d\n", string, index);

        return;
    } else if (index == len)
        return;

    memmove((void*)&string[index], (void*)&string[index+1], len-index);
}
