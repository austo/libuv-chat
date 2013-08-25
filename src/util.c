#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "util.h"


sched_t
simple_random(sched_t n) {
  unsigned rnd, limit = RAND_MAX - RAND_MAX % n;
  
  do {
    rnd = random();
  } while (rnd >= limit);
  return rnd % n;
}


void
init_shuffle(sched_t *array, size_t n) {
  sched_t i, r;

  array[0] = 0;

  for (i = 1; i < n; ++i) {
    r = simple_random(i);      
    array[i] = array[r]; 
    array[r] = i;
  }
}


void
shuffle(sched_t *array, size_t n) {
  sched_t i, r, t;

  for (i = n - 1; i > 0; --i) {
    r = simple_random(i + 1);
    t = array[r];
    array[r] = array[i];
    array[i] = t;
  }
}

// Use FY shuffle to fill zero-initialized arrays one step at a time
void
fill_disjoint_arrays(
  sched_t **schedules, size_t n_sched, size_t sched_len) {

  srandom(time(NULL));

  size_t i, j;

  sched_t *arr = malloc(sizeof(sched_t) * n_sched);
  init_shuffle(arr, n_sched);

  for (i = 0; i < sched_len; ++i) {
    for (j = 0; j < n_sched; ++j) {
      schedules[j][i] = arr[j];
    }
    shuffle(arr, n_sched);
  }
  free(arr);
}
