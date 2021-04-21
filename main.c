#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "lista.h"
#include "threads.h"



/* Para simulação com openmp descomentar INSTALL_OMP
 * e no arquivo threads.h utilizar apenas 2 threads
 * para utilizar as threads implementada no modulo da solução
 * manter comentado e variar os numeros de threads que desejar.
*/

//#define INSTALL_OMP //Install_openmp
#define INSTALL_DEBUG

#ifdef INSTALL_OMP
	#include <omp.h>
#endif

tipoDado matrizResultante[N][N];
char dados[N][LEN];


typedef struct {
	FILE * fptr;
	tipoDado matriz[N][N];
}path_arq; //struct para captura da matriz de entrada

path_arq path_arq_t[2];

static void * multiplicacao(void * argss)
{
	
    args * _argss = (args *)argss;
    lst_ptr p;
	tipoDado j;
	p = _argss->lista;

	int k = 0;
	
	while (p != NULL) {
		#ifdef INSTALL_OMP
			#pragma omp parallel for
		#endif
		for (j = 0;  j < N; j++) {
			for (k = 0; k < N; k++) {
					matrizResultante[p->dado][j] = matrizResultante[p->dado][j] + 
												   path_arq_t[0].matriz[p->dado][k] * path_arq_t[1].matriz[k][j];										
			}
		}
		_argss->ptrArq->statusArq[p->dado] = READY; //libera para escrita no arquivo
		p = p->prox;
	}

	pthread_exit( (void*) 0 );//Legado do retorno
}


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
typedef struct
{
	char str[LEN];
	int id;
	
}dado_coluna;


void * ler_matriz_entrada(void * args)
{
	path_arq * _path_arq_t = (path_arq*) args;
	
	int i, j;
	char str[10001], *token, dados[N][QTD_COLLUN][LEN];
	for (i = 0; fscanf(_path_arq_t->fptr, " %500[^\n]s", str) != EOF && i < N; i++) {
		token = strtok(str, ",");
		for (j = 0; token != NULL; j++) {
			
			strcpy(dados[i][j], token);
			printf("%d-%s ", i + 1, dados[i][j]);
			token = strtok(NULL, ",");
		}
		printf("\n");
	}
	printf("%s, %d\n", "Entrei", i);
	exit(EXIT_SUCCESS);
}

int main ()
{
	tipoDado i, j;
	int status;
	clock_t tempo;
	args _args[threads - 1]; //numero de args por threads de CPU
	struct argsArq _argsArq; //thread responsavel pelo arquivo de saida
	pthread_t thread_1; //thread responsavel pelo arquivo de entrada_1

	path_arq_t[0].fptr = open_arquivo("/home/felipe/Faculdade/Paralela-Matriz-Normalizacao/src/Matrizes/dataset_00_1000.csv", "r"); //path arq com a 2.matriz

	if (status_create( status = pthread_create((&thread_1), NULL, ler_matriz_entrada, (void *)&path_arq_t[0])));
	else exit(1);

	pthread_join(thread_1, NULL);
	fclose(path_arq_t[0].fptr);

	print_matriz(dados);
	exit(0xFF);

	_argsArq.arq = open_arquivo("matriz_resultante.csv", "w"); //path arq com o resultado da mult.

	for(i = 0; i < N; i++) _argsArq.statusArq[i] = WAIT; //coloca todas as linhas em estado de espera.

	/*Repassa o identificador para as threads*/
	for(i = 0; i < threads - 1; i++) {
		lst_init(&_args[i].lista);
		_args[i].id = i + 1;
	}
	_argsArq.id = i; //id threads arq saida.

	/*Repassa linha de trabalho balanceada da matriz, por thread*/
	for(i = 0; i < N; i++) lst_ins(&_args[i % (threads - 1)].lista, i);
	
	tempo = clock();
	/*Repassa função de trabalho*/
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
