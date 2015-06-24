/* 
 =======================================================================
 Function.c

 Jesse Bannon

 TODO
 =======================================================================
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Function.h"
#include "Boolean.h"
#include "Parse.h"

static int VAR_LIST_SIZE = 64;

/* 
 =======================================================================

        Structs

 =======================================================================
*/

struct function 
{
    char * str;
    char * variableList;
    functionPart * head;
};

struct numeric 
{
    double constant;
};

struct trigonometry 
{
    trigType trigType;
    functionPart * contents;
};

struct logarithm 
{
    double base;
    functionPart * contents;
};

struct function_part 
{
    char * str;
    union part_union 
    {
        functionPart* parenthesis;
        num * num;
        trig * trig;
        log * log;
    } part;
    
    union part_exponent 
    {
        num * num;
        functionPart * parenthesis;
    } exponent;

    opType operation;
    functionPart * prev;
    functionPart * next;
};

/* 
 =======================================================================

        Function Prototypes

 =======================================================================
*/

//TODO

/* 
 =======================================================================

        Functions

 =======================================================================
*/

void initializePart(
        functionPart** thePart
) {

    int i;
    bool openingParenthesis = 1;
    function * newPart;

    printf("thePart = %s\n", (*thePart)->str);
    for (i = 0; i < strlen((*thePart)->str); i++) 
    {
        if ((*thePart)->str[i] == '(')
        {
            
            //Do stuff
        } else 
        {
            if (openingParenthesis && i != 0)
                 //newPart = parseFunction(removeEnds((*thePart)->str, i));
            
            openingParenthesis = 0;
        }
    }
    //printInfo(newPart);

}

void initializeFunction(
        function ** func,
        const char * theFunction
) {
    (*func) = malloc(sizeof(function));

    (*func)->str = malloc(strlen(theFunction)+1);
    strcpy((*func)->str, theFunction);
    (*func)->head = NULL;
    (*func)->variableList = calloc(VAR_LIST_SIZE, sizeof(char));
}

functionPart ** getHead(
        function* theFunction
) {
    return &(theFunction->head);
}

void printInfo(
        function * theFunction
) {
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

    int i = 0;
    printf("Variables in ascending order: ");
    while (i < VAR_LIST_SIZE && theFunction->variableList[i])
        printf("%c ", theFunction->variableList[i++]);
    printf("\n");
}

void addToFunctionList(
        functionPart ** head, 
        char * functionBuilder,          
        const opType operation
) {
    functionPart* thePart = malloc(sizeof(functionPart));
    functionPart* currTemp = *head;

    thePart->str = malloc(strlen(functionBuilder) + 1);
    strcpy(thePart->str, functionBuilder);
    initializePart(&thePart);

    functionBuilder[0] = '\0';

    thePart->operation = operation;
    
    if (currTemp == NULL)
    {
        thePart->next = currTemp;
        thePart->prev = NULL;
        *head = thePart;
        return;
    }
    while (currTemp != NULL && currTemp->next != NULL)
    {
        currTemp = currTemp->next;
    }
    
    currTemp->next = thePart;
    thePart->prev = currTemp;
    thePart->next = NULL;
}

trigType isTrigFunction(
        const char * firstLetter
) {
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


int needsInsert(
        function * theFunction,
        const char variable
) {
    int i;
    for (i = 0; i < VAR_LIST_SIZE; i++)
    {
        if (theFunction->variableList[i] == variable)
            return -1;

        else if (theFunction->variableList[i] == '\0')
            return i;
    }
    return VAR_LIST_SIZE;
}

void insertVariableToList(
        char * variableList,
        char variable,
        int idx
) {
    if (idx == VAR_LIST_SIZE)
        variableList = realloc(variableList, VAR_LIST_SIZE*=2);

    variableList[idx] = variable;
    while (idx != 0 && variableList[idx-1] > variableList[idx])
    {
        variable = variableList[idx-1];
        variableList[idx-1] = variableList[idx];
        variableList[idx--] = variable;
    }
}

void addToVariableList(
    function * theFunction,
    char variable
) {
    int insertIdx = needsInsert(theFunction, variable);
    if (insertIdx < 0)
        return;
    else 
        insertVariableToList(theFunction->variableList,
                             variable,
                             insertIdx);
}
