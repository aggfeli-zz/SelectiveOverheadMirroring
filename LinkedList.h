/* 
 * File:   LinkedLink.h
 * Author: angelique
 *
 * Created on May 23, 2017, 11:20 AM
 */

#ifndef LINKEDLIST_H
#define	LINKEDLIST_H
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>

using namespace std;

struct ContentServer
{
    char *info;
    ContentServer * next;
    char *address;
    int port;   
    ContentServer(char *tmp)
    {
        info = new char[strlen(tmp) + 1];
        strcpy(info, tmp);
    };
    ContentServer(string tmp, string ad, string p)
    {
        info = new char[tmp.size() + 1];
        strcpy(info, tmp.c_str());
        address = new char[ad.size() + 1];
        strcpy(address, ad.c_str());
        port = stoi(p);
    };
    ContentServer(){};
};

class LinkedList {
public:
    LinkedList();
    LinkedList(const LinkedList& orig);
    virtual ~LinkedList();
    void InsertNode(char *tmp);
    void InsertNode(string tmp, string ad, string p);
    int GetLength();
    void DecreaseLength(){length--;};
    void PrintList();
    char * GetInfo(int counter);
    int GetData(string &data, string &ad, int &p, char *file);
private:
    ContentServer *head;

    int length; // Length is the number of data nodes.
};

#endif	/* LINKEDLIST_H */

