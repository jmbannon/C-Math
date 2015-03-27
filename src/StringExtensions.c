#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void append(char* string, const char c) {
    size_t len = strlen(string);
    string = (char *)realloc(string, len+1);
    if (string == NULL) {
        printf("* ERROR [append]:\n"
               "* Could not reallocate enough memory for functionBuilder.\n");
        return;
    }

    strncat(string, &c, 1);
}

void appendStr(char** string, const char* toCopy, const int charAmount) 
{
    if (charAmount < 0)
        printf("* ERROR [appendStr]:\n"
               "* Character amount must be greater than 0.\n");

    size_t len = strlen(*string);
    *string = (char *)realloc(*string, len+charAmount);
    if (!string) 
    {
        printf("* ERROR [append]:\n"
               "* Could not reallocate enough memory for functionBuilder.\n");
        return;
    }
    
    strncat(*string, toCopy, charAmount);
}

void removeChar(char* string, const int index) {
    size_t len = strlen(string);
    if (index < 0 || index > len) {
        printf("* ERROR [removeChar]:\n"
               "* Index out of range.\n");
        printf("*   String: %s\n*   Index: %d\n", string, index);
        return;
    } else if (index == len)
        return;

    memmove((void*)&string[index], (void*)&string[index+1], len-index);
}

void freeString(char* string) {
    free(string);
    string = malloc(0);
}
