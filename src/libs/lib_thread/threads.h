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
#include "../lib_lista_int/lista.h"

#define num_threads 6 //Defina a quantidade de threads a serem utilizadas.
#define N 1000
 //DEFINA O TAMANO DA MATRIZ AQUI
#define LEN 5
#define QTD_COLLUN 24
#define QTD_COLLUN_THREAD 4

/*semaphoro controle de IO para thread especificas de IO*/
#define WAIT 0
#define READY 1
#define EXECUTED -1

typedef unsigned long long int tipoDado;


typedef struct argsArq * ptrArgsArq;
struct argsArq {
	pthread_t thread;
    int statusArq[N];
    int id;
    FILE * arq;
};

typedef struct {
	pthread_t thread;
    lst_ptr lista;
    int id;
    char path_destino[5][100];
    FILE * fptr_origem;
    ptrArgsArq ptrArq;
}args;


void print_matriz(char * dados[N][LEN][QTD_COLLUN]);

void print_colun_matriz(char dados[][LEN][QTD_COLLUN], int j);

FILE *open_arquivo(char * str, char * modo);

void print_responsabilidade_thread(args * _args);

void create_threads(args * _args, int n, char *);

void thread_jobs(args * _args, int, int);
#endif //_THREADS_H
