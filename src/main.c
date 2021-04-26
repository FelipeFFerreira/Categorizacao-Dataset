#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "libs/lib_lista_int/lista.h"
#include "libs/lib_thread/lista_th.h"
#include "libs/lib_thread/threads.h"

//#define INSTALL_OMP //Install_openmp
#define INSTALL_DEBUG exit(0xA);

#ifdef INSTALL_OMP
	#include <omp.h>
#endif

#define HOLD 0
#define PROCEED 1
#define NEXT 2

typedef struct {
	FILE * fptr;
}path_arq; //struct para captura da matriz de entrada

path_arq path_arq_t[1];


 ///* Regiao de Variaves Globais no Escopo main.c *
static lst_ptr_th colun_date[QTD_COLLUN];
static char arq_origem[] = "C:\\GitHub\\Paralela-Matriz-Normalizacao\\arq_csvs\\dataset_00_1000.csv";
pthread_mutex_t mutex;


/// *Lidar com controle de escrita do arquivo de saida principal*
static void * solicitacao_arquivo(void * argsArq)
{
	ptr_args_arq _argssArq = (ptr_args_arq) argsArq;
	int i = 0, j = 0;

	while (i < N) {
        while (j < NUM_THREADS) {
            if (_argssArq->thread_buffer[i % (PROFUNDIDADE_BF)][j].state == DONE) {
                fprintf(_argssArq->arq_main, "%s", _argssArq->thread_buffer[i % (PROFUNDIDADE_BF)][j].word);
                _argssArq->thread_buffer[i % (PROFUNDIDADE_BF)][j].state = TO_DO;
                j += 1;
            }
        }
        j = 0;
        fprintf(_argssArq->arq_main, "\n");
        i += 1;
	}
	fclose(_argssArq->arq_main);
	return 0;
}

/// *Adicionar infos das colunas do arquivo de entrada*
static void add_lst_info_distinct(lst_ptr_th * l, char * str)
{
	lst_info_th info_t;
	int id_t = 0;
	strcpy(info_t.word, str);
	if (!lst_existing_th(*l, info_t, &id_t)) {
		info_t.id = id_t;
		lst_ins_th(l, info_t);
	}
}

/// *Verifica se info da coluna é um job a processar*
static bool is_my_job(lst_ptr l, int colun)
{
    while (l != NULL) {
        if (l->dado == colun)
            return true;
        l = l->prox;
    }
    return false;
}

/// *normaliza info separadamente do arquivo de entrada principal*
static int normalize_info_date(args args_t, char * str, int colun, int * _id_word)
{
    //pthread_mutex_lock(&(mutex_1));
    lst_info_th info_t;
    int colun_fptr = (colun % QTD_COLLUN_THREAD) == 0 ? QTD_COLLUN_THREAD - 1 : (colun % QTD_COLLUN_THREAD) - 1;
	strcpy(info_t.word, str);

	if (is_my_job(args_t.lista, colun)) {
        int id = lst_info_id_th(colun_date[colun - 1], info_t);
        if (id != -1) {
            *_id_word = id;
            fprintf(args_t.fptr_destinos[colun_fptr], "%d, %s\n", id, info_t.word);
            return PROCEED;
        }
        else
            return HOLD;
	}
	//pthread_mutex_unlock(&(mutex_1));
	return NEXT;
}

/// *normaliza info unico do arquivo de entrada em unica saida principal*
void * normaliza_colun_date(void * _args)
{
    //pthread_mutex_lock(&(mutex));
	args * args_t = (args*) _args;
	char str[1001], word[200], word_job[700] = "", *token;
	int i, j, id_word;
	bool repete = false;

    for (i = 0; fscanf(args_t->fptr_origem, " %500[^\n]s", str) != EOF && i < N; i++) {
        token = strtok(str, ",");
        for (j = 0; token != NULL && j < QTD_COLLUN; j++) {
            switch (normalize_info_date(*args_t, token, j + 1, &id_word)) {
                case HOLD : j--; //to do
                    repete = true;
                break;
                case PROCEED :
                    repete = false;
                    if (id_word == 0)
                        sprintf(word, "%s,", token);
                    else
                        sprintf(word, "%d,", id_word);
                    strcat(word_job, word);
                break;
                case NEXT :
                    break;
            }
            if (!repete)
                token = strtok(NULL, ",");
        }
        while (args_t->main_destino->thread_buffer[i % (PROFUNDIDADE_BF)][args_t->id - 1].state == DONE);
        strcpy(args_t->main_destino->thread_buffer[i % (PROFUNDIDADE_BF)][args_t->id - 1].word, word_job);
        args_t->main_destino->thread_buffer[i % (PROFUNDIDADE_BF)][args_t->id - 1].state = DONE;
        sprintf(word_job, "");
    }
    //pthread_mutex_unlock(&(mutex));
    return 0;
}

void * ler_matriz_entrada(void * args)
{
	path_arq * _path_arq_t = (path_arq*) args;
	char str[1001], *token;
	int i, j;

	for (int i = 0; i < QTD_COLLUN; i++)
        lst_init_th(&colun_date[i]);

	for (i = 0; fscanf(_path_arq_t->fptr, " %500[^\n]s", str) != EOF && i < N; i++) {
		token = strtok(str, ",");
		for (j = 0; token != NULL && j < QTD_COLLUN; j++) {
			add_lst_info_distinct(&colun_date[j], token);
			token = strtok(NULL, ",");
		}
	}
	return 0;
}


int main ()
{
	int i, status;
	clock_t tempo;
	args _args[NUM_THREADS]; //numero de args por threads de CPU
	struct args_arq args_main;
	pthread_t thread_1; //thread responsavel pelo arquivo de entrada

    create_threads(_args, NUM_THREADS, arq_origem, &args_main);
    thread_jobs(_args, QTD_COLLUN, NUM_THREADS, &args_main); //repassa trabalhos
    print_responsabilidade_thread(_args);

    path_arq_t[0].fptr = open_arquivo(arq_origem, "r"); //path dataset

    tempo = clock();
    if (status_create( status = pthread_create((&thread_1), NULL, ler_matriz_entrada, (void *)&path_arq_t[0])));
    else exit(0xF);
    if (status_create( status = pthread_create((&args_main.thread), NULL, solicitacao_arquivo, (void *)&args_main)));
    else exit(0xF);

    /*Repassa função de trabalho*/
	for(i = 0; i < NUM_THREADS; i++) {
		if (status_create(status = pthread_create((&_args[i].thread), NULL, normaliza_colun_date, (void *)&_args[i])));
		else exit(0xFF);
	}

    printf("\nEm execucao ...\n");
    /*Thered principal aguarda todas as thredes de trabalhos finalizarem*/
	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(_args[i].thread, NULL);
	}
	pthread_join(args_main.thread, NULL);

    printf("\nTerminando processo ...\n");
	printf("\n\n[Tempo Total Do Processo: %fs]\n", (float) (clock() - tempo)  / CLOCKS_PER_SEC);
	return 0;
}
