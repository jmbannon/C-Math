#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ParseUtilities.h"
#include "../Function.h"
#include "../../utilities/ErrorMessage.h"

void print_parse_error(
  const char * description,
  const char * file_name,
  const char * function,
        char * error_loc_buffer,
        int    index
) {
    parse_error(
        file_name,
        "parse_function",
        description,
        err_loc_msg(function, error_loc_buffer, index)); 
}

part_type get_part(
        unsigned int B_BOOLS,
        const part_type temp_part
) {
    if (CHECK(B_BOOLS, HAS_VAR))
        return VAR;
    else if (CHECK(B_BOOLS, HAS_NUM))
        return NUM;
    else if (CHECK(B_BOOLS, IS_FUN))
        return temp_part;
    else if (CHECK(B_BOOLS, IS_PAR))
        return PAR;
    else
        return NOPART;
}

op_type get_op(
        char c
) {
    switch(c)
    {
        case '+': return  ADD;
        case '-': return  SUB;
        case '/': return  DIV;
        case '\0': return NOOP;
        default: return MUL;
    }
}

/* Returns the string of the current function and the index the error
 * was thrown.
 */ 
char * err_loc_msg(
  const char * function,
        char * error_loc_buffer,
        int    index
) {
    if (index >= 0)
        sprintf(error_loc_buffer, 
            "Function: %s : Index: %d", function, index);
    else
        sprintf(error_loc_buffer, 
            "  Function: %s : Index: end of function", function);
    return error_loc_buffer;
}
