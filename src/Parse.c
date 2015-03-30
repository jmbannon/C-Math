/* 
 =======================================================================
 Parse.c

 Jesse Bannon
 
 Parses equations into a double linked list of functionParts. Each
 functionPart contains the following: 

 - String (of the part)
 - Union Type
   - Parenthesis (a pointer to head of the functionPart in parenthesis)
   - Numeric
   - Trig function
   - Log
 - Union Exponent
   - Numeric
   - Parenthesis (anything other than a numeric)
 - Operator (that proceeds the functionPart) 

 Will return null if there is a parsing error and print the
 problem and the index the problem occured at.
 =======================================================================
*/
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Parse.h"
#include "Function.h"
#include "StringExtensions.h"

function* parseFunction(
        const char* theFunction
) {
    int i; /* iterator */

    int parenthesisBalance = 0;

    int hasOperation = 0;
    int hasNegative = 0;
    int hasNumber = 0;
    int hasPrime = 0;
    int hasDecimal = 0;
    int hasExponent = 0;
    int hasVariable = 0;
 
    int isParenthesis = 0;
    int isFunction = 0;

    opType tempOp;
    
    function* func;
    func = initializeFunction(theFunction);

    char functionBuilder[1024] = {0};
    char c, tempChar;

    for (i = 0; i < strlen(theFunction); i++) 
    {
        c = theFunction[i];

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
                appendChar(functionBuilder, c);       

           hasOperation = 0;
        }

        /* LETTERS */
        else if (isalpha(c))  //should recognize log(, ln(, pi, e, L(), and !
        {

            // If letters proceed are a trig function, add preceding
            // function to equationList and create new function 
            if (isTrigFunction(&theFunction[i])) 
            {
                isFunction = 1;

                if (!hasOperation)
                {
                     addToFunctionList(getHead(func), functionBuilder, MUL);
                     //printf("New part!\n");
                }

                appendStr(functionBuilder, &theFunction[i], 3);
                i += 3;

                goto parseParenthesis;  // line 338

            }

            // Variable parse errors 
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

            // Assume it's a variable
            else
            {
                 if (hasVariable)
                 {
                     addToFunctionList(getHead(func), functionBuilder, MUL);
                     //printf("New Part!\n");
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
                 
                 // If letter precedes decimal, throw error. 
                 if (hasVariable) 
                 {
                     printf(
                     "* ERROR [parseFunction]:\n"
                     "* A decimal point cannot proceed a variable.\n"
                     "*   Function: %s\n*   Index: %d\n", theFunction, i);
                     
                     return NULL;
                 }

                 // If a number does not precede decimal, add 0 first
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
        else if (c == '-')        
        {                   
            if (!hasNegative) 
            {
                // If the function has numbers/variables and is not 
                // in parenthesis, act as operator.         
                if (strlen(functionBuilder) != 0 
                       && !isParenthesis 
                       && !isFunction 
                       && !hasOperation
                       && !hasExponent) 
                {
                    addToFunctionList(getHead(func), functionBuilder, SUB);
                    //printf("new part!\n");
                    hasOperation = 1;

                // Otherwise append to function
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

            // If negative sign is present
            else 
            {

                // If function already contains a negative and not in 
                // parenthesis string, throw exception 
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

                // If no parenthesis, act as subtraction operation: 
                // add function to list. 
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

                    addToFunctionList(getHead(func), functionBuilder, SUB);     
                    //printf("new part!\n");
                    hasOperation = 1;
                }

                // If within parenthesis, append to function to handle 
                // in recursion 
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
            parseExponent:  
                                                                       
            if (!hasExponent) 
            {
            
                // If exponent does not proceed number or variable, 
                // throw exception. 
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

                // Append to function and set exponent to true, negative 
                // and decimal to false for exponent number                 
                else 
                {
                    appendStr(functionBuilder, &theFunction[i], 1);
                    hasExponent = 1;
                    hasNegative = 0;
                    hasDecimal = 0;
                }

            // If exponent sign already exists, throw exception
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

            // If it is not in parenthesis or function, add 
            // function and operator to list 
            else if (!isParenthesis && !isFunction) 
            {
                switch(c) 
                {
                case '+': tempOp = ADD; break;
                case '*': tempOp = MUL; break;
                case '/': tempOp = DIV; break;
                default: tempOp = NOOP;
                }

                addToFunctionList(getHead(func), functionBuilder, tempOp);
                //printf("new part!\n");
            }

            // Otherwise append to function to deal with in recursion
            else
                appendStr(functionBuilder, &c, 1);

            // Clears negative, decimal, and exponent 
            // for next value after operation 
            hasOperation = 1;
            hasNegative = 0;
            hasDecimal = 0;
            hasExponent = 0;
            hasVariable = 0;
        }

        /* OPENING PARENTHESIS, NEEDS OVERHAUL */     
        else if (c == '(')                                               
        {                                                                
            
            // parseParenthesis: 
            // This goto handles anything that contains parenthesis.
            parseParenthesis:
            ++parenthesisBalance;
                                                                          
            if (!isParenthesis)
            {
                // (Function handles operations) 
                // Assume it's implicit multiplication
                printf("%d %d %d\n", hasOperation, isFunction, hasExponent);
                if (!hasOperation && !isFunction && !hasExponent 
                        && strlen(functionBuilder) != 0) 
                    addToFunctionList(getHead(func), functionBuilder, MUL);

                // Always appends opening parenthesis to 
                // appear in functionPart string
                appendStr(functionBuilder, &theFunction[i], 1);

                while (i < strlen(theFunction)-1)
                {
                    ++i;
                    if (theFunction[i] != ')' && theFunction[i] != '(') 
                        appendStr(functionBuilder, &theFunction[i], 1);
                    
                    else if (theFunction[i] == '(') {
                        ++parenthesisBalance;
                        appendStr(functionBuilder, &theFunction[i], 1);
                    } else 
                    {
                        --parenthesisBalance;
                        appendStr(functionBuilder, &theFunction[i], 1);
                        
                        if (parenthesisBalance == 0)
                        {
                            switch(theFunction[i+1]) 
                            {
                            case '+': tempOp = ADD; ++i; hasOperation = 1;
                                break; 
                            
                            case '-': tempOp = SUB; ++i; hasOperation = 1;
                                break; 

                            case '/': tempOp = DIV; ++i; hasOperation = 1;
                                break;

                            case '\0': tempOp = NOOP; 
                                break;

                            case '^':  isParenthesis = 0; isFunction = 0; 
                                       ++i; goto parseExponent;

                            default: tempOp = MUL; hasOperation = 1;
                                break; 
                            }
                            printf("the function builder = %s\n", functionBuilder);
                            addToFunctionList(getHead(func), 
                                              functionBuilder, 
                                              tempOp); 
                            break;
                        }  
                    }    
                }
            } else
                appendStr(functionBuilder, &c, 1);

            isFunction = 0;
            isParenthesis = 0;            
        }
 
        // Every close parenthesis should be handled 
        // in opening parenthesis parse
        else if (c == ')')                                                 
        {       
            printf(
            "* ERROR [parseFunction]:\n"
            "* Does not have an opening parenthesis.\n"
            "*   Function: %s\n*   Index: %d\n", theFunction,i);
          
            return NULL;
        }

    

    } //end of forloop

    //TODO
    // - End of parse errors
    // Append last function part

    printf("%s\n", functionBuilder);
    return func;
}

functionPart* parseFunctionPart(
        const char* functionStr
) {
    functionPart** funcPart;
    
    function* func = parseFunction(functionStr);
    funcPart = getHead(func);
    
    free(func);
    return *funcPart;
}

