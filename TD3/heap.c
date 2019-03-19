#include "heap.h"
#include <stdlib.h>
#include <stdio.h>

heap heap_create(int k, int (*f)(const void *, const void *)) {
  
  heap h = malloc(sizeof(*h));

  h->n = 0;
  h->nmax = k;
  h->array = malloc((k+1) * sizeof(void*));
  h->f = f;

  // printf("avant\n");
  // int d = f((void*)1, (void*)2);
  // printf("après\n");

  return h;
}

void heap_destroy(heap h) {
  free(h->array);
  free(h);
}

bool heap_empty(heap h) {
  return (h->n == 0) ? true : false;
}

bool heap_add(heap h, void *object) {
  
  // Dans un premier temps, on vérifie s'il y a de la place
  if(h->n == h->nmax) return false;

  // On peut ajouter l'élement à la fin
  h->n ++;
  h->array[h->n] = object;

  // Maintenant, on fait des "flips" en remontant
  int i = h->n;
  while(i != 1 && h->array[i/2] > h->array[i])
  {
    // Si la valeur du père est plus petite que la valeur du fils, on "flip"
    void* tmp = h->array[i/2];
    h->array[i/2] = h->array[i];
    h->array[i] = tmp;

    // On change l'index pour analyser un niveau plus haut
    i = i/2;
  }

  return true;
}

void *heap_top(heap h) {
  return h->array[1];
}

void *heap_pop(heap h) {
  // On vérifie si c'est vide
  if(heap_empty(h)) return NULL;

  // On récupère la tête
  void* val = heap_top(h);

  // Maintenant, on ecrase la tete:
  // On met le dernier élement à la tête de la liste
  h->array[1] = h->array[h->n];
  // On dit qu'il y a un élement en moins dans la liste (on vient d'écraser la tête)
  h->n --;

  // Maintenant on "fix" notre tas
  int index = 1;

  void * parent = h->array[index];
  void * left   = h->array[index*2+1];
  void * right  = h->array[index*2+1];
  
  while((left < parent || right < parent) && index < h->n)
  {
    if(left < parent)
    {
      // on inverse avec le premier fils
      void* tmp = h->array[index];
      h->array[index] = h->array[index*2];
      h->array[index*2] = tmp;
      index = index*2;
    }
    else
    {
      // on inverse avec le deuxieme fils
      void* tmp = h->array[index];
      h->array[index] = h->array[index*2+1];
      h->array[index*2+1] = tmp;
      index = index*2+1;
    }
  }

  return val;
}
