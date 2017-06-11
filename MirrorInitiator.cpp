/* 
 * File:   MirrorInitiator.cpp
 * Author: angelique
 *
 * Created on May 16, 2017, 6:53 PM
 */

#include <cstdlib>
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
#include "LinkedList.h"



/* ./MirrorInitiator -n MirrorServerAddress -p 59579 -s ContentServerAddress1:ContentServerPort1:dirorfile1:delay1,ContentServerAddress2:ContentServerPort2:dirorfile2:delay2
 * 
 */
void perror_exit (string message );

int main(int argc, char** argv)
{
    int i, MirrorServerPort, sock;
    char *MirrorServerAddress, *ContentServerInfo, *token;
    
    char hostname [50] , symbolicip [50], buf[256];
    struct hostent * mymachine ;
    struct in_addr ** addr_list ;

    struct sockaddr_in server ;
    struct sockaddr * serverptr = ( struct sockaddr *) &server ;

    LinkedList ContentServerList;
    
    for (i = 1; i < 6; i += 2)
    {
        if(strcmp(argv[i], "-n") == 0)
        {
            MirrorServerAddress = new char[strlen(argv[i+1]) + 1];
            strcpy(MirrorServerAddress, argv[i+1]);
            //cout << MirrorServerAddress << endl;
        }
        else if (strcmp(argv[i], "-p") == 0) 
        {
            MirrorServerPort = atoi(argv[i+1]);
            //cout << MirrorServerPort << endl;
        }
        else  
        {
            token = strtok(argv[i+1], ",");  
            while (token != NULL)
            {
                //cout << token << endl;
                ContentServerList.InsertNode(token);
                token = strtok(NULL, " ");
            }            
        }
    }
    
    if (isdigit(MirrorServerAddress[0]))
    {      
        struct in_addr myaddress;

        /* IPV dot-number into  binary form (network byte order) */
        inet_aton(MirrorServerAddress, &myaddress);

        mymachine = gethostbyaddr((const char*)&myaddress, sizeof(myaddress), AF_INET);

        if (mymachine != NULL){
            cout << "IP-address: " << MirrorServerAddress << " resolved to: "<< mymachine->h_name << endl;          
        }
        else{ 
            cout << "IP-address: " << MirrorServerAddress << " could not be resolved"<< endl;          
        }
    }
    else
    {
        if ( ( mymachine = gethostbyname (MirrorServerAddress) ) == NULL )
            cout << " Could not resolved Name : " << MirrorServerAddress << endl;
        else 
        {
            cout << " Name To Be Resolved : " << mymachine->h_name << endl ;
            cout << " Name Length in Bytes :  " << mymachine->h_length << endl;
            addr_list = ( struct in_addr **) mymachine -> h_addr_list ;
            for (i = 0; addr_list[i] != NULL ; i++) {
                strcpy ( symbolicip , inet_ntoa (*addr_list[i]) ) ;
                cout << mymachine->h_name  << " resolved to " << symbolicip << endl;
            }
        }
    }
    
    
 
    
    cout << "Socket begins" << endl;
    /* Create socket */
    if (( sock = socket ( AF_INET , SOCK_STREAM , 0) ) < 0)
        perror_exit ("socket");
    
    server.sin_family = AF_INET ; /* Internet domain */
    memcpy (&server.sin_addr , mymachine->h_addr , mymachine->h_length );
    server.sin_port = htons ( MirrorServerPort ); /* Server port */
    /* Initiate connection */
    if ( connect ( sock , serverptr , sizeof ( server )) < 0)
        perror_exit ("connect");
    cout << " Connecting to " << MirrorServerAddress<< " port " << MirrorServerPort << endl;
  
    int size = ContentServerList.GetLength();
    do 
    {        
        cout << "Message is ready to be sent to server!" << endl;
        if( send(sock , ContentServerList.GetInfo(size) , strlen(ContentServerList.GetInfo(size)) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        else puts("Message sent to server!");
        size--;
        
        sleep(1);
    } while ( size != 0) ; /* Finish on " end " */
        //memset(buf ,0 , 100);  //clear the variable

        close ( sock ); /* Close socket and exit */
    return 0;        
}

void perror_exit (string message )
{
    const char * tmp = message.c_str();
    perror ( tmp );
    exit ( EXIT_FAILURE );
}