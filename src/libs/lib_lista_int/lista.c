/* lista.c
 *
 * Implementação das operações sobre o TAD lista ordenada implementada
 * de forma encadeada.
 *
 *
 */

#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void lst_init(lst_ptr * l) {
    *l = NULL;
}

void lst_ins(lst_ptr * l, lst_info_int val) {
    lst_ptr n;
    if ((n = (lst_ptr) malloc(sizeof(struct lst_no_int))) == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria!\n");
        exit(1);
    }
     n->dado = val;

    if (*l == NULL) {
        *l = n;
        (*l)->prox = *l;
        return;
    }

    n->prox = (*l)->prox;
    (*l)->prox = n;
    *l = n;
    return;
}

void lst_print(lst_ptr l) {
    if (l != NULL) {
        lst_ptr p = l;
        printf("[ ");
        do {
            printf("%d ,", p->dado);
            p = p->prox;
        } while(p != l);
    printf(" ]\n");
    }
}
