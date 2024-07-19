#ifndef __intqueue_h
#define __intqueue_h

#include <stdio.h>
#include <stdlib.h>


typedef struct _intqueue intqueue;
struct _intqueue
{
  int *V;
  int size; /* dimensione massima della coda */
  int head; /* indice dell'ultima posizione occupata */
  int tail; /* indice della prima posizione libera */
};


void CreateQueueInt (intqueue *pQ, int size);

void DestroyQueueInt (intqueue *pQ);

int IsEmptyQueueInt (intqueue *pQ);

int FrontInt (intqueue *pQ);

void EnqueueInt (int i, intqueue *pQ);

void DequeueInt (intqueue *pQ);

#endif
