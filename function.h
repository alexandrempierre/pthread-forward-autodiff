#ifndef DIFFERENTIABLE_FUNCTION_H
#define DIFFERENTIABLE_FUNCTION_H

#include <complex.h>
#include <math.h>

int calculate_codomain_dimension (int domain_dim);

double complex *f (int domain_dim, int codomain_dim, 
                    double complex *input_values, 
                    double complex *output_values);


#endif