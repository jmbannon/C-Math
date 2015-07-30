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
#include "ParseUtilities.h"
#include "../Function.h"
#include "../../utilities/ErrorMessage.h"
#include "../../utilities/StringExtensions.h"
#include "../../utilities/Boolean.h"

static char * file_name = "Parse.c";
static char   error_loc_buffer[1024];

/* Function prototypes */
bool parse_digit(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS
);       

bool parse_factorial(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS
);

bool parse_alpha(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
);

bool parse_parenthesis(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
);

bool parse_exponent(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
);

bool parse_decimal(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
);

bool parse_dash(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
);

bool parse_operator(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
);


/** Parses the given function from a string into a sequential linked list of
  * FunctionParts with operators inbetween.  
  * WIP
  */
function * parseFunction(
        const char * func_str,
        function   * root_func
) {
    unsigned int i;
    unsigned int par_bal = 0;
    unsigned int B_BOOLS = 0;
    char func_builder[1024] = {0};
    char c;
    bool pc = true;
    op_type   temp_op;
    part_type temp_part; 
    part_type exp_part;
    function * func = initializeFunction(func_str);

    if (root_func == NULL)
        root_func = func;
    
    func->root_func = root_func;
    func->var_list = root_func->var_list;

    for (i = 0; i < strlen(func_str); i++) 
    {
        c = func_str[i];
        if (isdigit(c))
            pc = parse_digit(func, &i, func_builder, &B_BOOLS);

        else if (c == '!')
            pc = parse_factorial(func, &i, func_builder, &B_BOOLS);
 
        else if (isalpha(c)) //should recognize log(, ln(, pi, e, L(), and !
            pc = parse_alpha(func, &i, func_builder, &B_BOOLS, &temp_part);
        
        else if (c == '.') 
            pc = parse_decimal(func, &i, func_builder, &B_BOOLS, &temp_part);

        else if (c == '-')        
            pc = parse_dash(func, &i, func_builder, &B_BOOLS, &temp_part);

        else if (c == '^')
            pc = parse_exponent(func, &i, func_builder, &B_BOOLS, &temp_part);

        else if (c == '+' || c == '*' || c == '/') 
            pc = parse_operator(func, &i, func_builder, &B_BOOLS, &temp_part);

        else if (c == '(')                                               
            pc = parse_parenthesis(func, &i, func_builder, &B_BOOLS, &temp_part);

        else if (c == ')')                                                 
        {       
            print_parse_error(
                "Does not have an opening parenthesis.",
                file_name, func_str, error_loc_buffer, i);
            return NULL;
        } 
        else
        {
            print_parse_error(
                "Unknown character to parse.",
                file_name, func_str, error_loc_buffer, i);
            pc = false;
        }

        if (!pc)
            return NULL;
    }

    if (func_builder[0] != '\0')
    {
        addToFunctionList(func, 
                          func_builder,
                          get_part(B_BOOLS, temp_part),
                          NOOP);
        B_BOOLS = 0;
    }

    if (CHECK(B_BOOLS, HAS_OP))
    {
        print_parse_error(
            "Last operator does not operate on anything.\n",
            file_name, func_str, error_loc_buffer, -1);
        return NULL;
    }
    return func;
}

bool parse_digit(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS
) {
    char * func_str = func->str;
    if (CHECK(*B_BOOLS, HAS_VAR) && !CHECK(*B_BOOLS, HAS_EXP)) 
    {
       print_parse_error(
           "Cannot have digit immediately after variable without operation",
           file_name, func_str, error_loc_buffer, *i); 
       return false;
    }
    appendChar(func_builder, func_str[*i]);     
    SET_TRUE (*B_BOOLS, HAS_NUM);
    SET_FALSE(*B_BOOLS, HAS_OP);
    return true;
}

bool parse_factorial(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS
) {
    char * func_str = func->str;
    if (*i == 0 || 
       (!isdigit(func_str[*i-1]) &&
        !isalpha(func_str[*i-1])))
    {
        print_parse_error( 
            "Factorial must follow a value",
            file_name, func_str, error_loc_buffer, *i);
        return false;
    }
    else
       appendStr(func_builder, &func_str[*i], 1);
    return true;
}

