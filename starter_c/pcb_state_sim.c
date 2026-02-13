/*
 * CS 440 – PCB Simulator Starter (C)
 * Michael Gavina: 801970144
 *
 *
 * TODO: Add your name(s) and BearID(s)
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
	NEW, READY, RUNNING, WAITING, TERMINATED
} State;

typedef struct PCB {
	int pid;
	char name[32]; // Note this is an arbitrary size selection but me may want to change or specify.
	State state;
	int priority;
	int pc;
	int cpuTime;
} PCB;


// PBC *next will point to the PCB behind it

typedef struct ProcessNode {
	PCB *process;
	ProcessNode *next;
} ProcessNode;

// changed rdyQueue and waitQueue because they were the same struct and we need just one queue struct
// NOTE: For our queue implementation we could avoid malloc and some overhead with a fixed size queue like a fixed size c string but this is another way that allows an arbitrary size.
typedef struct Queue {
	ProcessNode *head;
	ProcessNode *tail;
	int size; // We *may* not need size
} Queue;

int enQueue(Queue *q, PCB *pcb) {
	ProcessNode *node = malloc(sizeof(ProcessNode));
	if(!node) {
		printf("Dang bro you ran out of memory.");
		return 0; // False
	};
	node->data = pcb;
	node->next = NULL;

	if(q->tail == NULL) {
		q->head = q->tail = node;
	} else {
		q->tail->next = node; // The tail points to our new node
		q->tail = node; // Now the tail is our new node so we have pushed the old tail
	}
	return 1; // True
}

PCB *deQueue(Queue *q) {
	if(q->head == NULL) {
		return NULL; // Cannot deQueue an empty queue!
	}

	ProcessNode *tmp = q->head;
	PCB *pcb = tmp->data;
	q->head = tmp->next;
	if(q->head == NULL) {
		q->tail = NULL;
	}
	free(tmp);
	return pcb;
}


typedef struct KernelState {
	PCB *running;
	queue *ready;
	queue *waiting;
} KernelState;

//This function should be double checked to see if it is running correctly
void addQueue(queue **Queue, PCB *process){
	
	if((*Queue)->size == 0){
	((*Queue)->head) = process;
	((*Queue)->tail) = process;
	}

	else{
	PCB cpyProcess;
  (*Queue)->tail->next = process; 
  (*Queue)->tail = process;
  

  /* had to change this because they should all be the same so when read off the the process table they are same process.
	* cpyProcess = *((*Queue)->tail);
	* (*Queue)->tail = process;
	* cpyProcess.next = process;
	*/
  }
  
	(*Queue)->size = (*Queue)->size + 1; // This would be mildly annoying to type in standard form: (*(*queue)).size. 
}

PCB popQueue(queue *Queue){//assumes that the list is not empty
	PCB process;
	process = *(Queue->head);
	Queue->head = process.next;	
	return process;
}




int procsimNum = 1; // I planned on using this to increment with every create. 


int procsimCreate(char name[32], int priority, queue *rdyQueue, queue *processTable){
  // I saw that we need a process table so we can simplifiy this to be just checking the process table.

	PCB *search = rdyQueue->head;
	while(search->next != NULL || processTable->size != 0){
		if (strcmp(search->name, name) == 0) {
			return -1;
		}
		search = search->next;
	}

	PCB process; // if we were feeling extra saucy we could allocate this to the heap with malloc to save precious stack space 
		     // In a larger system that would make sense.
	
	strcpy(process.name, name);
	process.pid = procsimNum;
	procsimNum++;
	process.state = READY;	
	process.priority = priority;
	process.pc = 0;
	process.cpuTime = 0;
	process.next = NULL;

	addQueue(&rdyQueue, &process);
  addQueue(&processTable, &process);

	return 1;
	
}



int procsimDispatch(queue *rdyQueue, PCB **running){
	
	if((*running)->state == RUNNING){
		return -1;
	}
	else{
		PCB process;
		process = popQueue(rdyQueue);	
		*running = &process; 
    (*running)->state = RUNNING;

	  return 1;
	}
}



int procsimTick(int n){


	return 0;
}



int procsimBlock(queue *waitQueue, PCB **running){
 
  if((**running).state == NULL){
    return -1;
  }
  else{
    (**running).state = WAITING;
    addQueue(&waitQueue, &(**running));
    (*running) = NULL;
  }

}



int procsimWake(char name[], queue *waitQueue){



	return 0;
}



int procsimExit(PCB **running){
  


	return 0;
}



void procsimStatus(){
	
}



int procsimKill(char name[], queue *rdyQueue, queue *waitQueue, PCB **running){
	return 0;
}



// TODO: process table
// TODO: READY queue
// TODO: WAITING queue
// TODO: RUNNING pointer
// TODO: BearID auto-STATUS interval

int main(int argc, char *argv[]) {
	// TODO: parse args
	// TODO: read trace file
	// TODO: dispatch commands
	queue rdyQueue;
	queue waitQueue;
	PCB *running = NULL;	
  queue processTable;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	FILE *file = fopen(argv[1], "r");
	if (!file) {
		perror("fopen");
		return 1;
	}

	char line[256];
	char firstWord[256];

	while (fgets(line, sizeof(line), file)) {
		if (sscanf(line, "%255s", firstWord) == 1) {
			printf("First word: %s\n", firstWord);

			if (strcmp(firstWord, "CREATE") == 0) {
				printf("CREATE\n");
			} else if(strcmp(firstWord, "DISPATCH") == 0) {
				printf("DISPATCH\n");
			} else if(strcmp(firstWord, "TICK") == 0) {
				printf("TICK\n");
			} else if(strcmp(firstWord, "BLOCK") == 0) {
				printf("BLOCK\n");
			} else if(strcmp(firstWord, "WAKE") == 0) {
				printf("WAKE\n");
			} else if(strcmp(firstWord, "EXIT") == 0) {
				printf("EXIT\n");
			} else if(strcmp(firstWord, "STATUS") == 0) {
				printf("STATUS\n");
			} else {
				printf("Command Unknown?\n");
			}
		}
	}

	fclose(file);
	return 0;
}

// TODO: implement command handlers
