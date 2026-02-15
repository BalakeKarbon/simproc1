/*
 * CS 440 – PCB Simulator Starter (C)
 * Michael Gavina: 801970144
 * Blake Karbon: 801945974
 *
 * TODO: Add your name(s) and BearID(s)
 */

  
// TODO: Please add your name Blake and bear number


// NOTE: For our queue implementation we could avoid malloc and some overhead with a fixed size queue like a fixed size c string but this is another way that allows an arbitrary size.

// NOTE: I wasn't sure if when he makes his own examples, that he would have more then the amount of processes we have in the test files. Fixed would be very fast. The sizing of the processes was never addressed in the document

// Sorry for any weird formating

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//              0     1       2        3          4
typedef enum { NEW, READY, RUNNING, WAITING, TERMINATED } State;

const char *states[] = {"NEW", "READY", "RUNNING", "WAITING", "TERMINATED"};


//start of structs

typedef struct PCB {
  int pid;
  char name[32]; // Note this is an arbitrary size selection but me may want to change or specify.
  State state;
  int priority;
  int pc;
  int cpuTime;
} PCB;

typedef struct ProcessNode {
  PCB *process;
  struct ProcessNode *next; // this didn't work before because the typedef is not declared until after the struct is done
} ProcessNode;

typedef struct Queue {
  ProcessNode *head;
  ProcessNode *tail;
  int size;
} Queue;

typedef struct KernelState {
  int step;
  PCB *running;
  Queue *ready;
  Queue *waiting;
  Queue *processTable; //I think that this is needed because of status, and could be very useful when freeing memory 
} KernelState;

// end of structs

// start of queue functions

int enQueue(Queue *q, PCB *pcb) {
  ProcessNode *node = (ProcessNode *)malloc(sizeof(ProcessNode));
  if (!node) {
    fprintf(stderr, "Dang bro you ran out of memory.\n");
    return 0; // False
  };
  // node->data = pcb // this is the original line but I'm pretty sure the process pointer is meant to be pointing to the pcb?
  node->process = pcb;
  node->next = NULL;

  if (q->tail == NULL) {
    q->head = q->tail = node;
  } else {
    q->tail->next = node; // The tail points to our new node
    q->tail = node; // Now the tail is our new node so we have pushed the old tail
  }
  return 1; // True
}

PCB *deQueue(Queue *q) {
  if (q->head == NULL) {
    return NULL; // Cannot deQueue an empty queue!
  }

  ProcessNode *tmp = q->head;
  PCB *pcb = tmp->process;
  q->head = tmp->next;
  if (q->head == NULL) {
    q->tail = NULL;
  }
  free(tmp);
  tmp = NULL; // dangling pointer
  return pcb;
}

PCB *removeQueue(Queue *q, char name[32]) { //this won't a problem for kill, just take the pointer and free it
  PCB *process;
    if(q->head == NULL){
    return NULL;
  }
  
  ProcessNode *right = q->head->next;
  ProcessNode *left = q->head;
  
  if(strcmp(left->process->name, name) == 0){
    process = deQueue(q);
    return process;
  }

  while(right != NULL){
    if(strcmp(right->process->name, name) == 0){
      process = right->process;
      left->next = right->next;
      free(right);
      right = NULL;
      return process;
    }
    left = right;
    right = right->next;
  }
  return NULL; // Unable to find process name
}

// end of queue functions


// start of procsim functions

int procsimNum = 1; // I planned on using this to increment with every create.

int procsimCreate(char name[32], int priority, KernelState *ks) {
  // I saw that we need a process table so we can simplifiy this to be just checking the process table.

  ProcessNode *search = ks->processTable->head;
  while (search->next == NULL) {
    if (strcmp(search->process->name, name) == 0) {
      return 0; // process already made
    }
    search = search->next;
  }

  PCB *process = (PCB *)malloc(sizeof(PCB));

  strcpy(process->name, name);
  process->pid = procsimNum;
  procsimNum++;
  process->state = NEW;
  process->priority = priority;
  process->pc = 0;
  process->cpuTime = 0;

  int returnCode = enQueue(ks->ready, process); //this might make overhead
  if (returnCode == 0) {
    return -1; // memory error
  }
  
  
  returnCode = enQueue(ks->processTable, process);
  if (returnCode == 0) {
    return -1; // memory error
  }
  process->state = READY; 
  return 1;
}

int procsimDispatch(KernelState *ks) {

  if (ks->running->state == RUNNING) {
    return 0; // process already running
  } else if (ks->ready->size <= 0) {
    return -1; // no processes in ready
  }
  PCB *tmp;

  tmp = deQueue(ks->ready);
  if (!tmp) {
    return -1; // no processes in ready
  }
  tmp->state = RUNNING;
  ks->running = tmp;

  return 1;
}

int procsimTick(int n, KernelState* ks) { 
  if(ks->running == NULL){
    return 0; //nothing to increase
  }
  
  ks->running->cpuTime = ks->running->cpuTime + n;
  ks->running->pc = ks->running->pc + n;
  return 1; 
}

int procsimBlock(char name[32], KernelState *ks) {
  if (!ks->running) {
    return 0; // no process in the running pointer
  } else if (ks->running->state != RUNNING) {
    return -1; // Something has messed up
  }
  int returnCode = enQueue(ks->waiting, ks->running); // queue, PCB
  if (returnCode == 0) {
    return -2; // memory error
  }

  ks->running = NULL;

  return 1;
}

