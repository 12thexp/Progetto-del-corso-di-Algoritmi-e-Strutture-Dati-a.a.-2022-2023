#ifndef __clique_h
#define __clique_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

/*
#define NO_ELEMENT NULL
#define NO_LIST    NULL
*/

typedef struct _elementoCQ elementoCQ;
typedef elementoCQ *listaclique_t;
typedef elementoCQ *posCQ_t;

struct _elementoCQ
{
	lista ListaAutori;
	posCQ_t succ;
	posCQ_t pred;
};

typedef struct _lista_queue lista_queue;
struct _lista_queue
{
	lista *V;
	int size; /* dimensione massima della coda */
	int head; /* indice dell'ultima posizione occupata */
	int tail; /* indice della prima posizione libera */
};

void CreateQueue(lista_queue *pQ, int size);

void DestroyQueue(lista_queue *pQ);

boolean IsEmptyQueue(lista_queue *pQ);

lista Front(lista_queue *pQ);

void Enqueue(lista ListaAutori, lista_queue *pQ);

void Dequeue(lista_queue *pQ);

listaclique_t creaclique();

void svuotaclique(listaclique_t C);

void distruggeclique(listaclique_t *pC);

lista leggeclique(listaclique_t C, posCQ_t p);

listaclique_t scriveclique(listaclique_t C, posCQ_t p, lista ListaAutori);

boolean cliquevuota(listaclique_t C);

posCQ_t primoclique(listaclique_t C);

posCQ_t ultimoclique(listaclique_t C);

posCQ_t succclique(listaclique_t C, posCQ_t p);

posCQ_t predclique(listaclique_t C, posCQ_t p);

boolean fineclique(listaclique_t C, posCQ_t p);

listaclique_t insclique_ordlex_nodup(listaclique_t C, lista ListaAutori);

listaclique_t insclique(listaclique_t C, posCQ_t p, lista ListaAutori);

listaclique_t cancclique(listaclique_t C, posCQ_t *pp);

int num_el_lista_clique(listaclique_t L);

void StampaListaCliques(listaclique_t L, int k);

#endif
