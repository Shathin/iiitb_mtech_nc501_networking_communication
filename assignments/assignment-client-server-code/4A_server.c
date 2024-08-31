/*
    Question : Write code for receiving IP addresses and subnet mask from client. Also, implement logic for finding out if both IP addresses belong to the same network and send the result back to the client.
*/

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
#define GAME_MESSAGE "Send two IP address & a subnet mask to me and I will let you know whether they belong to the same network defined by the subnet mask or not!\n"
#define FIRST_ADDRESS_PROMPT "What is the IP address of the first host? "
#define SECOND_ADDRESS_PROMPT "What is the IP address of the second host? "
#define SUBNET_MASK_PROMPT "What is the subnet mask that I must use? "

void tcp_connection_handler(int connectionFD);
void udp_connection_handler(int socketFD);
short isHostInSameNetwork(char *hostA, char *hostB, char *subnetMask);
void *splitIPintoOctets(char *host, int bits[4]);

void main()
{
    int socketFD, connectionFD;
    struct sockaddr_in serverAddress, clientAddress;
    int bindStatus; // Determines the success of the `bind` system call
    int option = 1;

    // Define the server's address
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

    ssize_t readBytes, writeBytes;                                // Number of bytes read from / sent to the client
    char hostA[20], hostB[20], subnetMask[20], writeBuffer[1000]; // A character buffer used for read from / writing to the client

    sprintf(writeBuffer, "%s%s%s", WELCOME_MESSAGE_TCP, GAME_MESSAGE, FIRST_ADDRESS_PROMPT);

    writeBytes = write(connectionFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing data to client!");
        _exit(0);
    }

    bzero(hostA, sizeof(hostA));

    readBytes = read(connectionFD, hostA, sizeof(hostA));
    if (readBytes == -1)
    {
        perror("Error while reading data from the client!");
        _exit(0);
    }

    printf("The client sent the first IP address as : %s\n", hostA);

    writeBytes = write(connectionFD, SECOND_ADDRESS_PROMPT, strlen(SECOND_ADDRESS_PROMPT));
    if (writeBytes == -1)
    {
        perror("Error while writing data to client!");
        _exit(0);
    }

    bzero(hostB, sizeof(hostB));

    readBytes = read(connectionFD, hostB, sizeof(hostB));
    if (readBytes == -1)
    {
        perror("Error while reading data from the client!");
        _exit(0);
    }

    printf("The client sent the second IP address as : %s\n", hostB);

    writeBytes = write(connectionFD, SUBNET_MASK_PROMPT, strlen(SUBNET_MASK_PROMPT));
    if (writeBytes == -1)
    {
        perror("Error while writing data to client!");
        _exit(0);
    }

    bzero(subnetMask, sizeof(subnetMask));

    readBytes = read(connectionFD, subnetMask, sizeof(subnetMask));
    if (readBytes == -1)
    {
        perror("Error while reading data from the client!");
        _exit(0);
    }

    printf("The client sent the subnet mask as : %s\n", subnetMask);

    bzero(writeBuffer, sizeof(writeBuffer));

    if (isHostInSameNetwork(hostA, hostB, subnetMask))
        strcpy(writeBuffer, "The the first host and the second host belong to the same network as per the given subnet mask!");
    else
        strcpy(writeBuffer, "The the first host and the second host don't belong to the same network as per the given subnet mask!");
    strcat(writeBuffer, "\n\nGood bye!");

    writeBytes = write(connectionFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to client!");
        _exit(0);
    }
}

void udp_connection_handler(int socketFD)
{
    ssize_t readBytes, writeBytes;                                                  // Number of bytes read from / sent to the client
    char dummyBuffer[100], hostA[20], hostB[20], subnetMask[20], writeBuffer[1000]; // A character buffer used for read from / writing to the client

    while (1)
    {
        struct sockaddr_in clientAddress;
        int clientAddressSize = sizeof(clientAddress);

        sprintf(writeBuffer, "%s%s%s", WELCOME_MESSAGE_UDP, GAME_MESSAGE, FIRST_ADDRESS_PROMPT);

        readBytes = recvfrom(socketFD, dummyBuffer, 1, MSG_WAITALL, (struct sockaddr *)&clientAddress, &clientAddressSize);
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

        readBytes = recvfrom(socketFD, hostA, sizeof(hostA), MSG_WAITALL, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (readBytes == -1)
        {
            perror("Error while receiving host A address from client!");
            _exit(0);
        }

        writeBytes = sendto(socketFD, SECOND_ADDRESS_PROMPT, strlen(SECOND_ADDRESS_PROMPT), MSG_CONFIRM, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
        if (writeBytes == -1)
        {
            perror("Error while writing data to client!");
            _exit(0);
        }

        readBytes = recvfrom(socketFD, hostB, sizeof(hostB), MSG_WAITALL, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (readBytes == -1)
        {
            perror("Error while receiving host B address from client!");
            _exit(0);
        }
        writeBytes = sendto(socketFD, SUBNET_MASK_PROMPT, strlen(SUBNET_MASK_PROMPT), MSG_CONFIRM, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
        if (writeBytes == -1)
        {
            perror("Error while writing data to client!");
            _exit(0);
        }

        readBytes = recvfrom(socketFD, subnetMask, sizeof(subnetMask), MSG_WAITALL, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (readBytes == -1)
        {
            perror("Error while receiving subnet mask from client!");
            _exit(0);
        }

        bzero(writeBuffer, sizeof(writeBuffer));

        if (isHostInSameNetwork(hostA, hostB, subnetMask))
            strcpy(writeBuffer, "The the first host and the second host belong to the same network as per the given subnet mask!");
        else
            strcpy(writeBuffer, "The the first host and the second host don't belong to the same network as per the given subnet mask!");
        strcat(writeBuffer, "\n\nGood bye!");

        writeBytes = sendto(socketFD, writeBuffer, strlen(writeBuffer), MSG_CONFIRM, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
        if (writeBytes == -1)
        {
            perror("Error while writing data to client!");
            _exit(0);
        }
    }
}

// 0 -> False, 1 -> True
short isHostInSameNetwork(char *hostA, char *hostB, char *subnetMask)
{

    int hostABits[4], hostBBits[4], subnetMaskBits[4];
    int iter;

    // Extracting the octets from the given string IP addresses
    splitIPintoOctets(hostA, hostABits);
    splitIPintoOctets(hostB, hostBBits);
    splitIPintoOctets(subnetMask, subnetMaskBits);

    for (iter = 0; iter < 4; iter++)
        if ((hostABits[iter] & subnetMaskBits[iter]) != (hostBBits[iter] & subnetMaskBits[iter]))
            return 0;

    return 1;
}

void *splitIPintoOctets(char *host, int bits[4])
{
    int iter = 1;
    bits[0] = atoi(strtok(host, "."));
    while (iter < 4)
    {
        bits[iter] = atoi(strtok(NULL, "."));
        iter++;
    }
}
