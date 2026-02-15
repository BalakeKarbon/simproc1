/*
 * CS 440 – PCB Simulator Starter (C)
 * Michael Gavina: 801970144
 * Blake Karbon: 801945974
 * 
 * TODO: Add your name(s) and BearID(s)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//              0     1       2        3          4
typedef enum { NEW, READY, RUNNING, WAITING, TERMINATED } State;

const char *states[] = {"NEW", "READY", "RUNNING", "WAITING", "TERMINATED"};


//start of structs

typedef struct PCB {
    int pid; // Extra
    char name[32];
    State state;
    int priority;
    int pc;
    int cpuTime;
} PCB;

typedef struct ProcessNode {
    PCB *process;
    struct ProcessNode *next;
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
    int pidCounter; // Extra
} KernelState;

// end of structs

// start of queue functions

void initQueue(Queue *q) {
    q->head = q->tail = NULL;
    q->size = 0;
}

int enQueue(Queue *q, PCB *pcb) {
    ProcessNode *node = (ProcessNode *)malloc(sizeof(ProcessNode));
    if (!node) {
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
    q->size++; // Increment our queue size
    return 1;
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
    tmp = NULL; // Prevent - dangling pointer
    q->size--;
    return pcb;
}

PCB *removeFromQueue(Queue *q, char name[]) {
    if (q->head == NULL) { 
        return NULL;
    }
    ProcessNode *curr = q->head;
    ProcessNode *prev = NULL;
    while (curr != NULL) {
        if (strcmp(curr->process->name, name) == 0) {
            if (prev == NULL) {
                q->head = curr->next;
            } else {
                prev->next = curr->next;
            }
            if (curr == q->tail) {
                q->tail = prev;
            }
            PCB *pcb = curr->process;
            free(curr);
            q->size--;
            return pcb;
        }
        prev = curr;
        curr = curr->next;
    }
    return NULL;
}

PCB *findInQueue(Queue *q, char name[]) {
    ProcessNode *curr = q->head;
    while (curr != NULL) {
        if (strcmp(curr->process->name, name) == 0) return curr->process;
        curr = curr->next;
    }
    return NULL;
}

// end of queue functions

// start of procsim functions

void procsimCreate(KernelState *ks, char name[], int priority) {
    if (findInQueue(ks->processTable, name)) {
        printf("[step=%d] CMD=CREATE %s %d | ERROR | process name already exists\n", ks->step, name, priority);
        return;
    }
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    pcb->pid = ++ks->pidCounter;
    strcpy(pcb->name, name);
    pcb->state = READY;
    pcb->priority = priority;
    pcb->pc = 0;
    pcb->cpuTime = 0;
    enQueue(ks->ready, pcb);
    enQueue(ks->processTable, pcb);
    printf("[step=%d] CMD=CREATE %s %d | OK | %s: NEW -> READY (pid=%d)\n", ks->step, name, priority, name, pcb->pid);
}

void procsimDispatch(KernelState *ks) {
    if (ks->running) {
        printf("[step=%d] CMD=DISPATCH | ERROR | CPU already running %s\n", ks->step, ks->running->name);
        return;
    }
    PCB *pcb = deQueue(ks->ready);
    if (!pcb) {
        printf("[step=%d] CMD=DISPATCH | ERROR | no READY processes\n", ks->step);
        return;
    }
    pcb->state = RUNNING;
    ks->running = pcb;
    printf("[step=%d] CMD=DISPATCH | OK | %s: READY -> RUNNING\n", ks->step, pcb->name);
}

void procsimTick(KernelState *ks, int n) {
    if (!ks->running) {
        printf("[step=%d] CMD=TICK %d | ERROR | no process running\n", ks->step, n);
        return;
    }
    ks->running->pc += n;
    ks->running->cpuTime += n;
    printf("[step=%d] CMD=TICK %d | OK | %s: pc+=%d cpuTime+=%d\n", ks->step, n, ks->running->name, n, n);
}

void procsimBlock(KernelState *ks, char name[]) {
    if (!ks->running) {
        printf("[step=%d] CMD=BLOCK %s | ERROR | no process running\n", ks->step, name);
        return;
    }
    if (strcmp(ks->running->name, name) != 0) {
        printf("[step=%d] CMD=BLOCK %s | ERROR | process %s is not RUNNING\n", ks->step, name, name);
        return;
    }
    PCB *pcb = ks->running;
    pcb->state = WAITING;
    enQueue(ks->waiting, pcb);
    ks->running = NULL;
    printf("[step=%d] CMD=BLOCK %s | OK | %s: RUNNING -> WAITING\n", ks->step, name, name);
}

void procsimWake(KernelState *ks, char name[]) {
    PCB *pcb = removeFromQueue(ks->waiting, name);
    if (!pcb) {
        printf("[step=%d] CMD=WAKE %s | ERROR | process %s not in WAITING\n", ks->step, name, name);
        return;
    }
    pcb->state = READY;
    enQueue(ks->ready, pcb);
    printf("[step=%d] CMD=WAKE %s | OK | %s: WAITING -> READY\n", ks->step, name, name);
}

void procsimExit(KernelState *ks, char name[]) {
    if (!ks->running) {
        printf("[step=%d] CMD=EXIT %s | ERROR | no process running\n", ks->step, name);
        return;
    }
    if (strcmp(ks->running->name, name) != 0) {
        printf("[step=%d] CMD=EXIT %s | ERROR | process %s is not RUNNING\n", ks->step, name, name);
        return;
    }
    PCB *pcb = ks->running;
    pcb->state = TERMINATED;
    ks->running = NULL;
    printf("[step=%d] CMD=EXIT %s | OK | %s: RUNNING -> TERMINATED\n", ks->step, name, name);
}

void procsimKill(KernelState *ks, char name[]) {
    PCB *pcb = findInQueue(ks->processTable, name);
    if (!pcb) {
        printf("[step=%d] CMD=KILL %s | ERROR | process %s does not exist\n", ks->step, name, name);
        return;
    }
    if (pcb->state == TERMINATED) {
        printf("[step=%d] CMD=KILL %s | ERROR | process %s is already TERMINATED\n", ks->step, name, name);
        return;
    }

    State oldState = pcb->state;
    const char *extra = "";

    if (ks->running && strcmp(ks->running->name, name) == 0) {
        ks->running = NULL;
        extra = " (CPU now NONE)";
    } else if (removeFromQueue(ks->ready, name)) {
        extra = " (removed from READY)";
    } else if (removeFromQueue(ks->waiting, name)) {
        extra = " (removed from WAITING)";
    }

    pcb->state = TERMINATED;
    printf("[step=%d] CMD=KILL %s | OK | %s: %s -> TERMINATED%s\n", ks->step, name, name, states[oldState], extra);
}

void printStatus(KernelState *ks, int isAuto) {
    if (!isAuto) {
        printf("[step=%d] CMD=STATUS | OK\n", ks->step);
    }
    printf("RUNNING: %s\n", ks->running ? ks->running->name : "NONE");
    
    printf("READY: [");
    ProcessNode *curr = ks->ready->head;
    while (curr) {
        printf("%s%s", curr->process->name, curr->next ? ", " : "");
        curr = curr->next;
    }
    printf("]\n");

    printf("WAITING: [");
    curr = ks->waiting->head;
    while (curr) {
        printf("%s%s", curr->process->name, curr->next ? ", " : "");
        curr = curr->next;
    }
    printf("]\n");

    printf("TABLE:\n");
    printf("%-4s %-8s %-10s %-4s %-4s %-7s\n", "PID", "NAME", "STATE", "PRIO", "PC", "CPUTIME");
    curr = ks->processTable->head;
    while (curr) {
        printf("%-4d %-8s %-10s %-4d %-4d %-7d\n", 
            curr->process->pid, curr->process->name, states[curr->process->state], 
            curr->process->priority, curr->process->pc, curr->process->cpuTime);
        curr = curr->next;
    }
}

// end of procsim functions

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <trace_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("fopen");
        return 1;
    }

    int N = 4; // Last digit of BEAR Number
    int R = N+3;
    printf("BearID last digit: %d\n", N);
    printf("Auto STATUS every: %d steps\n\n", R);
    printf("---- BEGIN LOG ----\n");

    KernelState ks;
    ks.step = 0;
    ks.running = NULL;
    ks.ready = (Queue *)malloc(sizeof(Queue));
    ks.waiting = (Queue *)malloc(sizeof(Queue));
    ks.processTable = (Queue *)malloc(sizeof(Queue));
    initQueue(ks.ready);
    initQueue(ks.waiting);
    initQueue(ks.processTable);
    ks.pidCounter = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '#' || line[0] == '\0') continue;

        char cmd[32];
        if (sscanf(line, "%s", cmd) != 1) continue;

        ks.step++;

        if (strcmp(cmd, "CREATE") == 0) {
            char name[32];
            int prio;
            sscanf(line, "CREATE %s %d", name, &prio);
            procsimCreate(&ks, name, prio);
        } else if (strcmp(cmd, "DISPATCH") == 0) {
            procsimDispatch(&ks);
        } else if (strcmp(cmd, "TICK") == 0) {
            int n;
            sscanf(line, "TICK %d", &n);
            procsimTick(&ks, n);
        } else if (strcmp(cmd, "BLOCK") == 0) {
            char name[32];
            sscanf(line, "BLOCK %s", name);
            procsimBlock(&ks, name);
        } else if (strcmp(cmd, "WAKE") == 0) {
            char name[32];
            sscanf(line, "WAKE %s", name);
            procsimWake(&ks, name);
        } else if (strcmp(cmd, "EXIT") == 0) {
            char name[32];
            sscanf(line, "EXIT %s", name);
            procsimExit(&ks, name);
        } else if (strcmp(cmd, "STATUS") == 0) {
            printStatus(&ks, 0);
        } else if (strcmp(cmd, "KILL") == 0) {
            char name[32];
            sscanf(line, "KILL %s", name);
            procsimKill(&ks, name);
        } else {
            printf("[step=%d] CMD=%s | ERROR | unknown command\n", ks.step, cmd);
        }

        if (ks.step > 0 && ks.step % R == 0) {
            printStatus(&ks, 1);
        }
    }

    printf("---- END LOG ----\n");
    fclose(file);

    ProcessNode *curr = ks.processTable->head;
    while (curr) {
        free(curr->process);
        ProcessNode *tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    free(ks.processTable);

    curr = ks.ready->head;
    while (curr) {
        ProcessNode *tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    free(ks.ready);

    curr = ks.waiting->head;
    while (curr) {
        ProcessNode *tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    free(ks.waiting);

    return 0;
}
