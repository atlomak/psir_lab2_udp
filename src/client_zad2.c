#include <arpa/inet.h>
#include <bits/types/clock_t.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include "myprot.h"
#include <sys/select.h>

//**** SERVER MANAGEMENT ****//

struct Server
{
    struct sockaddr_in server_ip;
    char id[2];
    long last_hello;
};

struct Server servers[2] = {0};

int add_server(struct Server servers[], char *id,
               struct sockaddr_in new_server);

void removeInactiveServers(struct Server *servers);

//**** UDP ****//

int setup_socket(struct addrinfo **r, const char *port, int *soc);

void cleanup(int soc, struct addrinfo *r);

//**** MAIN ****//

int main()
{
    struct addrinfo *r = NULL;
    int soc;

    struct Server servers[2] = {0};

    socklen_t saddr_len = sizeof(struct sockaddr_in);

    int pos;
    struct timeval waitThreshold;

    if (setup_socket(&r, "28848", &soc) != 0)
    {
        printf("ERROR: %s\n", strerror(errno));
        return 1;
    }

    fd_set readfds;
    waitThreshold.tv_sec = 0;
    waitThreshold.tv_usec = 5000;
    int p;

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(soc, &readfds);
        p = select(soc + 1, &readfds, NULL, NULL, &waitThreshold);

        if (FD_ISSET(soc, &readfds))
        {

            // Receive message from server

            char received_buffer[MAX_BUFF]; // Buffer for received message header=10 +
                                            // data
            struct sockaddr_in received_server;

            if ((pos = recvfrom(soc, received_buffer, MAX_BUFF, MSG_WAITALL,
                                (struct sockaddr *)&received_server, &saddr_len)) <
                0)
            {
                printf("ERROR: %s\n", strerror(errno));
            }
            else
            {

                int data;
                char id[7];

                if (read_hello(received_buffer, id, &data) == 0)
                {
                    printf("Got hello: %s\n", id);
                    add_server(servers, id, received_server);
                }
                else if (read_data(received_buffer, id, &data) == 0)
                {
                    printf("Got data A: %s %d\n", id, data);
                }
            }
        }
        else
        {
            removeInactiveServers(servers);
        }
    }

    cleanup(soc, r);

    return 0;
}

int setup_socket(struct addrinfo **r, const char *port, int *soc)
{
    struct addrinfo h;

    memset(&h, 0, sizeof(struct addrinfo));
    h.ai_family = PF_INET;
    h.ai_socktype = SOCK_DGRAM;
    h.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &h, r) != 0)
    {
        return 1;
    }

    *soc = socket((*r)->ai_family, (*r)->ai_socktype, (*r)->ai_protocol);

    if (*soc == -1)
    {
        return 1;
    }
    if (bind(*soc, (*r)->ai_addr, (*r)->ai_addrlen) != 0)
    {
        return 1;
    }
    return 0;
}

void cleanup(int soc, struct addrinfo *r)
{
    freeaddrinfo(r);
    close(soc);
}

int add_server(struct Server *servers, char *id,
               struct sockaddr_in new_server)
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    long currentMillis = currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000;

    if (strncmp(id, HELLO_A, 6) == 0)
    {
        if (servers[0].last_hello == 0)
        {
            servers[0].id[0] = 'A';
            printf("Added server A\n");
        }
        servers[0].last_hello = currentMillis;
        servers[0].server_ip = new_server;
        return 0;
    }
    else if (strncmp(id, HELLO_B, 6) == 0)
    {
        if (servers[1].last_hello == 0)
        {
            servers[1].id[0] = 'B';
            printf("Added server B\n");
        }
        servers[1].last_hello = currentMillis;
        servers[1].server_ip = new_server;
        return 0;
    }
    return 1;
}

void removeInactiveServers(struct Server *servers)
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    long currentMillis = currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000;

    for (int i = 0; i < 2; ++i)
    {
        if ((currentMillis - servers[i].last_hello) > 11550 && servers[i].last_hello != 0)
        {
            servers[i].last_hello = 0; // Assuming 0 indicates an uninitialized or removed server
            printf("Removed server %s\n", servers[i].id);
        }
    }
}