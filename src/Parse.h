/* 
 =======================================================================
 Parse.h

 Jesse Bannon

 TODO
 =======================================================================
*/

#ifndef PARSE_H_
#define PARSE_H_
#include "Function.h"

function * parseFunction(
        const char * theFunction,
        var * root_var_list
);

functionPart * parseFunctionPart(
        const char * functionStr,
        var * root_var_list
);

#endif
