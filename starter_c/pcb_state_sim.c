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


// We could make it so that PCB will point procsim_to the process behind it and that is the next PCB

// That will then make the queue very easy to implement.
typedef struct rdyQueue {
	PCB *head;
	PCB *tail;
	int size;

} rdyQueue;

typedef struct waitQueue {
	PCB *head;
	PCB *tail;
	int size;

} waitQueue;


//These are not final, feel free to change them, I thought if I just laid the ground work it would work well.

int procsim_create(char name[], int priority){
	return 0;
}

int procsim_dispatch(){
	return 0;
}

int procsim_tick(int n){
	return 0;
}

int procsim_block(char name[]){
	return 0;
}

int procsim_exit(char name[]){
	return 0;
}

void status(){

}


// This is extra credit which I would be down to go for

int procsim_kill(char name[]){

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
