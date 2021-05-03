/* threads.h
 *
 * Definição do TAD para operação com threads e funçoes especificas
 * sobre o arquivo threads.c, como também os protótipo das operações sobre
 * esse TAD.
 */

#ifndef _THREADS_H
#define _THREADS_H

#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "lista_th.h"
#include "../lib_lista_int/lista.h"

#define NUM_THREADS 6 //Defina a quantidade de threads a serem utilizadas.
#define N 1000
#define QTD_COLLUN 24
#define QTD_COLLUN_THREAD 4
#define PROFUNDIDADE_BF 4

/*semaphoro controle de IO para thread especificas de IO*/
#define DONE 1
#define TO_DO 0
#define BLOCK 1
#define LIVRE 0

typedef unsigned long long int tipoDado;
//enum states {AGUARDANDO = 0, PROCESSADO} state_colun;

typedef struct {
    sem_t mutexs_threads[NUM_THREADS + 1];
    sem_t mutexs_process[NUM_THREADS + 1];
}controles;

typedef struct {
    char word[700];
    bool state;
    bool bloqueio;
}tarefas;

typedef struct args_arq * ptr_args_arq;
struct args_arq {
	pthread_t thread;
    int id;
    FILE * arq_main;
    tarefas thread_buffer[PROFUNDIDADE_BF][NUM_THREADS];
};

typedef struct {
    int id;
	pthread_t thread;
    lst_ptr lista;
    FILE * fptr_destinos[QTD_COLLUN_THREAD];
    ptr_args_arq main_destino;
}args;

typedef struct {
    args _my_set[NUM_THREADS];

}args_memory;

void print_responsabilidade_thread(args * _args);

void create_threads(args * _args, int n, char *, ptr_args_arq, controles *);

void create_threads_mmory_set(args_memory *, unsigned int);

void thread_jobs(args * _args, int, int, ptr_args_arq);

FILE *open_arquivo(char * str, char * modo);
#endif //_THREADS_H
