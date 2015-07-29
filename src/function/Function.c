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
#include "../utilities/Boolean.h"
#include "parse/Parse.h"
#include "../utilities/StringExtensions.h"

static int VAR_LIST_SIZE = 64;

/* 
 =======================================================================

        Function Prototypes

 =======================================================================
*/

functionPart * parse_parenthesis_part(
        char     * par_str,
        function * root_func
);

/* 
 =======================================================================

        Functions

 =======================================================================
*/

void initialize_par(
        functionPart * part,
        function     * root_func
) {
    part->base.par = parse_parenthesis_part(part->str,
                                            root_func);
}

void initialize_var(
        functionPart * part,
        function     * root_func
) {
    assert(isalpha(part->str[0]));
    part->base.var.variable = part->str[0];
    addToVariableList(part->func, part->str[0]);
}

void initialize_num(
        functionPart * part,
        function     * root_func
) {
    int i = 0;
    char * temp = malloc(sizeof(part->str));
    while (isdigit(part->str[i]) || part->str[i] == '.'
                                 || part->str[i] == '-')
    {
        temp[i] = part->str[i];
        ++i;
    }
    part->base.num = atof(temp);
    free(temp);
}

void initialize_fun(
        functionPart * part,
        function     * root_func,
  const part_type      type
) {
    assert(type != NOPART);

    int i = 0;
    part->base.fun.type = type;
    switch(type)
    {
        case LN:   i += 2; break;
        case SQRT: i += 4; break;
        default:   i += 3; break;
    }
    part->base.fun.contents = parse_parenthesis_part(&part->str[i],
                                                     root_func);
}


/* Determines the part_union for the given functionPart.
 * Creates the part based on parsing the first char within its string
 */
void initializePart(
        functionPart * thePart,
  const part_type      type,
        function     * root_func
) {
    int i = 0, len = strlen(thePart->str);
    switch(type)
    {
        case PAR: initialize_par(thePart, root_func); break;
        case VAR: initialize_var(thePart, root_func); break;
        case NUM: initialize_num(thePart, root_func); break;
        default:  initialize_fun(thePart, root_func, type); break;
    }
    while (i < len && thePart->str[i] != '^') ++i;
    if (thePart->str[i] == '^')
    {
        if (thePart->str[i+1] == '(')
            thePart->exponent.par = parse_parenthesis_part(&thePart->str[i+1],
                                                           root_func);
    }
    else if (isalpha(thePart->str[i]))
    {
        thePart->exponent.var.variable = thePart->str[i];
        addToVariableList(thePart->func, thePart->str[i]);
    }
    else if (isdigit(thePart->str[i]) || thePart->str[i] == '-')
    {

    }
}

/* Parses the given parenthesis string and returns its functionPart.
 * par_str is expected to be passed in at the index of the opening parenthesis.
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
        char * functionBuilder,
        const part_type type, 
        const op_type operation
) {
    functionPart * thePart = malloc(sizeof(functionPart));
    functionPart * currTemp = func->head;

    printf("type = %d op = %d, thePart = %s\n", type, operation, functionBuilder);

    thePart->func = func;
    thePart->str = malloc(strlen(functionBuilder) + 1);
    strcpy(thePart->str, functionBuilder);
    initializePart(thePart, type, func->root_func);

    functionBuilder[0] = '\0';

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
        const char variable
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
        char variable,
        int idx
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
        char variable
) {
    int insertIdx = needs_var_insert(theFunction, variable);
    if (insertIdx >= 0)
        insertVariableToList(theFunction->var_list,
                             variable,
                             insertIdx);
}
