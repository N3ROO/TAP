#include "tools.h"
#include "heap.h" // il faut aussi votre code pour heap.c


// Une fonction de type "heuristic" est une fonction h() qui renvoie
// une distance (double) entre une position de départ et une position
// de fin de la grille. La fonction pourrait aussi dépendre de la
// grille, mais on ne l'utilisera pas forcément ce paramètre.
typedef double (*heuristic)(position,position,grid*);


// Heuristique "nulle" pour Dijkstra.
double h0(position s, position t, grid *G){
  return 0.0;
}


// Heuristique "vol d'oiseau" pour A*.
double hvo(position s, position t, grid *G){
  return fmax(abs(t.x-s.x),abs(t.y-s.y));
}


// Heuristique "alpha x vol_d'oiseau" pour A*.
static double alpha=0.5; // 0 = h0, 1 = hvo, 2 = approximation ...
double halpha(position s, position t, grid *G) {
  return alpha*hvo(s,t,G);
}

// Structure "noeud" pour le tas min Q.
typedef struct node {
  position pos;        // position (.x,.y) d'un noeud u
  double cost;         // coût[u]
  double score;        // score[u] = coût[u] + h(u,end)
  struct node* parent; // parent[u] = pointeur vers le père, NULL pour start
} *node;

int compareNodes(const void *x, const void *y) {
  return ((node) x)->score - ((node) y)->score;
}

// Les arêtes, connectant les cases voisines de la grille (on
// considère le 8-voisinage), sont valuées par seulement certaines
// valeurs possibles. Le poids de l'arête u->v, noté w(u,v) dans le
// cours, entre deux cases u et v voisines est déterminé par la valeur
// de la case finale v. Plus précisément, si la case v de la grille
// contient la valeur C, le poids de u->v vaudra weight[C] dont les
// valeurs numériques exactes sont définies ci-après. La liste des
// valeurs possibles d'une case est donnée dans "tools.h": V_FREE,
// V_WALL, V_WATER, ... Remarquer que weight[V_WALL]<0, ce n'est pas
// une valuation correcte. Ce n'est pas choquant puisque si en
// position (i,j) si G.value[i][j] = V_WALL, alors c'est que le sommet
// à cette position n'existe pas.

double weight[]={
    1.0,  // V_FREE
  -99.0,  // V_WALL
    3.0,  // V_SAND
    9.0,  // V_WATER
    2.3,  // V_MUD
    1.5,  // V_GRASS
    0.1,  // V_TUNNEL
};


// Votre fonction A_star(G,h) doit construire un chemin dans la grille
// G entre la position G.start et G.end selon l'heuristique h(). S'il
// n'y a pas de chemin, affichez un simple message d'erreur. Sinon,
// vous devez remplir le champs .mark de la grille pour que le chemin
// trouvé soit affiché plus tard par drawGrid(G). La convention est
// qu'en retour G.mark[i][j] = M_PATH ssi (i,j) appartient au chemin
// trouvé (cf. "tools.h").
//
// Pour gérer l'ensemble P, servez-vous du champs G.mark[i][j] (=
// M_USED ssi (i,j) est dans P). Par défaut, ce champs est initialisé
// partout à M_NULL par initGrid().
//
// Pour gérer l'ensemble Q, vous devez utiliser un tas min de noeuds
// (type node) avec une fonction de comparaison qui dépend du champs
// .score des noeuds. Pensez que la taille du tas Q est au plus la
// somme des degrés des sommets dans la grille. Pour visualiser un
// noeud de coordonnées (i,j) qui passe dans le tas Q vous pourrez
// mettre G.mark[i][j] = M_FRONT au moment où vous l'ajoutez.

