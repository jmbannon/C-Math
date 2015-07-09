/* 
 =======================================================================
 Function.h

 Jesse Bannon

 TODO
 =======================================================================
*/

#ifndef FUNCTION_H_
#define FUNCTION_H_

/* 
 =======================================================================

        Typedefs

 =======================================================================
*/

typedef enum operations 
        opType;

typedef enum trigFunctions 
        trigType;

typedef struct function 
        function;

typedef union base_union
        base;

typedef union exponent_union
        exponent;

typedef struct function_part 
        functionPart;

typedef struct variable 
        var;

typedef struct numeric 
        num;

typedef struct trigonometry 
        trig;

typedef struct logarithm 
        log;

/* 
 =======================================================================

        Enums

 =======================================================================
*/

enum operations
{
        NOOP, 
        ADD, 
        SUB, 
        MUL, 
        DIV
};

enum trigFunctions
{
        NOTRIG, 
        SIN, 
        COS, 
        TAN, 
        SEC, 
        CSC, 
        COT
};

enum partType
{
        NOPART, 
        PARENTHESIS,
        NUM, 
        TRIG, 
        LOG
};

/* 
 =======================================================================

        Function Prototypes

 =======================================================================
*/

function * initializeFunction(
        const char * theFunction
);

functionPart * getHead(
        function * theFunction
);

var * get_var_list(
        function * func
);

void set_var_list(
         function * func,
         var * var_list
);

void printInfo(
        function * theFunction
);

trigType isTrigFunction(
        const char * firstLetter
);

void addToFunctionList(
        function * func, 
        char * functionBuilder,
        const opType operation
);

void addToVariableList(
    function * theFunction,
    char variable
);

#endif

