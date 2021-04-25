/* threads.h
 *
 * Definição do TAD para operação com threads e funçoes especificas
 * sobre o arquivo main.c, como também os protótipo das operações sobre
 * esse TAD.
 *
 */

#ifndef _THREADS_H
#define _THREADS_H

#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "../lib_lista_int/lista.h"

#define NUM_THREADS 6 //Defina a quantidade de threads a serem utilizadas.
#define N 1000
 //DEFINA O TAMANO DA MATRIZ AQUI
#define LEN 5
#define QTD_COLLUN 24
#define QTD_COLLUN_THREAD 4
#define PROFUNDIDADE_BF 4

/*semaphoro controle de IO para thread especificas de IO*/
#define WAIT 0
#define READY 1
#define EXECUTED -1
#define DONE 1
#define TO_DO 0

#define BLOCK 1
#define LIVRE 0

typedef unsigned long long int tipoDado;
//enum states {AGUARDANDO = 0, PROCESSADO} state_colun;

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
    int lin_job_atual;
};

typedef struct {
    int id;
	pthread_t thread;
    lst_ptr lista;
    FILE * fptr_destinos[QTD_COLLUN_THREAD];
    FILE * fptr_origem;
    ptr_args_arq main_destino;
}args;


void print_matriz(char * dados[N][LEN][QTD_COLLUN]);

void print_colun_matriz(char dados[][LEN][QTD_COLLUN], int j);

FILE *open_arquivo(char * str, char * modo);

void print_responsabilidade_thread(args * _args);

void create_threads(args * _args, int n, char *, ptr_args_arq);

void thread_jobs(args * _args, int, int, ptr_args_arq);
#endif //_THREADS_H
