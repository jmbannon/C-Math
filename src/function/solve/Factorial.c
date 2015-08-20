#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "Factorial.h"
#include "../Function.h"

double integer_factorial(
        const double base
) {
    int factorial_sum = (int)round(base);
    int i = factorial_sum;
    assert(factorial_sum >= 0);
    assert(base + 0.00001 > factorial_sum && base - 0.00001 < factorial_sum);

    if (factorial_sum == 0)
        return 1.0;
  
    while (i > 1)
        factorial_sum *= --i;

    return factorial_sum;
}
