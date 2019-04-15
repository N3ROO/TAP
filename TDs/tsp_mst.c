//
// TSP - APPROXIMATION MST
//
// -> compléter uniquement tsp_mst() en fin de fichier
// -> la structure "graph" est définie dans tools.h

// Crée un graphe à n sommets et sans arêtes. Les listes (potentielles
// de voisins) sont de tailles n, mais attention les degrés ne sont
// pas initialisés ! Il faudra le faire impérativement dans tsp_mst().
graph createGraph(int n) {
  graph G;
  G.n = n;
  G.deg = malloc(n * sizeof(int *));
  G.list = malloc(n * sizeof(int *));
  for (int u = 0; u < n; u++)
    G.list[u] = malloc(n * sizeof(int)); // taille n par défaut
  return G;
}

// Libère un graphe G et ses listes.
void freeGraph(graph G) {
  for (int u = 0; u < G.n; u++)
    free(G.list[u]);
  free(G.list);
  free(G.deg);
}

// Ajoute l'arête u-v au graphe G de manière symétrique. Les degrés de
// u et v doivent être à jour et les listes suffisamment grandes.
void addEdge(graph G, int u, int v) {
  G.list[u][G.deg[u]++] = v;
  G.list[v][G.deg[v]++] = u;
}

// Une arête u-v avec un poids.
typedef struct {
  int u, v;      // extrémités de l'arête u-v
  double weight; // poids de l'arête u-v
} edge;

// Comparaison du poids de deux arêtes pour utiliser qsort(), par
// ordre décroissant. Ne pas hésiter à utiliser le "man" pour qsort()
// qui est une fonction de la libraire standard du C.
int compEdge(const void *e1, const void *e2) {
  double const x = ((edge *)e1)->weight;
  double const y = ((edge *)e2)->weight;
  return (x > y) - (x < y); // -1,0,+1 suivant que x>y, x=y, ou x<y
}

// Fusionne les arbres de racines x et y selon l'heuristique basée sur
// la hauteur.
void Union(int x, int y, int *parent, int *height) {
  if (height[x] > height[y])
    parent[y] = x;
  else {
    parent[x] = y;
    if (height[x] == height[y])
      height[y]++;
  }
}

// Renvoie la racine de l'arbre de x selon l'heuristique de la
// compression de chemin.
int Find(int x, int *parent) {
  if (x != parent[x])
    parent[x] = Find(parent[x], parent);
  return parent[x];
}

// Calcule dans le tableau Q l'ordre de première visite des sommets du
// graphe G selon un parcours en profondeur d'abord à partir du sommet
// u. Le paramètre p est le sommet parent de u. On doit avoir p<0 si u
// est l'origine du parcours (premier appel).
void dfs(graph G, int u, int *Q, int p) {
  static int t; // t = temps ou indice (variable globale) du tableau Q
  if (p < 0)
    t = 0;
  Q[t++] = u;
  for (int i = 0; i < G.deg[u]; i++)
    if (G.list[u][i] != p)
      dfs(G, G.list[u][i], Q, u);
}

double tsp_mst(point *V, int n, int *Q, graph T) {
  // Cette fonction à compléter doit calculer trois choses (=les
  // sorties) à partir de V et n (=les entrées):
  //
  // 1. le graphe T, soit l'arbre couvrant V de poids minimum;
  // 2. la tournée Q, soit l'ordre de visite selon le DFS de T;
  // 3. la valeur de la tournée Q.
  //
  // NB: Q et T doivent être créés et libérés par l'appelant. Il est
  // important de remettre à zéro le degré de tous les sommets de T
  // avant de le remplir.
  //
  // Algorithme:
  // 1. remplir puis trier le tableau d'arêtes avec qsort()
  // 2. répéter pour chaque arête u-v, mais pas plus de n-1 fois:
  //    si u-v ne forme pas un cycle dans T (<=> u,v dans des composantes
  //    différentes) alors ajouter u-v au graphe T
  // 3. calculer dans Q le DFS de T

  // E = tableau de toutes les arêtes définies à partir des n points de V

  int m = n * (n-1) / 2;

  edge *E = malloc(m * sizeof(edge));

  int eindex = 0;
  for(int i = 0; i < n; i ++)
  {
    for(int j = i + 1; j < n; j++){

      // Création de l'arête
      edge e;
      e.u = i;
      e.v = j;
      e.weight = dist(V[e.u], V[e.v]);
      
      // On ajoute l'arête au tableau
      E[eindex] = e;
      eindex ++;
    }
  }

  qsort(E, m, sizeof(edge), compEdge);

  // initialisation pour Union-and-Find
  int *parent = malloc(n * sizeof(int)); // parent[x]=parent de x (=x si racine)
  int *height = malloc(n * sizeof(int)); // height[x]=hauteur de l'arbre de racine x
  for (int x = 0; x < n; x++) {
    parent[x] = x; // chacun est racine de son propre arbre
    height[x] = 0; // hauteur nulle
  }

  // pensez à initialiser à zéro tous les degrés des sommets pour que
  // addEdge() fonctionne même après plusieurs appels à tsp_mst()

  for(int i = 0; i < T.n; i ++)
  {
    T.deg[i] = 0;  
  }

  int i = 0;
  int ecounter = 0;

  while(ecounter < n - 1)
  {
    edge e = E[i];

    int rootu = Find(e.u, parent);
    int rootv = Find(e.v, parent);

    if(rootu != rootv)
    {
      Union(rootu, rootv, parent, height);
      addEdge(T, e.u, e.v);
      ecounter ++;
    }
    
    i++;
  }

  // printf("pas segfault\n");

  // libère les tableaux
  free(parent);
  free(height);
  free(E);

  dfs(T, 0, Q, -1);      // calcule Q grâce au DFS à partir du sommet 0 de T
  return value(V, n, Q); // renvoie la valeur de la tournée
}
