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
        op_type;

typedef struct function 
        function;

typedef union base_union
        base;

typedef struct function_part 
        functionPart;

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

        ADD, 
        SUB, 
        MUL, 
        DIV,

        MOD
};

enum part_type_
{
        NOPART, 
        PAR,
        VAR,
        NUM,

        LOG,
        LN,

        SIN,
        COS,
        TAN,
        SEC,
        CSC,
        COT,

        SQRT
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

part_type isTrigFunction(
        const char * firstLetter
);

void addToFunctionList(
        function * func, 
        char * functionBuilder,
        const part_type type,
        const op_type operation
);

void addToVariableList(
        function * theFunction,
        char variable
);

#endif