bool parse_alpha(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
) {
    char * func_str = func->str;
    if (*i > 0 && func_str[*i-1] == '.') 
    {
        print_parse_error(
            "Invalid variable. Cannot proceed non-numeric symbol '.'",
            file_name, func_str, error_loc_buffer, *i); 
        return false;
    }
    // If letters proceed are a trig function, add preceding
    // function to equationList and create new function 
    else if ((*temp_part = is_func_type(&func_str[*i]))) 
    {
        if (*i != 0 && 
            !CHECK(*B_BOOLS, HAS_OP) &&
            !CHECK(*B_BOOLS, HAS_EXP))
        {
             addToFunctionList(func,
                               func_builder,
                               get_part(*B_BOOLS, *temp_part),
                               MUL);
             *B_BOOLS = 0;
        }
        SET_TRUE(*B_BOOLS, IS_FUN);
        appendStr(func_builder, &func_str[*i], 3);
        *i += 3;
        return parse_parenthesis(func, i, func_builder, B_BOOLS, temp_part);
    }
    // Exponent contains a variable -> assume that's all in exponent
    else if (*i > 0 && CHECK(*B_BOOLS, HAS_EXP) && func_str[*i-1] == '^')
    {
        appendStr(func_builder, &func_str[*i], 1);
        if (func_str[*i+1] == '!')
            appendStr(func_builder, &func_str[(*i)++ + 1], 1);

        addToFunctionList(func,
                          func_builder,
                          get_part(*B_BOOLS, *temp_part),
                          get_op(func_str[*i+1]));
        *B_BOOLS = 0;
    }
    // Assume it's a variable
    else
    {
         if (func_builder[0] != '\0')
         {
             addToFunctionList(func,
                               func_builder,
                               get_part(*B_BOOLS, *temp_part),
                               MUL);
             *B_BOOLS = 0;
         }
         appendStr(func_builder, &func_str[*i], 1);
         SET_TRUE(*B_BOOLS, HAS_VAR);
    }
    return true;
}

bool parse_parenthesis(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
) {
    char * func_str = func->str;
    int par_bal = 1;
    if (!CHECK(*B_BOOLS, IS_PAR))
    {
        // (Function handles operations) 
        // Assume it's implicit multiplication
        SET_TRUE(*B_BOOLS, IS_PAR);
        if (!CHECK(*B_BOOLS, HAS_OP)  &&
            !CHECK(*B_BOOLS, IS_FUN)  &&
            !CHECK(*B_BOOLS, HAS_EXP) &&
             strlen(func_builder) != 0)
        {
            addToFunctionList(func,
                              func_builder,
                              get_part(*B_BOOLS, *temp_part),
                              MUL);
        }

        // Always appends opening parenthesis to 
        // appear in functionPart string
        appendStr(func_builder, &func_str[*i], 1);

        while (*i < strlen(func_str)-1)
        {
            (*i)++;
            if (func_str[*i] != ')' && func_str[*i] != '(') 
                appendStr(func_builder, &func_str[*i], 1);
            
            else if (func_str[*i] == '(') {
                ++par_bal;
                appendStr(func_builder, &func_str[*i], 1);
            } 
            else 
            {
                --par_bal;
                appendStr(func_builder, &func_str[*i], 1);
                
                if (par_bal == 0)
                {
                    if (func_str[*i+1] == '!')
                        appendStr(func_builder, &func_str[(*i)++ + 1], 1);

                    op_type temp_op;
                    switch(func_str[*i+1]) 
                    {
                    case '+': temp_op = ADD; ++(*i);
                              SET_TRUE(*B_BOOLS, HAS_OP);
                              break;

                    case '-': temp_op = SUB; ++(*i);
                              SET_TRUE(*B_BOOLS, HAS_OP);
                              break;

                    case '/': temp_op = DIV; ++(*i);
                              SET_TRUE(*B_BOOLS, HAS_OP);
                              break;

                    case '\0': temp_op = NOOP;
                               SET_FALSE(*B_BOOLS, HAS_OP);
                               break;

                    case '^':  SET_FALSE(*B_BOOLS, IS_PAR);
                               SET_FALSE(*B_BOOLS, IS_FUN);
                               ++(*i);
                               return parse_exponent(func, i, func_builder, B_BOOLS, temp_part);

                    default: temp_op = MUL;
                             SET_TRUE(*B_BOOLS, HAS_OP);
                             break; 
                    }
                    addToFunctionList(func, 
                                      func_builder,
                                      get_part(*B_BOOLS, *temp_part),
                                      temp_op);
                    *B_BOOLS = 0;
                    break;
                }  
            }    
        }
    } else
        appendStr(func_builder, &func_str[*i], 1);

    SET_FALSE(*B_BOOLS, IS_FUN);
    SET_FALSE(*B_BOOLS, IS_PAR);         
    return true;
}

