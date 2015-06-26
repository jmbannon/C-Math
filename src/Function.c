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
#include "StringExtensions.h"

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
    function * func;
    char * str;
    union part_union 
    {
        functionPart * parenthesis;
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

/* Determines the part_union for the given functionPart.
 * Creates the part based on parsing the first char within its string
 */
void initializePart(
        functionPart * thePart
) {

    int i = 0, len = strlen(thePart->str);
    function * newPart;

    printf("thePart = %s\n", thePart->str);
    if (thePart->str[i] == '(')
    {
        while (thePart->str[i] == '(' && thePart->str[len-1-i] == ')') ++i;
        thePart->part.parenthesis = parseFunctionPart(substring(thePart->str, i, len-1-i));
    }
}

/* Initializes a function type with the given function string.
 */
function * initializeFunction(
        const char * theFunction
) {
    function * func = malloc(sizeof(function));

    func->str = malloc(strlen(theFunction)+1);
    strcpy(func->str, theFunction);
    func->head = NULL;
    func->variableList = calloc(VAR_LIST_SIZE, sizeof(char));
    return func;
}

/* Returns the head of a function's linked list of functionParts
 */
functionPart * getHead(
        function * theFunction
) {
    return theFunction->head;
}

/* Prints debugging info of the given function.
 */
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

/* Adds a valid piece of a function string to the function type's linked
 * list of functionParts with the given operation.  Then clears the function
 * builder for input of another functionPart if it exists.
 */
void addToFunctionList(
        function * func,
        char * functionBuilder,          
        const opType operation
) {
    functionPart * thePart = malloc(sizeof(functionPart));
    functionPart * head = getHead(func);
    functionPart * currTemp = head;

    thePart->func = func;
    thePart->str = malloc(strlen(functionBuilder) + 1);
    strcpy(thePart->str, functionBuilder);
    initializePart(thePart);

    functionBuilder[0] = '\0';

    thePart->operation = operation;
    
    if (currTemp == NULL)
    {
        thePart->next = currTemp;
        thePart->prev = NULL;
        head = thePart;
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

/* Returns a trigType enum if one exists for the beginning letter of a
 * string.
 */
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

/* Returns the index a variable should be inserted into to traverse a
 * variable array. Returns -1 if it exists within the function's variable
 * array.
 */
int needs_var_insert(
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

/* Inserts a variable into a given variable ordered-list by starting it
 * at the end of the array and bubble up until it is in ascending order.
 */
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

/* Inserts a variable into the function's variable array if it does not exist.
 */
void addToVariableList(
    function * theFunction,
    char variable
) {
    int insertIdx = needs_var_insert(theFunction, variable);
    if (insertIdx < 0)
        return;
    else 
        insertVariableToList(theFunction->variableList,
                             variable,
                             insertIdx);
}
