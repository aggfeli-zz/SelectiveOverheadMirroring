/* 
 * File:   MirrorServer.cpp
 * Author: angelique
 *
 * Created on May 16, 2017, 6:53 PM
 */

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <unistd.h> /* read , write , close */
#include <pthread.h>
#include <stdint.h>
#include "Workers_MirrorManagers.h"


using namespace std;

/* ./MirrorServer -p <port> -m <dirname> -w <threadnum>
 * ./MirrorServer -p 59579 -m temp -w 2
 */


void sigchld_handler ( int sig );
int threadnum, mtxfl; /* Variables visible by thread function */
struct buffer{
    char info[100];
    int counter;
    //int start;
    //int end ;
}buffer[100];

int main(int argc, char** argv)
{
    int i, port = 0 , sock , newsock, err, counter = 0, flag = 0;
    struct sockaddr_in server , client ;
    socklen_t clientlen ;
    struct sockaddr * serverptr =( struct sockaddr *) & server ;
    struct sockaddr * clientptr =( struct sockaddr *) & client ;
    struct hostent * rem ;
    mtxfl = 1;
    char *dirname;
    for (i = 1; i < 6; i += 2)
    {
        if(strcmp(argv[i], "-p") == 0)
        {
            port = atoi(argv[i+1]);         
            //cout << port << endl;
        }
        else if (strcmp(argv[i], "-m") == 0) 
        {
            dirname = new char[strlen(argv[i+1]) + 1];
            strcpy(dirname, argv[i+1]);
            //cout << dirname << endl;
        }
        else  
        {
            threadnum = atoi(argv[i+1]);         
            //cout << threadnum << endl;
        }
    }  
    
    
    /********************************Socket*************************************************************/
    
    /* Reap dead children asynchronously */
    signal(SIGCHLD, sigchld_handler);
    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);      /* The given port */
    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");
    /* Listen for connections */
    if (listen(sock, 5) < 0) perror_exit("listen");
    printf("Listening for connections to port %d\n", port);

        /* accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");
        printf("Accepted connection\n");
        
        cout << "Reading message from client" << endl;
        char buf[100];
        while(recv(newsock , buffer[counter].info , 100 , 0)  > 0 )
        {           
            cout << "Message from client:" << endl;
            printf("$$$ %s\n" , buffer[counter].info);   
            counter++;
        }
        printf("Closing connection.\n");
    close(newsock);

    pthread_t * workers , * MirrorManager ;

    if (( workers = (pthread_t*) malloc (threadnum * sizeof ( pthread_t )) ) == NULL ) {
        perror ("malloc");
        exit (1) ;
    }
    
    if (( MirrorManager = (pthread_t*) malloc (counter * sizeof ( pthread_t )) ) == NULL ) {
        perror ("malloc");
        exit (1) ;
    }

    for(long i = 0; i < counter && i < threadnum; i++)
    {
        initialize (&pool);
        pthread_mutex_init (&mtx , 0) ;
        pthread_cond_init (&cond_nonempty , 0) ;
        pthread_cond_init (&cond_nonfull , 0) ;
        
        string data(buffer[i].info);
        pthread_create (MirrorManager + i , 0, MirrorManagerJob , ( void *) &data) ;
        pthread_create (workers + i , 0, workerJob , ( void *) &data) ;
        pthread_join (*( MirrorManager + i) , 0) ;
        pthread_join ( *( workers + i) , 0) ;
        pthread_cond_destroy (&cond_nonempty ) ;
        pthread_cond_destroy (&cond_nonfull );
        pthread_mutex_destroy (&mtx );
    }
   
    //pr();
    return 0;   
}


/* Wait for all dead child processes */
void sigchld_handler (int sig) 
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}
