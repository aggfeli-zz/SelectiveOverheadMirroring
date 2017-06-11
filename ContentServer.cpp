/* 
 * File:   ContentServer.cpp
 * Author: angelique
 *
 * Created on May 20, 2017, 2:40 PM
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
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

/* ./ContentServer -p 9002 -d tmp
 * ./ContentServer -p 9101 -d tmp
 * ./ContentServer -p 9101 -d /home/angelique/temp
 */
void perror_exit ( string message );
void sigchld_handler ( int sig );

int main(int argc, char** argv) 
{
    char *dirorfilename, *tempfile;
    int i, port = 0 , sock , newsock, err, counter = 0, flag = 0;
    struct sockaddr_in server , client ;
    socklen_t clientlen ;
    struct sockaddr * serverptr =( struct sockaddr *) & server ;
    struct sockaddr * clientptr =( struct sockaddr *) & client ;
    struct hostent * rem ;
    
    for (i = 1; i < 4; i += 2)
    {
        if(strcmp(argv[i], "-p") == 0)
        {
            port = atoi(argv[i+1]);         
            //cout << port << endl;
        }
        else  
        {
            dirorfilename = new char[strlen(argv[i+1]) + 1];
            strcpy(dirorfilename, argv[i+1]);
            //cout << dirorfilename << endl;
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
    char buf[100], ContentServerID[100], command[100], file[100];
    int delay;
    while (1) {
        /* accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");
        /* Find client's address */
    //    	if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
    //   	    herror("gethostbyaddr"); exit(1);}
    //    	printf("Accepted connection from %s\n", rem->h_name);
        printf("Accepted connection\n");
        switch (fork()) {    /* Create child for serving client */
        case -1:     /* Error */
            perror("fork"); break;
        case 0:	     /* Child process */
            //cout << "Reading message from client" << endl;
            close(sock);  
            recv(newsock , buf , 100 , 0) ;
           // if(strcmp(buf, "End") == 0) break;
            cout << "Message from Server:" << endl;
            printf("ContentServer %s\n" , buf);   
            if (strcmp(buf, "LIST") == 0)
            {
                cout << "LIST COMMAND" << endl;               
                recv(newsock , buf , 100 , 0) ;
                strcpy(ContentServerID, buf);
                printf("ContentServer ID %s\n" , ContentServerID);  
                recv(newsock , buf , 100 , 0) ;
                delay = atoi(buf); 
                printf("ContentServer delay %d\n" , delay);  
                //sleep(2);
                DIR *dir;
                struct dirent *ent;
                if ((dir = opendir (dirorfilename)) != NULL) 
                {
                    /* print all the files and directories within directory */
                    while ((ent = readdir (dir)) != NULL) 
                    {
                      printf ("%s\n", ent->d_name);
                      if(strcmp(ent->d_name, ".") != 0)
                        if(send(newsock , ent->d_name , strlen(ent->d_name) , 0) < 0)
                            puts("Send failed");
                      sleep(1);
                    }
                    closedir (dir);
                } 
                else 
                {
                    /* could not open directory */
                    perror ("");
                    return EXIT_FAILURE;
                }
                //break;
            }
            if(strcmp(buf, "FETCH") == 0)
            {      
                //sleep(4);
                recv(newsock , buf , 100 , 0) ;
                //cout << "FETCH file:    " << buf << endl;
                strcpy(file, buf);
                printf("ContentServer file %s\n" , file); 
                sleep(5);//delay);
                
                sprintf(dirorfilename, "%s/%s", dirorfilename,file);
                cout <<"!!!!!!!!!!!!!!!     " << dirorfilename << endl;
                struct stat statbuf;
                stat(dirorfilename, &statbuf);
                if(S_ISDIR(statbuf.st_mode))
                {
                    //printf("directory\n");
                    DIR *dir;
                    struct dirent *ent;
                    if ((dir = opendir (dirorfilename)) != NULL) 
                    {
                        /* print all the files and directories within directory */
                        while ((ent = readdir(dir)) != NULL) 
                        {
                            printf ("%s\n", ent->d_name);
                            if(strcmp(ent->d_name, ".") != 0){
                            if(send(newsock , ent->d_name , strlen(ent->d_name) , 0) < 0)
                                puts("Send failed");
                            int f;
                            sprintf(tempfile, "%s/%s",dirorfilename, ent->d_name);
                            //cout << "File that will be transfered " << tempfile << endl;
                            if((f = open(tempfile, O_RDONLY)) < 0)
                            {
                                perror("open file with open");
                                //exit(1); /*sendfile and fstat need an int  */
                            }
                            else
                            {
                                if(fstat(f, &statbuf) < 0)
                                {
                                    perror("Error fstat");
                                }
                                for(i = 0; i < statbuf.st_size; i++) 
                                {                                   
                                    memset(buf, 0, sizeof buf);
                                    read(f, &buf, 1);
                                    if(send(newsock , buf , 1 , 0) < 0)
                                        puts("Send failed");
                                }
                                close(f);
                            }}
                            //sleep(1);
                        }
                        closedir (dir);
                    } 
                    else 
                    {
                        /* could not open directory */
                        perror ("");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    printf("File %s\n", dirorfilename);
                    int f;                   
                    //cout << "File that will be transfered " << tempfile << endl;
                    if((f = open(dirorfilename, O_RDONLY)) < 0)
                    {
                        perror("open file with open");
                        //exit(1); /*sendfile and fstat need an int  */
                    }
                    else
                    {
                        if(fstat(f, &statbuf) < 0)
                        {
                            perror("Error fstat");
                        }
                        for(i = 0; i < statbuf.st_size; i++) 
                        {                                   
                            memset(buf, 0, sizeof buf);
                            read(f, &buf, 1);
                            if(send(newsock , buf , 1 , 0) < 0)
                                puts("Send failed");
                        }
                        close(f);
                    }
                }               
            }
            exit(0); 
        }       
        close(newsock); /* parent closes socket to client */       
    }

    return 0;    
}

/* Wait for all dead child processes */
void sigchld_handler (int sig) 
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void perror_exit(string message)
{
    const char * tmp = message.c_str();
    perror(tmp);
    exit(EXIT_FAILURE);
}