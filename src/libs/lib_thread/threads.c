
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
	for(i = 0; i < threads - 1; i++) {
		printf("\nMeu iDThread: %d\n", _args[i].id);
		printf("As linhas x colunas que fiquei responsavel :)\n");
		lst_print(_args[i].lista);
	}
	printf("\nJá eu iDThread: %d fiquei responsavel pelo Arquivo :)\n", i +1);
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
        exit(1);
     }

    return arq;
}

void create_threads(args * _args, int n)
{
    int i;
    /*Repassa o identificador para as threads*/
	for(i = 0; i < n - 1; i++) {
		lst_init(&_args[i].lista);
		_args[i].id = i + 1;
	}
}

void thread_jobs(args * _args, int n)
{
    int i;
    /*Repassa linha de trabalho ciclica, por thread*/
	for(i = 0; i < n; i++)
        lst_ins(&_args[i % (n - 1)].lista, i);
}
