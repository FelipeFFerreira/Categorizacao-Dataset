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

static int normalize_info_date(args args_t, char * str, int colun)
{
    pthread_mutex_lock(&(mutex_1));
    //printf("\nID:%d estou aqui (normalize_info)\n", args_t.id);
    lst_info_th info_t;
	strcpy(info_t.word, str);
	//printf("search:%s, colun = %d\n", info_t.word, colun);

	if (is_my_job(args_t.lista, colun)) {
        int id = lst_info_id_th(colun_date[colun - 1], info_t);
        if (id != 0) {
            fprintf(args_t.fptr[colun - 1], "%d\n", id);
            printf("Escrevi no arq");
            return PROCEED;
        }
        else return HOLD;
	}
	return PROCEED;
    pthread_mutex_unlock(&(mutex_1));

}

void * normaliza_colun_date(void * _args)
{
    //pthread_mutex_lock(&(mutex_1));
	args * args_t = (args*) _args;
	char str[1001], *token;
	int i, j;
    printf("ID:%d estou aqui (normaliza_colun)\n", args_t->id);
    for (i = 0; fscanf(args_t->fptr_origem, " %500[^\n]s", str) != EOF && i < N; i++) {
        token = strtok(str, ",");
        for (j = 0; token != NULL && j < QTD_COLLUN; j++) {
            switch (normalize_info_date(*args_t, token, j + 1)) {
                case HOLD : j -= 1;
                            printf("HOLD");
                            //INSTALL_DEBUG
                    break;
                case PROCEED ://printf("HOLD");
                             //INSTALL_DEBUG
                    break;
            token = strtok(NULL, ",");
            }
        }
        printf("chegyue");
    }
    //pthread_mutex_unlock(&(mutex_1));
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
			//printf("%s\n", token);
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
	args _args[num_threads]; //numero de args por threads de CPU
	pthread_t thread_1; //thread responsavel pelo arquivo de entrada

    create_threads(_args, num_threads, arq_origem);
    thread_jobs(_args, QTD_COLLUN, num_threads); //repassa trabalhos

    path_arq_t[0].fptr = open_arquivo(arq_origem, "r"); //path arq com a 2.matriz

    if (status_create( status = pthread_create((&thread_1), NULL, ler_matriz_entrada, (void *)&path_arq_t[0])));
    pthread_join(thread_1, NULL);

    print_responsabilidade_thread(_args);
    /*Repassa função de trabalho*/
	for(i = 0; i < num_threads; i++) {
		//_args[i].ptrArq = &_argsArq;
		if (status_create(status = pthread_create((&_args[i].thread), NULL, normaliza_colun_date, (void *)&_args[i])));
		else exit(1);
	}

    /*Thered principal aguarda todas as thredes de trabalhos finalizarem*/
	for(i = 0; i < num_threads; i++) {
		pthread_join(_args[i].thread, NULL);
	}
	exit(0x2);
    sleep(10000);


    for(i = 0; i < num_threads; i++) {
		for (j = 0; j < 4; j++)
            fclose(_args[i].fptr[j]);
	}
    fclose(path_arq_t[0].fptr);

	fclose(path_arq_t[1].fptr);
	//lst_print(colun_date[1]);



	//for(i = 0; i < N; i++) _argsArq.statusArq[i] = WAIT; //coloca todas as linhas em estado de espera.

	/*Repassa o identificador para as threads*/
	for(i = 0; i < num_threads - 1; i++) {
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

	return EXIT_SUCCESS;
}
