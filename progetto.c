
/* Direttive */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lista.h"
#include "clique.h"
#include "intqueue.h"

/* Prototipi delle procedure secondarie */

/* funzione confronto per sorting, passata a QuickSort per indirizzo */
typedef int (*compar)(const void *, const void *);

void InterpretaLineaComando(int argc, char *argv[], char *nomeFile);

/*parsing file, carica articoli in Sequenza*/
articolo_st *CaricaArticoli(char *nomeFile);

/* rimuove gli spazi all'inizio e alla fine di una stringa */
char *RimuoviSpazi(char *str);

/* parsing stringa autori separati da "&" */
void LeggiAutori(char *autori, lista L, articolo_st *A, int n_articoli_tot);

/* parsing stringa citazioni separate da "&" */
void LeggiCitazioni(char *citazioni, articolo_st *articoli);


/* scorre la lista autori e per ciascuno riempie: collaboratori, num. citazioni, indice di Hirsch */
void GestioneAutore(lista L);

/* scorre gli autori di ogni articolo e riempie la lista collaboratori di ciascun autore */
void AggiungiCollaboratoreAutore(autore_st *Autore, lista L);

/* calcola l'indice di Hirsch dell'autore */
int Hirsch(autore_st *Autore);

/* ordina gli articoli in base al numero di volte che sono stati citati usando QuickSort */
void OrdinaArticoli(autore_st *Autore);

/* ordina gli autori in base a: num. articoli, citazioni e ordine alfabetico usando QuickSort */
void OrdinaAutori();


/* calcola il diametro e le coppie di autori */
int CalcolaDiametro();

/* visita in ampiezza e calcolo distanze */
void BFS(autore_st **V, int N, int s, int *D);

/* Calcola le cliques massimali */
void TrovaCliques();

/* calcola il clique number e il num. di sottoinsiemi di cardinalita' max */
int CliqueNumber_Sottoinsiemi(listaclique_t listaclique_max);

/* controlla se un autore esterno puo' essere aggiunto alla clique */
boolean collaboratore_esterno_clique(lista L, autore_st *Autore);

/* QuickSort generica */
/* void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*)); */

void Scambia(void **pa, void **pb);
int Partition(void **V, int s, int d, compar cmp);
/* QuickSort basata sulla funzione qsort del C con tipi generici in ingresso */
void QuickSort(void **V, int s, int d, compar cmp);

/* funzione di confronto. ordina elementi in base al numero di citazioni ricevute */
int f_confronto_cit(const void *a, const void *b);

/* funzione di confronto in QuickSort per gli autori */
int f_confronto_autori(const void *a, const void *b);

/* funzione per confronto lessicografico */
int f_confronto_lex(const void *a, const void *b);


/* variabili globali */
articolo_st *Sequenza;		   /* sequenza di articoli */
lista Autori;				   /* elenco degli autori */
int n_autori_tot = 0;		   /* totale autori */
listaclique_t listaclique_max; /* lista delle clique massimali */


