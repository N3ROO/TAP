#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "tp1.h"


// Fonction de comparaison des ordonnées de deux points
// A et B: renvoie -1 si "A<B", +1 si "A>B" et 0 sinon
int fcmp_y(const void *A, const void *B)
{
  const point *a = A;
  const point *b = B;

  return a->y == b->y ? 0 : a->y < b->y ? -1 : 1;
}

// Fonction de comparaison des abscisses de deux
// points A et B: renvoie -1 si "A<B", +1 si "A>B" et 0 sinon
int fcmp_x(const void *A, const void *B)
{
  const point *a = A;
  const point *b = B;

  return a->x == b->x ? 0 : a->x < b->x ? -1 : 1;
}

// Algorithme naïf en O(n²)
// On suppose que P contient au moins n >= 2 points
paire algo_naif(point *P, int n)
{

  // On initialise la paire la plus proche comme étant la paire
  // des deux premiers points. Cela fonctionne car on suppose
  // que P contient au moins n >= 2 points.
  paire plus_proche = {
    P[0],
    P[1]
  }; 

  // On va parcourir toutes les paires possibles, et les comparer
  // avec la paire la plus proche
  for(int i = 0; i < n - 1; i ++) // i < n-1 important pour éviter de traiter plusieurs fois le même point
  {
    for(int j = i + 1; j < n; j ++) // j = i + 1 important pour la même raison
    {
      // On récupère la paire (en constant pour dire explicitement
      // qu'on ne va pas modifier les valeurs)
      const point a = P[i];
      const point b = P[j];

      // On la compare avec la meilleure
      if(dist(a, b) < dist(plus_proche.A, plus_proche.B)){
        plus_proche.A = a;
        plus_proche.B = b;
      }
    }
  }

  return plus_proche;
}

// Algorithme rec
// Px = tableau de point trié selon les abcisses
// Py = tableau de point trié selon les coordonnées
// n = nombre de point dans les tableaux
paire rec(point *Px, point *Py, int n)
{
  // On retourne les points les plus proches si n = 2 ou n = 3
  if(n == 2 || n == 3) return algo_naif(Px,n);
  if(n < 2) printf("[rec/ERROR] n should not be lower than 2 (val=%i).\n", n);

  // Il nous faut le point médian de Px, c'est à dire le point au milieu du tableau Px
  point median = Px[n / 2];

  // Maintenant, on va créer quatre tableaux: Ax, Ay et Bx, By: 
  // A: son x est <=   au   x median
  // B: son x est >  que le x median

  // On prépare les tailles des tableaux pour y voir plus clair
  int nA = n / 2 + 1; // On fait +1 car c'est <=, il faut donc prendre la valeur du milieu
  int nB = n - nA; // On prend ce qui reste

  // On peut créer les tableaux
  point Ax[nA];
  point Ay[nA];
  point Bx[nB];
  point By[nB];

  // Maintenant on peut initialiser les tableaux
  for(int i = 0; i < n; i ++)
  {
    if(i < nA)
    {
      Ax[i] = Px[i];
      Ay[i] = Py[i];
    }
    else
    {
      Bx[i] = Px[i];
      By[i] = Py[i];
    }
  }

  // Finalement, on peut trier les tableaux
  qsort(Ax, nA, sizeof(point), fcmp_x);
  qsort(Ay, nA, sizeof(point), fcmp_y);
  qsort(Bx, nB, sizeof(point), fcmp_x);
  qsort(By, nB, sizeof(point), fcmp_y);

  // Tout est prêt, on peut faire les appels récursifs.
  // On veut récupérer les paires (a, a') et (b, b')
  paire pA = rec(Ax, Ay, nA);
  paire pB = rec(Bx, By, nB);

  // On a terminé avec nos appels récursifs, on peut alors calculer delta
  // En fait, delta c'est la distance le plus petite entre (a, a') et (b, b')
  double distA = dist(pA.A, pA.B);
  double distB = dist(pB.A, pB.B);
  double delta = distA < distB ? distA : distB;

  // Il nous faut maintenant créer Sy à partir de Py
  point Sy[n];
  int nSy = 0; // taille de Py

  // S contient tous les points p de Py, qui ont la proriété suivante:
  // - | x median - x de p | < delta
  for(int i = 0; i < n; i ++)
  {
    if(abs(median.x - Py[i].x) < delta)
    {
      Sy[nSy] = Py[i];
      nSy ++;
    }
  }

  // TODO: test if tailleSy < 2 ? display delta

  // Maintenant qu'on a Sy, on va chercher la valeur minimale:
  // pour chaque point s appartenant à Sy, on doit trouver le min
  // dist(s, j), avec j [s+1 ... s+7] (c'est un peu compliqué à expliquer
  // en français)

  paire pS = {
    Sy[0],
    Sy[1]
  };

  for(int i = 0; i < nSy; i ++)
  {
    for(int j = 1; j < 8; j ++)
    {
      // Attention à ne pas depasser la taille de nSy!
      if(i + j < nSy)
      {
        if(dist(Sy[i], Sy[i + j]) < dist(pS.A, pS.B)){
          pS.A = Sy[i];
          pS.B = Sy[i + j];
        }
      }
    }
  }

  // On arrive au bout! On a:
  // - (s, s'): la paire qu'on vient de calculer
  // - (a, a')
  // - (b, b') 
  // Il nous suffit tout simplement de renvoyer le minimum de ces 3 paires

  // On rappelle que delta est la valeur min entre (a, a') et (b, b')
  if(dist(pS.A, pS.B) < delta)
  {
    return pS;
  }
  else if(dist(pA.A, pA.B) < dist(pB.A, pB.B))
  {
    return pA;
  }
  else
  {
    return pB;
  }
}

