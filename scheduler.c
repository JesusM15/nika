#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Process *readyQueue = NULL; // inicio de la cola.

void mkprocess(char *name, int burst){
    Process *new = (Process *) malloc(sizeof(Process)); // reservo espacio para el nuevo proceso.
    strcpy(new->name, name);
    new->burst_time = burst;
    new->remaining_time = burst; 
    new->waiting = 0;
    new->turnaround = 0;
    new->next = NULL;

    if(readyQueue == NULL){
        readyQueue = new;
    } else {
        Process *tmp = readyQueue;
        while(tmp->next != NULL){
            tmp = tmp->next;
        }
        tmp->next = new;
    }

    printf("Proceso %s creado con burst %d\n", name, burst);
}

void lstprocss(){
    if(readyQueue == NULL){
        printf("La cola de procesos esta vacia.\n");
        return;
    }

    printf("\n*Ready Queue*\n");
    Process *tmp = readyQueue;
    while(tmp != NULL){
        printf("PID: %s | Burst Time: %d\n", tmp->name, tmp->burst_time);
        tmp = tmp->next;
    }
}

int count_processes(){
    int c = 0;
    Process *tmp = readyQueue;
    while(tmp != NULL){
        c++;
        tmp = tmp->next;
    }
    return c;
}

void printAndClean(const char *algorithm, float avgWaitingTime, float avgTurnAroundTime){
    if(readyQueue == NULL){
        printf("No hay procesos que imprimir.\n");
        return;
    }

    printf("\n * Tabla de resultados de: %s *\n", algorithm);
    printf("%-20s %-20s %-20s\n", "Process ID", "Waiting", "Turn Around");
    Process *tmp = readyQueue;
    
    while(tmp != NULL){
        printf("%-20s %-20d %-20d\n", tmp->name, tmp->waiting, tmp->turnaround);

        Process *aux = tmp;
        tmp = tmp->next;
        free(aux);
    }

    readyQueue = NULL;

    printf("--------------------------------------------------------\n");
    printf("Promedio Waiting Time:       %.2f\n", avgWaitingTime);
    printf("Promedio Turn Around Time:       %.2f\n", avgTurnAroundTime);
}

void simular_fcfs(){
    int waitingTimeAvg = 0;
    int turnAroundAvg = 0;
    int processCounter = 0;

    int clock = 0;
    // turnaround = waitingTotalTime + burstTime
    if(readyQueue == NULL) {
        printf("Cola de procesos vacia.\n");
        return;
    }

    Process *current = readyQueue; 

    while(current != NULL){
        current->waiting = clock;
        printf("Proceso %s asignado al CPU, Tiempo Actual: %d.\n", current->name, clock);

        clock += current->burst_time;
        current->remaining_time = 0;
        // calculos

        current->turnaround = current->waiting + current->burst_time;
        
        printf("Proceso %s sale del CPU, Tiempo Actual: %d.\n", current->name, clock);
        waitingTimeAvg += current->waiting;
        turnAroundAvg += current->turnaround;

        current = current->next;
    }

    processCounter = count_processes();

    waitingTimeAvg = waitingTimeAvg / processCounter;
    turnAroundAvg = turnAroundAvg / processCounter;

    printAndClean("FCFS", waitingTimeAvg, turnAroundAvg);
}