/* Programma principale */
int main(int argc, char *argv[])
{
	char nomeFile[ROW_LENGTH];

	InterpretaLineaComando(argc, argv, nomeFile);

	Autori = crealista();

	/* scorrendo il file, salva gli articoli in un vettore e gli autori in una lista */
	Sequenza = CaricaArticoli(nomeFile);

	/* scorre la lista autori e per ciascuno riempie: collaboratori, num. citazioni, indice di Hirsch */
	GestioneAutore(Autori);

	/* ordina gli autori in base a: articoli scritti, citazioni, ordine alfabetico */
	OrdinaAutori();

	/* stampa la lista degli autori */
	StampaListaAutori(Autori);

	/* calcola il diametro e lo stampa */
	CalcolaDiametro();

	/* calcola le clique massimali */
	listaclique_max = creaclique();
	TrovaCliques();

	/* calcola clique number */
	CliqueNumber_Sottoinsiemi(listaclique_max);

	printf("Clique massimali %d\n", num_el_lista_clique(listaclique_max));
	StampaListaCliques(listaclique_max, 1); /* passando 1 stampa con '&' */

	/* libera la memoria */
	free(Sequenza);
	distruggelista(&Autori);
	distruggeclique(&listaclique_max);


	return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------------------------------------- */
/* Definizione delle procedure secondarie */

void InterpretaLineaComando(int argc, char *argv[], char *nomeFile)
{
	if (argc != 2)
	{
		fprintf(stderr, "Errore nella riga di comando!\n");
		fprintf(stderr, "%s [nomedati]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strcpy(nomeFile, argv[1]);
}

/*parsing file, carica articoli in Sequenza*/
articolo_st *CaricaArticoli(char *nomeFile)
{
	FILE *fSequenza;
	int n_articoli_tot; /* numero totale articoli */
	int i;
	char autore[ROW_LENGTH];
	char citazioni[ROW_LENGTH];
	char rigaFile[ROW_LENGTH];
	char riga_tmp[ROW_LENGTH];
	articolo_st *Sequenza;
	const char s[2] = "|"; /* carattere separatore */
	char *token;

	/*apro il file in lettura*/
	fSequenza = fopen(nomeFile, "r");
	if (fSequenza == NULL)
	{
		fprintf(stderr, "errore nell'apertura del file %s\n", nomeFile);
		exit(EXIT_FAILURE);
	}

	/* Legge una riga dal file */
	fscanf(fSequenza, "%d\n", &n_articoli_tot);

	/* definizione del vettore che contiene gli articoli, di dim n_articoli_tot */
	Sequenza = calloc(n_articoli_tot, sizeof(articolo_st));
	if (Sequenza == NULL)
	{
		fprintf(stderr, "Memoria insufficiente per allocare un elemento!\n");
		exit(EXIT_FAILURE);
	}

	/* scorro gli articoli in ingresso */
	for (i = 0; i < n_articoli_tot; i++)
	{
		articolo_st *A = &Sequenza[i];		/* inserisco l'articolo corrente in Sequenza */
		A->autoriPerArticolo = crealista(); /* creo la lista dei suoi autori */

		fscanf(fSequenza, "%[^\n]\n", rigaFile); /* leggo la riga senza l'a capo */

		strcpy(riga_tmp, rigaFile); /* la copio in una stringa temporanea */

		/* strtok restituisce la prima stringa delimitata da s = "|" */
		token = strtok(riga_tmp, s);								 /* id */
		A->id = (int)strtol(RimuoviSpazi(token), (char **)NULL, 10); /* e' l'id, converto in int e inserisco nel campo corrispondente */

		/* passando NULL a strtok, la funzione riparte da dove aveva tagliato prima. */
		/* Eseguo la stessa op su titolo, autori e citazioni */
		token = strtok(NULL, s); /* titolo */
		strcpy(A->titolo, RimuoviSpazi(token));

		token = strtok(NULL, s); /* autore */
		strcpy(autore, (token));

		token = strtok(NULL, s); /* citazioni */
		strcpy(citazioni, (token));

		/* parsing e gestione della stringa di autori */
		LeggiAutori(autore, A->autoriPerArticolo, A, n_articoli_tot);

		/* parsing e gestione della stringa di citazioni*/
		LeggiCitazioni(citazioni, Sequenza);
	}
	fclose(fSequenza);
	return Sequenza;
}

/* rimuove gli spazi all'inizio e alla fine di una stringa */
char *RimuoviSpazi(char *str)
{
	char *end;

	/* rimuove spazio all'inizio */
	while (isspace((unsigned char)*str))
		str++;

	if (*str == 0)
		return str;

	/* rimuove spazio in fondo */
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end))
		end--;

	end[1] = '\0';

	return str;
}

/* -------------------------------------------------------------------------------------------------------- */
/* Funzioni gestione autori */

/* parsing stringa autori separati da "&" */
void LeggiAutori(char *autori, lista L, articolo_st *art, int n_articoli_tot)
{
	const char s[2] = "&";
	char *token;
	char riga_tmp[ROW_LENGTH];
	posizione p;
	autore_st *A;

	/* lavora sulla stringa autori */
	strcpy(riga_tmp, autori);
	token = strtok(riga_tmp, s);

	/* finche' il puntatore non raggiunge la fine della stringa */
	while (token != NULL)
	{
		/* toglie gli spazi */
		token = RimuoviSpazi(token);

		/* cerca l'autore nella lista */
		p = TrovaAutorePerNome(token, Autori);

		/* se l'autore non c'e ancora */
		if (p == NO_ELEMENT)
		{
			p = succlista(Autori, ultimolista(Autori)); /* sposta il puntatore per aggiungere un nuovo elemento alla lista */

			/* crea un autore A e ne inizializza i campi*/
			A = calloc(1, sizeof(autore_st));
			if (A == NULL)
			{
				fprintf(stderr, "Memoria insufficiente per allocare un elemento!\n");
				exit(EXIT_FAILURE);
			}
			strcpy(A->nome, token);							   /* nome dell'autore A */
			A->articoli = calloc(n_articoli_tot, sizeof(art)); /* vettore di articoli di lunghezza n_articoli_tot */
			if (A->articoli == NULL)
			{
				fprintf(stderr, "Memoria insufficiente per allocare un elemento!\n");
				exit(EXIT_FAILURE);
			}
			A->collaboratori = crealista(); /* lista per i collaboratori */

			/* inserisce A nella lista autori */
			inslista(Autori, p, A);

			/* incrementa contatore numero autori */
			n_autori_tot++;
		}

		/* se l'autore c'e gia' vi sposto p */
		else
		{
			A = p->Autore;
		}
		/* inserisco l'articolo corrente nel vettore articoli di A */
		A->articoli[A->n_articoli] = art;
		A->n_articoli += 1;

		/* aggiungo A alla lista autori dell'articolo corrente */
		p = succlista(L, ultimolista(L));
		inslista(L, p, A);

		/* passo al prossimo autore nella stringa */
		token = strtok(NULL, s);
	}
}

/* parsing stringa citazioni separate da "&" */
void LeggiCitazioni(char *citazioni, articolo_st *articoli)
{
	const char s[2] = "&";
	char *token;
	char riga_tmp[ROW_LENGTH];

	/* lavora sulla stringa citazioni */
	strcpy(riga_tmp, citazioni);
	token = strtok(riga_tmp, s);

	/* finche' trova citazioni */
	while (token != NULL)
	{
		/* converte ciascuna citazione in int */
		int cit = (int)strtol(RimuoviSpazi(token), (char **)NULL, 10);

		/* se non ci sono cit esce */
		if (cit == 0)
			return;

		/* altrimenti incrementa il contatore dell'articolo corrispondente */
		articoli[cit - 1].cit_tot += 1;

		/* passa alla prossima cit */
		token = strtok(NULL, s);
	}
}

/* scorre la lista autori e per ciascuno:
- aggiunge tutti i suoi collaboratori
- calcola il numero tot di volte che e' stato citato
- calcola l'indice di Hirsch*/
void GestioneAutore(lista L)
{
	int i;
	posizione p;
	autore_st *Autore;

	for (p = primolista(L); !finelista(L, p); p = succlista(L, p))
	{
		Autore = leggelista(L, p);
		for (i = 0; i < Autore->n_articoli; i++)
		{
			Autore->n_citazioni += Autore->articoli[i]->cit_tot;
			AggiungiCollaboratoreAutore(Autore, Autore->articoli[i]->autoriPerArticolo);
		}
		/* calcola l'indice di Hirsch dell'autore */
		Autore->hirsch = Hirsch(Autore);
	}
}

/* scorre gli autori di ogni articolo e riempie la lista collaboratori di ciascun autore */
void AggiungiCollaboratoreAutore(autore_st *Autore, lista L)
{
	autore_st *collaboratore;
	posizione pC;

	for (pC = primolista(L); !finelista(L, pC); pC = succlista(L, pC))
	{
		collaboratore = leggelista(L, pC);
		if (strcmp(collaboratore->nome, Autore->nome))
		{
			/* inserisce il collaboratore nella lista in ordine alfabetico */
			inslista_ordlex_nodup(Autore->collaboratori, collaboratore);
		}
	}
}

/* calcola l'indice di Hirsch dell'autore */
int Hirsch(autore_st *Autore)
{
	int i, j;

	/* ordina gli articoli per numero di citazioni ricevute */
	OrdinaArticoli(Autore);

	/* esce dal ciclo quando l'indice e' maggiore del num. citazioni */
	for (i = 0, j = 1; i < Autore->n_articoli; i++, j++)
	{
		if (j > Autore->articoli[i]->cit_tot)
			break;
	}
	/* il programma si ferma al valore successivo all'indice di Hirsch. Restituisce sempre j-1 tranne nel caso j=1*/
	return (j > 1) ? j - 1 : j;
}

/* ordina gli articoli in base al numero di volte che sono stati citati usando QuickSort */
void OrdinaArticoli(autore_st *Autore)
{
	int i;
	articolo_st **V = calloc(Autore->n_articoli, sizeof(articolo_st *));

	/* copia gli articoli in un vettore */
	for (i = 0; i < Autore->n_articoli; i++)
		V[i] = Autore->articoli[i];

	/* ordina il vettore */
	QuickSort((void **)V, 0, Autore->n_articoli - 1, f_confronto_cit);

	/* ricopia gli articoli nella struttura autore */
	for (i = 0; i < Autore->n_articoli; i++)
		Autore->articoli[i] = V[i];

	free(V);
}

/* ordina gli autori in base a: num. articoli, citazioni e ordine alfabetico usando QuickSort */
void OrdinaAutori() /* nessun argomento perche' la lista autori e' variabile globale */
{
	int i = 0;
	posizione p;
	autore_st **V = calloc(n_autori_tot, sizeof(autore_st));
	if (V == NULL)
	{
		fprintf(stderr, "Memoria insufficiente per allocare un elemento!\n");
		exit(EXIT_FAILURE);
	}

	/* copia gli articoli in un vettore */
	for (p = primolista(Autori); !finelista(Autori, p); p = succlista(Autori, p))
	{
		V[i] = leggelista(Autori, p);
		i++;
	}

	/* ordina il vettore */
	QuickSort((void **)V, 0, n_autori_tot - 1, f_confronto_autori);

	/* svuota la lista autori per poi riempirla da capo*/
	svuotalista(Autori);

	/* mette gli elementi ordinati nella lista */
	for (i = 0; i < n_autori_tot; i++)
	{
		p = succlista(Autori, ultimolista(Autori));
		inslista(Autori, p, V[i]);
	}

	free(V);
}


/* -------------------------------------------------------------------------------------------------------- */
/* Funzioni calcolo diametro. */

/* calcola il diametro e le coppie di autori */
int CalcolaDiametro()
{
	/* StampaAdiacenze(); */
	int i, j;
	int diametro;
	posizione p;
	listaclique_t lista_coppie;
	lista coppia_autori;
	autore_st *A1, *A2;
	autore_st **V;
	int *D;
	int **M;

	/* vettore di autori */
	V = calloc(n_autori_tot, sizeof(autore_st));
	if (V == NULL)
	{
		fprintf(stderr, "Memoria insufficiente per allocare un elemento!\n");
		exit(EXIT_FAILURE);
	}

	/* vettore delle distanze */
	D = (int *)calloc(n_autori_tot, sizeof(autore_st));
	if (D == NULL)
	{
		fprintf(stderr, "Memoria insufficiente per allocare un elemento!\n");
		exit(EXIT_FAILURE);
	}

	/* crea una matrice di adiacenza con entrate le distanze tra gli autori */
	M = calloc(n_autori_tot, sizeof(int *));
	if (M == NULL)
	{
		fprintf(stderr, "Memoria insufficiente per allocare un elemento!\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < n_autori_tot; i++)
	{
		M[i] = calloc(n_autori_tot, sizeof(int));
		if (M[i] == NULL)
		{
			fprintf(stderr, "Memoria insufficiente per allocare un elemento!\n");
			exit(EXIT_FAILURE);
		}
	}

	/* inizializza il vettore dei nodi con gli autori */
	i = 0;
	for (p = primolista(Autori); !finelista(Autori, p); p = succlista(Autori, p))
	{
		V[i] = leggelista(Autori, p);
		p->Autore->idx = i;
		i++;
	}

	/* calcola il diametro */
	diametro = 0;
	for (i = 0; i < n_autori_tot; i++)
	{
		/* azzera il vettore delle distanze */
		memset(D, 0, n_autori_tot * sizeof(int));

		/* visita in ampiezza, dopo la quale risale il cammino da nodo a sorgente per calcolare la lugnhezza */
		BFS(V, n_autori_tot, i, D);

		/* calcola la distanza massima */
		for (j = 0; j < n_autori_tot; j++)
		{
			if (D[j] > diametro)
				diametro = D[j];
			else if (D[j] < 0) /* caso in cui il grafo non e' connesso. il diametro e' il numero di autori */
			{
				D[j] = n_autori_tot;
				diametro = n_autori_tot;
			}
			M[i][j] = D[j];
		}
	}
	printf("Diametro %d\n", diametro);

	/* calcola la lista delle coppie con distanza = diametro */
	lista_coppie = creaclique();

	/* scorre il triangolo superiore della matrice ed estrae gli autori */
	for (i = 0; i < n_autori_tot; i++)
	{
		for (j = i + 1; j < n_autori_tot; j++)
		{
			if (M[i][j] < diametro)
				continue;

			coppia_autori = crealista();
			A1 = V[i];
			A2 = V[j];

			/* inserimento nella coppia in ordine alfabetico */
			inslista_ordlex_nodup(coppia_autori, A1);
			inslista_ordlex_nodup(coppia_autori, A2);

			/* inserimento nella lista coppie in ordine alfabetico */
			insclique_ordlex_nodup(lista_coppie, coppia_autori);
		}
	}

	/* stampa e deallocazione memoria */
	StampaListaCliques(lista_coppie, 0);

	distruggeclique(&lista_coppie);

	for (i = 0; i < n_autori_tot; i++)
		free(M[i]);
	free(M);
	free(V);
	free(D);

	return diametro;
}

/* visita in ampiezza e calcolo distanze */
void BFS(autore_st **V, int N, int s, int *D)
{
#define C_MARK -1 /* costante per marcare i nodi */

	int v, w, padre; /* nodi */
	int distanza;
	int i;
	intqueue Q; /* coda */
	posizione p;
	autore_st *Autore;

	int *C = calloc(N, sizeof(int)); /* vettore di marcatura */
	if (C == NULL)
	{
		fprintf(stderr, "Memoria insufficiente per allocare un elemento!\n");
		exit(EXIT_FAILURE);
	}

	/* inializza il vettore di marcatura con la costante C_MARK */
	memset(C, C_MARK, N * sizeof(int));
	/* TODO add error */

	/* Crea una coda vuota */
	CreateQueueInt(&Q, N);

	/* Marca la sorgente s e la inserisce nella coda */
	C[s] = s;
	EnqueueInt(s, &Q);

	/* Finche' la coda non e' vuota */
	while (!IsEmptyQueueInt(&Q))
	{
		/* Estrae l'elemento in testa alla coda */
		v = FrontInt(&Q);
		DequeueInt(&Q);
		Autore = V[v];

		/* scorre la lista collaboratori */
		for (p = primolista(Autore->collaboratori); !finelista(Autore->collaboratori, p); p = succlista(Autore->collaboratori, p))
		{
			w = p->Autore->idx;

			/* Se l'elemento adiacente non e' ancora stato visitato */
			if (C[w] == C_MARK)
			{
				/* Marca l'elemento adiacente e lo inserisce nella coda */
				C[w] = v;
				EnqueueInt(w, &Q);
			}
		}
	}

	padre = 0;
	for (i = 0; i < N; i++)
	{

		/* nodo non raggiungibile (-> grafo sconnesso) */
		if (C[i] == C_MARK)
		{
			distanza = C_MARK;
			D[i] = C_MARK;
			continue;
		}

		/* risalgo ciascun cammino fino alla sorgente */
		padre = i;
		distanza = 0;
		while (padre != s)
		{
			distanza++;
			padre = C[padre];
		}
		D[i] = distanza;
	}

	free(C);

	DestroyQueueInt(&Q);
}
/* -------------------------------------------------------------------------------------------------------- */
/* Funzioni per le clique. */

/* Calcola le cliques massimali */
void TrovaCliques()
{
#define QUEUE_SIZE 4096

	posizione p, p_ac;
	lista L = Autori;
	lista_queue Queue;
	lista_queue *pQueue = &Queue;
	lista clique;
	lista clique_corrente;

	/* crea una coda */
	CreateQueue(pQueue, QUEUE_SIZE);

	/* inizializzazione lista cliques banali (un singolo autore) */
	for (p = primolista(L); !finelista(L, p); p = succlista(L, p))
	{
		clique = crealista();
		inslista_ordlex_nodup(clique, p->Autore);

		Enqueue(clique, pQueue);
	}

	/* finche' la coda non e' vuota */
	while (!IsEmptyQueue(pQueue))
	{
		/* pop clique_corrente */
		clique_corrente = Front(pQueue);
		Dequeue(pQueue);

		clique = NULL;
		/* ciclo su tutti gli autori */
		for (p = primolista(L); !finelista(L, p); p = succlista(L, p))
		{
			/* aggiungo i collaboratori comuni a tutti gli el della clique */
			if (collaboratore_esterno_clique(clique_corrente, p->Autore))
			{

				clique = crealista();
				for (p_ac = primolista(clique_corrente); !finelista(clique_corrente, p_ac); p_ac = succlista(clique_corrente, p_ac))
				{
					/* copio elementi clique prec in una nuova clique */
					inslista_ordlex_nodup(clique, p_ac->Autore);
				}
				/* aggiungo il nuovo collaboratore */
				inslista_ordlex_nodup(clique, p->Autore);

				/* metto in coda la nuova clique */
				Enqueue(clique, pQueue);
			}
		}
		/* se non ho piu' collaboratori comuni da aggiungere (ovvero la clique e' massimale) */
		if (clique == NULL)
		{
			/* metto la clique massimale in una lista a parte */
			insclique_ordlex_nodup(listaclique_max, clique_corrente);
		}
		/* altrimenti la elimino */
		else
		{
			free(clique_corrente);
		}

		/* StampaQueueCliques(pQueue); */
	}
	/* StampaListaCliques(listaclique_max); */
}

/* controlla se un autore esterno puo' essere aggiunto alla clique */
boolean collaboratore_esterno_clique(lista L, autore_st *Autore)
{

	posizione p;
	boolean res = TRUE;

	for (p = primolista(L); !finelista(L, p); p = succlista(L, p))
	{
		/* controllo se l'autore e' gia' presenta nella clique */
		res = res & (f_confronto_lex(Autore, p->Autore) != 0);
		/* controllo che tutti i membri della clique sono collaboratori dell'autore */
		res = res & in_lista(Autore->collaboratori, p->Autore);
		if (!res)
			return res;
	}

	return res;
}

/* calcola il clique number e il num. di sottoinsiemi di cardinalita' max */
int CliqueNumber_Sottoinsiemi(listaclique_t listaclique_max)
{

	posizione p; /* scorre la lista autori */
	int cliqueNumber = 0;
	int c = 0;										/* num sottoinsiemi di cardinalita' massima */
	posCQ_t q = primoclique(listaclique_max);		/* primo el clique */
	posCQ_t ultimo = ultimoclique(listaclique_max); /* ultimo el clique */

	/* conta gli el della prima clique massimale (sono in ordine di cardinalita' non crescente) */
	for (p = primolista(q->ListaAutori); !finelista(q->ListaAutori, p); p = succlista(q->ListaAutori, p))
	{
		cliqueNumber++;
	}

	while (num_el_lista(q->ListaAutori) == cliqueNumber)
	{
		c++;
		/* controllo se ho raggiunto la fine della lista */
		if (q == ultimo)
			break;
		q = succclique(listaclique_max, q);
	}

	printf("Clique number %d Sottoinsiemi %d\n", cliqueNumber, c);
	return cliqueNumber;
}



/* -------------------------------------------------------------------------------------------------------- */
/* Procedure ordinamento */
void Scambia(void **pa, void **pb)
{
	void *temp;

	temp = *pa;
	*pa = *pb;
	*pb = temp;
}

/* Partiziona il vettore V fra s e d in un sottovettore V[s,q] di elementi <= pivot
   e un sottovettore V[q+1,d] di elementi > pivot, pone il pivot fra i due sottovettori
   e restituisce l'indice finale del pivot */
int Partition(void **V, int s, int d, compar cmp)
{
	int d1; /* ultimo elemento della prima tabella (elementi <= pivot) */
	int d2; /* primo elemento a destra della seconda tabella  (elementi > pivot) */

	d1 = s;
	for (d2 = s + 1; d2 <= d; d2++)
		if (cmp(V[d2], V[s]) <= 0)
		{
			d1++;
			Scambia(&V[d1], &V[d2]);
		}

	Scambia(&V[d1], &V[s]);

	return d1;
}

/* QuickSort basata sulla funzione qsort del C con tipi generici in ingresso e puntatore a funzione di confronto */
/* Ordina il sottovettore del vettore V compreso fra s e d con l'algoritmo QuickSort */
void QuickSort(void **V, int s, int d, compar cmp)
{
	int q;

	if (s < d)
	{
		q = Partition(V, s, d, cmp);
		QuickSort(V, s, q - 1, cmp);
		QuickSort(V, q + 1, d, cmp);
	}
}

/* funzione di confronto. ordina elementi in base al numero di citazioni ricevute */
int f_confronto_cit(const void *a, const void *b)
{
	const articolo_st *aa, *bb;
	int res;

	aa = a;
	bb = b;

	res = (aa->cit_tot - bb->cit_tot);
	return -res;
}

/* funzione di confronto in QuickSort per gli autori */
int f_confronto_autori(const void *a, const void *b)
{
	int res;
	const autore_st *aa, *bb;
	aa = a;
	bb = b;

	/* primo criterio: numero articoli non crescente*/
	if (aa->n_articoli == bb->n_articoli)
		/* se uguale, secondo criterio: numero citazioni non crescente */
		if (aa->n_citazioni == bb->n_citazioni)
			/* se uguale, terzo criterio: ordine alfabetico */
			res = -strcmp(aa->nome, bb->nome);
		else
			res = (aa->n_citazioni - bb->n_citazioni);
	else
		res = (aa->n_articoli - bb->n_articoli);

	return -res;
}

/* funzione per confronto lessicografico */
int f_confronto_lex(const void *a, const void *b)
{
	int res;
	const autore_st *aa, *bb;
	aa = a;
	bb = b;

	res = -strcmp(aa->nome, bb->nome);

	return -res;
}
