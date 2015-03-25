#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Parse.h"
#include "StringExtensions.h"

enum operations {NOOP, ADD, SUB, MUL, DIV, EXP};
enum trigFunctions {NOTRIG, SIN, COS, TAN, SEC, CSC, COT};

struct variable {
    char variable;
    double value;
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

struct polynomial {
    double constant;
    double exponent;
    var variable;
};

struct trigonometry {
    trigType trigType;
    function contents;
};

struct logarithm {
    double base;
    function contents;
};

struct function_part {
    char* str;
    union part_union {
        functionPart* parenthesis;
        poly poly;
        trig trig;
    } part;
    opType operation;
    functionPart* prev;
    functionPart* next;
};

void printInfo(function* theFunction) {
    printf("Full function: %s\n", theFunction->str);
    functionPart* curr = theFunction->head;

    printf("%x\n", theFunction->head);

    while (curr != NULL) {
        printf("Part: %s Op: %d\n", curr->str, curr->operation);
        curr = curr->next;
    }
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

function* parseFunction(const char* theFunction) 
{
    int i; /* iterator */

    int parenthesisBalance = 0;
    int parenthesisOpenIndex = -1;
    int functionParenthesisBalance = -1;
    int functionParenthesisOpenIndex = -1;

    int hasOperation = 0;
    int hasNegative = 0;
    int hasDecimal = 0;
    int hasExponent = 0;
 
    int isParenthesis = 0;
    int isFunction = 0;

    opType tempOp;
    
    function* func = malloc(sizeof(function));

    func->str = malloc(strlen(theFunction)+1);
    strcpy(func->str, theFunction);
    func->head = NULL;

    char* functionBuilder = malloc(0);
    char c, tempChar;

    for (i = 0; i < strlen(theFunction); i++) 
    {
        c = theFunction[i];
	printf("%c\n", c);

        /* NUMBERS */
        if (isdigit(c)) 
        {

            if (i == 0)
                append(functionBuilder, c);	

            else if (isalpha(theFunction[i-1]) || theFunction[i-1] == ')') 
            {
                printf("* ERROR [parseFunction]:\n"
                       "* A number cannot proceed a "
                       "letter or close parenthesis.\n");
                printf("*   Function: %s\n*   Index: %d\n", theFunction, i);
                return NULL;
            }

            else
                append(functionBuilder, c);       

            if (hasOperation)
                hasOperation = 0;
        }

        /* LETTERS */
        else if (isalpha(c)) 
        {

            if (index == 0)
                append(functionBuilder, c);

            /* If letters proceed are a trig function, add preceding
             * function to equationList and create new function 
             */
            else if (isTrigFunction(&theFunction[i])) 
            {
                isFunction = 1;
                ++parenthesisBalance;
                functionParenthesisBalance = parenthesisBalance;

                addToFunctionList(&func->head, functionBuilder, MUL);
                printf("New part!\n");
                appendStr(functionBuilder, &theFunction[i], 4);

                i += 3;
                parenthesisOpenIndex = i;

            }

            /* If char follows another char and is not a function,
               add previous function to function list and create a
               new function. For example: xy = x*y */
            else if (isalpha(theFunction[i-1]) 
                        && !isFunction 
                        && !isParenthesis) 
            {
                addToFunctionList(&func->head, functionBuilder, MUL);
                printf("new part!\n");
                appendStr(functionBuilder, &c, 1);
            }

            else if (theFunction[i-1] == '.' || theFunction[i-1] == ')') 
            {
                printf(
                "* ERROR [parseFunction]:\n"
                "* Invalid variable. Cannot proceed non-numeric symbols.\n"
                "* Fix later for variable multiplication\n"
                "*   Function: %s\n*   Index: %d\n", theFunction, i);
                return NULL;
            }

            else
                appendStr(functionBuilder, &c, 1);

            if (hasOperation)
                hasOperation = 0;

        }

        /* DECIMAL POINT */
        else if (c == '.') 
        {
            if (!hasDecimal) 
            {
                 tempChar = '0';

                 /* If no numbers precede, add 0 first. */
                 if (i == 0)                    
                     appendStr(functionBuilder, &tempChar, 1);
                 
                 /* If letter precedes decimal, throw error. */
                 else if (isalpha(theFunction[i-1])) 
                 {
                     printf(
                     "* ERROR [parseFunction]:\n"
                     "* A decimal point cannot proceed a variable.\n"
                     "*   Function: %s\n*   Index: %d\n", theFunction, i);
                     
                     return NULL;
                 
                 /* If a number does not precede decimal, add 0 first. */
                 else if (!isdigit(theFunction[i-1]))
                     appendStr(functionBuilder, &tempChar, 1);

                 /* Append and set hasDecimal to true. */
                 appendStr(functionBuilder, &c, 1);
                 hasDecimal = 1;
            }

            else 
            {
                printf(
                "* ERROR [parseFunction]:\n"
                "* Numeric already contains a decimal.\n"
                "*   Function: %s\n*   Index: %d\n", theFunction, i);
                return NULL;
            }
        }

        /* NEGATIVE/SUBTRACTION */
        else if (c == '-') 
        {
            if (!hasNegative) 
            {
                /* If the function has numbers/variables and is not 
                 * in parenthesis, act as operator. 
                 */            
                if (strlen(functionBuilder) != 0 
                       && !isParenthesis 
                       && !isFunction 
                       && !hasOperation) 
                {
                    addToFunctionList(&func->head, functionBuilder, SUB);
                    printf("new part!\n");
                    hasOperation = 1;

                /* Otherwise append to function. */
                } else 
                {
                    if (i == 0) 
                    {
                        appendStr(functionBuilder, &c, 1);
                        hasNegative = 1;
                    }

                    else if (theFunction[i-1] == '.') {
                        printf(
                        "* ERROR [parseFunction]:\n"
                        "* Cannot subtract a decimal.\n"
                        "*   Function: %s\n*   Index: %d\n", theFunction, i);
                        
                        return NULL;
                    }

                    else 
                    {
                        appendStr(functionBuilder, &c, 1);
                        hasNegative = 1;
                    }
                }                
            }

            /* If negative sign is present. */
            else 
            {

                /* If function already contains a negative and not in 
                 * parenthesis string, throw exception 
                 */
                if (strcmp(functionBuilder, "-") == 0 
                        && !isParenthesis 
                        && !isFunction) 
                {
                    printf(
                    "* ERROR [parseFunction]:\n"
                    "* A subtraction operation already exists.\n"
                    "*   Function: %s\n*   Index: %d\n", theFunction, i);
                    return NULL;
                }

                /* If no parenthesis, act as subtraction operation: 
                 * add function to list. 
                 */
                if (!isParenthesis && !isFunction) 
                {
                    if (hasOperation) 
                    {
                        printf(
                        "* ERROR [parseFunction]:\n"
                        "* A subtraction operator already exists.\n"
                        "*   Function: %s\n*   Index: %d\n", theFunction, i);
                        
                        return NULL;
                    }

                    addToFunctionList(&func->head, functionBuilder, SUB);     
                    printf("new part!\n");
                    hasOperation = 1;
                }

                /* If within parenthesis, append to function to handle 
                 * in recursion 
                 */
                else
                    appendStr(functionBuilder, &c, 1);

                hasNegative = 0;
                hasDecimal = 0;
                hasExponent = 0;
            }
        }

        /* EXPONENT */
        else if (c == '^') 
        {
            if (!hasExponent) 
            {
            
                /* If exponent does not proceed number or variable, 
                 * throw exception. 
                 */
                if (i == 0 
                       || (!isdigit(theFunction[i-1]) 
                           && !isalpha(theFunction[i-1]) 
                           && theFunction[i-1] != ')')) 
                {
                    printf(
                    "* ERROR [parseFunction]:\n"
                    "* Must proceed a number, variable, or parenthesis.\n"
                    "*   Function: %s\n*   Index: %d\n", theFunction, i);

                    return NULL;
                }

                /* Append to function and set exponent to true, negative 
                 * and decimal to false for exponent number 
                 */
                else 
                {
                    appendStr(functionBuilder, &c, 1);
                    hasOperation = 1;
                    hasExponent = 1;
                    hasNegative = 0;
                    hasDecimal = 0;
                }

                /* If exponent sign already exists, throw exception */
            } else 
            {
                printf(
                "* ERROR [parseFunction]:\n"
                "* Function already contains an exponent.\n"
                "*   Function: %s\n*   Index: %d\n", theFunction, i);
                
                return NULL;
            }
        }

        /* OPERATORS (BESIDES SUBTRACTION) */
        else if (c == '+' || c == '*' || c == '/') 
        {    
            if (i == 0 || hasOperation) 
            {
                printf(
                "* ERROR [parseFunction]:\n"
                "* Must proceed a number, variable, or parenthesis.\n"
                "*   Function: %s\n*   Index: %d\n", theFunction, i);

                return NULL;
            }

            /* If it is not in parenthesis or function, add 
             * function and operator to list 
             */
            else if (!isParenthesis && !isFunction) 
            {
                switch(c) 
                {
                case '+': tempOp = ADD; break;
                case '*': tempOp = MUL; break;
                case '/': tempOp = DIV; break;
                default: tempOp = NOOP;
                }

                addToFunctionList(&func->head, functionBuilder, tempOp);
                printf("new part!\n");
            }

            /* Otherwise append to function to deal with in recursion*/
            else
                appendStr(functionBuilder, &c, 1);

            /* Clears negative, decimal, and exponent 
             * for next value after operation 
             */
            hasOperation = 1;
            hasNegative = 0;
            hasDecimal = 0;
            hasExponent = 0;
        }

        else if (c == '(') 
        {
            if (!isParenthesis)
            {
                parenthesisOpenIndex = i;
                isParenthesis = 1;

                if (i == 0 || strlen(functionBuilder) == 0)
                    appendStr(functionBuilder, &c, 1);

                else {
                    addToFunctionList(&func->head, functionBuilder, MUL);
                    appendStr(functionBuilder, &c, 1);
                } 
            } else
                appendStr(functionBuilder, &c, 1);

            if (hasOperation)
                hasOperation = 0;

            ++parenthesisBalance;
        }
 
        else if (c == ')') 
        {
            --parenthesisBalance;
 
            if (parenthesisBalance < 0 || hasOperation) 
            {
                printf(
                "* ERROR [parseFunction]:\n"
                "* Must proceed a number, variable, or parenthesis.\n"
                "*   Function: %s\n*   Index: %d\n", theFunction,i);
          
                return NULL;
            }

            else if (isFunction
                        && (functionParenthesisBalance == parenthesisBalance))
            {
                isFunction = 0;
                if (i != strlen(theFunction) - 1)
                {
                    switch(theFunction[i+1])
                    {
                        case '+': tempOp = ADD;
                        case '-': tempOp = SUB;
                        case '/': tempOp = DIV;
                        //what if there's this? (7x + 4)^8, how are you going to
                        //read the exponent?
                    }
                }
                // recursion to new entry
            }
 
        }

    

    } //end of forloop

    printf("%s\n", functionBuilder);
    return func;
}

