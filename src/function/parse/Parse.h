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


part * parse_part(
  const char     * part_str,
        function * root_func
);

#endif
