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

function * parse(
  const char * func_str
);


functionPart * parseFunctionPart(
  const char     * functionStr,
        function * root_func
);

#endif
