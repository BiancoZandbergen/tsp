/**
 * Implementation of the Nearest Neighbour algorithm to find
 * a cheap tour for the Traveling Salesman problem.
 *  
 * Usage: HeuristicTour <number of cities> <input file>  
 * 
 * Author: Bianco Zandbergen
 */
#include <stdio.h>
#include <stdlib.h>

#define TRUE        1
#define FALSE       0
#define INFINITE    0
#define UNVISITED   0
#define VISITED     1
#define UNCONNECTED 0
#define CONNECTED   1

/* The solution is returned using this struct */
struct result {
  int cost;
  int *tour;
  int tour_size;
};

/* structure for single linked vertex lists */
struct vertex {
  int city;
  struct vertex *next;
};

/* structure used to save edge info */
struct edge {
  int city1;
  int city2;
  int weight;
};

/**
 * load the distances between cities from file
 * into a dynamic allocated 2D array. Return a pointer to this array.
 */
int ** loadCitiesFromFile(int nr_cities, char *file)
{
  int **cities; /* pointer to 2D array */
  int val;
  int i, j;
  FILE *fp;
  
  fp  = fopen(file, "r");
  
  if (fp == NULL)
    return NULL;
  
  /* allocate memory for first dimension (array of pointers to int arrays) */
  cities = malloc(nr_cities * sizeof(int *));
  
  /* allocate memory for second dimension (int arrays) */
  for (i=0; i<nr_cities; i++) {
    cities[i] = malloc(nr_cities * sizeof(int));
  }  
  
  /* fill 2D array with distances between cities from file */
  i = j = 0;
  
  while (!feof(fp)) {
    if (fscanf(fp, "%d", &val)) {
       if (j == nr_cities) {
         j = 0;
         i++;
         
         if (i >= nr_cities)
           break;
       }
       
       cities[i][j] = val;
       j++;
    }
  }
  
  fclose(fp);
  
  return cities; /* pointer to 2D array */
}

/**
 * Perform a recursive depth-first-search on a graph
 * For a given node it will check if there are adjacent
 * unvisited nodes and calls itself with the adjacent node
 * as argument. Saves the trail in a list.
 */    
void dfs(int node,       // given node
         int nr_cities,  // total number of cities
         int **g,        // adjacency matrix (2D array)
         int *visited,   // array indicating for each city whether visited
         struct vertex **head, // pointer-pointer to head of trail list
         struct vertex **tail) // pointer-pointer to tail of trail list
{
  int i;
  struct vertex *v_new;
  
  // the given node is now visited
  visited[node] = VISITED;
  
  /* save forward trail to list */
  v_new = malloc(sizeof(struct vertex));
  v_new->next = NULL;
  v_new->city = node;
  
  if (*head == NULL) {
    *head = *tail = v_new;
  } else {
    (*tail)->next = v_new;
    *tail = v_new;
  }
  
  /* check for all cities */
  for (i=0; i<nr_cities; i++) {
    
    /* if adjacent city and unvisited */
    if (g[node][i] == CONNECTED && visited[i] == UNVISITED) {
      
      /* call self recursively with the adjacent city as start node */
      dfs(i, nr_cities, g, visited, head, tail);
      
      /* save backward trail to list */
      v_new = malloc(sizeof(struct vertex));
      v_new->next = NULL;
      v_new->city = node;
      
      if (*head == NULL) {
        *head = *tail = v_new;
      } else {
        (*tail)->next = v_new;
        *tail = v_new;
      }
    }
  } // end for i
}

/**
 * Returns a as cheap as possible tour  using the Nearest
 * Neighbour algorithm, given the number of cities and
 * an adjacency matrix of distances between cities
 */
struct result getHeuristicTour(int nr_cities, int **cities)
{
  struct vertex *v_head = NULL; // head of visited vertex list for Prim's algo
  struct vertex *v_tail = NULL; // tail of visited vertex list for Prim's algo
  struct vertex *p_head = NULL; // head of path list for traversing MST
  struct vertex *p_tail = NULL; // tail of path list for traversing MST
  struct vertex *v; // for temporary assignments
  int *visited;     // array indicating for each city if it has been visited
  int **mst; // 2D array containing adjacency matrix with Minimal Spanning Tree  
  struct result res; // we return the result in this struct
  int i, j;
  