// Algorithme récursif en O(nlogn)
// On suppose que P contient au moins n>=2 points
paire algo_rec(point *P, int n)
{
  // On créé les tableaux Px et Py
  point Px[n];
  point Py[n];

  // On les initialise
  for(int i = 0; i < n; i ++)
  {
    Px[i] = P[i];
    Py[i] = P[i];
  }

  // On les trie
  qsort(Px, n, sizeof(point), fcmp_x);
  qsort(Py, n, sizeof(point), fcmp_y);

  // On désormais utiliser l'algo recursif
  return rec(Px, Py, n);
}

int main(int argc, char *argv[]){

  srandom(time(NULL));

  if(argc==2){
    //
    // partie à ne pas modifier
    //
    if(!isdigit(argv[1][0])){ printf("Forgot \"naif\" or \"rec\"?.\n"); exit(1); }
    int n=atoi(argv[1]);
    point *P=point_random(n);
    if((P==NULL)||(n<1)){ printf("Empty point set.\n"); exit(1); }
    printf("%i\n",n);
    for(int i=0;i<n;i++)
      printf(FORM" "FORM"\n",P[i].x,P[i].y);
    return 0;
  }

  if(argc==3){
    //
    // partie à ne pas modifier
    //
    int n;
    paire res={{-1,0},{0,0}};
    point *P=read(argv[1],&n);
    if(!strcmp(argv[2],"naif")) res=algo_naif(P,n);
    if(!strcmp(argv[2],"rec")) res=algo_rec(P,n);
    if(res.A.x<0){ printf("Unknown algorithm.\n"); exit(1); }
    printf("point A: "FORM" "FORM"\n",res.A.x,res.A.y);
    printf("point B: "FORM" "FORM"\n",res.B.x,res.B.y);
    printf("distance: "FORM"\n",dist(res.A,res.B));
    return 0;
  }

  // pour vos tests personnels, si nécessaire, modifier le main()
  // seulement ci-dessous

  ;;;;
  ;;;;
  ;;;;

  return 0;
}
