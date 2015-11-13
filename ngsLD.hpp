#pragma once

#include <gsl/gsl_statistics.h>
#include "read_data.hpp"
#include "threadpool.h"

extern bool SIG_COND;
extern char const* version;

// Struct to store all input arguments
typedef struct {
  char *in_geno;
  bool in_bin;
  bool in_probs;
  bool in_logscale;
  uint64_t n_ind;
  uint64_t n_sites;
  char *pos;
  uint64_t max_dist;
  double min_maf;
  bool call_geno;
  double N_thresh;
  double call_thresh;
  double rnd_sample;
  uint64_t seed;
  char *out;
  FILE *out_fh;
  uint n_threads;
  bool version;
  uint verbose;

  double ***geno_lkl;     // n_ind * n_sites+1 * N_GENO
  double *maf;            // n_sites+1
  char **labels;          // n_sites+1
  double *pos_dist;       // n_sites+1

  double** expected_geno; // n_ind * n_sites+1

  threadpool_t *thread_pool;
} params;

// Pthread structure
typedef struct {
  params *pars;
  uint64_t site;
  gsl_rng* rnd_gen;
} pth_struct;

// parse_args.cpp
void init_pars(params* );
void parse_cmd_args(params*, int, char**);
void init_output(params* );

void calc_pair_LD (void*);
double pearson_r (double*, double*, uint64_t);
void bcf_pair_LD (double*, double**, double**, double, double, uint64_t);
int pair_freq_iter(int, double**, double**, double*);