  /* allocate memory for first dimension (array of pointers to int arrays) */
  mst = malloc(nr_cities * sizeof(int *));
  
  /* allocate memory for second dimension (int arrays) */
  for (i=0; i<nr_cities; i++) {
    mst[i] = malloc(nr_cities * sizeof(int));
  }
  
  /* allocate memory for visited array */
  visited = malloc(nr_cities * sizeof(int));
  
  /* init visited and MST arrays */
  for (i=0; i<nr_cities; i++) {
    visited[i] = UNVISITED;
    for (j=0; j<nr_cities; j++) {
      mst[i][j] = UNVISITED;
    }
  }
  
  res.cost = INFINITE;
  res.tour_size = nr_cities + 1;
  res.tour = malloc(res.tour_size * sizeof(int));
  
  /* Execute Prim's algorithm to find Minimal Spanning Tree
     Start with adding our first vertex (0) to the list of
     visited vertices */
  v_head = malloc(sizeof(struct vertex));
  v_tail = v_head;
  v_head->city = 0;
  v_head->next = NULL;
  
  /* for all remaining vertices */
  for (i=1; i<nr_cities; i++) {
    struct vertex *vp1, *vp2;
    int found;
    struct edge e;
    struct vertex *v_new = malloc(sizeof(struct vertex));
    
    e.weight = INFINITE;

    /* for all vertices in v */
    for (vp1=v_head; vp1!=NULL; vp1=vp1->next) {
      
      /* for all distances between vp1 and other vertices */
      for (j=0; j<nr_cities; j++) {

        if (cities[vp1->city][j] == INFINITE)
          continue; // not connected or self
        
        /* check if the other vertex is already visited */
        for (found=FALSE, vp2=v_head; vp2!=NULL; vp2=vp2->next) {
          if (vp2->city == j)
            found = TRUE;
        }

        if (found == FALSE && 
           (e.weight == INFINITE || 
            cities[vp1->city][j] < e.weight)) {
          /* found a new cheapest next vertex
             update vertex and edge structures */
          v_new->city = j;
          e.city1 = vp1->city;
          e.city2 = j;
          e.weight = cities[vp1->city][j];
        }
      } // end for j
    } // end for vp1
    
    // add vertex to visited vertices list
    v_new->next = NULL;
    v_tail->next = v_new;
    v_tail = v_new;
        
    // add new connection in Minimal Spanning Tree matrix
    mst[e.city1][e.city2] = CONNECTED;
    mst[e.city2][e.city1] = CONNECTED;
  } // end for i
  
  /* perform recursive depth-first-search on the Minimal Spanning Tree
     starting from city 0. The trail is saved in a list */
  dfs(0, nr_cities, mst, visited, &p_head, &p_tail);
  
  // clear visited array for re-use
  for (i=0; i<nr_cities; i++) {
    visited[i] = UNVISITED;
  }
  
  v = p_head;
  i = 0;
  
  /* create a path that does not visit a city twice by taking short-cuts
     using triangle inequality. We achieve this by traversing the path list
     and saving every unique node (skip duplicates) except the last one.
     Meanwhile we also calculate the costs of the tour. */
  while (v != NULL) {
    if (visited[v->city] == UNVISITED || v->next == NULL) {
      visited[v->city] = VISITED;
      res.tour[i] = v->city;
      
      if (i > 0) {
        res.cost += cities[res.tour[i-1]][res.tour[i]];
      }
      
      i++; 
    }
    v = v->next;
  }
  
  return res;
}

int main(int argc, char *argv[])
{
  int **cities; // adjacency matrix of cities
  struct result heuristicTour;
  int i, nr_cities;
  
  if (argc != 3) {
    printf("Usage: %s <number of cities> <input file>\n", argv[0]);
    return 1;
  }
  
  nr_cities = atoi(argv[1]);
  
  cities = loadCitiesFromFile(nr_cities,  argv[2]);
  
  if (cities == NULL) {
    printf("Error loading input file\n");
    return 1;
  }

  /* calculate heuristic tour */
  heuristicTour = getHeuristicTour(nr_cities, cities);

  /* print heuristic tour */
  printf("heuristic tour: ");
  
  for (i=0; i<heuristicTour.tour_size; i++) {
    printf("%d ", heuristicTour.tour[i]);
  }
  
  printf("\ntour cost:    %d\n", heuristicTour.cost);
  
  return 0;
}
