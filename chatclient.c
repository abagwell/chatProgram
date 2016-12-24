/*
Name: Andrew Bagwell
Date: 04/30/2016
Assignment: Project1
File: chatclient.c
Language: C
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

//constants

#define HANDLE_MAX 10
#define MESS_LGTH 500

//prototypes

void runClient(char*, char*);
void doChat(int, char*);
void signalIntHandle(int);

int main(int argc, char *argv[]) {


    //command line constraints

    if (argc != 3) {
        fprintf(stderr,"usage: chatclient [hostname] [port number]\n");
        exit(1);
    }

    //handle signal interruption..

    signal(SIGINT, signalIntHandle);

    runClient(argv[1], argv[2]);

    return 0;

    }

//---------------------chat function----------
    //This function takes the socket and the user handle as parameters. T
    //This function handles chat conversation between the client and the server hosts. 
//--------------------------------------------

void doChat(int socketPassed, char* userHandle) {

    //create the containers to hold the messages

    char msgRcvd[MESS_LGTH];
    char msgSent[MESS_LGTH];

    //the chat continues until one side quits or a connection is lost

    while (1) {

    //send message to server

        char preCatMsg[500];    //this will store the message typed from the console...

        //get message from user input

        printf("%s> ", userHandle);
        fgets(preCatMsg, MESS_LGTH, stdin);

        char *lastChar;

        //strip off the end line character 

         if ((lastChar=strchr(preCatMsg, '\n')) != NULL)
                *lastChar = '\0';

        //combine the user entered message with the handle...

        sprintf(msgSent, "%s> %s", userHandle, preCatMsg);

        //send that combined message to client...

        send(socketPassed, msgSent, MESS_LGTH, 0);

        //if server wants to close, it's closed here

        if(strcmp(preCatMsg, "\\quit")==0) {

            printf("Goodbye...Thanks for chatting.\n");
            return;
        }

    //get the message from server
      
        recv(socketPassed, msgRcvd, MESS_LGTH, 0);

         //determine if message is a quit message, using substring

        char *phrase; 
        if ((phrase = strstr(msgRcvd, "\\quit"))!=NULL) {
            printf("Connection closed by user..\n");
            return;
        }

    //display message from client

        printf("%s\n", msgRcvd);

    }

}

//---------------------client function----------
    //This function takes the hostName and the port passed as parameters.
    //This is the driver function of the program and is called within main().
//--------------------------------------------



void runClient(char* hostName, char* portPassed) {

    //create structs to get info about server...

    struct addrinfo hints;
    struct addrinfo* servInfo;
    int hostSocket;
    char handle[10]; //handle limit is 10 chars

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    //check for errors

    int errCheck;
    if ((errCheck = getaddrinfo(hostName, portPassed, &hints, &servInfo)) != 0) {

        fprintf(stderr, "Error -getaddinfo failure: %s\n", gai_strerror(errCheck));
        exit(1);
    }

    //create host/client socket with call to socket()

    hostSocket = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);

    //error check

     if (hostSocket == -1) {
        exit(1);
    }

    //connect to the server

    int status;
    status = connect(hostSocket, servInfo->ai_addr, servInfo->ai_addrlen);

    if (status == -1) {
        exit(1);
    }

    printf("Connected...to %s\n", hostName);

    freeaddrinfo(servInfo); //no longer needed..

    //get the handle of the user here......

    printf("Please enter a handle of %d characters or less: ", HANDLE_MAX);
    fgets(handle, HANDLE_MAX, stdin);

    //strip the newline symbol off and replace with /0
    char *lastChar;

    if ((lastChar=strchr(handle, '\n')) != NULL)
        *lastChar = '\0';

    printf("\n");
    
    //call chat function

    doChat(hostSocket, handle);

    //close socket

    close(hostSocket);

    return;

}

//---------------------siginal function----------
    //This function takes the signal received as a parameter
    //This is function is called in conjunction with SIGNAL. It handles the cleanup from that occurence.
//--------------------------------------------

void signalIntHandle(int sigReceived) {

    printf("\nConnection lost....signal interrupted.\n");
    
    exit(0);
}


