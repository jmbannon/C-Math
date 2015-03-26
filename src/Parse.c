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

void printInfo(function* theFunction) {
    if (!theFunction)
        return;

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
    int hasVariable = 0;
 
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

            if (hasVariable) 
            {
                printf(
                "* ERROR [parseFunction]:\n"
                "* A number cannot proceed a letter\n"
                "*   Function: %s\n*   Index: %d\n", theFunction, i);

                return NULL;
            }

            else
                append(functionBuilder, c);       

           hasOperation = 0;
        }

        /* LETTERS */
        else if (isalpha(c)) 
        {

            /* If letters proceed are a trig function, add preceding
             * function to equationList and create new function 
             */
            if (isTrigFunction(&theFunction[i])) 
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

            /* Variable parse errors */
            else if (theFunction[i-1] == '.') 
            {
                printf(
                "* ERROR [parseFunction]:\n"
                "* Invalid variable. Cannot proceed non-numeric symbols.\n"
                "* Fix later for variable multiplication\n"
                "*   Function: %s\n*   Index: %d\n", theFunction, i);
                return NULL;
            }

            else if (isParenthesis || isFunction)
                appendStr(functionBuilder, &c, 1);

            /* Assume it's a variable */
            else
            {
                 if (hasVariable)
                 {
                     addToFunctionList(&func->head, functionBuilder, MUL);
                     printf("New Part!\n");
                 } else 
                     hasVariable = 1;

                 appendStr(functionBuilder, &c, 1);
            }

            hasOperation = 0;
        }

        /* DECIMAL POINT */
        else if (c == '.') 
        {
            if (!hasDecimal) 
            {
                 tempChar = '0';
                 
                 /* If letter precedes decimal, throw error. */
                 if (hasVariable) 
                 {
                     printf(
                     "* ERROR [parseFunction]:\n"
                     "* A decimal point cannot proceed a variable.\n"
                     "*   Function: %s\n*   Index: %d\n", theFunction, i);
                     
                     return NULL;
                 }

                 /* If a number does not precede decimal, add 0 first. */
                 else if (i == 0 || !isdigit(theFunction[i-1]))
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
        else if (c == '-')                                                        //If a negative sign is not present and there is a variable, then it is subtraction
        {                                                                         //else, if there is a variable, it's an error
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
                    if (i != 0 && theFunction[i-1] == '.') {
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

        /* EXPONENT NEEDS REWORK!!!!!!!!!! */                                         // Since exponent is a pointer of every functionPart, it must
        else if (c == '^')                                                            // find the most recent functionPart and determine whether it is a
        {                                                                             // number or other (should be in parenthesis and be classified as functionPart).
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
            hasVariable = 0;
        }

        /* OPENING PARENTHESIS, NEEDS OVERHAUL */                         //Should check to see if there is an operator, if not add to functionBuilder as MUL
        else if (c == '(')                                                //Should iterate through entire string adding characters until closing parenthesis is met,
        {                                                                 //then recursion parseFunction and save functionPart head to parenthesis part.
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
 
        else if (c == ')')                                                        //Should always be acknowledged in '(' to close parenthesis, if it is found in function
        {                                                                         //string then it is either a recursive (isFunction/isParenthesis) or an error.
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

functionPart* parseFunctionPart(const char* functionStr) {
    functionPart* funcPart;
    function* func = malloc(sizeof(function));
    
    func = parseFunction(functionStr);
    funcPart = func->head;
    
    free(func);
    return funcPart;
}

