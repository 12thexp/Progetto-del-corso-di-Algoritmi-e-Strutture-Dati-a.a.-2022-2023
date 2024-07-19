#include "lista.h"

lista crealista()
{
	lista L;

	L = (lista)calloc(1, sizeof(elemento));
	if (L == NULL)
	{
		fprintf(stderr, "Errore nell'allocazione di una lista!\n");
		exit(EXIT_FAILURE);
	}
	L->pred = L;
	L->succ = L;

	return L;
}

void svuotalista(lista L)
{
	posizione p;

	while (!listavuota(L))
	{
		p = primolista(L);
		canclista(L, &p);
	}
}

void distruggelista(lista *pL)
{
	posizione p;

	while (!listavuota(*pL))
	{
		p = primolista(*pL);
		canclista(*pL, &p);
	}

	free(*pL);
	*pL = NULL;
}

autore_st *leggelista(lista L, posizione p)
{
	return p->Autore;
}

lista scrivelista(lista L, posizione p, autore_st *Autore)
{
	p->Autore = Autore;
	return L;
}

boolean listavuota(lista L)
{
	return (L->succ == L);
}

posizione primolista(lista L)
{
	return L->succ;
}

posizione ultimolista(lista L)
{
	return L->pred;
}

posizione succlista(lista L, posizione p)
{
	return p->succ;
}

posizione predlista(lista L, posizione p)
{
	return p->pred;
}

boolean finelista(lista L, posizione p)
{
	return (p == L);
}

/* inserisce autori in una lista in ordine lessicografico senza duplicati */
lista inslista_ordlex_nodup(lista L, autore_st *Autore)
{
	posizione p;
	posizione q;

	q = (posizione)calloc(1, sizeof(elemento));
	if (q == NULL)
	{
		fprintf(stderr, "Errore nell'allocazione dell'elemento %s!\n", Autore->nome);
		exit(EXIT_FAILURE);
	}

	for (p = primolista(L); !finelista(L, p); p = succlista(L, p))
	{
		int cmp = strcmp(Autore->nome, p->Autore->nome);
		if (cmp < 0) /* esce dal ciclo quando S1 viene prima di S2*/
			break;
		else if (cmp == 0) /* se l'elemento e' gia' presente */
			return L;
		else
			continue;
	}
	/* inserisce l'elemento nella lista */
	q->Autore = Autore;
	q->pred = p->pred;
	q->succ = p;

	p->pred->succ = q; /* NB: non scambiarla con la seguente! */
	p->pred = q;

	return L;
}

lista inslista(lista L, posizione p, autore_st *Autore)
{
	posizione q;

	q = (posizione)calloc(1, sizeof(elemento));
	if (q == NULL)
	{
		fprintf(stderr, "Errore nell'allocazione dell'elemento %s!\n", Autore->nome);
		exit(EXIT_FAILURE);
	}
	q->Autore = Autore;
	q->pred = p->pred;
	q->succ = p;

	p->pred->succ = q; /* NB: non scambiarla con la seguente! */
	p->pred = q;

	return L;
}

lista canclista(lista L, posizione *pp)
{
	posizione p = *pp;

	*pp = p->succ;
	p->pred->succ = p->succ;
	p->succ->pred = p->pred;
	free(p);

	return L;
}

/* Controlla se un autore è in una lista */
boolean in_lista(lista L, autore_st *Autore)
{
	posizione p;

	for (p = primolista(L); !finelista(L, p); p = succlista(L, p))
	{
		if (strcmp(p->Autore->nome, Autore->nome) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/* Trova un autore in una lista */
posizione TrovaAutorePerNome(char *nome, lista L)
{
	posizione p;
	autore_st *ps;

	for (p = primolista(L); !finelista(L, p); p = succlista(L, p))
	{
		ps = leggelista(L, p);
		if (strcmp(ps->nome, nome) == 0)
			return p;
	}

	return NO_ELEMENT;
}

/* Numero di element in lista. */
int num_el_lista(lista L)
{
	posizione p;
	int n = 0;

	for (p = primolista(L); !finelista(L, p); p = succlista(L, p))
		n++;

	return n;
}

/* Confronta due liste.
  L1 < L2 --> -1
  L1 = L2 --> 0
  L1 > L2 --> 1

   Si assume che le liste siano ordinate. */
boolean confronta_lista(lista L1, lista L2)
{
	posizione p1, p2;
	int n1 = num_el_lista(L1);
	int n2 = num_el_lista(L2);

	int res;

	if (n1 < n2)
		return -1;

	if (n1 > n2)
		return 1;

	for (p1 = primolista(L1), p2 = primolista(L2);
		 !finelista(L1, p1) && !finelista(L2, p2);
		 p1 = succlista(L1, p1), p2 = succlista(L2, p2))
	{
		res = strcmp(p1->Autore->nome, p2->Autore->nome);

		if (res == 0)
			continue;
		else
			return -res;
	}

	return 0;
}

/* stampa una lista di autori */
void StampaListaAutori(lista L)
{
	posizione p;
	autore_st *Autore;

	for (p = primolista(L); !finelista(L, p); p = succlista(L, p))
	{
		Autore = leggelista(L, p);
		printf("%s %d %d %d\n", Autore->nome, Autore->n_articoli, Autore->n_citazioni, Autore->hirsch);
	}
}