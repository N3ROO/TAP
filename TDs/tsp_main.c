#include "tools.h"

#include "tsp_brute_force.c"
#include "tsp_prog_dyn.c"
#include "tsp_approx.c"
#include "tsp_mst.c"

int main(int argc, char *argv[]) {

  unsigned seed = time(NULL) % 1000;
  printf("seed: %u\n", seed); // pour rejouer la même chose au cas où
  srandom(seed);
  TopChrono(0); // initialise tous les chronos

  const int n = (argv[1] && atoi(argv[1])) ? atoi(argv[1]) : 10;
  point *V = generatePoints(n); // n points au hasard
  //point *V = generateCircles(n,3); // n points sur k=2 cercles au hasard
  int *P = malloc(n * sizeof(int)); // P = la tournée
  P[0] = -1; // permutation qui ne sera pas dessinée par drawTour()

  init_SDL_OpenGL(); // initialisation avant de dessiner
  drawTour(V, n, NULL); // dessine seulement les points
  update = false; // permet un affichage rapide de drawTour() dans les boucles

  
  // brute-force -> FAIT
  /*
  {
    TopChrono(1); // départ du chrono 1
    printf("value: %g\n", tsp_brute_force(V, n, P));
    printf("running time: %s\n", TopChrono(1)); // durée
    update = true; // force l'affichage
    while (running) { // affiche le résultat et attend (q pour sortir)
      drawTour(V, n, P);  // dessine la tournée
      handleEvent(update); // attend un évènement ou pas
    }
  }
  */

  
  // brute-force optimisé -> SEGFAULT
  /*
  {
    TopChrono(1); // départ du chrono 1
    printf("value: %g\n", tsp_brute_force_opt(V, n, P));
    printf("running time: %s\n", TopChrono(1)); // durée
    update = true; // force l'affichage
    while (running) { // affiche le résultat et attend (q pour sortir)
      drawTour(V, n, P);  // dessine la tournée
      handleEvent(update); // attend un évènement ou pas
    }
  }
  */

  // programmation dynamique -> PAS FAIT
/*
  {
    TopChrono(1); // départ du chrono 1
    printf("value: %g\n", tsp_prog_dyn(V, n, P));
    printf("running time: %s\n", TopChrono(1)); // durée
    update = true; // force l'affichage
    while (running) { // affiche le résultat et attend (q pour sortir)
      drawTour(V, n, P);  // dessine la tournée
      handleEvent(update); // attend un évènement ou pas
    }
  }
*/

  // flip -> FAIT
/*
  {
    TopChrono(1);         // départ du chrono 1
    printf("value: %g\n", tsp_flip(V, n, P));
    printf("running time: %s\n", TopChrono(1)); // durée

    while (running) { // affiche le résultat et attend (q pour sortir)
      update = true; // force l'affichage
      if (first_flip(V, n, P)>0)
	      update = false;
      drawTour(V, n, P);  // dessine la tournée
      handleEvent(update); // attend un évènement ou pas
    }
  }
*/

  // greedy -> PAS FAIT
  /*
  {
    TopChrono(1);         // départ du chrono 1
    printf("value: %g\n", tsp_greedy(V, n, P));
    printf("running time: %s\n", TopChrono(1)); // durée
    while (running) { // affiche le résultat et attend (q pour sortir)
      update = true; // force l'affichage
      if (first_flip(V, n, P)>0)
	      update = false;
      drawTour(V, n, P);  // dessine la tournée
      handleEvent(update); // attend un évènement ou pas
    }
  }
  */
 
  // mst -> FAIT
  
  {
    TopChrono(1);         // départ du chrono 1
    graph T=createGraph(n); // graphe vide
    printf("value: %g\n", tsp_mst(V,n,P,T));
    printf("running time: %s\n", TopChrono(1)); // durée
    bool new_redraw = true;
    while(running){
      update = true;
      if (new_redraw) tsp_mst(V,n,P,T);
      if (first_flip(V,n,P)>0) update = false;
      drawGraph(V,n,P,T); // mettre P = NULL pour n'afficher que l'arbre
      new_redraw = handleEvent(update);
    }
    freeGraph(T);
  }

  // Libération de la mémoire
  TopChrono(-1);
  free(V);
  free(P);

  cleaning_SDL_OpenGL();
  return 0;
}
