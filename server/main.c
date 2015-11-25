/*
    Trabalho prático da disciplina MATA58 - Sistemas Operacionais
    
    > Nome do projeto
        Monkey Message
    
    > Descrição
        Chat simples no estilo Cliente Servidor
    
    > Alunos
        Jeferson Lima
        Ramon Dias
        Airton Serra
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lib/crc32.c"

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
                ANSI_COLOR_YELLOW "                                                                      `------'\n" ANSI_COLOR_RESET)

/* Macros */
#define INFO_MSG(text)      fprintf(stderr, ANSI_COLOR_BLUE   "[INFO] "    text ANSI_COLOR_RESET)
#define WARNING_MSG(text)   fprintf(stderr, ANSI_COLOR_YELLOW "[WARNING] " text ANSI_COLOR_RESET)
#define ERROR_MSG(text)     fprintf(stderr, ANSI_COLOR_RED    "[ERROR] "   text ANSI_COLOR_RESET)
#define clearNewLine()      scanf("%*c");
#define EXECUTE_ONCE(func)  do{ func }while(ONE_TIME)

/* Global Enum's */
enum commands {
    COMMAND_INVALID,
    COMMAND_EXIT,
    COMMAND_SEND,
    COMMAND_LIST,
    COMMAND_HELP,
};

/* Global Variables */
struct sockaddr_in server_address, client_address[MAX_NUM_CLIENTS];
pthread_t clients_thread[MAX_NUM_CLIENTS];
int addr_length[MAX_NUM_CLIENTS];
int server_port, socket_ref, client_counter = 0;
int clients_ref[MAX_NUM_CLIENTS];
int clients_t[MAX_NUM_CLIENTS];
pthread_t print_thread;

/* Functions */
void closeProgram(){
    /* Exit the program */
    printf( "\nExiting the Server...\n"
            "Thank you!\n");
    exit(EXIT_SUCCESS);
}

void showHelp(){
    printf( "Commands available:\n"
            "\tHELP\tShow this help message\n"
            "\tEXIT\tClose the application\n"
            "If you want a complete description of a program use HELP <command>\n");
}

void showHelpCommand(char *params){
    if(params[0] == BLANK_SPACE || params[0] == 0)
        printf("Param malformated or not specified\n");
    else
        printf("PARAM: %s\n", params);
}

void sigHandler(int sigNumber){
    switch(sigNumber){
        case SIGINT:
        case SIGQUIT:
            closeProgram();
            break;
    }
}

_Bool checkArgument(char *port, int *result){
    char *erro;
    *result = (int) strtol(port, &erro, 10);
    return (*erro == 0) && (*result < 65535);
}

void FAILURE_MSG(const char *msg){
    fprintf(stderr, ANSI_COLOR_RED "[FAIL] %s" ANSI_COLOR_RESET, msg);

    printf( "\nSome errors occured!\n"
            "\nExiting the Server...\n"
            "Thank you!\n");

    exit(EXIT_FAILURE);
}

uint32_t hash_djb_code(char *string, char **params){
    uint32_t hash = 5381;
    int i;
    for(i = 0; string[i] != 0 && string[i] != BLANK_SPACE; i++){ hash = ((hash << 5) + hash) + string[i];}
    
    *params = string[i] == BLANK_SPACE ? &string[i+1] : NULL;
        
    return hash;
}

int checkCommand(char *command, char **params){
    switch(hash_djb_code(command, params)){
        case HASH_EXIT: return COMMAND_EXIT;
        case HASH_HELP: return COMMAND_HELP;
        case HASH_LIST: return COMMAND_LIST;
    }
    return COMMAND_INVALID;
}

void gotoxy(int x,int y){
    printf("\x1b[%d;%df",y,x);
}

/* Threads */
void *clientThread(void *client_raw){
    int client_id = (long) client_raw;
    int client_msg;
    char msg_received[MAX_LENGTH_MSG];
    
    while(true){
        bzero(msg_received, MAX_LENGTH_MSG);
        client_msg = read(clients_ref[client_id], msg_received, MAX_LENGTH_MSG);
        if((client_msg < 0) || (strlen(msg_received) == 0)){
            fprintf(stderr, ANSI_COLOR_RED "[ERROR] Error on client Nº %d! Client disconected\n" ANSI_COLOR_RESET, client_id);
            printf("Client %d disconected!\n", client_id);
            pthread_exit(NULL);
        }
        else{
            //gotoxy(1,2);
            printf("Client #%d -- Message > %s", client_id, msg_received);
        }
    }
    
    pthread_exit(NULL);
}

void *clientAccept(){
    while((client_counter != MAX_NUM_CLIENTS)){
        addr_length[client_counter] = sizeof(clients_ref[client_counter]);
        if((clients_ref[client_counter] = accept(   socket_ref,
                                                    (struct sockaddr *) &client_address[client_counter],
                                                    &addr_length[client_counter])) == -1)
            ERROR_MSG("Error on accepting client!\n");
        
        INFO_MSG("New client connected\n");
        
        if((clients_t[client_counter] = pthread_create( &clients_thread[client_counter],
                                                        NULL, clientThread,
                                                        (void *) ((long) client_counter))))
            ERROR_MSG("Failed when try to start the client thread\n");
        
        INFO_MSG("Client thread created\n");
        
        client_counter++;
    }
    printf("byee");
    pthread_exit(NULL);
}

