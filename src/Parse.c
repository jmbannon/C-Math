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

#define HAS_OP  (0)
#define HAS_NEG (1)
#define HAS_NUM (2)
#define HAS_DEC (3)
#define HAS_EXP (4)
#define HAS_VAR (5)
#define IS_PAR  (6)
#define IS_TRIG (7)

#define SET_TRUE(BOOL, value) (BOOL |= 1 << value)
#define SET_FALSE(BOOL, value) (BOOL &= ~(1 << value))
#define CHECK(BOOL, value) (BOOL & 1 << value)

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
        const char * theFunction,
        var * root_var_list
) {
    int i, par_bal = 0;       /* parenthesis balance */
    unsigned int B_BOOLS = 0,
                 E_BOOLS = 0; /* booleans for base and exponent */

    op_type   temp_op;        /* temporary operation */
    part_type temp_part;      /* temporary part */

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
            if (CHECK(B_BOOLS, HAS_VAR) && !CHECK(B_BOOLS, HAS_EXP)) 
            {
                print_parse_error(
                    "Cannot have digit immediately after variable without operation",
                    theFunction, i); 
                return NULL;
            }

           appendChar(func_builder, c);     
           SET_TRUE (B_BOOLS, HAS_NUM);
           SET_FALSE(B_BOOLS, HAS_OP);
        }
        /* FACTORIAL */
        else if (c == '!')
        {
            if (i == 0 || (!isdigit(theFunction[i-1])
                           && !isalpha(theFunction[i-1])
                           && theFunction[i-1] != '!'))
            {
                print_parse_error(
                    "Factorial must follow a value",
                    theFunction, i);
                return NULL;
            }
            else
               appendStr(func_builder, &c, 1);
        } 
        /* LETTERS */
        else if (isalpha(c))  //should recognize log(, ln(, pi, e, L(), and !
        {
            // If letters proceed are a trig function, add preceding
            // function to equationList and create new function 
            if ((temp_part = isTrigFunction(&theFunction[i]))) 
            {
                if (i != 0 && !CHECK(B_BOOLS, HAS_OP)
                           && !CHECK(B_BOOLS, HAS_EXP))
                {
                     addToFunctionList(func,
                                       func_builder,
                                       get_part(B_BOOLS, temp_part),
                                       MUL);
                     B_BOOLS = 0;
                }
                SET_TRUE(B_BOOLS, IS_TRIG);
                appendStr(func_builder, &theFunction[i], 3);
                i += 3;

                goto parseParenthesis;
            }
            // Variable parse errors 
            else if (theFunction[i-1] == '.') 
            {
                print_parse_error(
                    "Invalid variable. Cannot proceed non-numeric symbol '.'",
                    theFunction, i); 
                return NULL;
            }
            // Exponent contains a variable -> assume that's all in exponent
            else if (CHECK(B_BOOLS, HAS_EXP) && theFunction[i-1] == '^')
            {
                appendStr(func_builder, &c, 1);
                if (theFunction[i+1] == '!')
                    appendStr(func_builder, &theFunction[i++ + 1], 1);

                addToFunctionList(func,
                                  func_builder,
                                  get_part(B_BOOLS, temp_part),
                                  get_op(theFunction[i+1]));
                B_BOOLS = 0;
            }
            // Assume it's a variable
            else
            {
                 if (func_builder[0] != '\0')
                 {
                     addToFunctionList(func,
                                       func_builder,
                                       get_part(B_BOOLS, temp_part),
                                       MUL);
                     B_BOOLS = 0;
                 }
                 appendStr(func_builder, &c, 1);
                 SET_TRUE(B_BOOLS, HAS_VAR);
            }
        }

        /* DECIMAL POINT */
        else if (c == '.') 
        {
            if (!CHECK(B_BOOLS, HAS_DEC)) 
            {
                 temp_char = '0';
                 
                 // If letter precedes decimal, throw error. 
                 if (CHECK(B_BOOLS, HAS_VAR)) 
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
                 SET_TRUE(B_BOOLS, HAS_DEC);
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
                    if (i != 0 && theFunction[i-1] == '.')
                    {
                        print_parse_error(
                            "Cannot subtract a decimal.",
                            theFunction, i); 
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
                        theFunction, i); 
                    return NULL;
                }
                else if (CHECK(B_BOOLS, HAS_OP)) 
                {
                    print_parse_error(
                        "A subtraction operator already exists",
                        theFunction, i);
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
        {           
            parseExponent:  
            if (!CHECK(B_BOOLS, HAS_EXP)) 
            {
                // If exponent does not proceed number or variable, 
                // throw exception. 
                if (i == 0 || (!isdigit(theFunction[i-1]) 
                           && !isalpha(theFunction[i-1]) 
                           && theFunction[i-1] != ')')) 
                {
                    print_parse_error(
                        "Exponent must proceed a number, variable, or parenthesis.",
                        theFunction, i);
                    return NULL;
                }
                // Append to function and set exponent to true, negative 
                // and decimal to false for exponent number                 
                else 
                {
                    appendStr(func_builder, &theFunction[i], 1);
                    SET_TRUE (B_BOOLS, HAS_EXP);
                    SET_FALSE(B_BOOLS, HAS_NEG);
                    SET_FALSE(B_BOOLS, HAS_DEC);
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
            if (i == 0 || CHECK(B_BOOLS, HAS_OP)) 
            {
                print_parse_error(
                    "Operation must proceed a number, variable, or parenthesis.",
                    theFunction, i);
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
        {                                                                
            // parseParenthesis: 
            // This goto handles anything that contains parenthesis.
            parseParenthesis:
            ++par_bal;
                                                                          
            if (!CHECK(B_BOOLS, IS_PAR))
            {
                // (Function handles operations) 
                // Assume it's implicit multiplication
                SET_TRUE(B_BOOLS, IS_PAR);
                if (!CHECK(B_BOOLS, HAS_OP) && !CHECK(B_BOOLS, IS_TRIG)
                                            && !CHECK(B_BOOLS, HAS_EXP) 
                                            && strlen(func_builder) != 0)
                {
                    addToFunctionList(func,
                                      func_builder,
                                      get_part(B_BOOLS, temp_part),
                                      MUL);
                }

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
                            if (theFunction[i+1] == '!')
                                appendStr(func_builder, &theFunction[i++ + 1], 1);

                            switch(theFunction[i+1]) 
                            {
                            case '+': temp_op = ADD; ++i;
                                      SET_TRUE(B_BOOLS, HAS_OP);
                                      break;

                            case '-': temp_op = SUB; ++i;
                                      SET_TRUE(B_BOOLS, HAS_OP);
                                      break;

                            case '/': temp_op = DIV; ++i;
                                      SET_TRUE(B_BOOLS, HAS_OP);
                                      break;

                            case '\0': temp_op = NOOP;
                                       SET_FALSE(B_BOOLS, HAS_OP);
                                       break;

                            case '^':  SET_FALSE(B_BOOLS, IS_PAR);
                                       SET_FALSE(B_BOOLS, IS_TRIG);
                                       ++i; goto parseExponent;

                            default: temp_op = MUL;
                                     SET_TRUE(B_BOOLS, HAS_OP);
                                     break; 
                            }
                            printf("the function builder = %s\n", func_builder);
                            addToFunctionList(func, 
                                              func_builder,
                                              get_part(B_BOOLS, temp_part),
                                              temp_op);
                            B_BOOLS = 0;
                            break;
                        }  
                    }    
                }
            } else
                appendStr(func_builder, &c, 1);

            SET_FALSE(B_BOOLS, IS_TRIG);
            SET_FALSE(B_BOOLS, IS_PAR);         
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
    {
        addToFunctionList(func, 
                          func_builder,
                          get_part(B_BOOLS, temp_part),
                          NOOP); 
    }

    if (CHECK(B_BOOLS, HAS_OP))
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
    //printf("%s\n", func_builder);
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

part_type get_part(
        unsigned int B_BOOLS,
        const part_type temp_part
) {
    if (CHECK(B_BOOLS, HAS_VAR))
        return VAR;
    else if (CHECK(B_BOOLS, HAS_NUM))
        return NUM;
    else if (CHECK(B_BOOLS, IS_TRIG))
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
