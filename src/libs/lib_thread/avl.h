/* avl.h
 * Definição do TAD para representar uma árvore AVL e protótipo
 * de suas operações.
 *
 * Felipe Ferreira
 */

#ifndef _AVL_H
#define _AVL_H

#include <stdio.h>
#include <stdbool.h>

// Definição da estrutura de dados
typedef struct {
    char word[300];
	 int id;
	unsigned int count;
}avl_info;

typedef struct avl_no * avl_tree;

struct avl_no {
    avl_info dado;
    avl_tree esq;
    avl_tree dir;
    int bal; // balanceamento do nó
};


bool new_distinct_info_column(avl_tree t, avl_info x, int * id);

/*
 * Função que faz uma rotação para a esquerda na árvore AVL t.
 */
void rotacao_esq(avl_tree *);

/*
 * Função que faz o balanceamento da árvore AVL t após uma remoção
 * em sua subárvore esquerda e sinaliza se houve uma
 * dimuição da altura dessa subárvore através de h.
 */
void balance_esq(avl_tree * t, bool * h);

/*
 * Função que faz o balanceamento da árvore AVL t após uma remoção
 * em sua subárvore direita e sinaliza se houve uma
 * dimuição da altura dessa subárvore através de h.
 */
void balance_dir(avl_tree *, bool *);

/*
 * Função que devolve um ponteiro para o nó que contém o menor
 * valor na árvore AVL t e sinaliza através de h se
 * houve uma diminuição da altura de t.
 */
avl_tree get_min(avl_tree * t, bool * h);

/*
 * Função que adiciona um novo node na arvore.
 */
void search_new_info(avl_tree * t, avl_info x, bool * h, bool * k, int * id);

/*
 *	Função que lista todas as coberturas vacinais, pecorrenco a arvore avl.
 */
void print_avl(avl_tree t);

/*
 *	Função que realiza a busca de uma determinada cobertura vacinal, dado
 *  o seu respectivo codigo de município.
 */
avl_tree search_info(avl_tree , char * x);

/*
 * Função que lista as menores coberturas vacinais, pecorrendo a arvore avl.
 */
void maiores_coberturas(avl_tree);

/*
 * Função que lista as maiores coberturas vacinais, pecorrendo a arvore avl.
 */
void menores_coberturas(avl_tree);

#endif // _AVL_H

