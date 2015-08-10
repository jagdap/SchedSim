// ---------------------------------------------------------
/*
	File: TCB_Queue.cpp
	Package: SchedulerSim

	Description: A simple list function for TCBs.
	License/Copyright: None
*/
//---------------------------------------------------------
/*-------------------------------------------------------------------------*/
/* INCLUDES */
/*-------------------------------------------------------------------------*/
#include "TCB_Queue.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>
//Use namespace std
using namespace std;

//---------------------------------------------------------
/*  CLASS METHODS					*/
//---------------------------------------------------------

//Constructor for TCB_Queue class
TCB_Queue::TCB_Queue(int max_size)
{
    this->max_size=max_size;        //Initialize max_size of the queue
    this->size=0;          //Initialize size
    this->head=NULL;        //Initialize head
    this->tail=NULL;        //Initialize tail
}

//Destructor for TCB_Queue Class
TCB_Queue::~TCB_Queue()
{
    TCBnode* temp;          //Temp node
    int i=0, size=this->size;

    //Loop through the queue
    for(i=0;i<size;i++)
    {
        temp=dequeue();     //Dequeue
        delete temp;        //Delete the node
    }
}

//This function enqueues a node
//Return values:
//      0 - The queue is full, node was not enqueued
//      1 - The node was successfully enqueued
int TCB_Queue::enqueue(TCBnode* _tcbn)
{
    //If the queue is full,
    if(this->size >= this->max_size)
    {
        printf("The queue is full.");
        return 0;   //Return 0
    }
    //Else if the queue is not empty,
    else if(this->tail!=NULL)
    {
        //Point the current tails pointer to the new node
        this->tail->next=_tcbn;
        //Point the previous pointer on the new node to the current tail
        _tcbn->prev=this->tail;
        //Update the tail pointer to the new node
        this->tail=_tcbn;
	// ADDED: make sure the tail doesn't point to anything
	tail->next = NULL;
    }
    //Else the queue is empty
    else
    {
        //Update the head and tail pointers to the new node
        this->head=_tcbn;
        this->tail=_tcbn;
    }

    //Increment the node size and return the new size
    return ++this->size;
}

//This function dequeues a node
//Return Value:
//      NULL - The queue was empty
//      Not NULL - Pointer to the dequeue node
TCBnode* TCB_Queue::dequeue()
{
    //Initialize the nodeDQ pointer to the head of the queue
    TCBnode* nodeDQ=this->head;

    //Print debug information if needed
    #ifdef DEBUG
    printf("\nDequeue from a queue with %d nodes.\n", this->size);
    #endif // DEBUG

    //If the queue is empty
    if(this->head==NULL)
    {
        printf("The queue is empty.\n\n");
        return NULL;
    }
    //Else if the queue contains one node
    else if(this->size==1)
    {
        //Update the head and tail pointers to NULL
        this->head=NULL;
        this->tail=NULL;
    }
    //Else the queue has at least two nodes
    else
    {
        //Update the head pointer to the original second node
        this->head=this->head->next;
        //Update the new head previous pointer to NULL
        this->head->prev=NULL;
    }

    //To prevent a circularly linked list in case the dequeued node is enqueued later on
    //set the next pointer on the dequeued node to NULL
    nodeDQ->next=NULL;

    //Decrement the node size
    this->size--;
	printf("size now: %d \n",size);
    //Return a pointer to the node to be dequeued
    return nodeDQ;
}

// ADDED:
//This function moves a TCBnode from its current location and
//places it at the tail
//Return values:
// 0 Successfully moved node
// 1 Error encountered
int TCB_Queue::moveToTail(TCBnode* inNode){
	// Is it at the head?
	if (inNode == head){
		head = inNode->next;
		head->prev = NULL;
		// Reduce the size before enqueue increases it again
		size--;
		return enqueue(inNode);
	// Is it already at the tail?
	} else if (inNode == tail){
		// do nothing
		return 0;
	// Is it in the middle, or not in at all?
	} else{
		inNode->prev->next = inNode->next;
		inNode->next->prev = inNode->prev;
		//Reduce the size before enqueue increases it again
		size--;
		return enqueue(inNode);;
	}	
}

//This function displays the pid of each node in the queue
//Return vlaues:
//      N/A
void TCB_Queue::display()
{
    //Create a pointer to the head of the queue
    TCBnode* node=this->head;
    int counter=0;  //Initialize sizeer

    //Display the number of nodes in the queue
    printf("\n\nTCB size: %d.\n", this->size);

    //Loop through the queue
    while(node!=NULL)
    {
        //Print the PID of the current node
	// ADDED - prints the contents of the process as well
        printf("%d: PID: %d CONTENTS: %s\n",++counter, node->tcb.pid, node->tcb.program_ctr);
        node=node->next;    //Move the node pointer to the next node
    }
}

//Getters and Setters for private variables
int TCB_Queue::get_max_size(){return max_size;}
int TCB_Queue::set_max_size(int _max_size){max_size =_max_size;}
int TCB_Queue::get_size(){return size;}
int TCB_Queue::set_size(int _size){size =_size;}

//This function returns a pointer to the tail of the queue
//Return values:
//      NULL - The queue is empty
//      Not NULL - Pointer to the tail of the queue
TCBnode* TCB_Queue::getTail()
{
    //Return the tail pointer of the queue
    return this->tail;
}

//This function returns a pointer to the head of the queue
//Return values:
//      NULL - The queue is empty
//      Not NULL - Pointer to the head of the queue
TCBnode* TCB_Queue::getHead()
{
    //Return the head pointer of the queue
    return this->head;
}

//This function creates a new node to be placed in the queue
//Returns a pointer to a new TCB node
TCBnode* TCB_Queue::createTCBnode(uint8_t pid,
                                  char* pc,
                                  unsigned int r1,
                                  unsigned int r2,
                                  TCB*    parent,
                                  TCB*    childList,
                                  TCB*    siblingList,
                                  char* openFilesList,
                		  unsigned long	createTime
                		  #ifdef REAL_TIME
                		      ,   unsigned long	deadline
                		  #endif
                		  )  
{
    //Call the TCBnode constructor to create a new node
    TCBnode* newNode = new TCBnode(pid,
                                   pc,
                                   r1,
                                   r2,
                                   parent,
                                   childList,
                                   siblingList,
                                   openFilesList,
                                   createTime
                                   #ifdef REAL_TIME
                                   , deadline
                                   #endif // REAL_TIME
                                   );

    //Return the pointer to the new node
    return newNode;
}

//This function creates a new node when only given the pid
//Return a pointer to the newly created node
TCBnode* TCB_Queue::createTCBnodePID(uint8_t pid, char * pc)
{
    //Return a pointer created using the createTCBnode function
    return this->createTCBnode(pid, pc,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00000000
                            #ifdef REAL_TIME
                            , 0x00000000
                            #endif // REAL_TIME
                            );
}










