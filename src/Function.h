#ifndef FUNCTION_H_
#define FUNCTION_H_

typedef enum operations opType;
typedef enum trigFunctions trigType;
typedef struct function function;
typedef struct function_part functionPart;
typedef struct function_part_base base;

typedef struct variable var;
typedef struct variableList varList;

typedef struct numeric num;
typedef struct trigonometry trig;
typedef struct logarithm log;

enum operations {NOOP, ADD, SUB, MUL, DIV, EXP};
enum trigFunctions {NOTRIG, SIN, COS, TAN, SEC, CSC, COT};
enum partType {NOPART, PARENTHESIS, NUM, TRIG, LOG};

struct variable {
    char variable;
    int prime;
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

struct numeric {
    double constant;
};

struct trigonometry {
    trigType trigType;
    functionPart* contents;
};

struct logarithm {
    double base;
    functionPart* contents;
};

struct function_part {
    char* str;
    union part_union 
    {
        functionPart* parenthesis;
        num* num;
        trig* trig;
        log* log;
    } part;
    
    union part_exponent 
    {
        num* num;
        functionPart* parenthesis;
    } exponent;

    opType operation;
    functionPart* prev;
    functionPart* next;
};

void printInfo(function* theFunction);
trigType isTrigFunction(const char* firstLetter);
void addToFunctionList(functionPart** head, char* functionBuilder,
                       const opType operation);

#endif

