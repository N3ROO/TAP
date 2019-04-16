//
// NOM: Gallon
// Prénom: LIlian
// Groupe: A3
//
// Performances (très bon CPU):
// 10 000 points:
// - naif: de 1,20s à 1,22s
// -  rec: de 0,02s à 0,05s
// 60 000 points:
// - naif: trop long
// -  rec: 0.07s
// 
// Utilisation de la mémoire:
// Aucune fuite pour rec: valgrind --leak-check=full ./tp huge.txt rec
// Aucune fuite pour naif: algrind --leak-check=full ./tp p1000.txt naif
 
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


paire rec(point *Px, point *Py, int n)
{
  // Voici les normes de notation que nous allons utiliser:
  // nX: la variable contenant la taille du tableau X
  // pX: la paire la plus proche dans le tableau X
  // const <type> <var>: on utilise const pour indiquer explicitement
  //    que la variable ne va pas changer de valeur 

  // On retourne les points les plus proches si n = 2 ou n = 3
  if(n == 2 || n == 3) return algo_naif(Px, n);
  // Ce cas ne devrait jamais arriver, mais au moins on a l'info si on a un bug bizarre
  if(n < 2) printf("[rec/ERROR] n should not be lower than 2 (val=%i).\n", n);

  // Il nous faut le point médian de Px, c'est à dire le point au milieu du tableau Px
  const point median = Px[n / 2];

  // Maintenant, on va créer quatre tableaux: Ax, Ay et Bx, By: 
  // A: son x est <=   au   x median
  // B: son x est >  que le x median

  // On prépare les tailles des tableaux pour y voir plus clair
  unsigned int nA = n / 2;  // On prend la moitié
  unsigned int nB = n - nA; // On prend ce qui reste

  // On peut créer les tableaux. Note importante:
  // Sur un très grand nombre de points, il n'est plus possible
  // de créer des tableaux sur le tas "point Ax[nA];", on va donc
  // faire des malloc.
  point *Ax = malloc(nA * sizeof(*Ax));
  point *Ay = malloc(nA * sizeof(*Ay));
  point *Bx = malloc(nB * sizeof(*Bx));
  point *By = malloc(nB * sizeof(*By));

  // Maintenant on peut initialiser les tableaux
  for(unsigned int i = 0; i < n; i ++)
  {
    if(i < nA)
    {
      Ax[i] = Px[i];
      Ay[i] = Py[i];
    }
    else
    {
      // Attention, il faut commencer à 0, d'où le i-nA, j'ai
      // dû passé 30 minutes à débug ce petit détail
      Bx[i-nA] = Px[i];
      By[i-nA] = Py[i];
    }
  }

  // Il n'est pas nécessaire de trier les tableaux car ils le sont déjà:
  // Les tableaux sont issus de Px et Py qui ont été triés dans algo_rec.

  // Cela permet de ne pas lancer les récursions si on sait qu'on a pas
  // assez de points.
  if(nA < 2 || nB < 2) {
    printf("[rec/ERROR] nombre de points < 2 : nA=%i et nB=%i\n", nA, nB);
    exit(-1);
  }

  // Tout est prêt, on peut faire les appels récursifs.
  // On veut récupérer les paires (a, a') et (b, b')
  const paire pA = rec(Ax, Ay, nA);
  const paire pB = rec(Bx, By, nB);

  // On n'a plus besoins des tableaux Ax, Ay, et Bx, By, on peut donc les libérer
  free(Ax);
  free(Ay);
  free(Bx);
  free(By);

  // On a terminé avec nos appels récursifs, on peut alors calculer delta
  // En fait, delta c'est la distance le plus petite entre (a, a') et (b, b')
  double distA = dist(pA.A, pA.B);
  double distB = dist(pB.A, pB.B);
  double delta = distA < distB ? distA : distB;

  // Il nous faut maintenant créer Sy à partir de Py
  point *Sy = malloc(n*sizeof(*Sy));
  unsigned int nSy = 0; // taille de Py

  // S contient tous les points p de Py, qui ont la proriété suivante:
  // - | x median - x de p | < delta
  for(unsigned int i = 0; i < n; i ++)
  {
    if(abs(median.x - Py[i].x) < delta)
    {
      Sy[nSy] = Py[i];
      nSy ++;
    }
  }

  // Maintenant qu'on a Sy, on va chercher la valeur minimale:
  // pour chaque point s appartenant à Sy, on doit trouver le min
  // dist(s, j), avec j [s+1 ... s+7] (c'est un peu compliqué à expliquer
  // en français, mais c'est bien écrit dans le pdf)

  paire pS = {
    Sy[0],
    Sy[1]
  };

  for(unsigned int i = 0; i < nSy; i ++)
  {
    for(unsigned int j = 1; j < 8; j ++)
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

  // On n'a plus besoin de Sy, donc on le lib_ère
  free(Sy);

  // On arrive au bout! On a:
  // - (s, s'): la paire qu'on vient de calculer
  // - (a, a')
  // - (b, b') 
  // Il nous suffit tout simplement de renvoyer le minimum de ces 3 paires

  // On rappelle que delta est la valeur min entre (a, a') et (b, b')
  if(dist(pS.A, pS.B) < delta)
    return pS;
  else if(dist(pA.A, pA.B) < dist(pB.A, pB.B))
    return pA;
  else
    return pB;
}

// algorithme récursif en O(nlogn)
// on suppose que P contient au moins n>=2 points
paire algo_rec(point *P, int n)
{
  // On créé les tableaux Px et Py
  point *Px = malloc(n * sizeof(*Px));
  point *Py = malloc(n * sizeof(*Py));

  // On les initialise
  for(int i = 0; i < n; i ++)
  {
    Px[i] = P[i];
    Py[i] = P[i];
  }

  // On les trie
  qsort(Px, n, sizeof(point), fcmp_x);
  qsort(Py, n, sizeof(point), fcmp_y);

  // On peut désormais utiliser l'algo recursif
  paire resultat = rec(Px, Py, n);

  // On peut libérer nos tableaux
  free(Px);
  free(Py);

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
    free(P);
    return 0;
  }

  // pour vos tests personnels, si nécessaire, modifier le main()
  // seulement ci-dessous
  
  ;;;;
  ;;;;
  ;;;;
  
  return 0;
}
