
/* lista_th.c
 *
 */

#include "lista_th.h"


void lst_init_th(lst_ptr_th * l)
{
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
        *l = n;
        (*l)->prox = *l;
        return;
    }

    n->prox = (*l)->prox;
    (*l)->prox = n;
    *l = n;
    return;
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
    if (l != NULL) {
        lst_ptr_th p = l;
        do {
            if (!strcmp(p->dado.word, x.word)) {
                p->dado.count++;
                return true;
            }
            p = p->prox;
            count += 1;
        } while (p != l);
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
    return NOT_EXIST;
}
