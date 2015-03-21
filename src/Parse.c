#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Parse.h"
#include "StringExtensions.h"

enum operations {NOOP, ADD, SUB, MUL, DIV};
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
    double exponent;
    union part_union {
        functionPart* parenthesis;
        poly poly;
        trig trig;
    } part;
    opType operation;
    functionPart* next;
};

trigType isTrigFunction(const char* firstLetter) {
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

function* parseFunction(const char* theFunction) {
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
    
    function* func = malloc(sizeof(function));
    function* head = func;

    functionPart* part = func->head;

    char* functionBuilder = malloc(0);
    char c;

    for (i = 0; i < strlen(theFunction); i++) {
        c = theFunction[i];
	printf("%c\n", c);

        /* Always appends numbers */
        if (isdigit(c)) {

            if (i == 0)
                append(functionBuilder, c);	

            else if (isalpha(theFunction[i-1]) || theFunction[i-1] == ')') {
                printf("* ERROR [parseFunction]:\n"
                       "* A number cannot proceed a letter or close parenthesis.\n");
                printf("*   Function: %s\n*   Index: %d\n", theFunction, i);
                return NULL;
            }

            else
                append(functionBuilder, c);       

            if (hasOperation)
                hasOperation = 0;
        }

        /* Always appends letters */
        else if (isalpha(c)) {

            if (index == 0)
                append(functionBuilder, c);

            /* If letters proceed make up existing function, add preceding
               function to equationList and create new function */
            else if (isTrigFunction(&theFunction[i])) {
                isFunction = 1;
                ++parenthesisBalance;
                functionParenthesisBalance = parenthesisBalance;
                part = malloc(sizeof(functionPart));
                part->str = malloc(sizeof(functionBuilder));
                strcpy(part->str, functionBuilder);
                
                freeString(functionBuilder);
                appendStr(functionBuilder, &theFunction[i], 4);

                i += 3;
                parenthesisOpenIndex = i;
          
                printf("recognizes sin(\n");
                printf("%s\n", part->str);

            }
        
        }

        
    }
    printf("before %s\n", functionBuilder);
    printf("%d\n", strlen(functionBuilder));
    removeChar(functionBuilder, 2);
    printf("after %s\n", functionBuilder);

    
}

