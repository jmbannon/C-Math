/* 
 =======================================================================
 Function.c

 Jesse Bannon

 TODO
 =======================================================================
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "Function.h"
#include "parse/Parse.h"
#include "../utilities/Boolean.h"
#include "../utilities/StringExtensions.h"

static int VAR_LIST_SIZE = 64;

/* 
 =======================================================================

        Function Prototypes

 =======================================================================
*/

/*
 *  Converts a string into a double
 */
double initialize_num(
        char * number
);

/*
 *  Converts a string into a var struct with the variable initialized.
 */
var initialize_var(
        char         * variable,
        function     * root_func
);

/*
 *  Converts a string into a fun struct with
 *  the type and contents initialized.
 */
fun initialize_fun(
        char      * part_str,
        function  * root_func,
  const part_type   type
);

/*
 *  Converts a string into a functionPart struct containing
 *  the parenthesis contents.
 */
functionPart * parse_parenthesis_part(
        char     * par_str,
        function * root_func
);

/*
 *  Assigns the base struct some type.
 */
void initialize_base(
        function * root_func,
        char     * str,
  const part_type  type,
        base       base
);

/* 
 =======================================================================

        Functions

 =======================================================================
*/

var initialize_var(
        char         * variable,
        function     * root_func
) {
    assert(isalpha(variable[0]));
    var var;
    var.variable = variable[0];
    addToVariableList(root_func, variable[0]);
    return var;
}

double initialize_num(
        char * number
) {
    int i = 0;
    double num;
    char * number_str = calloc(sizeof(number), sizeof(char));

    while (isdigit(number[i])
               || number[i] == '.'
               || number[i] == '-')
    {
        number_str[i] = number[i];
        ++i;
    }
    num = atof(number_str);
    free(number_str);
    return num;
}

fun initialize_fun(
        char      * part_str,
        function  * root_func,
  const part_type   type
) {
    int i = 0;
    fun fun;

    fun.type = type;
    switch(type)
    {
        case LN:   i += 2; break;
        case SQRT: i += 4; break;
        default:   i += 3; break;
    }
    fun.contents = parse_parenthesis_part(&part_str[i],
                                           root_func);
    return fun;
}

void initialize_base(
        function * root_func,
        char     * str,
  const part_type  type,
        base       base)
{
    assert(type != NOPART);
    switch(type)
    {
    case PAR: base.par = parse_parenthesis_part(str, root_func); break;
    case VAR: base.var = initialize_var(str, root_func); break;
    case NUM: base.num = initialize_num(str); break;
     default: base.fun = initialize_fun(str, root_func, type);
    }
}

/* Determines the part_union for the given functionPart.
 * Creates the part based on parsing the first char within its string
 */
void initializePart(
        function     * root_func,
        functionPart * thePart,
  const part_type      base_type,
  const part_type      exp_type
) {
    assert(base_type != NOPART);
    int i = 0, len = strlen(thePart->str);

    initialize_base(root_func,
                    thePart->str,
                    base_type,
                    thePart->base);

    while (i < len && thePart->str[i] != '^') ++i;
    if (thePart->str[i++] == '^')
        initialize_base(root_func,
                        &thePart->str[i],
                        exp_type,
                        thePart->exponent);
}

/* Parses the given parenthesis string and returns its functionPart.
 * par_str is expected to be passed in at the index 
 * of the opening parenthesis.
 */
functionPart * parse_parenthesis_part(
        char     * par_str,
        function * root_func
) {
    int i = 0, par_bal = 1, len = strlen(par_str);
    while (i < len-1 && par_bal != 0)
    {
        if (par_str[i+1] == '(') ++par_bal;
        else if (par_str[i+1] == ')') --par_bal;
        
        if (par_bal != 0) ++i;
    }
    assert(par_bal == 0);
    return parseFunctionPart(substring(par_str, 1, i), root_func);
}


/* Initializes a function type with the given function string.
 */
function * initializeFunction(
  const char * theFunction
) {
    function * func = malloc(sizeof(function));
    func->str = malloc(strlen(theFunction)+1);
    strcpy(func->str, theFunction);
    func->head = NULL;
    func->var_list = calloc(VAR_LIST_SIZE, sizeof(char));
    return func;
}

