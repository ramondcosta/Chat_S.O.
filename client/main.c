#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* Constants */
#define SOCKET_ERROR        -1
#define BLANK_SPACE         ' '
#define MAX_COMMAND_LENGTH  250
#define MAX_NUM_CLIENTS     100
#define MAX_LENGTH_MSG      255
#define ONE_TIME            0
/* Colors */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/* Commands in DJB Hash */
#define HASH_EXIT           0x7C8465BF
#define HASH_LIST           0x7C87FDE1
#define HASH_HELP           0x7C85BA6E
/* Ascii *-* */
#define showLogoAscii()\
        printf( ANSI_COLOR_YELLOW "                                                                          ',\n" ANSI_COLOR_RESET \
                ANSI_COLOR_RED "                                                               " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW "      .-`-,\\__\n" ANSI_COLOR_RESET\
                ANSI_COLOR_RED "                                                               " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW "        .\"`   `,\n" ANSI_COLOR_RESET\
                ANSI_COLOR_RED "   *       )     )     )         )            )                " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW "      .'_.  ._  `;.\n" ANSI_COLOR_RESET\
                ANSI_COLOR_RED " (  `   ( /(  ( /(  ( /(      ( /(     (   ( /(  (      *   )  " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW "    __ / `      `  `.\\ .--.\n" ANSI_COLOR_RESET\
                ANSI_COLOR_RED " )\\))(  )\\()) )\\()) )\\())(    )\\())    )\\  )\\()) )\\   ` )  /(  " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW "  /--,| 0)   0)     )`_.-,)\n" ANSI_COLOR_RESET\
                ANSI_COLOR_RED "((_)()\\((_)\\ ((_)\\|((_)\\ )\\  ((_)\\   (((_)((_)((((_)(  ( )(_)) " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW " |    ;.-----.__ _-');   /\n" ANSI_COLOR_RESET\
                ANSI_COLOR_RED "(_()((_) ((_) _((_)_ ((_|(_)__ ((_)  )\\___ _((_)\\ _ )\\(_(_())  " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW "  '--./         `.`/  `\"`\n" ANSI_COLOR_RESET\
                ANSI_COLOR_RED "|  \\/  |/ _ \\| \\| | |/ /| __\\ \\ / /   / __| || (_)_\\(_)_   _|  " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW "     :   '`      |.\n" ANSI_COLOR_RESET\
                ANSI_COLOR_RED "| |\\/| | (_) | .` | ' < | _| \\ V /   | (__| __ |/ _ \\   | |    " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW "     | \\     /  //\n" ANSI_COLOR_RESET\
                ANSI_COLOR_RED "|_|  |_|\\___/|_|\\_|_|\\_\\|___| |_|     \\___|_||_/_/ \\_\\  |_|    " ANSI_COLOR_RESET\
                ANSI_COLOR_YELLOW "      \\ '---'  //'\n" ANSI_COLOR_RESET \
                ANSI_COLOR_YELLOW "                                                                      `------'\n" ANSI_COLOR_RESET);

int main(int argc , char *argv[])
{
    int sock,len;
    struct sockaddr_in server;
    char message[3000] , server_reply[4000];

    showLogoAscii()

    if(argc < 4){
        printf("Please use the following way: <client> <ip> <port>\n\nMonkey Chat Client");
        return EXIT_FAILURE;
    }

    strcpy(message, argv[1]);
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
    return EXIT_SUCCESS;
}