void A_star(grid G, heuristic h){

  // On initialise Q, qui contiendra les sommets à visiter
  heap Q = heap_create(G.X * G.Y * 8, compareNodes);

  // On ajoute le noeud de début à Q
  node start = malloc(sizeof(*start));
  start->parent = NULL;
  start->pos = G.start;
  start->cost = 0;
  start->score = start->cost + h(G.start, G.end, &G);
  heap_add(Q, start);
  
  // On marque ce sommet comme étant le sommet en train d'être visité
  G.mark[start->pos.x][start->pos.y] = M_FRONT;

  // Variable qui indique si un chemin a été trouvé
  bool pathFound = false;

  int exploredNodes = 0;

  while(!heap_empty(Q) && !pathFound && running)
  {
    // Choisir u appartient à Q tel que le coût de u est minimum, puis le supprimer de q
    node u = heap_pop(Q);

    // Si u = t alors renvoyer le chemin de s à t grâce à la relation parent
    if(u->pos.x == G.end.x && u->pos.y == G.end.y){
      // On marque le sommet courant comme faisant partie du chemin
      G.mark[u->pos.x][u->pos.y] = M_PATH;
      drawGrid(G);

      // On va parcourir tous les parents, et on va les marquer comme faisant
      // partie du chemin
      node parent = u->parent;
      while(parent != NULL){
        G.mark[parent->pos.x][parent->pos.y] = M_PATH;
        drawGrid(G);
        parent = parent->parent; 
      }

      // On pense bien à indiquer qu'un chemin a été trouvé pour terminer la boucle
      pathFound = true;
    }

    if(pathFound) continue;

    // Si u appartient à P, on continue la boucle, sinon on l’ajouter à P
    if(G.mark[u->pos.x][u->pos.y] != M_USED){
      // M_USED "modélise" l'appartenance à P. P étant l'ensemble des sommets visités
      G.mark[u->pos.x][u->pos.y] = M_USED;
      drawGrid(G);
    }
    // Pour tout voisin v de u tel que :
    // v n'appartient pas à P
    // v n'est pas un mur
    for(int i = u->pos.x - 1; i <= u->pos.x + 1; i ++){
      for(int j = u->pos.y - 1; j <= u->pos.y + 1; j ++){

        if(G.mark[i][j] == M_USED) continue; // test appartenance à P
        if(G.value[i][j] == V_WALL) continue; // test v est un mur

        // On calcule le cout : c'est le cout du noeud précèdent, plus le cout
        // du noeud courant

        double c = u->cost + weight[G.value[i][j]];

        // On peut créer le noeud v
        node v = malloc(sizeof(*v));
        v->parent = u;
        v->pos.x = i;
        v->pos.y = j;
        v->cost = c;
        v->score = v->cost + h(v->pos, G.end, &G);

        if( (i == u->pos.x - 1 || i == u->pos.x + 1) && (j == u->pos.y - 1 || j == u->pos.y + 1)){
          // C'est un coin, donc on va dire que la diagonale coute un peu plus cher
          v->score += 0.00001;
        }
        
        // on ajoute v à Q, et on le marque comme sommet en cours de visite
        if(G.mark[i][j] != M_FRONT)
        {
          heap_add(Q, v);
          G.mark[i][j] = M_FRONT;
          exploredNodes++;
        }
      }
    }
  }

  // Renvoyer l’erreur : " le chemin n’a pas été trouvé "
  if(!pathFound){
    printf("Aucun chemin trouvé\n");
  } else {
    printf("Chemin trouvé\n");
  }

  // Dans tous les cas on libère la mémoire
  heap_destroy(Q);

  ;;;
  // Pensez à dessiner la grille avec drawGrid(G) à chaque fois que
  // possible, par exemple, lorsque vous ajoutez un sommet à P mais
  // aussi lorsque vous reconstruisez le chemin à la fin de la
  // fonction. Lorsqu'un sommet passe dans Q vous pourrez le marquer
  // M_FRONT (dans son champs .mark) pour le distinguer des sommets de
  // P (couleur différente).
  ;;;
  // Après avoir extrait un noeud de Q, il ne faut pas le détruire,
  // sous peine de ne plus pouvoir reconstruire le chemin trouvé! Vous
  // pouvez réfléchir à une solution simple pour libérer tous les
  // noeuds devenus inutiles à la fin de la fonction. Une fonction
  // createNode() peut simplifier votre code.
  ;;;
  // Les bords de la grille sont toujours constitués de murs (V_WALL) ce
  // qui évite d'avoir à tester la validité des indices des positions
  // (sentinelle).
  ;;;
  ;;;
  // Améliorations quand vous aurez fini:
  //
  // (1) Une fois la cible atteinte, afficher son coût ainsi que le
  // nombre de sommets visités (somme des .mark != M_NULL). Cela
  // permettra de comparer facilement les différences d'heuristiques,
  // h0() vs. hvo().
  //
  // (2) Le chemin a tendance à zizaguer, c'est-à-dire à utiliser
  // aussi bien des arêtes horizontales que diagonales (qui ont le
  // même coût), même pour des chemins en ligne droite. Essayer de
  // rectifier ce problème d'esthétique en modifiant le calcul de
  // score[v] de sorte qu'à coût[v] égale les arêtes (u,v)
  // horizontales ou verticales soient favorisées.
  //
  // (3) Modifier votre implémentation du tas dans heap.c de façon à
  // utiliser un tableau de taille variable, en utilisant realloc() et
  // une stratégie "doublante": lorsqu'il n'y a pas plus assez de
  // place dans le tableau, on double sa taille. On peut imaginer que
  // l'ancien paramètre 'n' devienne non pas le nombre maximal
  // d'éléments, mais la taille initial du tableau (comme par exemple
  // n=4).
  //
  // (4) Gérer plus efficacement la mémoire en libérant les noeuds
  // devenus inutiles.
  //
  ;;;
}

