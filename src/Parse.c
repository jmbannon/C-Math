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
#include "utilities/ErrorMessage.h"
#include "utilities/StringExtensions.h"
#include "utilities/Boolean.h"

#define HAS_OP  (0)
#define HAS_NEG (1)
#define HAS_NUM (2)
#define HAS_DEC (3)
#define HAS_EXP (4)
#define HAS_VAR (5)
#define IS_PAR  (6)
#define IS_FUN (7)

#define SET_TRUE(BOOL, value) (BOOL |= 1 << value)
#define SET_FALSE(BOOL, value) (BOOL &= ~(1 << value))
#define CHECK(BOOL, value) (BOOL & 1 << value)

static char * file_name = "Parse.c";
static char error_loc_buffer[1024];

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

void print_parse_error(
        const char * description,
        const char * function,
        int index
);

char * err_loc_msg(
        const char * function,
        int index
);

part_type get_part(
        unsigned int B_BOOLS,
        part_type temp_part
);

op_type get_op(
        char c
);

/** Parses the given function from a string into a sequential linked list of
  * FunctionParts with operators inbetween.  
  * WIP
  */
function * parseFunction(
        const char * func_str,
        var        * root_var_list
) {
    /* iterator, parse_check, parenthesis_balance, base_bools */
    unsigned int i, pc = true, par_bal = 0, B_BOOLS = 0;
    op_type   temp_op;             /* temporary operation */
    part_type temp_part, exp_part; /* temporary part, exponent part */

    function * func = initializeFunction(func_str);
    if (root_var_list != NULL)
        set_var_list(func, root_var_list);

    char func_builder[1024] = {0};
    char c, temp_char;

    for (i = 0; i < strlen(func_str); i++) 
    {
        c = func_str[i];
        /* NUMBERS */
        if (isdigit(c))
            pc = parse_digit(func, &i, func_builder, &B_BOOLS);

        /* FACTORIAL */
        else if (c == '!')
            pc = parse_factorial(func, &i, func_builder, &B_BOOLS);
 
        /* LETTERS */
        else if (isalpha(c)) { //should recognize log(, ln(, pi, e, L(), and !
            pc = parse_alpha(func, &i, func_builder, &B_BOOLS, &temp_part);
        }
        /* DECIMAL POINT */
        else if (c == '.') 
        {
            if (!CHECK(B_BOOLS, HAS_DEC)) 
            {
                 temp_char = '0';
                 
                 // If letter precedes decimal, throw error. 
                 if (isalpha(func_str[i-1])) 
                 {
                     print_parse_error(
                        "A decimal point cannot proceed a variable.",
                        func_str, i); 
                     return NULL;
                 }

                 // If a number does not precede decimal, add 0 first
                 else if (i == 0 || !isdigit(func_str[i-1]))
                 {
                     appendStr(func_builder, &temp_char, 1);
                     SET_TRUE(B_BOOLS, HAS_NUM);
                 }
                 /* Append and set has_dec to true. */
                 appendStr(func_builder, &c, 1);
                 SET_TRUE(B_BOOLS, HAS_DEC);
            }
            else 
            {
                print_parse_error(
                    "Numeric already contains a decimal.",
                    func_str, i); 
                return NULL;
            }
        }

        /* NEGATIVE/SUBTRACTION */
        else if (c == '-')        
        {                   
            if (!CHECK(B_BOOLS, HAS_NEG)) 
            {
                // If the function has numbers/variables, act as subtraction
                if (strlen(func_builder) != 0 
                       && !CHECK(B_BOOLS, HAS_OP)
                       && !CHECK(B_BOOLS, HAS_EXP)) 
                {
                    printf("subtraction func list: %s", func_builder);
                    addToFunctionList(func,
                                      func_builder,
                                      get_part(B_BOOLS, temp_part),
                                      SUB);
                    B_BOOLS = 0;
                    SET_TRUE(B_BOOLS, HAS_OP);

                // Otherwise act as negative symbol and append to buffer
                } 
                else 
                {
                    if (i != 0 && func_str[i-1] == '.')
                    {
                        print_parse_error(
                            "Cannot subtract a decimal.",
                            func_str, i); 
                        return NULL;
                    }
                    appendStr(func_builder, &c, 1);
                    SET_TRUE(B_BOOLS, HAS_NEG);
                }                
            }
            // If negative sign is present
            else 
            {
                // If function already contains a negative
                if (strcmp(func_builder, "-") == 0) 
                {
                    print_parse_error(
                        "A subtraction operation already exists.",
                        func_str, i); 
                    return NULL;
                }
                else if (CHECK(B_BOOLS, HAS_OP)) 
                {
                    print_parse_error(
                        "A subtraction operator already exists",
                        func_str, i);
                    return NULL;
                }
                addToFunctionList(func,
                                  func_builder,
                                  get_part(B_BOOLS, temp_part),
                                  SUB); 
                B_BOOLS = 0;    
                SET_TRUE(B_BOOLS, HAS_OP);
            }
        }

        /* EXPONENT */                                        
        else if (c == '^')
            pc = parse_exponent(func, &i, func_builder, &B_BOOLS, &temp_part);

        /* OPERATORS (BESIDES SUBTRACTION) */
        else if (c == '+' || c == '*' || c == '/') 
        {  
            if (i == 0 || CHECK(B_BOOLS, HAS_OP))
            {
                print_parse_error(
                    "Operation must proceed a number, variable, or parenthesis.",
                    func_str, i);
                return NULL;
            }

            switch(c) 
            {
                case '+': temp_op = ADD; break;
                case '*': temp_op = MUL; break;
                case '/': temp_op = DIV; break;
                default: temp_op = NOOP;
            }
            addToFunctionList(func,
                              func_builder,
                              get_part(B_BOOLS, temp_part),
                              temp_op);
            B_BOOLS = 0;
            SET_TRUE(B_BOOLS, HAS_OP);
        }

        /* OPENING PARENTHESIS, NEEDS OVERHAUL */     
        else if (c == '(')                                               
            pc = parse_parenthesis(func, &i, func_builder, &B_BOOLS, &temp_part);

        // Every close parenthesis should be handled 
        // in opening parenthesis parse
        else if (c == ')')                                                 
        {       
            print_parse_error(
                "Does not have an opening parenthesis.",
                func_str, i);
            return NULL;
        } 
        else
        {
            print_parse_error(
                "Unknown character to parse.",
                func_str, i);
            pc = false;
        }

        if (!pc)
            return NULL;
    } //end of forloop

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
            func_str, -1);
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
    char * func_str = get_func_str(func);
    if (CHECK(*B_BOOLS, HAS_VAR) && !CHECK(*B_BOOLS, HAS_EXP)) 
    {
       print_parse_error(
           "Cannot have digit immediately after variable without operation",
           func_str, *i); 
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
    char * func_str = get_func_str(func);
    if (*i == 0 || 
       (!isdigit(func_str[*i-1]) &&
        !isalpha(func_str[*i-1])))
    {
        print_parse_error(
            "Factorial must follow a value",
            func_str, *i);
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
    char * func_str = get_func_str(func);
    if (*i > 0 && func_str[*i-1] == '.') 
    {
        print_parse_error(
            "Invalid variable. Cannot proceed non-numeric symbol '.'",
            func_str, *i); 
        return false;
    }
    // If letters proceed are a trig function, add preceding
    // function to equationList and create new function 
    else if ((*temp_part = isTrigFunction(&func_str[*i]))) 
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
    char * func_str = get_func_str(func);
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
    char * func_str = get_func_str(func);
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
                func_str, *i);
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
                if (isTrigFunction(&func_str[*i+1]))
                    exp_part = isTrigFunction(&func_str[*i+1]);
                else
                    exp_part = VAR;
            }
            else if (func_str[*i+1] == '(')
                exp_part = PAR;
            else
            {
                print_parse_error(
                    "Exponent must preceed a number, variable, or parenthesis.",
                    func_str, *i+1);
                return false;
            }
        }

    // If exponent sign already exists, throw exception
    }
    else 
    {
        print_parse_error(
            "Function already contains an exponent.",
            func_str, *i);
        return false;
    }
    return true;
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

part_type get_part(
        unsigned int B_BOOLS,
        const part_type temp_part
) {
    if (CHECK(B_BOOLS, HAS_VAR))
        return VAR;
    else if (CHECK(B_BOOLS, HAS_NUM))
        return NUM;
    else if (CHECK(B_BOOLS, IS_FUN))
        return temp_part;
    else if (CHECK(B_BOOLS, IS_PAR))
        return PAR;
    else
        return NOPART;
}

op_type get_op(
        char c
) {
    switch(c)
    {
        case '+': return  ADD;
        case '-': return  SUB;
        case '/': return  DIV;
        case '\0': return NOOP;
        default: return MUL;
    }
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
