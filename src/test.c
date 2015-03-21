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

int main(void) {
    char* test = "123banana";

    printf("Equation Parser!");
    parseFunction(test);
    return 0;
}
