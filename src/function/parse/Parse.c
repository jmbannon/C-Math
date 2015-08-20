/* 
 =======================================================================
 Parse.c

 Jesse Bannon
 
 Parses equations into a double linked list of parts. Each
 part contains the following: 

 - String (of the part)
 - Union Type
   - Parenthesis (a pointer to head of the part in parenthesis)
   - Numeric
   - Trig function
   - Log
 - Union Exponent
   - Numeric
   - Parenthesis (anything other than a numeric)
 - Operator (that proceeds the part) 

 Will return null if there is a parsing error and print the
 problem and the index the problem occured at.
 =======================================================================
*/
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "Parse.h"
#include "ParseUtilities.h"
#include "../Function.h"
#include "../../utilities/ErrorMessage.h"
#include "../../utilities/StringExtensions.h"

static char * file_name = "Parse.c";
static char   error_loc_buffer[1024];

/* Function prototypes */
function * parse_function(
  const char     * func_str,
        function * root_func
);

bool parse_digit(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags
);       

bool parse_factorial(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags
);

bool parse_alpha(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
);

bool parse_parenthesis(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
);

bool parse_exponent(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
);

bool parse_decimal(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
);

bool parse_dash(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
);

bool parse_operator(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
);

void clear_buffers(
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_part,
        part_type    * exp_type
);

function * parse(
  const char * func_str
) {
    return parse_function(func_str, NULL);
}

/** Parses the given function from a string into a sequential linked list of
  * FunctionParts with operators inbetween.  
  * WIP
  */
