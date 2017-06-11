/* 
 * File:   LinkedLink.cpp
 * Author: angelique
 * 
 * Created on May 23, 2017, 11:20 AM
 */

#include "LinkedList.h"

LinkedList::LinkedList() 
{
    head = new ContentServer;
    head->next = NULL;
    length = 0;
}

LinkedList::LinkedList(const LinkedList& orig) {
}

LinkedList::~LinkedList() 
{    
    ContentServer * p = head;
    ContentServer * q = head;
    int counter = 0;
    while (q != NULL && counter < length)
    {
        p = q;
        q = p -> next;
        if (q != NULL) 
        { 
            if (p->address != NULL)delete []p->address;
            if (p->info!= NULL)delete []p->info; 
            delete p;
            counter++;
        }
    }
}

void LinkedList::InsertNode(char *tmp)
{
    ContentServer *temp = new ContentServer(tmp);
    if (!head -> next)              //If list is empty insert here
    {
        head->next = temp;
        length++;
        return;
    }
    temp->next = head->next;
    head->next = temp;
    length++;
}

void LinkedList::InsertNode(string tmp, string ad, string p)
{
    ContentServer *temp = new ContentServer(tmp, ad, p);
    if (!head -> next)              //If list is empty insert here
    {
        head->next = temp;
        length++;
        return;
    }
    temp->next = head->next;
    head->next = temp;
    length++;
}

int LinkedList::GetLength()
{
    return length;
}


char * LinkedList::GetInfo(int counter)
{
    ContentServer * p = head->next;
    int tmp = length;
    while (p)
    {     
        if(tmp == counter)return p->info;      
        if (p->next == NULL) break;
        else p = p->next;
        tmp--;
    }
}

int LinkedList::GetData(string &data, string &ad, int &port, char *file)
{
    ContentServer * p = head->next;
    ContentServer * previous = head;    
    while (p)
    {             
        if (p->info == NULL) break;            
        if(strcmp(file, p->info) == 0)
        {
            data = p->info;
            ad = p->address;
            port = p->port;
            previous->next = p->next;
            delete []p->address;
            delete []p->info; 
            delete p;
            return 1;      
        }
        if (p->next == NULL) break;
        else {p = p->next;previous = previous->next;}
        
    }
    return 0;
}

void LinkedList::PrintList()      //Print whole list
{
    if (length == 0)
    {
        cout << "\n{ }\n";
        return;
    }
    ContentServer * p = head->next;
    cout << "\n{ ";
    while (p)
    {
        cout << p->info;
        if (p -> next) cout << ", ";
        else break;
        p = p->next;
    }
    cout << "}\n";
}