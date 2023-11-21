#ifndef MYPROT_H
#define MYPROT_H

// ********** PROTOCOL ********** //

#define HELLO_A "HELLOA"
#define HELLO_B "HELLOB"

#define REQUEST_DATA "GDATA"
#define SEND_DATA_A "SDATAA"
#define SEND_DATA_B "SDATAB"

#define A_SERVER 0
#define B_SERVER 1

#define MAX_BUFF sizeof(struct Protocol)

struct Protocol
{
    char type[7];
    char data[4];
};

// ********** PROTOCOL FUNCTIONS ********** //

int read_hello(char *buffer, char *type, int *data);

int read_data(char *buffer, char *type, int *data);

#endif
