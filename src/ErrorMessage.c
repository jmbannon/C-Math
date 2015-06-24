#include <stdio.h>
#include <stdlib.h>

void print_error(const char * file,
                 const char * function,
                 const char * message
) {
    printf("ERROR ------- \n");
    printf("[%s] [%s]\n", file, function);
    printf("%s\n", message);
}
