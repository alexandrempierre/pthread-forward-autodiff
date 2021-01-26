/*
 * The only code modified here should be the one inside function f
 * The return of function f should not be modified
 */
#include "function.h"

/* The codomain dimension must be a function of domain dimension (even if it 
 is a constant function that don't use the domain dimension for anything) */
int calculate_codomain_dimension (int domain_dim) {
    return domain_dim*domain_dim;
}

/* This could be any function but with the same prototype and the same arguments
 in the same order */
double complex *f (int domain_dim, int codomain_dim, 
                    double complex *input_values, 
                    double complex *output_values) {
    int i, j, k = 0;
    double complex x1, x2, s, c;

    for (i = 0; i < domain_dim; i++) {
        x1 = input_values[i];
        for (j = 0; j < domain_dim; j++) {
            x2 = input_values[j];
            s = csin(x1*x2);
            c = ccos(x1*x2);
            output_values[k++] = s*s + c*c;
        }
    }
    
    return output_values;
}