function * parse_function(
  const char     * func_str,
        function * root_func
) {
    unsigned int i;
    unsigned int par_bal = 0;
    unsigned int pflags = 0;
    char part_buffer[1024] = {0};
    char c;
    bool pc = true;
    op   temp_op   = NOOP;
    part_type base_type = NOPART; 
    part_type exp_type  = NOPART;
    function * func = initializeFunction(func_str);

    if (root_func == NULL)
        root_func = func;
    
    func->root_func = root_func;
    func->var_list = root_func->var_list;

    for (i = 0; i < strlen(func_str); i++) 
    {
        c = func_str[i];
        if (isdigit(c))
            pc = parse_digit(func, &i, part_buffer, &pflags);

        else if (c == '!')
            pc = parse_factorial(func, &i, part_buffer, &pflags);
 
        else if (isalpha(c)) //should recognize log(, ln(, pi, e, L(), and !
            pc = parse_alpha(func, &i, part_buffer, &pflags, &base_type, &exp_type);
        
        else if (c == '.') 
            pc = parse_decimal(func, &i, part_buffer, &pflags, &base_type, &exp_type);

        else if (c == '-')        
            pc = parse_dash(func, &i, part_buffer, &pflags, &base_type, &exp_type);

        else if (c == '^')
            pc = parse_exponent(func, &i, part_buffer, &pflags, &base_type, &exp_type);

        else if (c == '+' || c == '*' || c == '/') 
            pc = parse_operator(func, &i, part_buffer, &pflags, &base_type, &exp_type);

        else if (c == '(')                                               
            pc = parse_parenthesis(func, &i, part_buffer, &pflags, &base_type, &exp_type);

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

    if (part_buffer[0] != '\0')
    {
        addToFunctionList(func, 
                          part_buffer,
                          get_part(pflags, base_type),
                          exp_type,
                          NOOP);
        clear_buffers(part_buffer, &pflags, &base_type, &exp_type);
    }

    if (CHECK(pflags, HAS_OP))
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
        char         * part_buffer,
        unsigned int * pflags
) {
    char * func_str = func->str;
    if (CHECK(*pflags, HAS_VAR) && !CHECK(*pflags, HAS_EXP)) 
    {
       print_parse_error(
           "Cannot have digit immediately after variable without operation",
           file_name, func_str, error_loc_buffer, *i); 
       return false;
    }
    append_char(part_buffer, func_str[*i]);     
    SET_TRUE (*pflags, HAS_NUM);
    SET_FALSE(*pflags, HAS_OP);
    return true;
}

bool parse_factorial(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags
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
       append_str(part_buffer, &func_str[*i], 1);
    return true;
}

bool parse_alpha(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
) {
    part_type fun_part;
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
    else if ((fun_part = is_func_type(&func_str[*i]))) 
    {
        if (*i != 0 && 
            !CHECK(*pflags, HAS_OP) &&
            !CHECK(*pflags, HAS_EXP))
        {
             addToFunctionList(func,
                               part_buffer,
                               get_part(*pflags, *base_type),
                               *exp_type,
                               MUL);
             clear_buffers(part_buffer, pflags, base_type, exp_type);
        }
        *base_type = fun_part;
        SET_TRUE(*pflags, IS_FUN);
        append_str(part_buffer, &func_str[*i], 3);
        *i += 3;
        return parse_parenthesis(func, i, part_buffer, pflags, base_type, exp_type);
    }
    // Exponent contains a variable -> assume that's all in exponent
    else if (*i > 0 && CHECK(*pflags, HAS_EXP) && func_str[*i-1] == '^')
    {
        append_str(part_buffer, &func_str[*i], 1);
        if (func_str[*i+1] == '!')
            append_str(part_buffer, &func_str[(*i)++ + 1], 1);

        addToFunctionList(func,
                          part_buffer,
                          get_part(*pflags, *base_type),
                          *exp_type,
                          get_op(func_str[*i+1]));
        clear_buffers(part_buffer, pflags, base_type, exp_type);
    }
    // Assume it's a variable
    else
    {
         if (part_buffer[0] != '\0')
         {
             addToFunctionList(func,
                               part_buffer,
                               get_part(*pflags, *base_type),
                               *exp_type,
                               MUL);
             clear_buffers(part_buffer, pflags, base_type, exp_type);
         }
         append_str(part_buffer, &func_str[*i], 1);
         SET_TRUE(*pflags, HAS_VAR);
    }
    return true;
}

bool parse_parenthesis(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
) {
    char * func_str = func->str;
    int par_bal = 1;
    if (!CHECK(*pflags, IS_PAR))
    {
        // (Function handles operations) 
        // Assume it's implicit multiplication
        if (!CHECK(*pflags, HAS_OP)  &&
            !CHECK(*pflags, IS_FUN)  &&
            !CHECK(*pflags, HAS_EXP) &&
             strlen(part_buffer) != 0)
        {
            addToFunctionList(func,
                              part_buffer,
                              get_part(*pflags, *base_type),
                              *exp_type,
                              MUL);
        }

        // Always appends opening parenthesis to 
        // appear in part string
        SET_TRUE(*pflags, IS_PAR);
        append_str(part_buffer, &func_str[*i], 1);

        while (*i < strlen(func_str)-1)
        {
            (*i)++;
            if (func_str[*i] != ')' && func_str[*i] != '(') 
                append_str(part_buffer, &func_str[*i], 1);
            
            else if (func_str[*i] == '(') {
                ++par_bal;
                append_str(part_buffer, &func_str[*i], 1);
            } 
            else 
            {
                --par_bal;
                append_str(part_buffer, &func_str[*i], 1);
                
                if (par_bal == 0)
                {
                    if (func_str[*i+1] == '!')
                        append_str(part_buffer, &func_str[(*i)++ + 1], 1);

                    op temp_op;
                    switch(func_str[*i+1]) 
                    {
                    case '+': temp_op = ADD; ++(*i);
                              SET_TRUE(*pflags, HAS_OP);
                              break;

                    case '-': temp_op = SUB; ++(*i);
                              SET_TRUE(*pflags, HAS_OP);
                              break;

                    case '/': temp_op = DIV; ++(*i);
                              SET_TRUE(*pflags, HAS_OP);
                              break;

                    case '\0': temp_op = NOOP;
                               SET_FALSE(*pflags, HAS_OP);
                               break;

                    case '^':  SET_FALSE(*pflags, IS_PAR);
                               SET_FALSE(*pflags, IS_FUN);
                               ++(*i);
                               return parse_exponent(func,
                                                     i,
                                                     part_buffer,
                                                     pflags,
                                                     base_type,
                                                     exp_type);
                    default: temp_op = MUL;
                             SET_TRUE(*pflags, HAS_OP);
                             break; 
                    }
                    addToFunctionList(func, 
                                      part_buffer,
                                      get_part(*pflags, *base_type),
                                      *exp_type,
                                      temp_op);
                    clear_buffers(part_buffer, pflags, base_type, exp_type);
                    break;
                }  
            }    
        }
    } else
        append_str(part_buffer, &func_str[*i], 1);

    SET_FALSE(*pflags, IS_FUN);
    SET_FALSE(*pflags, IS_PAR);         
    return true;
}

bool parse_exponent(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
) {
    char * func_str = func->str;
    if (!CHECK(*pflags, HAS_EXP)) 
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
            append_str(part_buffer, &func_str[*i], 1);
            SET_TRUE (*pflags, HAS_EXP);
            SET_FALSE(*pflags, HAS_NEG);
            SET_FALSE(*pflags, HAS_DEC);

            if (isdigit(func_str[*i+1])
                    || func_str[*i+1] == '.'
                    || func_str[*i+1] == '-')
                *exp_type = NUM;
            else if (isalpha(func_str[*i+1]))
            {
                if ((*exp_type = is_func_type(&func_str[*i+1])))
                    { /* Is assigned */ }
                else
                    *exp_type = VAR;
            }
            else if (func_str[*i+1] == '(')
                *exp_type = PAR;
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
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
) {
    char * func_str = func->str;
    if (!CHECK(*pflags, HAS_DEC)) 
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
             append_str(part_buffer, "0", 1);
             SET_TRUE(*pflags, HAS_NUM);
         }
         /* Append and set has_dec to true. */
         append_str(part_buffer, &func_str[*i], 1);
         SET_TRUE(*pflags, HAS_DEC);
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
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
) {
    char * func_str = func->str;

    if (!CHECK(*pflags, HAS_NEG)) 
    {
        // If the function has numbers/variables, act as subtraction
        if (strlen(part_buffer) != 0 
               && !CHECK(*pflags, HAS_OP)
               && !CHECK(*pflags, HAS_EXP)) 
        {
            addToFunctionList(func,
                              part_buffer,
                              get_part(*pflags, *base_type),
                              *exp_type,
                              SUB);
            clear_buffers(part_buffer, pflags, base_type, exp_type);
            SET_TRUE(*pflags, HAS_OP);

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
            append_str(part_buffer, &func_str[*i], 1);
            SET_TRUE(*pflags, HAS_NEG);
        }                
    }
    // If negative sign is present
    else 
    {
        // If function already contains a negative
        if (strncmp(part_buffer, "-", 1) == 0) 
        {
            print_parse_error(
                "A subtraction operation already exists.",
                file_name, func_str, error_loc_buffer, *i); 
            return false;
        }
        else if (CHECK(*pflags, HAS_OP)) 
        {
            print_parse_error(
                "An operator already exists",
                file_name, func_str, error_loc_buffer, *i);
            return false;
        }
        addToFunctionList(func,
                          part_buffer,
                          get_part(*pflags, *base_type),
                          *exp_type,
                          SUB);
 
        clear_buffers(part_buffer, pflags, base_type, exp_type);    
        SET_TRUE(*pflags, HAS_OP);
    }
    return true;
}

bool parse_operator(
        function     * func,
        unsigned int * i,
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_type,
        part_type    * exp_type
) {
    char * func_str = func->str;
    op temp_op;

    if (*i == 0 || CHECK(*pflags, HAS_OP))
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
                      part_buffer,
                      get_part(*pflags, *base_type),
                      *exp_type,
                      temp_op);

    clear_buffers(part_buffer, pflags, base_type, exp_type);
    SET_TRUE(*pflags, HAS_OP);
    return true;
}

part * parse_part(
  const char     * functionStr,
        function * root_func
) {
    part * funcPart;
    
    function * func = parse_function(functionStr, root_func);
    funcPart = func->head;
    
    free(func);
    return funcPart;
}

void clear_buffers(
        char         * part_buffer,
        unsigned int * pflags,
        part_type    * base_part,
        part_type    * exp_type
) {
    part_buffer[0] = '\0';
    *pflags = 0;
    *base_part = NOPART;
    *exp_type  = NOPART;    
}
