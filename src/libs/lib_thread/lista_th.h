/* listaord.h
 *
 * Definição do TAD para representar uma lista ordenada implementada
 * de forma encadeada e protótipo das operações sobre esse TAD.
 *
 */

#ifndef _LISTA_TH_H
#define _LISTA_TH_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NOT_EXIST -1

typedef struct
{
	char word[300];
	unsigned id;
	unsigned int count;
}info_date;
typedef info_date lst_info_th;

typedef struct lst_no_th * lst_ptr_th;
struct lst_no_th {
    lst_info_th dado;
    lst_ptr_th prox;
};

/* inicializa a lista ordenada */
void lst_init_th(lst_ptr_th *);

/* insere um novo elemento na lista ordenada */
void lst_ins_th(lst_ptr_th *, lst_info_th);

/* remove um elemento da lista ordenada */
bool lst_rem_th(lst_ptr_th *, lst_info_th);

/* imprime os elementos da lista ordenada */
void lst_print_th(lst_ptr_th);

/*Verifica se a info já existe na lista*/
bool lst_existing_th(lst_ptr_th l, lst_info_th, int *);

/* Retorna id de uma informaçao da lista*/
unsigned int lst_info_id_th(lst_ptr_th l, lst_info_th x);
#endif

