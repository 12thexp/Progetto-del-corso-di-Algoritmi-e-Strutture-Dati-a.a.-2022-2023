#ifndef __lista_h
#define __lista_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 256
#define ROW_LENGTH 500

#define boolean int
#define TRUE 1
#define FALSE 0

#define NO_ELEMENT NULL
#define NO_LIST NULL

typedef struct _elemento elemento;
typedef elemento *lista;
typedef elemento *posizione;

typedef struct articolo articolo_st;
struct articolo
{
	int id;
	char titolo[ROW_LENGTH];
	lista autoriPerArticolo;
	/* int *cit; */
	/* int n_cit; */
	int cit_tot;
};

typedef struct autore autore_st;
struct autore
{
	char nome[MAX_NAME];
	int n_articoli;
	int n_citazioni;
	struct articolo **articoli;
	lista collaboratori;
	int hirsch;
	int idx;
};

struct _elemento
{
	autore_st *Autore;
	posizione succ;
	posizione pred;
};

lista crealista();

void svuotalista(lista L);

void distruggelista(lista *pL);

autore_st *leggelista(lista L, posizione p);

lista scrivelista(lista L, posizione p, autore_st *Autore);

boolean listavuota(lista L);

posizione primolista(lista L);

posizione ultimolista(lista L);

posizione succlista(lista L, posizione p);

posizione predlista(lista L, posizione p);

boolean finelista(lista L, posizione p);

lista inslista_ordlex_nodup(lista L, autore_st *Autore);

lista inslista(lista L, posizione p, autore_st *Autore);

lista canclista(lista L, posizione *pp);

boolean in_lista(lista L, autore_st *Autore);

posizione TrovaAutorePerNome(char *nome, lista L);

boolean confronta_lista(lista L1, lista L2);

int num_el_lista(lista L);

void StampaListaAutori(lista L);

#endif
