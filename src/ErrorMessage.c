#include <stdio.h>
#include <stdlib.h>

void parse_error(const char * file,
                 const char * function,
                 const char * description,
                 const char * function_location
) {
    printf(" * PARSING ERROR *\n");
    printf(" * File: %s : Function: %s\n", file, function);
    printf(" * %s\n", description);
    printf(" * %s\n", function_location);
}