bool parse_exponent(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
) {
    char * func_str = func->str;
    if (!CHECK(*B_BOOLS, HAS_EXP)) 
    {
        // If exponent does not proceed number or variable, 
        // throw exception. 
        if (*i == 0 ||
            (!isdigit(func_str[*i-1]) &&
             !isalpha(func_str[*i-1]) &&
              func_str[*i-1] != ')')) 
        {
            print_parse_error(
                "Exponent must proceed a number, variable, or parenthesis.",
                file_name, func_str, error_loc_buffer, *i);
            return false;
        }
        // Append to function and set exponent to true, negative 
        // and decimal to false for exponent number                 
        else 
        {
            part_type exp_part;
            appendStr(func_builder, &func_str[*i], 1);
            SET_TRUE (*B_BOOLS, HAS_EXP);
            SET_FALSE(*B_BOOLS, HAS_NEG);
            SET_FALSE(*B_BOOLS, HAS_DEC);

            if (isdigit(func_str[*i+1]) ||
                    func_str[*i+1] == '.' ||
                    func_str[*i+1] == '-')
                exp_part = NUM;
            else if (isalpha(func_str[*i+1]))
            {
                if (is_func_type(&func_str[*i+1]))
                    exp_part = is_func_type(&func_str[*i+1]);
                else
                    exp_part = VAR;
            }
            else if (func_str[*i+1] == '(')
                exp_part = PAR;
            else
            {
                print_parse_error(
                    "Exponent must preceed a number, variable, or parenthesis.",
                    file_name, func_str, error_loc_buffer, *i+1);
                return false;
            }
        }

    // If exponent sign already exists, throw exception
    }
    else 
    {
        print_parse_error(
            "Function already contains an exponent.",
            file_name, func_str, error_loc_buffer, *i);
        return false;
    }
    return true;
}

bool parse_decimal(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
) {
    char * func_str = func->str;
    if (!CHECK(*B_BOOLS, HAS_DEC)) 
    {
         // If letter precedes decimal, throw error. 
         if (isalpha(func_str[*i-1])) 
         {
             print_parse_error(
                "A decimal point cannot proceed a variable.",
                file_name, func_str, error_loc_buffer, *i); 
             return false;
         }

         // If a number does not precede decimal, add 0 first
         else if (*i == 0 || !isdigit(func_str[*i-1]))
         {
             appendStr(func_builder, "0", 1);
             SET_TRUE(*B_BOOLS, HAS_NUM);
         }
         /* Append and set has_dec to true. */
         appendStr(func_builder, &func_str[*i], 1);
         SET_TRUE(*B_BOOLS, HAS_DEC);
    }
    else 
    {
        print_parse_error(
            "Numeric already contains a decimal.",
            file_name, func_str, error_loc_buffer, *i); 
        return false;
    }
    return true;
}

bool parse_dash(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
) {
    char * func_str = func->str;

    if (!CHECK(*B_BOOLS, HAS_NEG)) 
    {
        // If the function has numbers/variables, act as subtraction
        if (strlen(func_builder) != 0 
               && !CHECK(*B_BOOLS, HAS_OP)
               && !CHECK(*B_BOOLS, HAS_EXP)) 
        {
            addToFunctionList(func,
                              func_builder,
                              get_part(*B_BOOLS, *temp_part),
                              SUB);
            *B_BOOLS = 0;
            SET_TRUE(*B_BOOLS, HAS_OP);

        // Otherwise act as negative symbol and append to buffer
        } 
        else 
        {
            if (*i != 0 && func_str[*i-1] == '.')
            {
                print_parse_error(
                    "Cannot subtract a decimal.",
                    file_name, func_str, error_loc_buffer, *i); 
                return false;
            }
            appendStr(func_builder, &func_str[*i], 1);
            SET_TRUE(*B_BOOLS, HAS_NEG);
        }                
    }
    // If negative sign is present
    else 
    {
        // If function already contains a negative
        if (strncmp(func_builder, "-", 1) == 0) 
        {
            print_parse_error(
                "A subtraction operation already exists.",
                file_name, func_str, error_loc_buffer, *i); 
            return false;
        }
        else if (CHECK(*B_BOOLS, HAS_OP)) 
        {
            print_parse_error(
                "An operator already exists",
                file_name, func_str, error_loc_buffer, *i);
            return false;
        }
        addToFunctionList(func,
                          func_builder,
                          get_part(*B_BOOLS, *temp_part),
                          SUB); 
        *B_BOOLS = 0;    
        SET_TRUE(*B_BOOLS, HAS_OP);
    }
    return true;
}

bool parse_operator(
        function     * func,
        unsigned int * i,
        char         * func_builder,
        unsigned int * B_BOOLS,
        part_type    * temp_part
) {
    char * func_str = func->str;
    op_type temp_op;

    if (*i == 0 || CHECK(*B_BOOLS, HAS_OP))
    {
        print_parse_error(
            "Operation must proceed a number, variable, or parenthesis.",
            file_name, func_str, error_loc_buffer, *i);
        return false;
    }

    switch(func_str[*i]) 
    {
        case '+': temp_op = ADD; break;
        case '*': temp_op = MUL; break;
        case '/': temp_op = DIV; break;
        default:  temp_op = NOOP;
    }
    addToFunctionList(func,
                      func_builder,
                      get_part(*B_BOOLS, *temp_part),
                      temp_op);
    *B_BOOLS = 0;
    SET_TRUE(*B_BOOLS, HAS_OP);
    return true;
}

functionPart * parseFunctionPart(
        const char * functionStr,
        function   * root_func
) {
    functionPart * funcPart;
    
    function * func = parseFunction(functionStr, root_func);
    funcPart = func->head;
    
    free(func);
    return funcPart;
}

