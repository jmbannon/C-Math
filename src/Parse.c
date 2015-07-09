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
        const char * theFunction,
        var * root_var_list
) {
    int i; /* iterator */
    int par_bal = 0;

    bool has_op  = false;
    bool has_neg = false;
    bool has_num = false;
    bool has_dec = false;
    bool has_exp = false;
    bool has_var = false;
    bool is_par  = false;
    bool is_func = false;

    op_type temp_op;
    
    function * func = initializeFunction(theFunction);
    if (root_var_list != NULL)
        set_var_list(func, root_var_list);

    char func_builder[1024] = {0};
    char c, temp_char;

    for (i = 0; i < strlen(theFunction); i++) 
    {
        c = theFunction[i];

        /* NUMBERS */
        if (isdigit(c)) 
        {
            if (has_var) 
            {
                print_parse_error(
                    "Cannot have digit immediately after variable without operation",
                    theFunction, i); 
                return NULL;
            }
            else
                appendChar(func_builder, c);       

           has_op = false;
        }

        /* LETTERS */
        else if (isalpha(c))  //should recognize log(, ln(, pi, e, L(), and !
        {

            // If letters proceed are a trig function, add preceding
            // function to equationList and create new function 
            if (isTrigFunction(&theFunction[i])) 
            {
                is_func = true;

                if (!has_op)
                {
                     addToFunctionList(func, func_builder, MUL);
                     //printf("New part!\n");
                }

                appendStr(func_builder, &theFunction[i], 3);
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
            else if (is_par || is_func)
                appendStr(func_builder, &c, 1);

            // Assume it's a variable
            else
            {
                 if (func_builder[0] != '\0')
                 {
                     addToFunctionList(func, func_builder, MUL);
                     //printf("New Part!\n");
                 }
                     
                 appendStr(func_builder, &c, 1);
                 has_var = true;
            }

            has_op = false;
        }

        /* DECIMAL POINT */
        else if (c == '.') 
        {
            if (!has_dec) 
            {
                 temp_char = '0';
                 
                 // If letter precedes decimal, throw error. 
                 if (has_var) 
                 {
                     print_parse_error(
                        "A decimal point cannot proceed a variable.",
                        theFunction, i); 
                     return NULL;
                 }

                 // If a number does not precede decimal, add 0 first
                 else if (i == 0 || !isdigit(theFunction[i-1]))
                     appendStr(func_builder, &temp_char, 1);

                 /* Append and set has_dec to true. */
                 appendStr(func_builder, &c, 1);
                 has_dec = true;
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
            if (!has_neg) 
            {
                // If the function has numbers/variables and is not 
                // in parenthesis, act as operator.         
                if (strlen(func_builder) != 0 
                       && !is_par 
                       && !is_func 
                       && !has_op
                       && !has_exp) 
                {
                    addToFunctionList(func, func_builder, SUB);
                    //printf("new part!\n");
                    has_op = true;
                    has_neg = false;
                    has_dec = false;
                    has_exp = false;
                    has_var = false;

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
                        appendStr(func_builder, &c, 1);
                        has_neg = true;
                    }
                }                
            }

            // If negative sign is present
            else 
            {
                // If function already contains a negative and not in 
                // parenthesis string, throw exception 
                if (strcmp(func_builder, "-") == 0 
                        && !is_par 
                        && !is_func) 
                {
                    print_parse_error(
                        "A subtraction operation already exists.",
                        theFunction, i); 
                    return NULL;
                }

                // If no parenthesis, act as subtraction operation: 
                // add function to list. 
                if (!is_par && !is_func) 
                {
                    if (has_op) 
                    {
                        print_parse_error(
                            "A subtraction operator already exists",
                            theFunction, i);
                        
                        return NULL;
                    }

                    addToFunctionList(func, func_builder, SUB);     
                    //printf("new part!\n");
                    has_op = true;
                }

                // If within parenthesis, append to function to handle 
                // in recursion 
                else
                    appendStr(func_builder, &c, 1);

                has_neg = false;
                has_dec = false;
                has_exp = false;
                has_var = false;
            }
        }

        /* EXPONENT */                                        
        else if (c == '^')                                                
        {           
            parseExponent:  
            if (!has_exp) 
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
                    appendStr(func_builder, &theFunction[i], 1);
                    has_exp = true;
                    has_neg = false;
                    has_dec = false;
                    has_var = false;
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
            if (i == 0 || has_op) 
            {
                print_parse_error(
                    "Must proceed a number, variable, or parenthesis.",
                    theFunction, i);

                return NULL;
            }

            // If it is not in parenthesis or function, add 
            // function and operator to list 
            else if (!is_par && !is_func) 
            {
                switch(c) 
                {
                case '+': temp_op = ADD; break;
                case '*': temp_op = MUL; break;
                case '/': temp_op = DIV; break;
                default: temp_op = NOOP;
                }

                addToFunctionList(func, func_builder, temp_op);
                //printf("new part!\n");
            }

            // Otherwise append to function to deal with in recursion
            else
                appendStr(func_builder, &c, 1);

            // Clears negative, decimal, and exponent 
            // for next value after operation 
            has_op = true;
            has_neg = false;
            has_dec = false;
            has_exp = false;
            has_var = false;
        }

        /* OPENING PARENTHESIS, NEEDS OVERHAUL */     
        else if (c == '(')                                               
        {                                                                
            
            // parseParenthesis: 
            // This goto handles anything that contains parenthesis.
            parseParenthesis:
            ++par_bal;
                                                                          
            if (!is_par)
            {
                // (Function handles operations) 
                // Assume it's implicit multiplication
                printf("%d %d %d\n", has_op, is_func, has_exp);
                if (!has_op && !is_func && !has_exp 
                        && strlen(func_builder) != 0) 
                    addToFunctionList(func, func_builder, MUL);

                // Always appends opening parenthesis to 
                // appear in functionPart string
                appendStr(func_builder, &theFunction[i], 1);

                while (i < strlen(theFunction)-1)
                {
                    ++i;
                    if (theFunction[i] != ')' && theFunction[i] != '(') 
                        appendStr(func_builder, &theFunction[i], 1);
                    
                    else if (theFunction[i] == '(') {
                        ++par_bal;
                        appendStr(func_builder, &theFunction[i], 1);
                    } 
                    else 
                    {
                        --par_bal;
                        appendStr(func_builder, &theFunction[i], 1);
                        
                        if (par_bal == 0)
                        {
                            switch(theFunction[i+1]) 
                            {
                            case '+': temp_op = ADD; ++i; has_op = true;
                                break; 
                            
                            case '-': temp_op = SUB; ++i; has_op = true;
                                break; 

                            case '/': temp_op = DIV; ++i; has_op = true;
                                break;

                            case '\0': temp_op = NOOP; has_op = false;
                                break;

                            case '^':  is_par = false; 
                                       is_func = false; 
                                       ++i; goto parseExponent;

                            default: temp_op = MUL; has_op = 1;
                                break; 
                            }
                            printf("the function builder = %s\n", func_builder);
                            addToFunctionList(func, 
                                              func_builder, 
                                              temp_op); 

                            has_neg = false;
                            has_dec = false;
                            has_exp = false;
                            has_var = false;
                            break;
                        }  
                    }    
                }
            } else
                appendStr(func_builder, &c, 1);

            is_func = false;
            is_par = false;           
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

    if (func_builder[0] != '\0')
        addToFunctionList(func, 
                          func_builder, 
                          NOOP); 

    if (has_op)
    {
        print_parse_error(
            "Last operator does not operate on anything.\n",
            theFunction, -1);
        return NULL;
    }

    if (par_bal != 0)
    {
        print_parse_error(
            "Parenthesis do not balance.",
            theFunction, -1);
        return NULL;
    }

    printf("%s\n", func_builder);
    return func;
}

functionPart * parseFunctionPart(
        const char * functionStr,
        var * root_var_list
) {
    functionPart * funcPart;
    
    function * func = parseFunction(functionStr, root_var_list);
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
