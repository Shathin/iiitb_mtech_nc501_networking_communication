#include <stdio.h>      // Import for `printf`, `scanf` & `perror`
#include <sys/socket.h> // Import for `socket`
#include <sys/types.h>  // Import for `socket`
#include <netinet/ip.h> // Import for `sockaddr_in` struct
#include <unistd.h>     // Import for `_exit`
#include <string.h>     // Import for `bzero`
#include <arpa/inet.h>  // Import for `inet_addr`

#include "./4_server_address.h"

void tcp_connection();
void udp_connection();

void main()
{
    int choice;

    printf("How would you like to connect to the server? \n 1. TCP 2. UDP\nEnter your choice : ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        tcp_connection();
        break;
    case 2:
        udp_connection();
        break;
    default:
        printf("Invalid choice!\n");
    }
}

void tcp_connection()
{
    int socketFD;
    int connectStatus;
    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_TCP);
    serverAddress.sin_port = htons(SERVER_PORT_TCP);

    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1)
    {
        perror("Error while creating TCP client socket!");
        _exit(0);
    }
    printf("TCP client socket created!\n");
    connectStatus = connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectStatus == -1)
    {
        perror("Error connecting to server!");
        _exit(0);
    }
    printf("Successfully connected to server via TCP!\n");

    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[20];

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(socketFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading from server!");
        _exit(0);
    }
    printf("%s\n", readBuffer);

    bzero(writeBuffer, sizeof(writeBuffer));
    scanf("%s", writeBuffer);

    writeBytes = write(socketFD, writeBuffer, sizeof(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to server!");
        _exit(0);
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(socketFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading from server!");
        _exit(0);
    }
    printf("Sorted Sequence : %s\n", readBuffer);
}

void udp_connection()
{
    int socketFD;
    int connectStatus;
    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_UDP);
    serverAddress.sin_port = htons(SERVER_PORT_UDP);

    socketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFD == -1)
    {
        perror("Error while creating TCP client socket!");
        _exit(0);
    }
    printf("UDP client socket created!\n");

    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[20];
    int serverAddressSize = sizeof(serverAddress);

    writeBytes = sendto(socketFD, "1", strlen("1"), MSG_CONFIRM, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (writeBytes == -1)
    {
        perror("Error while writing data to client!");
        _exit(0);
    }

    bzero(readBuffer, sizeof(readBuffer));

    readBytes = recvfrom(socketFD, readBuffer, sizeof(readBuffer), MSG_WAITALL, (struct sockaddr *)&serverAddress, &serverAddressSize);
    if (readBytes == -1)
    {
        perror("Error while receiving host A address from client!");
        _exit(0);
    }

    printf("%s\n", readBuffer);

    bzero(writeBuffer, sizeof(writeBuffer));
    scanf("%s", writeBuffer);

    writeBytes = sendto(socketFD, writeBuffer, strlen(writeBuffer), MSG_CONFIRM, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (writeBytes == -1)
    {
        perror("Error while writing data to client!");
        _exit(0);
    }

    bzero(readBuffer, sizeof(readBuffer));

    readBytes = recvfrom(socketFD, readBuffer, sizeof(readBuffer), MSG_WAITALL, (struct sockaddr *)&serverAddress, &serverAddressSize);
    if (readBytes == -1)
    {
        perror("Error while receiving host A address from client!");
        _exit(0);
    }
    printf("Sorted Sequence : %s\n", readBuffer);
}
