//
//  server1.c
//  
//
//  Created by Niyaz Murshed on 2016-12-13.
//
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    
    int sock;
    struct sockaddr_in server;
    int mysock;
    char buff[1024];
    int rval;
    
    
    //create socket
    
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        perror("FAILED TO CREATE A SOCKET");
        exit(0);
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5000);
    
    if(bind(sock, (struct sockaddr *)&server, sizeof(server))){
        perror("Bind Failed");
        exit(1);

    }
    
    //Listen
    
    listen(sock,5);
    
    //Accept
    
    do{
        mysock=accept(sock,(struct sockaddr *) 0,0);
        if (mysock==-1) {
            perror("Accept Failed");
        }
        else{
            memset(buff,0,sizeof(buff));
            
            if((rval=recv(mysock,buff,sizeof(buff),0))<0)
                perror("Error Reading stream ");
            
            else if (rval==0)
                printf("Ending conneciton");
            
            else{
                printf("MSG: %s",buff);
                printf("Message received (rval = %d)\n",rval);
                close(mysock);
            }
        }
    }while(1);
}
