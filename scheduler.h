#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef struct Process {
    char name[32];
    int burst_time; // tiempo total de CPU requerido.
    int remaining_time; //para round robin
    int arrival; // para calcular metricas
    int waiting; // waiting time
    int turnaround; // tiempo de retorno turnardound time
    struct Process *next; 
} Process;

typedef struct {
    Process *start;
    Process *end;
    int size;
} ReadyQueue;


void mkprocess(char *name, int burst);
void lstprocss();
void simular_fcfs();
void simular_sfj();
void simular_rr();

#endif