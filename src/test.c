/*
 =======================================================================
 test.c

 Jesse Bannon

 TODO
 =======================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "function/parse/Parse.h"
#include "function/Function.h"

int main(void) 
{
     char * test1 = "4!+.354x^-5.+(74)^(23^(22^(21)))!45sin(59x)!+(45!)";
     char * test2 = "2*x^3+3.41/7.2!";
     char * test3 = "u+b";
     function * theFunc = parse(test1);
     printInfo(theFunc);
     function * theFunc2 = parse(test2);
     printInfo(theFunc2);
     function * theFunc3 = parse(test3);
     printInfo(theFunc3);
     return 0;
}
