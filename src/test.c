/*
 ============================================================================
 Name        : Jesse Bannon
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "Parse.h"
#include "Function.h"

int main(void) {
    char* test = ".354^-5+(74x-56y)sin(59x)+(45)";
    function* theFunc = parseFunction(test);
    printInfo(theFunc);
    return 0;
}
