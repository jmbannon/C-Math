#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <stdbool.h>
#include "FunctionSolve.h"
#include "../Function.h"
#include "../parse/Parse.h"

/*
 * Copies the function (copy by memory!!)
 */
function * copy_function(
        function * func
) {
    return parse(func->str);
}

/*
 * Returns the value assigned for the variable.
 */
double get_var_value(
        function * func,
        char variable
) {
    int i = 0;
    while (i < VAR_LIST_SIZE && func->var_list[i].variable != variable) ++i;
    return func->var_list[i].num;
}

double get_value(
        part * part
) {
    int i = 0;
    switch(part->base_type)
    {
    case VAR: return get_var_value(part->func, part->base.var.variable);
    default:  assert(false); return 0;
    }
}

void solve_function(
        function * original_func,
        unsigned   n_args,
                   ...
) {
    va_list var_args;
    int i = 0;
    function * func = copy_function(original_func);
    part     * curr = func->head;
    part     * builder;
    double     sum;
    double     temp_val;

    va_start(var_args, n_args);
    for (i = 0; i < n_args; i++)
    {
        if (func->var_list[i].variable != '\0')
        {
            func->var_list[i].num = va_arg(var_args, double);
        }
        else
            break;
    }
    va_end(var_args);

    temp_val = get_value(curr);
    sum = temp_val;
    printf("%lf\n", sum);
    //while (curr->next != NULL)
    //{

    //}
}
