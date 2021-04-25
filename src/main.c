#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "libs/lib_lista_int/lista.h"
#include "libs/lib_thread/lista_th.h"
#include "libs/lib_thread/threads.h"



/* Para simulação com openmp descomentar INSTALL_OMP
 * e no arquivo threads.h utilizar apenas 2 threads
 * para utilizar as threads implementada no modulo da solução
 * manter comentado e variar os numeros de threads que desejar.
*/

//#define INSTALL_OMP //Install_openmp
#define INSTALL_DEBUG exit(0xA);

#ifdef INSTALL_OMP
	#include <omp.h>
#endif

tipoDado matrizResultante[N][N];
//static char dados[N][QTD_COLLUN][LEN]; // go to pq static aq?

#define HOLD 0
#define PROCEED 1
#define NEXT 2

typedef struct {
	FILE * fptr;
}path_arq; //struct para captura da matriz de entrada

path_arq path_arq_t[1];

static lst_ptr_th colun_date[QTD_COLLUN];
static char arq_origem[] = "C:\\GitHub\\Paralela-Matriz-Normalizacao\\arq_csvs\\dataset_00_1000.csv";
static int state;
pthread_mutex_t mutex;
pthread_mutex_t mutex_1;




static void * multiplicacao(void * argss)
{}


void * solicitacao_arquivo(void * argsArq)
{
	ptr_args_arq _argssArq = (ptr_args_arq) argsArq;

	int i = 0, j = 0;

	while (i < N) {
        _argssArq->lin_job_atual = i % PROFUNDIDADE_BF;
        while (j < NUM_THREADS) {
            //printf("[%d]%s\n", i,  _argssArq->thread_buffer[i % QTD_COLLUN_THREAD][j].word);
            if (_argssArq->thread_buffer[i % (PROFUNDIDADE_BF)][j].state == DONE) {
                //printf("TH-ARQ> [%d][%d].%s\n", i % PROFUNDIDADE_BF, j, _argssArq->thread_buffer[i % (PROFUNDIDADE_BF)][j].word);
                fprintf(_argssArq->arq_main, "%s", _argssArq->thread_buffer[i % (PROFUNDIDADE_BF)][j].word);
                _argssArq->thread_buffer[i % (PROFUNDIDADE_BF)][j].state = TO_DO;
                j += 1;
                _argssArq->lin_job_pendente = j;
            }
            //if (i > 500)return;
            //printf("%d.[%d][%d]TH->ARQ> Aguardando linha:%d ..\n", i, i % PROFUNDIDADE_BF, j, _argssArq->lin_job_pendente);
        }
        j = 0;
        fprintf(_argssArq->arq_main, "\n");
        i += 1;
	}
	fclose(_argssArq->arq_main);
	pthread_exit( (void*) 0 );	//Legado do retorno

}


/*
 * ******* Regiao de Variaves Globais *************
 */

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


static bool is_my_job(lst_ptr l, int colun)
{
    while (l != NULL) {
        if (l->dado == colun)
            return true;
        l = l->prox;
    }
    return false;
}

static int normalize_info_date(args args_t, char * str, int colun, int * _id_word)
{
    //pthread_mutex_lock(&(mutex_1));
    //printf("\nID:%d aqui (normalize_info)\n", args_t.id);
    lst_info_th info_t;
    int colun_fptr = (colun % QTD_COLLUN_THREAD) == 0 ? QTD_COLLUN_THREAD - 1 : (colun % QTD_COLLUN_THREAD) - 1;
	strcpy(info_t.word, str);
	char str_debug[70];

	if (is_my_job(args_t.lista, colun)) {
        int id = lst_info_id_th(colun_date[colun - 1], info_t);
        if (id != -1) {
            *_id_word = id;
            fprintf(args_t.fptr_destinos[colun_fptr], "%d, %s\n", id, info_t.word); //to do, quando id == 0
            return PROCEED;
        }
        else {
                return HOLD;
        }
	}
	//pthread_mutex_unlock(&(mutex_1));
	return NEXT;
}

