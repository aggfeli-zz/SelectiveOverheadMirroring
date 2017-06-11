/* 
 * File:   Workers-MirrorManagers.h
 * Author: angelique
 *
 * Created on May 28, 2017, 8:32 PM
 */

#ifndef WORKERS_MIRRORMANAGERS_H
#define	WORKERS_MIRRORMANAGERS_H
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
# include <netdb.h> 
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <unistd.h> /* read , write , close */
#include "LinkedList.h"
#define POOL_SIZE 10

using namespace std;

typedef  struct {
    char data[POOL_SIZE][100];
    char address[POOL_SIZE][50];
    int port[POOL_SIZE];   
    int start ;
    int end ;
    int count ;
} pool_t ;

extern pthread_mutex_t mtx ;
extern pthread_cond_t cond_nonempty ;
extern pthread_cond_t cond_nonfull ;
extern pool_t pool ;

void pr();
void initialize (pool_t * pool);
void place ( pool_t * pool , string data , string ContentServerAddress, string ContentServerPort);
char * obtain ( pool_t * pool, string &Address, int &port, char * file );
void * MirrorManagerJob ( void * ptr );
void * workerJob ( void * ptr );
void perror_exit(string message);
#endif	/* WORKERS_MIRRORMANAGERS_H */

