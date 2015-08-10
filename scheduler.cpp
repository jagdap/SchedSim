// ---------------------------------------------------------
/*
	File: scheduler.cpp
	Package: SchedulerSim

	Author: John Pecarina
	Date: 17 July 2015

	Description: A simple dispatcher function for a computer.
	License/Copyright: None
*/
//---------------------------------------------------------
/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "TCB.h"
#include "TCB_Queue.h"
//Use the standard namespace
using namespace std;

/*--------------------------------------------------------------------------*/
/* CLASS METHODS */
/*--------------------------------------------------------------------------*/
   
Scheduler::Scheduler(){
	//Intialize the new, ready, wait and terminated queues
	nq = new TCB_Queue(MAX_QUEUE_SIZE); //new_queue;
	rq = new TCB_Queue(MAX_QUEUE_SIZE); //ready_queue;
	wq = new TCB_Queue(MAX_QUEUE_SIZE); //wait_queue;
	tq = new TCB_Queue(MAX_QUEUE_SIZE); //terminated_queue;
	last_run = 0;

	//build an idle TCB node
	char *nop = "N";	
	idle = tq->createTCBnodePID(0, nop);
}

void Scheduler::load_new_job(uint8_t pid, char* program){
	//Fill the new queue with jobs
	TCBnode* node;
	node=nq->createTCBnodePID(pid, program);
	nq->enqueue(node);
	//nq->display();
	printf("%d\n", nq->get_size());
}

void Scheduler::load_last_job(uint8_t pid, char* program){
	//Set the last TCB (which will exit when needed)
	last=nq->createTCBnodePID(pid, program);
}

void Scheduler::run_scheduler(){
	printf("In SCHEDULER::RunScheduler\n");
	//One way you could do this...
	resume_threads();	//moves threads from wait if signals rec'd
	add_new_threads();	//moves threads from new to ready		
	sort_ready_queue();	//sort the ready queue from the dispatcher
	/* sort_ready_queue() can be implemented in several ways depending on the Algorithm requested */
	rq->display();
}

TCBnode** Scheduler::get_last_TCBnode(){return &last;}
TCBnode** Scheduler::get_idle_TCBnode(){return &idle;}
void Scheduler::set_idle_TCBnode(TCBnode ** _node){idle = *_node;}
void Scheduler::set_last_run(long l){last_run = l;}
long Scheduler::get_last_run(){	return last_run;}

// Added functions

// Move threads from wait if signals rec'd
void Scheduler::resume_threads(){
	// The threads were moved into the wait Q when save_state_of_CPU
	// was run by the kernel
	
	// Assumption: by the time scheduler is called, all processes in the waitQ are done waiting
	// To implement this assumption, we'll the program counter for every process in the Q by 1 (to the next op)
	TCBnode* curNode = wq->getHead();	
	while (curNode != NULL){
		curNode->tcb.program_ctr++;
		curNode = curNode->next;
	}	
	// Now, we'll move all processes back to the ready q
	while (rq->get_size() <= rq->get_max_size() && wq->get_size()>0) rq->enqueue(wq->dequeue());
	//printf("WQ size: %d", wq->get_size());
}
// Move threads from new to ready
void Scheduler::add_new_threads(){
	// If there's space, add threads from the new Q to the ready Q
	// These threads were added here by init() in the kernel
	while (wq->get_size() <= wq->get_max_size() && nq->get_size()>0) rq->enqueue(nq->dequeue());
}

void Scheduler::sort_ready_queue(){
	//schedAlgorithm = SJF;
	switch(SCHEDALGORITHM){
		case SJF:{
				// Sort the ready queue to put the shortest job in the front
				int curSize = 0, nextSize = 0;
				TCBnode* next, *cur = rq->getHead();
				if (cur!=NULL) next = cur->next;
				
				int i = 0;
				while (i<rq->get_size()-1 && cur != NULL & next != NULL){
					// Figure out how long cur is
					while (*(cur->tcb.program_ctr + curSize)!='\0') curSize++;
					// Figure out how long next is
					while (*(next->tcb.program_ctr + nextSize)!='\0') nextSize++;
					// if current is greater than next, send cur to tail
					//printf ("cur: %s,%d %d  nex: %s,%d %d\n", cur->tcb.program_ctr, curSize,cur->tcb.pid, next->tcb.program_ctr, nextSize, next->tcb.pid);
					if (curSize > nextSize) {
						rq->moveToTail(cur);
					//	printf("sending to tail...\n");
						cur = rq->getHead();
						next = cur->next;
						i = 0;// Reset i to start at beginning
					} else { // Correct order, move on to next
						//printf("%s is less than %s\n",cur->tcb.program_ctr, next->tcb.program_ctr); 
						cur = cur->next;
						next = next->next;
						i++;
					}
					// Reset our sizes for the next iteration
					curSize = 0;
					nextSize = 0;
					//printf ("size of head = %d\n", curSize);
				}
			}
			break;
		case RR8:{
				// Set quantum to 8	
			}
			break;
		case FCFS:
		default:;
	}
}


	