/* Prints debugging info of the given function.
 */
void printInfo(
        function * theFunction
) {
    if (theFunction == NULL)
        return;

    printf("Full function: %s\n", theFunction->str);
    functionPart* curr = theFunction->head;

    while (curr != NULL) 
    {
        printf("Part: %s Op: %d\n", curr->str, curr->operation);
        curr = curr->next;
    }

    int i = 0;
    printf("Variables in ascending order: ");
    while (i < VAR_LIST_SIZE && theFunction->var_list[i].variable)
        printf("%c ", theFunction->var_list[i++].variable);
    printf("\n");
}

/* Adds a valid piece of a function string to the function type's linked
 * list of functionParts with the given operation.  Then clears the function
 * builder for input of another functionPart if it exists.
 */
void addToFunctionList(
        function * func,
        char     * functionBuilder,
  const part_type  base_type, 
  const part_type  exp_type,
  const op_type    operation
) {
    functionPart * thePart = malloc(sizeof(functionPart));
    functionPart * currTemp = func->head;

    printf("base = %d exp = %d op = %d, thePart = %s\n", base_type,
                                                exp_type,
                                                operation,
                                                functionBuilder);

    thePart->func = func;
    thePart->str = malloc(strlen(functionBuilder) + 1);
    strcpy(thePart->str, functionBuilder);
    initializePart(func->root_func, thePart, base_type, exp_type);
    thePart->operation = operation;
    
    if (currTemp == NULL)
    {
        thePart->next = currTemp;
        thePart->prev = NULL;
        func->head = thePart;
        return;
    }
    while (currTemp != NULL && currTemp->next != NULL)
    {
        currTemp = currTemp->next;
    }
    
    currTemp->next = thePart;
    thePart->prev = currTemp;
    thePart->next = NULL;
}

/* Returns a trigType enum if one exists for the beginning letter of a
 * string.
 */
part_type is_func_type(
  const char * firstLetter
) {
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

    else if (strncmp(firstLetter, "asin(", 5) == 0)
        return ASIN;

    else if (strncmp(firstLetter, "acos(", 5) == 0)
        return ACOS;

    else if (strncmp(firstLetter, "atan(", 5) == 0)
        return ATAN;

    else if (strncmp(firstLetter, "sqrt(", 5) == 0)
        return SQRT;

    else if (strncmp(firstLetter, "abs(", 4) == 0)
        return ABS;

    else if (strncmp(firstLetter, "log(", 4) == 0)
        return LOG;

    else if (strncmp(firstLetter, "ln(", 3) == 0)
        return LN;

    else
        return NOPART;
}

/* Returns the index a variable should be inserted into to traverse a
 * variable array. Returns -1 if it exists within the function's variable
 * array.
 */
int needs_var_insert(
        function * theFunction,
  const char       variable
) {
    int i;
    for (i = 0; i < VAR_LIST_SIZE; i++)
    {
        if (theFunction->var_list[i].variable == variable)
            return -1;

        else if (theFunction->var_list[i].variable == '\0')
            return i;
    }
    return VAR_LIST_SIZE;
}

/* Inserts a variable into a given variable ordered-list by starting it
 * at the end of the array and bubble up until it is in ascending order.
 */
void insertVariableToList(
        var * var_list,
        char  variable,
        int   idx
) {
    if (idx == VAR_LIST_SIZE)
        var_list = realloc(var_list, VAR_LIST_SIZE*=2);

    var_list[idx].variable = variable;
    while (idx != 0 && var_list[idx-1].variable > var_list[idx].variable)
    {
        variable = var_list[idx-1].variable;
        var_list[idx-1].variable = var_list[idx].variable;
        var_list[idx--].variable = variable;
    }
}

/* Inserts a variable into the function's variable array if it does not exist.
 */
void addToVariableList(
        function * theFunction,
        char       variable
) {
    int insertIdx = needs_var_insert(theFunction, variable);
    if (insertIdx >= 0)
        insertVariableToList(theFunction->var_list,
                             variable,
                             insertIdx);
}
