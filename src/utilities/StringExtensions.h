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

void append_char(
        char * string, 
  const char   c
);

void append_str(
        char * string, 
  const char * toCopy, 
  const int    charAmount
);

void remove_char(
        char * string, 
  const int    index
);

#endif
