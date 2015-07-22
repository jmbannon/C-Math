/* 
 =======================================================================
 StringExtensions.h

 Jesse Bannon

 TODO
 =======================================================================
*/

#ifndef STRINGEXTENSIONS_H_
#define STRINGEXTENSIONS_H_

char * substring(
        char * string,
        const int beg_idx,
        const int end_idx
);

void appendChar(
        char* string, 
        const char c
);

void appendStr(
        char* string, 
        const char* toCopy, 
        const int charAmount
);

void removeChar(
        char* string, 
        const int index
);

void freeString( 
        char* string
);

#endif
