#ifndef STRINGEXTENSIONS_H_
#define STRINGEXTENSIONS_H_

void appendChar(char* string, const char c);
void appendStr(char* string, const char* toCopy, const int charAmount);
void removeChar(char* string, const int index);
void freeString(char* string);

#endif
