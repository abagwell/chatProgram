/*
Name: Andrew Bagwell
Date: 04/30/2016
Assignment: Project1
File: chatserv.cpp
Language: C++
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

//constants 

#define BACKLOG  20
#define MESS_LGTH 500
#define SERVER_HDLE "Serv"

//function prototypes

void buildServer(char*);
void doChat(int);
void signalIntHandle(int);



int main(int argc, char *argv[]) {

    //command line constraints

    if (argc != 2) {
            fprintf(stderr,"usage: chatserv [port number]\n");
            exit(1);
        }

    //handle the possibility of signal interruption..

    signal(SIGINT, signalIntHandle);
    
    buildServer(argv[1]);
    
    return 0;

}

//------------------chat function--------------------------
//This function handles the actual chatting between client and server
//---------------------------------------------------------

void doChat(int socketPassed) {

    char msgRcvd[MESS_LGTH];
    char msgSent[MESS_LGTH];
    printf("\n");
    printf("Beginning new chat session, wait for client message and then respond. Type \\quit to quit chat.\n\n");

    while (1) {

    //get the message from client

        recv(socketPassed, msgRcvd, MESS_LGTH, 0);

         //determine if message is a quit message

        char *phrase; 
        if ((phrase = strstr(msgRcvd, "\\quit"))!=NULL) {
            printf("Connection closed by user..\n");
            return;
        }

    //display message from client

        printf("%s\n", msgRcvd); //changed something here...

    //send message to client

        char preCatMsg[500];    //this will store the message typed from the console...

        std::cout << SERVER_HDLE << "> ";
        fgets(preCatMsg, MESS_LGTH, stdin);

        char *lastChar;

         if ((lastChar=strchr(preCatMsg, '\n')) != NULL)
                *lastChar = '\0';

        //combine the user entered message with the handle...

        sprintf(msgSent, "%s> %s", SERVER_HDLE, preCatMsg);   //changed something here

        //send that combined message to client...

        send(socketPassed, msgSent, MESS_LGTH, 0);

        //if server wants to close, it's closed here

        if(strcmp(preCatMsg, "\\quit")==0) {

            std::cout << "Goodbye.." << std::endl;
            return;
        }

    }

}

//----------------------BUILD SERVER FUNCTION----------------
//This is the main driver function of the program. 

//-----------------------------------------------------------

void buildServer(char* portPassed) {

//create structs for storing information about server

    struct addrinfo hints;
    struct addrinfo* servInfo;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    //check for errors in getaddrinfo function

    int errCheck;
    if ((errCheck = getaddrinfo(NULL, portPassed, &hints, &servInfo)) != 0) {

        fprintf(stderr, "Error -getaddinfo failure: %s\n", gai_strerror(errCheck));
        exit(1);
    }

//This is where the socket for the server is created...

    int serverSocket = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);

    //check for errors in socket function

    if (serverSocket == -1) {
        exit(1);
    }

    //bind the socket with bind function

    errCheck = bind(serverSocket, servInfo->ai_addr, servInfo->ai_addrlen);
    if (errCheck == -1) {
        fprintf(stderr, "Error bind() failure");
        exit(1);
    }

    //begin listening for conections

    errCheck = listen(serverSocket, BACKLOG);

    //check for errors in the listen() function

    if (errCheck == -1) {
        fprintf(stderr, "Error listen() failure");
        exit(1);
    }

    std::cout << "Server up and listening on: " << portPassed << std::endl;

    //server begins listening and continues to do so until exiting...

    while(1) {

        printf("\n");
        printf("Waiting for connection..\n");
        
        //client socket info is obtained...

        struct sockaddr_storage clientAddress;
        socklen_t addressSize = sizeof(clientAddress);

        int connectionSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &addressSize);

        if (connectionSocket == -1) {
                std::cout << "Connection failed.." << std::endl;
                exit(1);
            }

        doChat(connectionSocket);   //call this function to do the chat

        close(connectionSocket);

    }

    freeaddrinfo(servInfo);

    return;

}

void signalIntHandle(int sigReceived) {

    printf("\nConnection lost....signal interrupted.\n");
    exit(0);
}




