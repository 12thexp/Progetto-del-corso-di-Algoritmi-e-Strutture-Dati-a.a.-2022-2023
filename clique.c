#include "clique.h"

void CreateQueue(lista_queue *pQ, int size)
{
	pQ->V = (lista *)calloc(size, sizeof(lista));
	if (pQ->V == NULL)
	{
		fprintf(stderr, "Errore nell'allocazione della coda!\n");
		exit(EXIT_FAILURE);
	}

	pQ->size = size;
	pQ->head = 0;
	pQ->tail = 0;
}

void DestroyQueue(lista_queue *pQ)
{
	free(pQ->V);
	pQ->V = NULL;
	pQ->size = 0;
	pQ->head = 0;
	pQ->tail = 0;
}

boolean IsEmptyQueue(lista_queue *pQ)
{
	return (pQ->head == pQ->tail);
}

lista Front(lista_queue *pQ)
{
	return pQ->V[pQ->head];
}

void Enqueue(lista ListaAutori, lista_queue *pQ)
{
	pQ->V[pQ->tail] = ListaAutori;
	pQ->tail++;
	/* Nell'applicazione alla BFS, non succede mai */
	if (pQ->tail >= pQ->size)
		pQ->tail -= pQ->size;
}

void Dequeue(lista_queue *pQ)
{
	pQ->head++;
	/* Nell'applicazione alla BFS, non succede mai */
	if (pQ->head >= pQ->size)
		pQ->head -= pQ->size;
}

listaclique_t creaclique()
{
	listaclique_t C;

	C = (listaclique_t)calloc(1, sizeof(elemento));
	if (C == NULL)
	{
		fprintf(stderr, "Errore nell'allocazione di una listaclique_t!\n");
		exit(EXIT_FAILURE);
	}
	C->pred = C;
	C->succ = C;

	return C;
}

void svuotaclique(listaclique_t C)
{
	posCQ_t p;

	while (!cliquevuota(C))
	{
		p = primoclique(C);
		cancclique(C, &p);
	}
}

void distruggeclique(listaclique_t *pC)
{
	posCQ_t p;

	while (!cliquevuota(*pC))
	{
		p = primoclique(*pC);
		cancclique(*pC, &p);
	}

	free(*pC);
	*pC = NULL;
}

lista leggeclique(listaclique_t C, posCQ_t p)
{
	return p->ListaAutori;
}

listaclique_t scriveclique(listaclique_t C, posCQ_t p, lista ListaAutori)
{
	p->ListaAutori = ListaAutori;
	return C;
}

boolean cliquevuota(listaclique_t C)
{
	return (C->succ == C);
}

posCQ_t primoclique(listaclique_t C)
{
	return C->succ;
}

posCQ_t ultimoclique(listaclique_t C)
{
	return C->pred;
}

posCQ_t succclique(listaclique_t C, posCQ_t p)
{
	return p->succ;
}

posCQ_t predclique(listaclique_t C, posCQ_t p)
{
	return p->pred;
}

boolean fineclique(listaclique_t C, posCQ_t p)
{
	return (p == C);
}

/* inserisce una clique in una lista di clique in ordine alfabetico senza duplicati */
listaclique_t insclique_ordlex_nodup(listaclique_t C, lista ListaAutori)
{
	posCQ_t p;
	posCQ_t q;

	q = (posCQ_t)calloc(1, sizeof(elemento));
	if (q == NULL)
	{
		fprintf(stderr, "Errore nell'allocazione dell'elemento %s!\n", ListaAutori->Autore->nome);
		exit(EXIT_FAILURE);
	}

	for (p = primoclique(C); !fineclique(C, p); p = succclique(C, p))
	{

		int cmp = confronta_lista(ListaAutori, p->ListaAutori);
		if (cmp > 0)
			break;
		else if (cmp == 0)
			return C;
		else
			continue;
	}

	q->ListaAutori = ListaAutori;
	q->pred = p->pred;
	q->succ = p;

	p->pred->succ = q; /* NB: non scambiarla con la seguente! */
	p->pred = q;

	return C;
}

listaclique_t insclique(listaclique_t C, posCQ_t p, lista ListaAutori)
{
	posCQ_t q;

	q = (posCQ_t)calloc(1, sizeof(elementoCQ));
	if (q == NULL)
	{
		fprintf(stderr, "Errore nell'allocazione dell'elementoCQ %p!\n", (void *)ListaAutori);
		exit(EXIT_FAILURE);
	}
	q->ListaAutori = ListaAutori;
	q->pred = p->pred;
	q->succ = p;

	p->pred->succ = q; /* NB: non scambiarla con la seguente! */
	p->pred = q;

	return C;
}

listaclique_t cancclique(listaclique_t C, posCQ_t *pp)
{
	posCQ_t p = *pp;

	*pp = p->succ;
	p->pred->succ = p->succ;
	p->succ->pred = p->pred;
	free(p);

	return C;
}

int num_el_lista_clique(listaclique_t L)
{

	posCQ_t q;
	int n = 0;

	for (q = primoclique(L); !fineclique(L, q); q = succclique(L, q))
		n++;

	return n;
}

/* stampa una lista di clique (cioe' una lista di liste di autori) */
void StampaListaCliques(listaclique_t L, int k)
{
	posCQ_t q; /* scorre le clique */

	for (q = primoclique(L); !fineclique(L, q); q = succclique(L, q))
	{
		posizione p; /* scorre la lista autori */
		autore_st *Autore;
		posizione ultimo = ultimolista(q->ListaAutori);

		for (p = primolista(q->ListaAutori); !finelista(q->ListaAutori, p); p = succlista(q->ListaAutori, p))
		{
			Autore = leggelista(q->ListaAutori, p);
			printf("%s", Autore->nome);
			if (p != ultimo)
			{
				if (k == 1)
					printf(" & "); 	/* per la stampa delle clique */
				else 
					printf(" ");	/* per le coppie piu' lontane */
			}
		}
		printf("\n");
	}
}