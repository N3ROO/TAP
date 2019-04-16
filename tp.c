//
// NOM: Gallon
// Prénom: LIlian
// Groupe: A3
//
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tp.h"


// fonction de comparaison (à completer ...) des ordonnées de deux
// points A et B: renvoie -1 si "A<B", +1 si "A>B" et 0 sinon
int fcmp_y(const void *A, const void *B)
{
  // On nous donne void*, il nous font donc caster les variables
  const point *a = A;
  const point *b = B;

  return a->y < b->y ? -1 : a->y > b->y ? 1 : 0;
}


// fonction de comparaison (à completer ...) des abscisses de deux
// points A et B: renvoie -1 si "A<B", +1 si "A>B" et 0 sinon
int fcmp_x(const void *A, const void *B)
{
  // On nous donne void*, il nous font donc caster les variables
  const point *a = A;
  const point *b = B;

  return a->x < b->x ? -1 : a->x > b->x ? 1 : 0;
}


// algorithme naïf en O(n^2)
// on suppose que P contient au moins n>=2 points
paire algo_naif(point *P, int n)
{  
  // On initialise la paire la plus proche comme étant la paire
  // des deux premiers points. Cela fonctionne car on suppose
  // que P contient au moins n >= 2 points.
  paire plusProche = {
    P[0],
    P[1]
  }; 

  // On va parcourir toutes les paires possibles, et les comparer
  // avec la paire la plus proche
  for(unsigned int i = 0; i < n - 1; i ++) // i < n-1 important pour éviter de traiter plusieurs fois le même point
  {
    for(unsigned int j = i + 1; j < n; j ++) // j = i + 1 important pour la même raison
    {
      // On récupère la paire (en constant pour dire explicitement
      // qu'on ne va pas modifier les valeurs)
      const point a = P[i];
      const point b = P[j];

      // On la compare avec la meilleure
      if(dist(a, b) < dist(plusProche.A, plusProche.B)){
        plusProche.A = a;
        plusProche.B = b;
      }
    }
  }

  return plusProche;
}


// algorithme récursif en O(nlogn)
// on suppose que P contient au moins n>=2 points
paire algo_rec(point *P, int n)
{
  paire resultat={{0,0},{0,0}};
  return resultat;
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
