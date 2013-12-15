/**
 * Implementation of an exhaustive search algorithm to find
 * the optimal tour for the Traveling Salesman problem.
 * Uses the Steinhaus Johnson Trotter algorithm to generate
 * permutations. Tours that only differ by a different starting
 * point or direction are omitted: for N cities there will be
 * (N-1)! / 2 permutations checked.
 * 
 * Usage: CheapestTour <number of cities> <input file>  
 * 
 * Author: Bianco Zandbergen
 */
#include <stdio.h>
#include <stdlib.h>

/* direction of integer for Steinhaus Johnson Trotter algorithm */
#define LEFT     -1
#define RIGHT    1

#define INFINITE 0

/* The optimal solution is returned using this struct */
struct result {
  int cost;
  int *tour;
  int tour_size;
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
 * calculate the factorial of a given number
 */ 
int factorial(int n)
{
  int result, i;
  result = 1;
  
  for (i=2; i<=n; i++) {
    result = result * i;
  }
  
  return result;  
}

/**
 * Returns the optimal tour given the number of cities
 * and a matrix of distances between cities
 */  
struct result getOptimalTour(int nr_cities, int **cities)
{
  int *p; /* array containing permutations of 0 ... N-1 integers  */
  int *d; /* array containing the direction of the integers */
  int cost, next_city;
  int largestMobile;
  int largestMobileIndex;
  int temp;
  int i, j;
  int nr_perm;
  struct result res;
  
  res.cost = INFINITE;
  res.tour_size = nr_cities + 1;
  res.tour = malloc(res.tour_size * sizeof(int));
  
  /** 
   * we generate (n-1)! / 2 perms
   * index 0 of p[] is constant and our start city
   */
  nr_perm = factorial(nr_cities-1) / 2; 
  
  p = malloc(nr_cities * sizeof(int));
  d = malloc(nr_cities * sizeof(int));
  
  /* all integers start with direct LEFT */
  for (i=0; i<nr_cities; i++) {
    p[i] = i;
    d[i] = LEFT;
  }

  for (i=0; i<nr_perm; i++) {
    /* calculate cost of current tour */
    cost = 0;
    for(j=0; j<nr_cities; j++) {
      next_city = (j < nr_cities-1) ? j+1 : 0;
      cost += cities[p[j]][p[next_city]];      
    }
    
    /* new shortest tour? */
    if (cost < res.cost || res.cost == INFINITE) {
      res.cost = cost;      
      for (j=0; j<nr_cities; j++) {
        res.tour[j] = p[j];
      }
    }
    
    /* find largest mobile integer for next permutation */
    largestMobile = 0;
    for (j=1; j<nr_cities; j++) {
      if (!( j == 1 && d[j] == LEFT) && !(j == nr_cities-1 && d[j] == RIGHT))
      {
        if (p[j] > p[j+d[j]] && p[j] > largestMobile) {
          largestMobile = p[j];
          largestMobileIndex = j;
        }
      }
    }
    
    /* swap the largest mobile integer with the adjacent one in its direction */
    temp = p[largestMobileIndex];
    p[largestMobileIndex] = p[largestMobileIndex + d[largestMobileIndex]];
    p[largestMobileIndex + d[largestMobileIndex]] = temp;
      
    /* perform the same operation on the direction array to keep it synced */
    temp = d[largestMobileIndex + d[largestMobileIndex]];
    d[largestMobileIndex + d[largestMobileIndex]] = d[largestMobileIndex];
    d[largestMobileIndex] = temp;
      
    /* swap the direction of all integers larger than the current largest mobile */
    for (j=1; j<nr_cities; j++) {
      if(p[j] > largestMobile) {
        d[j] = d[j] == LEFT ? RIGHT : LEFT;
      }
    }  
  }
  
  /* connect the last city to the starting point for our final result */
  res.tour[res.tour_size-1] = res.tour[0];
  
  return res;
}

int main(int argc, char *argv[])
{
  int **cities; // adjacency matrix of cities
  struct result optimalTour;
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
  
  /* calculate optimal tour */
  optimalTour = getOptimalTour(nr_cities, cities);
  
  /* print optimal tour */
  printf("optimal tour: ");
  
  for (i=0; i<optimalTour.tour_size; i++) {
    printf("%d ", optimalTour.tour[i]);
  }
  
  printf("\ntour cost:    %d\n", optimalTour.cost);
  
  return 0;
}
