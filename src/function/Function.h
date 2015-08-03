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

typedef enum part_type_
        part_type;

typedef enum operations_
        op;

typedef struct function 
        function;

typedef union base_union
        base;

typedef struct function_part 
        part;

typedef struct variable 
        var;

typedef struct numeric 
        num;

typedef struct other_function 
        fun;

/* 
 =======================================================================

        Enums

 =======================================================================
*/

enum operations_
{
        NOOP,

        ADD, // Add 
        SUB, // Subtract
        MUL, // Multiply
        DIV, // Divide

        MOD  // Modular
};

enum part_type_
{
        NOPART, 
        PAR, // Parenthesis
        VAR, // Variable
        NUM, // Number

        LOG, // Logarithm
        LN,  // Natural Logarithm

        SIN, // Sine
        COS, // Cosine
        TAN, // Tangent
        SEC, // Secant
        CSC, // Cosecant
        COT, // Cotangent

        ASIN,// Inverse Sine
        ACOS,// Inverse Cosine
        ATAN,// Inverse Tangent

        ABS, // Absolute Value
        SQRT // Square Root
};

/*
 ======================================================================

        Structures

 ======================================================================
*/

struct function 
{
    char     * str;
    function * root_func;
    var      * var_list;
    part     * head;
};

struct variable
{
    char       variable;
    double   * num;
    function * func;    
};

struct other_function 
{
    part_type type;
    part * contents;
};

union base_union 
{
    part * par;
    var var;
    double num;
    fun fun;
};

struct function_part 
{
    function  * func;
    char      * str;
    part_type   base_type;
    part_type   exp_type;
    base        base;
    base        exponent;
    op          operation;
    part      * prev;
    part      * next;
};

/* 
 =======================================================================

        Function Prototypes

 =======================================================================
*/


static int VAR_LIST_SIZE = 64;

function * initializeFunction(
        const char * theFunction
);

void printInfo(
        function * theFunction
);

part_type is_func_type(
        const char * firstLetter
);

void addToFunctionList(
        function  * func, 
        char      * functionBuilder,
  const part_type   base_type,
  const part_type   exp_type,
  const op     operation
);

void addToVariableList(
        function * theFunction,
        char       variable
);

#endif

