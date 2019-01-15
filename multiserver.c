//
//  multiserver.c
//
//
//  Created by Niyaz Murshed on 2016-12-15.
//
//

#include <stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include "roadmap.h"

void *connection_handler(void *);

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    
    //retrive the map from the file
    
    retrieve(FILENAME);
    
    
    
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
    
    //Listen
    listen(socket_desc , 3);
    
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    
    
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
        
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
        
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*)new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        
        //Now join the thread , so that we dont terminate before the thread
        // pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
    
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    
    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
    
    //Send some messages to the client
    message = "Now type something and i shall repeat what you type";
    write(sock , message , strlen(message));
    //  memset(message, 0, sizeof(message));
    
    
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        char *tmp = NULL;
        char *routemap = NULL;
        char sp[30],des[30],t1[30],t2[30];
        int size = 0;
        char *route = NULL;
        tmp = strtok (client_message,"|");
        strcpy(sp,tmp);
        printf ("sp : %s\n",sp);
        tmp = strtok (NULL, "|");
        strcpy(des,tmp);
        printf ("des : %s\n",des);
        
        
        // TODO: Check return
       
        strcpy(t1,vertex(sp));
        strcpy(t2,vertex(des));
        trip(t1,t2, &route, &size);
        
        if(size > 0) {
            printf("Size: %d\n", size);
        

            //Send the message back to client
            write(sock, route, size);
            free(route);
            
        } else {
            
            //Send the message back to client
            write(sock , client_message , strlen(client_message));
            memset(client_message, 0, sizeof(client_message));
        }
        
        
        
        
    }
    
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    
    //Free the socket pointer
    free(socket_desc);
    
    return 0;
}