/* Main Program */
int main(int argc, char *argv[]){
    /* System Variables */
    struct sigaction SYS_ACTION;
    pthread_t client_accept;
    int client_accept_thread;
    
    /* Setup the program*/
    memset(&SYS_ACTION, 0, sizeof(SYS_ACTION));
    SYS_ACTION.sa_handler = sigHandler;
    SYS_ACTION.sa_flags = 0;
    sigaction(SIGINT, &SYS_ACTION, NULL);
    sigaction(SIGQUIT, &SYS_ACTION, NULL);
    
    /* Startup message */
    system("clear");
    printf("Monkey Chat Server, version 1.0-snapshot\n");
    
    /* Program Execution */
    switch(argc){
        /* If the number of arguments is greater than 2 throw an error messge */
        default:
            FAILURE_MSG("Invalid number of arguments\n");
        
        /* If no argument is passed show the program help */
        case 1:
            printf( "\nUsage:\n"\
                    "\tserver\t\t\tShow this help message\n"\
                    "\tserver\t[port]\t\tInitiate the chat server on the desired port\n"\
                    "\tserver\t[port] -i\tEnter in interactive mode\n"\
                    "\nUse '-i' to enter the server in interactive mode, running the server\n"\
                    "on interactive mode allow you to interact with the server\n");
            break;
        
        /* If there's only one or two argument and they are valid, start the server */
        case 2:
        case 3:;
            int port;
            char command[MAX_COMMAND_LENGTH];
            char *params;
            
            printf("\nStarting Server...\n");
            
            /* If the port is valid continue else throw a message error */
            if(checkArgument(argv[1], &port)){
                if(port < 1025){
                    printf("Port lower than 1025. Errors could occur\n");
                    WARNING_MSG("Port is lower than 1025. Make sure you are root.\n");
                }
                printf( "Port selected is %d\n\n", port);
            }
            else {
                /* If the argument is not a valid port so exit the program */
                FAILURE_MSG("Invalid port specified\n");
                break;
            }
            
            /* Define the server port and some other configurations */
            server_port = port;
            bzero((char *) &server_address, sizeof(server_address));
            server_address.sin_family = AF_INET;
            server_address.sin_addr.s_addr = INADDR_ANY;
            server_address.sin_port = htons(port);
            
            INFO_MSG("Trying to start the communication...\n");
            
            /* Try open the socket */
            if ((socket_ref = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
                FAILURE_MSG("Error openning the socket\n");
            
            /* Try to bind */
            if (bind(socket_ref, (struct sockaddr *) &server_address, sizeof(server_address)) == SOCKET_ERROR)
                FAILURE_MSG("Error on binding\n");

            INFO_MSG("Socket started!\n");
            
            /* Try to start the thread that will accept clients */
            if((client_accept_thread = pthread_create(&client_accept, NULL, clientAccept, NULL)))
                FAILURE_MSG("Failed when try to start the client accept thread\n");
            
            INFO_MSG("Thread created, prepare the system to accept clients...\n");
            
            /* Try to accept the maximum clients on the desired socket */
            if(listen(socket_ref, MAX_NUM_CLIENTS) == -1)
                FAILURE_MSG("Failed when try to accept clients.\n");
            
            INFO_MSG("Accept now is allowed. Waiting for clients...\n\n");

            showLogoAscii();
            printf(">> Server Started <<\n"); 
            
            if  ((argc == 3) && ((strlen(argv[2]) == 1) &&
                ((argv[2][0] == '-') && (argv[2][0] == 'i')))){
                printf( ANSI_COLOR_GREEN 
                        "[!] Interactive MODE [!]\n"
                        ANSI_COLOR_RESET
                        "Type EXIT or enter CTRL+D to exit the server:\n\n"
                        ">> ");
                
                /* Start the console */
                while(scanf("%[^\n]s", command) != EOF){
                    /* Clear the "\n" that is inside the buffer */
                    clearNewLine();
                    /* Search for the commnd input */
                    switch(checkCommand(command, &params)){
                        case COMMAND_INVALID:
                            ERROR_MSG("Invalid command!\n");
                            printf("If you want close use CTRL+D or EXIT\n");
                            break;
                        case COMMAND_EXIT:
                            if(params == NULL){
                                printf( "\nExiting the Server...\n"
                                        "Thank you!\n");
                                pthread_exit(NULL);
                                return EXIT_SUCCESS;                         
                            }
                            else
                                printf("Use CTRL+D or EXIT instead!\n");
                            break;
                        case COMMAND_HELP:
                            params == NULL ? showHelp() : showHelpCommand(params);
                            break;
                        case COMMAND_LIST:
                            pthread_join(print_thread, NULL);
                            break;
                    }

                    bzero(command, sizeof(command));
                    printf(">> ");
                }
                break;
            }
            
            /* If mode is non interactive just print the messages */
            printf("Hit CTRL+C to close the application!\n");
            while(true);
            
            break;
    }
    
    /* Exit the program */
    printf( "\nExiting the Server...\n"
            "Thank you!\n");
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}
