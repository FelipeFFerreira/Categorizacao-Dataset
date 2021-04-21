
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

void lst_ins(lst_ptr * l, lst_info val) {

    lst_ptr n;
    if ((n = (lst_ptr) malloc(sizeof(struct lst_no))) == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria!\n");
        exit(1);
    }
    n->dado = val;
    if (*l == NULL) {
        n->prox = *l;
        *l = n;
        return;
    }
    else {
        lst_ptr p = *l;
         while (p->prox != NULL) {
            p = p->prox;
         }
         n->prox = p->prox;
         p->prox = n;
         return;
    }
}

void lst_print(lst_ptr l) {
    printf("[ ");
    while (l != NULL) {
        printf("[%s, %d", l->dado.word, l->dado.id);
        l = l->prox;
    }
    printf("\b ]\n");
}

bool lst_existing(lst_ptr l, lst_info x, int * id)
{
    int count = 0;
    while (l != NULL) {
        if (!strcmp(l->dado.word, x.word))
            return true;
        l = l->prox;
        count += 1;
    }
    *id = count + 1;
    return false;
}