int procsimWake(char name[32], KernelState *ks) { 
  PCB *process;
  if(ks->waiting == NULL){
    return 0; // wating is empty, nothing to wake
  }
  
  process = removeQueue(ks->waiting, name);

  if(!process){
    return -1; // there was no process with that name in waiting
  }
  
  int returnCode = enQueue(ks->ready, process);
  process->state = READY;

  if(returnCode == 0){
    return -2; //Memory error
  }

  return 1; 
}

int procsimExit(KernelState *ks) { 
  if(ks->running == NULL){
    return 0; // no process running
  }
  PCB *process = ks->running;
  ks->running = NULL;
  process->state = TERMINATED; // Can't be freed yet because needed with process table
  return 1;
}

void procsimStatus(KernelState *ks) {
  printf("RUNNING:");
  if(ks->running == NULL){
  printf(" NONE\n");
  }
  else{
  printf(" %s\n", ks->running->name);
  }
  
  ProcessNode *chain = ks->ready->head;

  printf("READY: [");
  while(chain != NULL){
    printf(",");
    chain = chain->next;
    if(chain == NULL);
    else printf(" %s",chain->process->name);
    }
  printf("]\n");

  chain = ks->waiting->head;

  printf("WAITING: [");
  while(chain != NULL){
    printf(",");
    chain = chain->next;
    if(chain == NULL);
    else printf(" %s",chain->process->name);
  }
  printf("]\n");
  
  printf("TABLE:\n");
  printf("PID\tNAME\tSTATE\tPRIO\tPC\tCPUTIME\n");
 
  chain = ks->processTable->head;

  while(chain != NULL){
  printf("%d\t%s\t%s\t%d\t%d\t%d\t%d\n", chain->process->pid, chain->process->name, states[chain->process->state],chain->process->priority,chain->process->pc,chain->process->cpuTime);
  chain = chain->next;
  }
}

int procsimKill(char name[], KernelState *ks) { return 1; }

// end of procsim functions


// TODO: process table
// TODO: READY queue
// TODO: WAITING queue
// TODO: RUNNING pointer
// TODO: BearID auto-STATUS interval

int main(int argc, char *argv[]) {
  // TODO: parse args
  // TODO: read trace file
  // TODO: dispatch commands
  // TODO: implement command handlers
 
  const int STATUS_STEPS = 7;

  KernelState *ks = (KernelState *)malloc(sizeof(KernelState));
  ks->ready = (Queue *)malloc(sizeof(Queue));
  ks->waiting = (Queue *)malloc(sizeof(Queue));
  ks->processTable = (Queue *)malloc(sizeof(Queue));
 

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
    if (sscanf(line, "%31s", firstWord) == 1) {
      printf("%s",line);
      ks->step++;
      printf("First word: %s\n", firstWord);
      if (strcmp(firstWord, "CREATE") == 0) {
        printf("%d CREATE\n",ks->step);
	PCB *new = malloc(sizeof(PCB));
	sscanf(line, "%*s %31s %d", new->name, &new->priority);
	enQueue(ks->ready, new); // We may have a function for these.
      } else if (strcmp(firstWord, "DISPATCH") == 0) {
        printf("%d DISPATCH\n",ks->step);
	ks->running = deQueue(ks->ready);
      } else if (strcmp(firstWord, "TICK") == 0) {
        printf("%d TICK\n",ks->step);
	int count = 0;
	sscanf(line, "%*s %d", count);
	procsimTick(count, ks); 
      } else if (strcmp(firstWord, "BLOCK") == 0) {
        printf("%d BLOCK\n",ks->step);
	enQueue(ks->waiting,ks->running);
	ks->running = deQueue(ks->ready);
      } else if (strcmp(firstWord, "WAKE") == 0) {
        printf("%d WAKE\n",ks->step);

      } else if (strcmp(firstWord, "EXIT") == 0) {
        printf("%d EXIT\n",ks->step);
      } else if (strcmp(firstWord, "STATUS") == 0) {
        printf("%d STATUS\n",ks->step);
      } else {
	ks->step--;
        printf("Command Unknown?\n");
      }
    }
  }

  fclose(file);
 
  if(ks->running != NULL){
    free(ks->running);
    ks->running = NULL;
  }
  
  ProcessNode *memoryFree = ks->processTable->head; // I think that if I free the all of the process on the process table, I can just free the processNodes in each queue.
  ks->processTable->head = ks->processTable->tail = NULL;
  
  while(memoryFree != NULL){
    if(memoryFree->process != NULL){ // there should be one process that will be NULL, if there is one in running
      free(memoryFree->process);
      memoryFree->process = NULL;
    }
    ProcessNode *tmp = memoryFree;
    memoryFree = memoryFree->next;
    free(tmp);
    tmp = NULL;
 
  } 

  memoryFree = NULL;
  free(ks->processTable);
  ks->processTable = NULL;

  memoryFree = ks->ready->head;
  ks->ready->head = ks->ready->tail = NULL;
  
  while(memoryFree != NULL){
    ProcessNode *tmp = memoryFree;
    memoryFree = memoryFree->next;
    free(tmp);
    tmp = NULL;
  }

  memoryFree = NULL;
  free(ks->ready);
  ks->ready = NULL;


  memoryFree = ks->waiting->head;
  ks->waiting->head = ks->waiting->tail = NULL;
  
  while(memoryFree != NULL){
    ProcessNode *tmp = memoryFree;
    memoryFree = memoryFree->next;
    free(tmp);
    tmp = NULL;
  }

  memoryFree = NULL;
  free(ks->waiting);
  ks->waiting = NULL;

  return 0;
}

