
/* lista.c
 *
 * Implementação das operações sobre o TAD lista ordenada implementada
 * de forma encadeada.
 *
 *
 */

#include "lista_th.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void lst_init_th(lst_ptr_th * l) {
    *l = NULL;
}

void lst_ins_th(lst_ptr_th * l, lst_info_th val) {

    lst_ptr_th n;
    if ((n = (lst_ptr_th) malloc(sizeof(struct lst_no_th))) == NULL) {
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
        lst_ptr_th p = *l;
         while (p->prox != NULL) {
            p = p->prox;
         }
         n->prox = p->prox;
         p->prox = n;
         return;
    }
}

void lst_print_th(lst_ptr_th l) {
    printf("[ ");
    while (l != NULL) {
        printf("[%s,%d] ", l->dado.word, l->dado.id);
        l = l->prox;
    }
    printf(" ]\n");
}

bool lst_existing_th(lst_ptr_th l, lst_info_th x, int * id)
{
    int count = -1;
    while (l != NULL) {
        if (!strcmp(l->dado.word, x.word))
            return true;
        l = l->prox;
        count += 1;
    }
    *id = count + 1;
    return false;
}

unsigned int lst_info_id_th(lst_ptr_th l, lst_info_th x)
{
    while (l != NULL) {
        if (!strcmp(l->dado.word, x.word))
            return l->dado.id;
        l = l->prox;
    }
    return 0;
}
