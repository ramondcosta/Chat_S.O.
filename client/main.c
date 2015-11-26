#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
 
int main(int argc , char *argv[])
{
    int sock,len;
    struct sockaddr_in server;
    char message[3000] , server_reply[4000];
    
    strcpy(message,argv[1]);
    len = strlen(message);
    message[len++] = '\n';
    message[len] = '\0';
    strcpy(&message[len],argv[2]);
    len = strlen(message);
    message[len++] = '\n';
    message[len] = '\0';
    strcpy(&message[len],argv[3]);
    //puts(message);

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    //"127.0.0.1"
    server.sin_addr.s_addr = inet_addr(argv[2]);
    server.sin_family = AF_INET;
    server.sin_port = htons( atoi(argv[3]) );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
     
    //Send some data
    if( send(sock , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return 1;
    }
     
    //Receive a reply from the server
    if( recv(sock , server_reply , 4000 , 0) < 0)
    {
        puts("recv failed");
        //break;
    }
     
    puts("Server reply :");
    //printf("%s\n", server_reply);
    puts(server_reply);

    //keep communicating with server
    while(1)
    {
        printf("Enter message : ");

        scanf("%s" , message);
        
        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
         
        //Receive a reply from the server
        if( recv(sock , server_reply , 4000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
         
        puts("Server reply :");
        //printf("%s\n", server_reply);
        puts(server_reply);
        printf("%s\n", message);
    }
     
    close(sock);
    return 0;
}