/* 
 =======================================================================
 Parse.h

 Jesse Bannon

 TODO
 =======================================================================
*/

#ifndef PARSE_H_
#define PARSE_H_
#include "../Function.h"

function * parseFunction(
        const char * func_str,
        function   * root_func
);

functionPart * parseFunctionPart(
        const char * functionStr,
        function   * root_func
);

#endif
