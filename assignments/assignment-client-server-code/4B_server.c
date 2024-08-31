#include <stdio.h>      // Import for `printf`, `scanf` & `perror`
#include <string.h>     // Import for string functions
#include <stdlib.h>     // Import for `atoi`
#include <sys/socket.h> // Import for `socket`, `bind`, `listen` `accept`
#include <sys/types.h>  // Import for `socket`, `bind`, `listen` `accept`
#include <netinet/ip.h> // Import for `sockaddr_in` struct
#include <unistd.h>     // Import for `_exit`
#include <arpa/inet.h>  // Import for `inet_addr`

#include "./4_server_address.h"

#define WELCOME_MESSAGE_TCP "Hello there client! You're now connected to the server via TCP!\n"
#define WELCOME_MESSAGE_UDP "Hello there client! You're now connected to the server via UDP!\n"
#define GAME_MESSAGE "Send me five or more alphabets & I'll sort them for you!\n"
#define SEQUENCE_PROMPT "What are is the sequence to be sorted? "

void tcp_connection_handler(int connectionFD);
void udp_connection_handler(int socketFD);
int comparator(const void *a, const void *b);

void main()
{
    int socketFD, connectionFD;
    struct sockaddr_in serverAddress, clientAddress;
    int bindStatus; // Determines the success of the `bind` system call
    int option = 1;

    serverAddress.sin_family = AF_INET;

    // A call to `fork` is used to create a child process. The reason for this being, we want to create two server's, one that lets the client connect via TCP and the other than let's the client connect via UDP
    // The parent process will use TCP while the child will use UDP
    if (!fork())
    {
        // Child process will enter this branch
        printf("A UDP server is being setup on the process with PID : %d\n", getpid());

        socketFD = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketFD == -1)
        {
            perror("Error while creating UDP socket!");
            _exit(0);
        }
        printf("UDP Server socket successfully created!\n");

        serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_UDP);
        serverAddress.sin_port = htons(SERVER_PORT_UDP);

        bindStatus = bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
        if (bindStatus == -1)
        {
            perror("Error while binding the address to the UDP server socket!");
            _exit(0);
        }
        if (setsockopt(socketFD, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0)
        {
            perror("Error while trying to resuse socket!");
            _exit(0);
        }
        printf("UDP Server address successfully bound to the server socket!\n");

        udp_connection_handler(socketFD);
    }
    else
    {
        // Parent process will enter this branch

        int listenStatus; // Determines the success of the `listen` system call
        int connectionFD; // A file descriptor used for communication with the client

        printf("A TCP server is being setup on the process with PID : %d\n", getpid());

        /*
            AF_INET -> Communicate using the network via IPv4
            SOCK_STREAM -> Send data as streams a.k.a. use TCP
        */
        socketFD = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFD == -1)
        {
            perror("Error while creating TCP socket!");
            _exit(0);
        }
        printf("TCP Server socket successfully created!\n");

        serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_TCP);
        serverAddress.sin_port = htons(SERVER_PORT_TCP);
        bindStatus = bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
        if (bindStatus == -1)
        {
            perror("Error while binding the address to the server socket!");
            _exit(0);
        }
        if (setsockopt(socketFD, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0)
        {
            perror("Error while trying to resuse socket!");
            _exit(0);
        }
        printf("TCP Server address successfully bound to the server socket!\n");

        listenStatus = listen(socketFD, 10);
        if (listenStatus == -1)
        {
            perror("Error while listening to the network via the server socket!");
            close(socketFD);
            _exit(0);
        }
        printf("TCP Server is now listening for client connections!\n");

        while (1)
        {
            int clientAddressSize = sizeof(clientAddress);
            connectionFD = accept(socketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);
            if (connectionFD == -1)
                perror("Error while accepting client connection!");
            else
            {
                if (!fork())
                {
                    printf("Client successfully connected to server via TCP!\n");
                    tcp_connection_handler(connectionFD); // Delegates the communication part to another function
                }
            }
        }
    }
}

void tcp_connection_handler(int connectionFD)
{

    ssize_t readBytes, writeBytes;           // Number of bytes read from / sent to the client
    char readBuffer[100], writeBuffer[1000]; // A character buffer used for read from / writing to the client

    sprintf(writeBuffer, "%s%s%s", WELCOME_MESSAGE_TCP, GAME_MESSAGE, SEQUENCE_PROMPT);

    writeBytes = write(connectionFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing data to client!");
        _exit(0);
    }

    bzero(readBuffer, sizeof(readBuffer));

    readBytes = read(connectionFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading data from the client!");
        _exit(0);
    }

    printf("The client sent the sequence as : %s\n", readBuffer);

    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, readBuffer);
    qsort(writeBuffer, strlen(writeBuffer), sizeof(char), comparator);

    writeBytes = write(connectionFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to client!");
        _exit(0);
    }
}

void udp_connection_handler(int socketFD)
{
    ssize_t readBytes, writeBytes;           // Number of bytes read from / sent to the client
    char readBuffer[100], writeBuffer[1000]; // A character buffer used for read from / writing to the client

    while (1)
    {
        struct sockaddr_in clientAddress;
        int clientAddressSize = sizeof(clientAddress);

        sprintf(writeBuffer, "%s%s%s", WELCOME_MESSAGE_UDP, GAME_MESSAGE, SEQUENCE_PROMPT);

        readBytes = recvfrom(socketFD, readBuffer, 1, MSG_WAITALL, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (readBytes == -1)
        {
            perror("Error while receiving initial data from client!");
            _exit(0);
        }
        writeBytes = sendto(socketFD, writeBuffer, strlen(writeBuffer), MSG_CONFIRM, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
        if (writeBytes == -1)
        {
            perror("Error while writing data to client!");
            _exit(0);
        }

        readBytes = recvfrom(socketFD, readBuffer, sizeof(readBuffer), MSG_WAITALL, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (readBytes == -1)
        {
            perror("Error while receiving sequence from client!");
            _exit(0);
        }
        printf("The client sent the sequence as : %s\n", readBuffer);

        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, readBuffer);
        qsort(writeBuffer, strlen(writeBuffer), sizeof(char), comparator);

        writeBytes = sendto(socketFD, writeBuffer, strlen(writeBuffer), MSG_CONFIRM, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
        if (writeBytes == -1)
        {
            perror("Error while writing data to client!");
            _exit(0);
        }
    }
}

int comparator(const void *a, const void *b)
{
    return (*(char *)a - *(char *)b);
}