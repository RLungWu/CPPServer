#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void errif(bool condition, const char* error_messages){
    if(condition){
        perror(error_messages);
        exit(EXIT_FAILURE);
    }
}