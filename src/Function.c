#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Function.h"

struct variable {
    char variable;
    int prime;
};

struct variableList {
    var var;
    var* next;
};

struct function {
    char* str;
    functionPart* head;
    varList variables;
};

struct numeric {
    double constant;
};

struct trigonometry {
    trigType trigType;
    functionPart* contents;
};

struct logarithm {
    double base;
    functionPart* contents;
};

struct function_part {
    char* str;
    union part_union 
    {
        functionPart* parenthesis;
        num* num;
        trig* trig;
        log* log;
    } part;
    
    union part_exponent 
    {
        num* num;
        functionPart* parenthesis;
    } exponent;

    opType operation;
    functionPart* prev;
    functionPart* next;
};

function* initializeFunction(const char* theFunction) {
    function* func = malloc(sizeof(function));

    func->str = malloc(strlen(theFunction)+1);
    strcpy(func->str, theFunction);
    func->head = NULL;
    return func;
}

functionPart** getHead(function* theFunction) {
    return &(theFunction->head);
}

void printInfo(function* theFunction) 
{
    if (!theFunction)
        return;

    printf("Full function: %s\n", theFunction->str);
    functionPart* curr = theFunction->head;

    printf("%x\n", theFunction->head);

    while (curr != NULL) 
    {
        printf("Part: %s Op: %d\n", curr->str, curr->operation);
        curr = curr->next;
    }
}

void addToFunctionList(functionPart** head, char* functionBuilder,
                       const opType operation)
{
    functionPart* thePart = malloc(sizeof(functionPart));
    functionPart* currTemp = *head;

    thePart->str = malloc(strlen(functionBuilder) + 1);
    strcpy(thePart->str, functionBuilder);

    freeString(functionBuilder);

    thePart->operation = operation;
    
    if (currTemp == NULL)
    {
        thePart->next = currTemp;
        thePart->prev = NULL;
        *head = thePart;
        return;
    }
    while (currTemp->next != NULL)
    {
        currTemp = currTemp->next;
    }
    
    currTemp->next = thePart;
    thePart->prev = currTemp;
    thePart->next = NULL;
}

trigType isTrigFunction(const char* firstLetter) 
{
    if (strncmp(firstLetter, "sin(", 4) == 0)
        return SIN;

    else if (strncmp(firstLetter, "cos(", 4) == 0)
        return COS;

    else if (strncmp(firstLetter, "tan(", 4) == 0)
        return TAN;

    else if (strncmp(firstLetter, "sec(", 4) == 0)
        return SEC;

    else if (strncmp(firstLetter, "csc(", 4) == 0)
        return CSC;

    else if (strncmp(firstLetter, "cot(", 4) == 0)
        return COT;
         
    else
        return NOTRIG;
}
