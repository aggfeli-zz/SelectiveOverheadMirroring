#include "Workers_MirrorManagers.h"

int num_of_items = 15;
pthread_mutex_t mtx ;
pthread_cond_t cond_nonempty ;
pthread_cond_t cond_nonfull ;
pool_t pool ;
LinkedList ContentServerList;

void pr()
{
    ContentServerList.PrintList();
}
  
void perror_exit(string message)
{
    const char * tmp = message.c_str();
    perror(tmp);
    exit(EXIT_FAILURE);
}
    
void initialize (pool_t * pool) 
{
    pool->start = 0;
    pool->end = -1;
    pool->count = 0;
}

void place ( pool_t * pool , string data , string ContentServerAddress, string ContentServerPort) 
{
    pthread_mutex_lock (&mtx) ;
    ContentServerList.InsertNode(data, ContentServerAddress, ContentServerPort);
    pthread_mutex_unlock (&mtx);
    return;
}


char * obtain ( pool_t * pool, string &Address, int &port, char * file ) 
{
    string data;
    pthread_mutex_lock (& mtx ) ;   
    while ( ContentServerList.GetLength() <= 0)//pool->count <= 0) 
    {
        printf (" >> Found Buffer Empty \n");       
        pthread_cond_wait (& cond_nonempty , & mtx );
    }
    
    /****************************************************************/
    
    int found = ContentServerList.GetData(data, Address, port, file);
  
    if(found == 1)
    {             
        char *data1 = new char[data.size() + 1];
        strcpy(data1, data.c_str());
        ContentServerList.DecreaseLength();
        pthread_mutex_unlock (& mtx );
        return data1;
    }
    
    char *data1 = new char[strlen("no") + 1];
    strcpy(data1, "no");
    pthread_mutex_unlock (& mtx );
    
    /******************************************************/
    return data1;
}

void * MirrorManagerJob( void * ptr )
{
    cout << "MirrorManagerJob" << endl;
    string str = *reinterpret_cast<string*>(ptr);
    cout << str << endl;
    
        
    /**************************************Socket*************************************************/
    char hostname [50] , symbolicip [50], buf[100];
    struct hostent * mymachine ;
    struct in_addr ** addr_list ;
    int sock;
    struct sockaddr_in server ;
    struct sockaddr * serverptr = ( struct sockaddr *) &server ;

    char *ContentServerAddress = strtok(strdup(str.c_str()), ":");
    char *ContentServerPort = strtok(NULL, ":");
    char *dirorfilename = strtok(NULL, ":");
    char *delay = strtok(NULL, ":");
    //cout << dirorfilename << endl;
    //cout << ContentServerAddress << endl;
    //cout << ContentServerPort << endl;
    //cout << delay << endl;
    
    cout << "Socket begins  //// Manager" << endl;
    //if (strcmp(ContentServerAddress, "ContentServerAddress1") == 0)strcpy(ContentServerPort,"9002");
    //else strcpy(ContentServerPort,"9101");
    //strcpy(ContentServerAddress,"192.168.1.230");//192.168.1.230");
    
    if ( ( mymachine = gethostbyname (ContentServerAddress) ) == NULL )
        cout << " Could not resolved Name : " << ContentServerAddress << endl;
    else 
    {
        cout << " Name To Be Resolved : " << mymachine->h_name << endl ;
        cout << " Name Length in Bytes :  " << mymachine->h_length << endl;
        addr_list = ( struct in_addr **) mymachine->h_addr_list ;
        for (int i = 0; addr_list[i] != NULL ; i++) {
            strcpy ( symbolicip , inet_ntoa (*addr_list[i]) ) ;
            cout << mymachine->h_name  << " resolved to " << symbolicip << endl;
        }
    }
    /* Create socket */
    if (( sock = socket ( AF_INET , SOCK_STREAM , 0) ) < 0)
        perror_exit ("socket");
    
    server.sin_family = AF_INET ; /* Internet domain */
    memcpy (&server.sin_addr , mymachine->h_addr , mymachine->h_length );
    server.sin_port = htons ( atoi(ContentServerPort) ); /* Server port */
    /* Initiate connection */
    if ( connect ( sock , serverptr , sizeof ( server )) < 0)
        perror_exit ("connect");
    cout << " Connecting to " << ContentServerAddress<< " port " << ContentServerPort << endl;
    int size = 3;
    do {
        cout << "Message is ready to be sent to ContentServer from Manager!" << endl;
        if(size == 3){
            if( send(sock , "LIST" , sizeof("LIST") , 0) < 0)
            {
                puts("Send failed");
               // break;
            }
        }
       else if (size == 2)
       {
           sleep(1);
            if( send(sock , "ContentServerID" , sizeof("ContentServerID") , 0) < 0)
            {
                puts("Send failed");
                //break;
            }
        }
        else
        {
           sleep(1);
            if( send(sock , delay , sizeof(delay) , 0) < 0)
            {
                puts("Send failed");
                //break;
            }
        }    
        size--;
        
        if(size == 0)
        {
            cout << "Content Server List of files-directories!" << endl;
            sleep(2);
            while(recv(sock , buf, 100 , 0)  > 0 )
            {                
                printf("%s\n" , buf);   
                
                place (&pool , buf, ContentServerAddress, ContentServerPort);
                printf (" producer : %d \n" , num_of_items ) ;
                num_of_items --;
                pthread_cond_signal (&cond_nonempty) ;
                usleep (0) ;                
                memset(buf, 0, sizeof buf);
            }
        }
    } while ( size != 0) ; /* Finish on " end " */
    
    
    close ( sock ); /* Close socket and exit */   
    pthread_exit (0) ;
}