void A_star2(grid G, heuristic h){
 // TODO
}

int main(int argc, char *argv[]){

  unsigned seed=time(NULL)%1000;
  printf("seed: %u\n",seed); // pour rejouer la même grille au cas où
  srandom(seed);


  // tester les différentes grilles et positions s->t ...

  //grid G = initGridPoints(80,60,V_FREE,1); // grille uniforme
  //position s={G.X/4,G.Y/2}, t={G.X/2,G.Y/4}; G.start=s; G.end=t; // s->t
  //grid G = initGridPoints(64,48,V_WALL, 0.2); // grille de points aléatoires
  grid G = initGridLaby(15, 15, 5); // labyrinthe aléatoire
  //grid G = initGridLaby(50, 50, 5); // labyrinthe aléatoire
  // position tmp; SWAP(G.start,G.end,tmp); // t->s (inverse source et cible)
  //grid G = initGridFile("mygrid.txt"); // grille à partir d'un fichier
 
  // pour ajouter à G des "régions" de différent types:

  // addRandomBlob(G, V_WALL,   (G.X+G.Y)/20);
  // addRandomBlob(G, V_SAND,   (G.X+G.Y)/15);
  // addRandomBlob(G, V_WATER,  (G.X+G.Y)/3);
  // addRandomBlob(G, V_MUD,    (G.X+G.Y)/3);
  // addRandomBlob(G, V_GRASS,  (G.X+G.Y)/15);
  // addRandomBlob(G, V_TUNNEL, (G.X+G.Y)/4);

  // constantes à initialiser avant init_SDL_OpenGL()
  scale = fmin((double)width/G.X,(double)height/G.Y); // zoom courant
  delay = 80; // délais pour l'affichage (voir tools.h)
  hover = false; // interdire déplacement de points
  init_SDL_OpenGL(); // à mettre avant le 1er "draw"
  drawGrid(G); // dessin de la grille avant l'algo
  update = false; // accélère les dessins répétitifs

  alpha=3;
  A_star(G, halpha); // heuristique: h0, hvo, alpha*hvo

  update = true; // force l'affichage de chaque dessin
  while (running) { // affiche le résultat et attend
    drawGrid(G); // dessine la grille
    handleEvent(true); // attend un évènement
  }

  freeGrid(G);
  cleaning_SDL_OpenGL();
  return 0;
}
