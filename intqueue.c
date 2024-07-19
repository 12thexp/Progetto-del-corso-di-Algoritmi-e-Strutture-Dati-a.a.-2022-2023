#include "intqueue.h"


void CreateQueueInt (intqueue *pQ, int size)
{
  pQ->V = (int *) calloc(size,sizeof(int));
  if (pQ->V == NULL)
  {
    fprintf(stderr,"Errore nell'allocazione della coda!\n");
    exit(EXIT_FAILURE);
  }

  pQ->size = size;
  pQ->head = 0;
  pQ->tail = 0;
}


void DestroyQueueInt (intqueue *pQ)
{
  free(pQ->V);
  pQ->V = NULL;
  pQ->size = 0;
  pQ->head = 0;
  pQ->tail = 0;
}


int IsEmptyQueueInt (intqueue *pQ)
{
  return (pQ->head == pQ->tail);
}


int FrontInt (intqueue *pQ)
{
  return pQ->V[pQ->head];
}


void EnqueueInt (int i, intqueue *pQ)
{
  pQ->V[pQ->tail] = i;
  pQ->tail++;
  /* Nell'applicazione alla BFS, non succede mai */
  if (pQ->tail >= pQ->size) pQ->tail -= pQ->size;
}


void DequeueInt (intqueue *pQ)
{
  pQ->head++;
  /* Nell'applicazione alla BFS, non succede mai */
  if (pQ->head >= pQ->size) pQ->head -= pQ->size;
}
