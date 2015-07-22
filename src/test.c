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
    char * test = "4!+.354x^-5!+(74)^(23^(22^(21)))!45sin(59x)!+(45!)";
    char * test2 = "2*x^3+3.41/7.2!";
    char * test3 = "log(ln(873zb^sin(98(e^b!)))!)";
    function * theFunc = parseFunction(test, NULL);
    printInfo(theFunc);
    function * theFunc2 = parseFunction(test2, NULL);
    printInfo(theFunc2);
    function * theFunc3 = parseFunction(test3, NULL);
    printInfo(theFunc3);
    return 0;
}
