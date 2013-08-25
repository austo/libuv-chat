#ifndef UTIL_H
#define UTIL_H


typedef unsigned sched_t;

sched_t
simple_random(sched_t n);

void
init_shuffle(sched_t *array, size_t n);

void
shuffle(sched_t *array, size_t n);

void
fill_disjoint_arrays(sched_t **schedules, size_t n_sched, size_t sched_len);

#endif