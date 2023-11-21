#include "myprot.h"
#include <string.h>
#include <stdlib.h>

int read_hello(char *buffer, char *type, int *data) {

    struct Protocol *proto = (struct Protocol *)buffer;

    if (!strcmp(HELLO_A, proto->type) || !strcmp(HELLO_B, proto->type)) {
        strncpy(type, proto->type, 7);
        *data = atoi(proto->data);
        return 0;
    }
    return 1;
}

int read_data(char *buffer, char *type, int *data) {

    struct Protocol *proto = (struct Protocol *)buffer;

    if (!strcmp(SEND_DATA_A, proto->type)) {
        strncpy(type, proto->type, 7);
        *data = atoi(proto->data);
        return 0;
    }
    return 1;
}