void * normaliza_colun_date(void * _args)
{
    //pthread_mutex_lock(&(mutex));
	args * args_t = (args*) _args;
	char str[1001], word[100], word_job[700] = "", *token;
	int i, j, id_word;
	bool repete = false;
    //printf("ID:%d estou aqui (normaliza_colun)\n", args_t->id);
    for (i = 0; fscanf(args_t->fptr_origem, " %500[^\n]s", str) != EOF && i < N; i++) {
        token = strtok(str, ",");
        for (j = 0; token != NULL && j < QTD_COLLUN; j++) {
            switch (normalize_info_date(*args_t, token, j + 1, &id_word)) {
                case HOLD : j--; //to do
                           //printf("HOLD\n");
                            repete = true;
                    break;
                case PROCEED ://printf("PROCEED\n");
                                repete = false;
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

        //printf("%s\n", word_job);
        //printf("TH_MAIN> %d.[%d][th-%d]. %s\n\n", i, i % QTD_COLLUN_THREAD, args_t->id - 1, args_t->main_destino->thread_buffer[i % QTD_COLLUN_THREAD][args_t->id - 1].word);
        //printf("%d \n", i % QTD_COLLUN_THREAD);

        sprintf(word_job, "");
        //printf("world_job: %s\n", word_job);
        //system("pause");
    }
    //pthread_mutex_unlock(&(mutex));
}

void * ler_matriz_entrada(void * args)
{
	path_arq * _path_arq_t = (path_arq*) args;
	char str[1001], *token;
	int i, j;

	state = WAIT;
	for (int i = 0; i < QTD_COLLUN; i++)
        lst_init_th(&colun_date[i]);

	for (i = 0; fscanf(_path_arq_t->fptr, " %500[^\n]s", str) != EOF && i < N; i++) {
		token = strtok(str, ",");
		for (j = 0; token != NULL && j < QTD_COLLUN; j++) {
			add_lst_info_distinct(&colun_date[j], token);
			token = strtok(NULL, ",");
		}
	}
	state = EXECUTED;
}


int main ()
{
	tipoDado i, j;
	int status;
	clock_t tempo;
	args _args[NUM_THREADS]; //numero de args por threads de CPU
	struct args_arq args_main;
	pthread_t thread_1; //thread responsavel pelo arquivo de entrada

    create_threads(_args, NUM_THREADS, arq_origem, &args_main);
    thread_jobs(_args, QTD_COLLUN, NUM_THREADS, &args_main); //repassa trabalhos

    path_arq_t[0].fptr = open_arquivo(arq_origem, "r"); //path dataset

    if (status_create( status = pthread_create((&thread_1), NULL, ler_matriz_entrada, (void *)&path_arq_t[0])));
    else exit("0xF");
    if (status_create( status = pthread_create((&args_main.thread), NULL, solicitacao_arquivo, (void *)&args_main)));
    else exit("0xF");
    //pthread_join(thread_1, NULL); //go to, so funciona se usar

    //print_responsabilidade_thread(_args);
    /*Repassa função de trabalho*/
	for(i = 0; i < NUM_THREADS; i++) {
		//_args[i].ptrArq = &_argsArq;
		if (status_create(status = pthread_create((&_args[i].thread), NULL, normaliza_colun_date, (void *)&_args[i])));
		else exit(0x555);
	}
    printf("Em execucao ...\n");
    /*Thered principal aguarda todas as thredes de trabalhos finalizarem*/
	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(_args[i].thread, NULL);
	}
	pthread_join(args_main.thread, NULL);
	exit(0);



    for(i = 0; i < NUM_THREADS; i++) {
		for (j = 0; j < 4; j++);
          // fclose(_args[i].fptr[j]);
	}
    fclose(path_arq_t[0].fptr);

	fclose(path_arq_t[1].fptr);
	//lst_print(colun_date[1]);



	//for(i = 0; i < N; i++) _argsArq.statusArq[i] = WAIT; //coloca todas as linhas em estado de espera.

	/*Repassa o identificador para as threads*/
	for(i = 0; i < NUM_THREADS - 1; i++) {
		lst_init(&_args[i].lista);
		_args[i].id = i + 1;
	}
	//_argsArq.id = i; //id threads arq saida.

	/*Repassa linha de trabalho balanceada da matriz, por thread*/
	//for(i = 0; i < N; i++) lst_ins(&_args[i % (threads - 1)].lista, i);

	tempo = clock();


	//Thered delegada para escrita do resulto no arquivo.
	//if(status_create(status = pthread_create((&_argsArq.thread), NULL, solicitacao_arquivo, (void *)&_argsArq)));
	//else exit(1);


	//pthread_join(_argsArq.thread, NULL);

	/*print jobs de cada thread*/
	print_responsabilidade_thread(_args);

	//fclose(_argsArq.arq);
	//imprimirMatriz(matrizResultante);

	printf("\nTerminando processo ...\n");
	printf("\n\n[Tempo Total Do Processo: %fs]\n", (float) (clock() - tempo)  / CLOCKS_PER_SEC);

	return 0;
}
