#ifndef PARSEUTILITIES_H_
#define PARSEUTILITIES_H_

#include "../Function.h"
#include "Parse.h"

#define HAS_OP  (0)
#define HAS_NEG (1)
#define HAS_NUM (2)
#define HAS_DEC (3)
#define HAS_EXP (4)
#define HAS_VAR (5)
#define IS_PAR  (6)
#define IS_FUN  (7)

#define SET_TRUE(BOOL, value) (BOOL |= 1 << value)
#define SET_FALSE(BOOL, value) (BOOL &= ~(1 << value))
#define CHECK(BOOL, value) (BOOL & 1 << value)

void print_parse_error(
  const char * description,
  const char * file_name,
  const char * function,
        char * error_loc_buffer,
        int    index
);

char * err_loc_msg(
  const char * function,
        char * error_loc_buffer,
        int index
);

part_type get_part(
        unsigned int B_BOOLS,
        part_type    temp_part
);

op get_op(
        char c
);

#endif
