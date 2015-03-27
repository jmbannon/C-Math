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


function* initializeFunction(const char* theFunction);
functionPart** getHead(function* theFunction);
void printInfo(function* theFunction);
trigType isTrigFunction(const char* firstLetter);
void addToFunctionList(functionPart** head, char* functionBuilder,
                       const opType operation);

#endif

