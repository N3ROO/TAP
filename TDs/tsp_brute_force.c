//
//  TSP - BRUTE-FORCE
//
// -> la structure "point" est définie dans tools.h

double dist(point A, point B) {
  return sqrt((A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y));
}

double value(point *V, int n, int *P) {
  double sum = 0;
  for(int i =0; i < n-1; i ++){
    sum += dist(V[P[i]], V[P[i+1]]);
  }
  return sum + dist(V[P[n-1]], V[P[0]]);
}

double tsp_brute_force(point *V, int n, int *Q) {
  int P[n];
  for(int i = 0; i < n; i++){
    P[i] = i;
  }
  
  double min = value(V, n, P);
  while(NextPermutation(P, n)){
    if(min > value(V, n, P)){
      min = value(V, n, P);
      for(int i = 0; i < n; i ++){
        Q[i] = P[i];
      }
    }
  }
  return min;
}

void MaxPermutation(int *P, int n, int k) {
  // faut trier la partie de P en déscendant dans l'intervalle [k, n]
  return;
}

double value_opt(point *V, int n, int *P, double w) {
  double sum = 0;
  int i = 0;
  while(sum += dist(V[P[i]], V[P[0]]) < w && i < n-1){
    sum += dist(V[P[i]], V[P[i+1]]);
    i ++;
  }

  sum += dist(V[P[i]], V[P[0]]);
  if(sum < w){
    return sum;
  }
  return -(i +1);
}

double tsp_brute_force_opt(point *V, int n, int *Q) {
  int P[n];
  for(int i = 0; i < n; i++){
    P[i] = i;
  }
  
  double min = value(V, n, P);
  while(NextPermutation(P, n)){
    if(value_opt(V, n, P, min) > 0){
      min = value(V, n, P);
      for(int i = 0; i < n; i ++){
        Q[i] = P[i];
      }
    }
  }
  return min;
}
