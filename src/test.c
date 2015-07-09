/* 
 =======================================================================
 test.c

 Jesse Bannon

 TODO
 =======================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "Parse.h"
#include "Function.h"

int main(void) 
{
    char* test = "4+.354x^-5+(74)^(23^(22^(21)))45sin(59x)+(45)";
    function* theFunc = parseFunction(test, NULL);
    printInfo(theFunc);
    return 0;
}
