#ifndef _RELATEDNESS_H_
#define _RELATEDNESS_H_	

#include <stdio.h>
#include <cmath>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

#include "interface.h" 
#include "map-file.h"
#include "genotype.h"
#include "genotype_pair.h"
#include "relatedness_data.h"

#ifndef NOOMP
#include <omp.h>
#endif

#ifndef NOGSL
#include <gsl/gsl_multimin.h>
#endif

void inc_f(Relatedness &, const Genotype_pair &, const size_t &);
void inc_theta(Relatedness &, const Genotype_pair &, const size_t &);
void inc_gamma(Relatedness &, const Genotype_pair &, const size_t &);
void inc_Delta(Relatedness &, const Genotype_pair &, const size_t &);

/** \ingroup COMMANDS
  * @{*/
/// Estiamtes the relatedness of individuals with known genotypic probabilites.
int estimateRel(int argc, char *argv[]);
/** @}*/
#endif 
