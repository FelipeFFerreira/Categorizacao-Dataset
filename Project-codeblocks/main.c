#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../lista_th.h"
#include "../threads.h"
#include "../main.h"



/* Para simula��o com openmp descomentar INSTALL_OMP
 * e no arquivo threads.h utilizar apenas 2 threads
 * para utilizar as threads implementada no modulo da solu��o
 * manter comentado e variar os numeros de threads que desejar.
*/

//#define INSTALL_OMP //Install_openmp
#define INSTALL_DEBUG

#ifdef INSTALL_OMP
	#include <omp.h>
#endif

tipoDado matrizResultante[N][N];
//static char dados[N][QTD_COLLUN][LEN]; // go to pq static aq?


typedef struct {
	FILE * fptr;
	tipoDado matriz[N][N];
}path_arq; //struct para captura da matriz de entrada

path_arq path_arq_t[2];

static lst_ptr colun_date[QTD_COLLUN];
static int state;



static void * multiplicacao(void * argss)
{}


void * solicitacao_arquivo(void * argsArq)
{

	tipoDado i = 0;
	tipoDado j, pronto = N;
	ptrArgsArq _argssArq = (ptrArgsArq) argsArq;

	while (i < N) {
		if (_argssArq->statusArq[i] == READY) {
			for (j = 0; j < N; j++) {
				fprintf(_argssArq->arq, "%lu  ", matrizResultante[i][j]);
			}
			fprintf(_argssArq->arq, "%c", '\n');
			_argssArq->statusArq[i] = EXECUTED;
			i += 1;
		}
	}

	fclose(_argssArq->arq);
	pthread_exit( (void*) 0 );	//Legado do retorno
}


/*
 * ******* Regiao de Variaves Globais *************
 */

static void add_lst_info_distinct(lst_ptr * l, char * str)
{
	lst_info info_t;
	int id_t = 0;
	strcpy(info_t.word, str);

	if (!lst_existing(*l, info_t, &id_t)) {

		info_t.id = id_t;
		lst_ins(l, info_t);
	}
}

static void normalize_info_date(lst_ptr l, char * str)
{
	lst_info info_t;
	strcpy(info_t.word, str);
	int id = lst_info_id(l, info_t);
	if (id != 0)
		fprintf(path_arq_t[1].fptr, "%d\n", id);
	fprintf(path_arq_t[1].fptr, "nao existe\n");
}

void * normaliza_colun_date(void * args)
{
	path_arq * _path_arq_t = (path_arq*) args;
	char str[1001], *token;
	int i, j;
	while (state == WAIT) {
		for (i = 0; fscanf(_path_arq_t->fptr, " %500[^\n]s", str) != EOF && i < N; i++) {
			token = strtok(str, ",");
			for (j = 0; token != NULL; j++) {
				if (j == 1) normalize_info_date(colun_date[1], token);
				token = strtok(NULL, ",");
			}
		}
	}
}

void * ler_matriz_entrada(void * args)
{
	path_arq * _path_arq_t = (path_arq*) args;
	char str[1001], *token;
	int i, j;

	state = WAIT;
	for (int i = 0; i < QTD_COLLUN; i++) lst_init(&colun_date[i]);

	for (i = 0; fscanf(_path_arq_t->fptr, " %500[^\n]s", str) != EOF && i < N; i++) {
		token = strtok(str, ",");
		for (j = 0; token != NULL; j++) {
			add_lst_info_distinct(&colun_date[j], token);
			token = strtok(NULL, ",");
		}
	}
	state = EXECUTED;

	//exit(EXIT_SUCCESS);
}


int main ()
{
	tipoDado i, j;
	int status;
	clock_t tempo;
	args _args[threads - 1]; //numero de args por threads de CPU
	struct argsArq _argsArq; //thread responsavel pelo arquivo de saida
	pthread_t thread_1; //thread responsavel pelo arquivo de entrada_1
	pthread_t thread_2; //thread responsavel pelo arquivo de entrada_1

	create_threads(args, threads);
    _argsArq.arq = open_arquivo("C:\\GitHub\\Paralela-Matriz-Normalizacao\\Matrizes\\dataset_00_1000.csv", "r"); //path arq matriz

    thread_jobs(args, QTD_COLLUN); //repassa trabalhos

	path_arq_t[1].fptr = open_arquivo("C:\\GitHub\\Paralela-Matriz-Normalizacao\\Matrizes\\colun_1.csv", "w"); //path arq com a 2.matriz

	if (status_create( status = pthread_create((&thread_1), NULL, ler_matriz_entrada, (void *)&path_arq_t[0])));
	else exit(1);

	if (status_create( status = pthread_create((&thread_2), NULL, normaliza_colun_date, (void *)&path_arq_t[0])));
	else exit(1);

	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);
	fclose(path_arq_t[0].fptr);
	fclose(path_arq_t[1].fptr);
	lst_print(colun_date[1]);
	exit(0xFF);

	_argsArq.arq = open_arquivo("matriz_resultante.csv", "w"); //path arq com o resultado da mult.

	for(i = 0; i < N; i++) _argsArq.statusArq[i] = WAIT; //coloca todas as linhas em estado de espera.





	tempo = clock();
	/*Repassa fun��o de trabalho*/
	for(i = 0; i < threads - 1; i++) {
		_args[i].ptrArq = &_argsArq;
		if (status_create(status = pthread_create((&_args[i].thread), NULL, multiplicacao, (void *)&_args[i])));
		else exit(1);
	}

	//Thered delegada para escrita do resulto no arquivo.
	if(status_create(status = pthread_create((&_argsArq.thread), NULL, solicitacao_arquivo, (void *)&_argsArq)));
	else exit(1);

	/*Thered principal aguarda todas as thredes de trabalhos finalizarem para proseguir*/
	for(i = 0; i < threads - 1; i++) {
		pthread_join(_args[i].thread, NULL);
	}
	pthread_join(_argsArq.thread, NULL);

	/*print jobs de cada thread*/
	print_responsabilidade_thread(_args);

	fclose(_argsArq.arq);
	//imprimirMatriz(matrizResultante);

	printf("\nTerminando processo ...\n");
	printf("\n\n[Tempo Total Do Processo: %fs]\n", (float) (clock() - tempo)  / CLOCKS_PER_SEC);

	return EXIT_SUCCESS;
}