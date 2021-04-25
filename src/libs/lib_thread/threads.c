
/* threads.c
 *
 * Definição das operações sobre o TAD threads e implementação das funçoes especificas
 * sobre o arquivo main.c.
 * esse TAD.
 */

#include "threads.h"
#include "lista_th.h"
#include "../lib_lista_int/lista.h"
#include <stdlib.h>

void print_matriz(char * dados[N][LEN][QTD_COLLUN])
{
	tipoDado i, j;
	for (i = 0; i < N; i++) {
		for (int j = 0; j < QTD_COLLUN; j++)
			//printf("%s ", dados[i][j]);
		printf("\n");
	}
	printf("\n");
}

void print_colun_matriz(char dados[][LEN][QTD_COLLUN], int j)
{
	tipoDado i;
	for (int i = 0; i < N; i++)
	{
		//printf("%d.%s\n", i + 1, dados[i][j]);
	}
}

void print_responsabilidade_thread(args * _args)
{
	int i;
	printf("\n____________________________________________________\n");
	for(i = 0; i < NUM_THREADS; i++) {
		printf("\nMeu iDThread: %d\n", _args[i].id);
		printf("As linhas x colunas que fiquei responsavel :)\n");
		lst_print(_args[i].lista);
	}
	//printf("\nJá eu iDThread: %d fiquei responsavel pelo Arquivo :)\n", i +1);
}

bool status_create(int status)
{
	if (status) {
		printf("Erro criando thread, retornou codigo %d\n", status );
		return false;
	}

	return true;
}

FILE *open_arquivo(char * str, char * modo) {

    FILE * arq; // Arquivo lógico
    if ((arq = fopen(str, modo)) == NULL) {
        printf(stderr, "Erro na abertura do arquivo %s\n", "filename");
        exit(0x3);
     }

    return arq;
}

void create_threads(args * _args, int n, char * arq_origem, ptr_args_arq _args_main)
{
    int i;
    /*Repassa o identificador para as threads*/
	for(i = 0; i < n; i++) {
		lst_init(&_args[i].lista);
		_args[i].id = i + 1;
		_args[i].fptr_origem = open_arquivo(arq_origem, "r");
	}
	_args_main->id = i;
}

void thread_jobs(args * _args, int n , int n_threads, ptr_args_arq _args_main)
{
    int i, j, qtd_colun = 0, n_thread = 0;
    char str_path[100];

    /*Repassa linha de trabalho ciclica, por thread*/
	while (qtd_colun < QTD_COLLUN - 1) {
            //_args[n_thread].id = n_thread;
            _args[n_thread].main_destino = _args_main;
        for (j = 0; j < 4; j++) {
            qtd_colun += 1;
            lst_ins(&_args[n_thread].lista, qtd_colun);
            sprintf(str_path, "C:\\GitHub\\Paralela-Matriz-Normalizacao\\arq_csvs\\colun-%d.csv", qtd_colun);
            _args[n_thread].fptr_destinos[j] = open_arquivo(str_path, "w");
        }
        n_thread += 1;
	}
	for (i = 0; i < QTD_COLLUN_THREAD; i++)
        for (j = 0; j < NUM_THREADS; j++) {
            _args_main->thread_buffer[i][j].state = TO_DO;
        }
	_args_main->arq_main = open_arquivo("C:\\GitHub\\Paralela-Matriz-Normalizacao\\arq_csvs\\principal-normalizado.csv", "w");
	_args_main->lin_job_pendente = 0;
	_args_main->lin_job_atual = 0;
}
