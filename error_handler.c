#include "error_handler.h"

void handle_error(const char *message){
    perror(message);
    exit(errno);
}
