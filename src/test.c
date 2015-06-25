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
    char* test = "4+.354x^-5+(74)^(43x)45sin(59x)+(45)";
    function* theFunc = parseFunction(test);
    printInfo(theFunc);
    return 0;
}
