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
	char name[32];
	State state;
	int priority;
	int pc;
	int cpuTime;
	struct PCB *next;
} PCB;


// PBC *next will point to the PCB behind it


// changed rdyQueue and waitQueue because they were the same struct and we need just one queue struct
typedef struct queue{
	PCB *head;
	PCB *tail;
	int size;

} queue;

//This function should be double checked to see if it is running correctly
void addQueue(PCB *process, queue **Queue){
	
	if((*Queue)->size == 0){
	((*Queue)->head) = process;
	((*Queue)->tail) = process;
	}

	else{
	PCB cpyProcess;
	cpyProcess = *((*Queue)->tail);
	(*Queue)->tail = process;
	cpyProcess.next = process;
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

	addQueue(&process, &rdyQueue);
  addQueue(&process, &processTable);

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
	  return 1;
	}
}



int procsimTick(int n){


	return 0;
}



int procsimBlock(queue *waitQueue, PCB **running){
 
  


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