void * workerJob(void * ptr)
{
    char buf[100];
    string ContentServerAddress;
    int ContentServerPort;
    
    string str = *reinterpret_cast<string*>(ptr);
    char *token = strtok(strdup(str.c_str()), ":");
    token = strtok(NULL, ":");
    token = strtok(NULL, ":");
    cout << "strrrrrrr  " << token << endl;
    strcpy(token, "t.txt");
    strcpy(buf, obtain (&pool, ContentServerAddress, ContentServerPort, token));
   
    if (strcmp(buf , "no") == 0)
    {
        cout << "Content Server can't obtain " << token << endl;
        pthread_exit (0) ;
    }
    
    printf (" consumer : %s \n" , buf );
    pthread_cond_signal (&cond_nonfull );
    usleep (500000) ;
    
    /**************************************Socket*************************************************/
    char hostname [50] , symbolicip [50];
    struct hostent * mymachine ;
    struct in_addr ** addr_list ;
    int sock;
    struct sockaddr_in server ;
    struct sockaddr * serverptr = ( struct sockaddr *) &server ;

    cout << "Socket begins //// Worker" << endl;
   
    if ( ( mymachine = gethostbyname (ContentServerAddress.c_str()) ) == NULL )
        cout << " Could not resolved Name : " << ContentServerAddress << endl;
    else 
    {
        cout << " Name To Be Resolved : " << mymachine->h_name << endl ;
        cout << " Name Length in Bytes :  " << mymachine->h_length << endl;
        addr_list = ( struct in_addr **) mymachine->h_addr_list ;
        for (int i = 0; addr_list[i] != NULL ; i++) {
            strcpy ( symbolicip , inet_ntoa (*addr_list[i]) ) ;
            cout << mymachine->h_name  << " resolved to " << symbolicip << endl;
        }
    }
    /* Create socket */
    if (( sock = socket ( AF_INET , SOCK_STREAM , 0) ) < 0)
        perror_exit ("socket");
    
    server.sin_family = AF_INET ; /* Internet domain */
    memcpy (&server.sin_addr , mymachine->h_addr , mymachine->h_length );
    server.sin_port = htons ( ContentServerPort ); /* Server port */
    /* Initiate connection */
    if ( connect ( sock , serverptr , sizeof ( server )) < 0)
        perror_exit ("connect");
    cout << " Connecting to " << ContentServerAddress<< " port " << ContentServerPort << endl;
    int size = 2;
    do {
        cout << "Message is ready to be sent to ContentServer from Worker!" << endl;
        if(size == 2){
            if( send(sock , "FETCH" , sizeof("FETCH") , 0) < 0)
            {
                puts("Send failed");
               // break;
            }
        }
       else
       {
            sleep(2);
            if( send(sock , buf , sizeof(buf) , 0) < 0)
            {
                puts("Send failed");
                //break;
            }
        }       
        size--;
        //memset(buf, 0, sizeof buf);
        if(size == 0)
        {
            cout << "Content Server Fetch of files-directories!" << endl;           
            sleep(1); 
            FILE *fp;
            fp = fopen(token, "ab"); 
            if(NULL == fp)
            {
                printf("Error opening file");
                break;
            }
            while(recv(sock , buf, 100 , 0)  > 0 )
            {                
                printf("%s\n" , buf);   
                fwrite(buf, 1,100,fp);                              
            }
        }
    } while ( size != 0) ; /* Finish on " end " */
    close ( sock ); /* Close socket and exit */
    pthread_exit (0) ;
}