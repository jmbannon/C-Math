#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Parse.h"

enum operations {NOOP, ADD, SUB, MUL, DIV};
enum trigFunctions {NOTRIG, SIN, COS, TAN, SEC, CSC, COT};

struct variable {
    char variable;
    double value;
};

struct variableList {
    var var;
    var* next;
};

struct function {
    char* str;
    functionPart* head;
    varList variables;
};

struct polynomial {
    var variable;
};

struct trigonometry {
    trigType trigType;
    function contents;
};


struct function_part {
    char* str;
    double constant;
    double exponent;
    union part_union {
        functionPart* parenthesis;
        poly poly;
        trig trig;
    } part;
    opType operation;
    functionPart* next;
};

void append(char* string, char c) {
    size_t len = strlen(string);
    string = (char *)realloc(string, len+1);
    if (string == NULL) {
        printf("ERROR: Could not reallocate enough memory for functionBuilder\n");
        return;
    }

    strncat(string, &c, 1);
}

void refresh(char* string) {
    free(string);
    string = malloc(0);
}

function* parseFunction(const char* theFunction) {
    int i;
    char* functionBuilder = malloc(0);

    for (i = 0; i < strlen(theFunction); i++) {
	char c = theFunction[i];

	printf("%c\n", c);
        if (isdigit(theFunction[i])) {
             if (i == 0) {
                  append(functionBuilder, c);	
             } else
		append(functionBuilder, c);       

     
        }        
    }
    printf("before %s\n", functionBuilder);
    refresh(functionBuilder);
    printf("after %s\n", functionBuilder);

    
}

