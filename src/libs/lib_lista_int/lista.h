/* listaord.h
 *
 * Definição do TAD para representar uma lista ordenada implementada
 * de forma encadeada e protótipo das operações sobre esse TAD.
 *
 */

#ifndef _LISTA_H
#define _LISTA_H
#include <stdbool.h>

typedef int lst_info_int;

typedef struct lst_no_int * lst_ptr;
struct lst_no_int {
    lst_info_int dado;
    lst_ptr prox;
};

/* inicializa a lista ordenada */
void lst_init(lst_ptr *);

/* insere um novo elemento na lista ordenada */
void lst_ins(lst_ptr *, lst_info_int);

/* remove um elemento da lista ordenada */
bool lst_rem(lst_ptr *, lst_info_int);

/* imprime os elementos da lista ordenada */
void lst_print(lst_ptr);

#endif
