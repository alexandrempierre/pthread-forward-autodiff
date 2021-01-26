/*
 * This file should not be modified
 */

#ifndef DIFFERENTIABLE_FUNCTION_H
#define DIFFERENTIABLE_FUNCTION_H

#include <complex.h>
#include <math.h>

/* The codomain dimension must be a function of domain dimension (even if it 
 is a constant function that don't use the domain dimension for anything) */
int calculate_codomain_dimension (int domain_dim);

/* This could be any function but with the same prototype and the same arguments
 in the same order */
double complex *f (int domain_dim, int codomain_dim, 
                    double complex *input_values, 
                    double complex *output_values);


#endif