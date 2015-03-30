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

char* removeEnds(
        char* functionPart_str
) {
    size_t len = strlen(functionPart_str);
    char* endRemoved = malloc(len-1);
    strncpy(endRemoved, functionPart_str+1, len-2);
    printf("returning %s\n", endRemoved);
    return endRemoved;
} 

void appendChar(
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

void appendStr(
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

void removeChar(
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
