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
#include "ErrorMessage.h"
#include "StringExtensions.h"
#include "Boolean.h"

static char * file_name = "Parse.c";
static char error_loc_buffer[1024];

/* Function prototypes */
void print_parse_error(
        const char * description,
        const char * function,
        int index
);

char * err_loc_msg(
        const char * function,
        int index
);

/** Parses the given function from a string into a sequential linked list of
  * FunctionParts with operators inbetween.  
  * WIP
  */
function * parseFunction(
        const char * theFunction
) {
    int i; /* iterator */
    int parenthesisBalance = 0;

    bool hasOperation = false;
    bool hasNegative = false;
    bool hasNumber = false;
    bool hasPrime = false;
    bool hasDecimal = false;
    bool hasExponent = false;
    bool hasVariable = false;
 
    bool isParenthesis = false;
    bool isFunction = false;

    opType tempOp;
    
    function * func = initializeFunction(theFunction);

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
                print_parse_error(
                    "Cannot have digit immediately after variable without operation",
                    theFunction, i); 
                return NULL;
            }
            else
                appendChar(functionBuilder, c);       

           hasOperation = false;
        }

        /* LETTERS */
        else if (isalpha(c))  //should recognize log(, ln(, pi, e, L(), and !
        {

            // If letters proceed are a trig function, add preceding
            // function to equationList and create new function 
            if (isTrigFunction(&theFunction[i])) 
            {
                isFunction = true;

                if (!hasOperation)
                {
                     addToFunctionList(func, functionBuilder, MUL);
                     //printf("New part!\n");
                }

                appendStr(functionBuilder, &theFunction[i], 3);
                i += 3;

                goto parseParenthesis;  // line 338

            }

            // Variable parse errors 
            else if (theFunction[i-1] == '.') 
            {
                print_parse_error(
                    "Invalid variable. Cannot proceed non-numeric symbol '.'",
                    theFunction, i); 
                return NULL;
            }
            
            // Append and deal with later if it's in parenthesis or function
            else if (isParenthesis || isFunction)
                appendStr(functionBuilder, &c, 1);

            // Assume it's a variable
            else
            {
                 if (functionBuilder[0] != '\0')
                 {
                     addToFunctionList(func, functionBuilder, MUL);
                     //printf("New Part!\n");
                 }
                     
                 appendStr(functionBuilder, &c, 1);
                 hasVariable = true;
            }

            hasOperation = false;
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
                     print_parse_error(
                        "A decimal point cannot proceed a variable.",
                        theFunction, i); 
                     return NULL;
                 }

                 // If a number does not precede decimal, add 0 first
                 else if (i == 0 || !isdigit(theFunction[i-1]))
                     appendStr(functionBuilder, &tempChar, 1);

                 /* Append and set hasDecimal to true. */
                 appendStr(functionBuilder, &c, 1);
                 hasDecimal = true;
            }
            else 
            {
                print_parse_error(
                    "Numeric already contains a decimal.",
                    theFunction, i); 
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
                    addToFunctionList(func, functionBuilder, SUB);
                    //printf("new part!\n");
                    hasOperation = true;
                    hasNegative = false;
                    hasDecimal = false;
                    hasExponent = false;
                    hasVariable = false;

                // Otherwise append to function
                } 
                else 
                {
                    if (i != 0 && theFunction[i-1] == '.')
                    {
                        print_parse_error(
                            "Cannot subtract a decimal.",
                            theFunction, i); 
                        return NULL;
                    }
                    else 
                    {
                        appendStr(functionBuilder, &c, 1);
                        hasNegative = true;
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
                    print_parse_error(
                        "A subtraction operation already exists.",
                        theFunction, i); 
                    return NULL;
                }

                // If no parenthesis, act as subtraction operation: 
                // add function to list. 
                if (!isParenthesis && !isFunction) 
                {
                    if (hasOperation) 
                    {
                        print_parse_error(
                            "A subtraction operator already exists",
                            theFunction, i);
                        
                        return NULL;
                    }

                    addToFunctionList(func, functionBuilder, SUB);     
                    //printf("new part!\n");
                    hasOperation = true;
                }

                // If within parenthesis, append to function to handle 
                // in recursion 
                else
                    appendStr(functionBuilder, &c, 1);

                hasNegative = false;
                hasDecimal = false;
                hasExponent = false;
                hasVariable = false;
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
                    print_parse_error(
                        "Must proceed a number, variable, or parenthesis.",
                        theFunction, i);

                    return NULL;
                }

                // Append to function and set exponent to true, negative 
                // and decimal to false for exponent number                 
                else 
                {
                    appendStr(functionBuilder, &theFunction[i], 1);
                    hasExponent = true;
                    hasNegative = false;
                    hasDecimal = false;
                    hasVariable = false;
                }

            // If exponent sign already exists, throw exception
            }
            else 
            {
                print_parse_error(
                    "Function already contains an exponent.",
                    theFunction, i);
                
                return NULL;
            }
        }

        /* OPERATORS (BESIDES SUBTRACTION) */
        else if (c == '+' || c == '*' || c == '/') 
        {    
            if (i == 0 || hasOperation) 
            {
                print_parse_error(
                    "Must proceed a number, variable, or parenthesis.",
                    theFunction, i);

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

                addToFunctionList(func, functionBuilder, tempOp);
                //printf("new part!\n");
            }

            // Otherwise append to function to deal with in recursion
            else
                appendStr(functionBuilder, &c, 1);

            // Clears negative, decimal, and exponent 
            // for next value after operation 
            hasOperation = true;
            hasNegative = false;
            hasDecimal = false;
            hasExponent = false;
            hasVariable = false;
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
                    addToFunctionList(func, functionBuilder, MUL);

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
                    } 
                    else 
                    {
                        --parenthesisBalance;
                        appendStr(functionBuilder, &theFunction[i], 1);
                        
                        if (parenthesisBalance == 0)
                        {
                            switch(theFunction[i+1]) 
                            {
                            case '+': tempOp = ADD; ++i; hasOperation = true;
                                break; 
                            
                            case '-': tempOp = SUB; ++i; hasOperation = true;
                                break; 

                            case '/': tempOp = DIV; ++i; hasOperation = true;
                                break;

                            case '\0': tempOp = NOOP; hasOperation = false;
                                break;

                            case '^':  isParenthesis = false; 
                                       isFunction = false; 
                                       ++i; goto parseExponent;

                            default: tempOp = MUL; hasOperation = 1;
                                break; 
                            }
                            printf("the function builder = %s\n", functionBuilder);
                            addToFunctionList(func, 
                                              functionBuilder, 
                                              tempOp); 

                            hasNegative = false;
                            hasDecimal = false;
                            hasExponent = false;
                            hasVariable = false;
                            break;
                        }  
                    }    
                }
            } else
                appendStr(functionBuilder, &c, 1);

            isFunction = false;
            isParenthesis = false;           
        }
 
        // Every close parenthesis should be handled 
        // in opening parenthesis parse
        else if (c == ')')                                                 
        {       
            print_parse_error(
                "Does not have an opening parenthesis.",
                theFunction, i);
            return NULL;
        }

    

    } //end of forloop

    //TODO
    // - End of parse errors
    // Append last function part

    if (functionBuilder[0] != '\0')
        addToFunctionList(func, 
                          functionBuilder, 
                          NOOP); 

    if (hasOperation)
    {
        print_parse_error(
            "Last operator does not operate on anything.\n",
            theFunction, -1);
        return NULL;
    }

    if (parenthesisBalance != 0)
    {
        print_parse_error(
            "Parenthesis do not balance.",
            theFunction, -1);
        return NULL;
    }

    printf("%s\n", functionBuilder);
    return func;
}

functionPart * parseFunctionPart(
        const char * functionStr
) {
    functionPart * funcPart;
    
    function * func = parseFunction(functionStr);
    funcPart = getHead(func);
    
    free(func);
    return funcPart;
}

void print_parse_error(
        const char * description,
        const char * function,
        int index
) {
    parse_error(
        file_name,
        "parse_function",
        description,
        err_loc_msg(function, index)); 
}

/* Returns the string of the current function and the index the error
 * was thrown.
 */ 
char * err_loc_msg(
        const char * function,
        int index
) {
    if (index >= 0)
        sprintf(error_loc_buffer, 
            "Function: %s : Index: %d", function, index);
    else
        sprintf(error_loc_buffer, 
            "  Function: %s : Index: end of function", function); 
    return error_loc_buffer;
}
