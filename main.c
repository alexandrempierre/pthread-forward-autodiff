/*
 * This file should not be modified
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "function.h"

/* Some arguments will only be necessary to do the load balancing between 
 threads, I called that the thread_args */
typedef struct thread_args {
    int thread_index;
    int threads_total;
} t_thread_args;

/* Other arguments may be necessary to calculate the derivative, those are 
 problem_args */
typedef struct problem_args {
    double complex *input_values;
    double complex *output_values;
    double **jacobian_matrix;

    int domain_dim;
    int codomain_dim;
    double eps;
} t_problem_args;

/* The thread task function only gets one argument so I created a structure to
 put it all together */
typedef struct args {
    t_thread_args *thread_info;
    t_problem_args *problem_info;
} t_args;

/* Function prototypes */
double **allocate_matrix (int rows, int columns);
void free_matrix (int rows, double **matrix);
void print_matrix (int rows, int columns, double **matrix);

void *jacobian_columns (void *arg); // thread task function

int main(int argc, char const *argv[]) {
    int i;
    
    /* Thread-related variables */
    pthread_t *thread_id;
    int threads_total, thread_index;

    /* Problem-related variables */
    int domain_dim, codomain_dim;
    double complex *input_values, *evaluated_f;
    double **jacobian_matrix;
    double eps, input_aux;

    /* Thread task function argument */
    t_thread_args *thread_info;
    t_problem_args *problem_info;
    t_args *args;

    if (argc < 3) {
        fprintf(stderr, "ERROR -- not enough arguments, try: %s <threads number> <dimension of the domain>\n", argv[0]);
        return 1;
    }
    
    /* Allocationg and getting values for some variables */
    sscanf(argv[1], "%d", &threads_total);
    if ( threads_total <= 0 ) {
        fprintf(stderr, "ERROR -- the number of threads should be greater than zero\n");
        return 2;
    }
    
    sscanf(argv[2], "%d", &domain_dim);
    if ( domain_dim <= 0 ) {
        fprintf(stderr, "ERROR -- domain dimension should be greater than zero\n");
        return 2;
    }
    codomain_dim = calculate_codomain_dimension(domain_dim);

    thread_id = (pthread_t *) malloc( threads_total * sizeof(pthread_t) );
    if ( thread_id == NULL ) {
        fprintf(stderr, "ERROR -- malloc -- thread_id\n");
        return 3;
    }

    input_values = (double complex *) malloc( domain_dim * sizeof(double complex) );
    if ( input_values == NULL ) {
        fprintf(stderr, "ERROR -- malloc -- input_values\n");
        return 3;
    }

    evaluated_f = (double complex *) malloc( codomain_dim * sizeof(double complex) );
    if ( evaluated_f == NULL ) {
        fprintf(stderr, "ERROR -- malloc -- evaluated_f\n");
        return 3;
    }

    /* Reading the coordinates of the point where we want to evaluate our
     function and its derivative (the Jacobian matrix) */
    eps = 1.0;
    for (i = 0; i < domain_dim; i++) {
        scanf( "%lf", &input_aux );
        input_values[i] = input_aux;
        
        /* We're using complex numbers but we need a little tweak in its 
         arithmetic so that i^2 = 0. So we use an eps little enough so as  
         (eps*I)^2 is cancelled by floating arithmetic if added to any of 
         the input coordinates */
        while ( (input_aux + eps*eps) - input_aux ) { eps /= 2.0; }
    }
    
    evaluated_f = f(domain_dim, codomain_dim, input_values, evaluated_f);

    jacobian_matrix = allocate_matrix(codomain_dim, domain_dim);
    
    /* Thread creation and start loop */
    for (thread_index = 0; thread_index < threads_total; thread_index++) {
        /* Allocation of argument variables */
        thread_info = (t_thread_args *) malloc( sizeof(t_thread_args) );
        if ( thread_info == NULL ) {
            fprintf(stderr, "ERROR -- malloc -- thread_info\n");
            return 3;
        }
        
        problem_info = (t_problem_args *) malloc( sizeof(t_problem_args) );
        if ( problem_info == NULL ) {
            fprintf(stderr, "ERROR -- malloc -- problem_info\n");
            return 3;
        }
        
        args = (t_args *) malloc( sizeof(t_args) );
        if ( args == NULL ) {
            fprintf(stderr, "ERROR -- malloc -- args\n");
            return 3;
        }

        /* Passing some values to the arguments */
        thread_info->thread_index = thread_index;
        thread_info->threads_total = threads_total;

        problem_info->domain_dim = domain_dim;
        problem_info->codomain_dim = codomain_dim;
        // passing a reference to the Jacobian Matrix to each thread
        problem_info->jacobian_matrix = jacobian_matrix;
        problem_info->eps = eps;
        
        /* Each thread will need to make little changes to the input array 
         to do its task, so I allocated space to copy the input coordinates 
         into each thread */
        problem_info->input_values = (double complex *) malloc( domain_dim * sizeof(double complex) );
        if ( problem_info->input_values == NULL ) {
            fprintf(stderr, "ERROR -- malloc -- problem_info->input_values\n");
            return 3;
        }

        /* Since each thread will be using slightly different input values it's 
         necessary that each have its own space to store intermediate results */
        problem_info->output_values = (double complex *) malloc( codomain_dim * sizeof(double complex) );
        if ( problem_info->output_values == NULL ) {
            fprintf(stderr, "ERROR -- malloc -- problem_info->output_values\n");
            return 3;
        }

        /* Copying the input values to the thread arguments */
        memcpy(problem_info->input_values, input_values, 
            domain_dim * sizeof(double complex));
        
        /* Building the single pointer that I can pass to the thread task */
        args->problem_info = problem_info;
        args->thread_info = thread_info;

        /* Thread creation itself */
        if ( pthread_create(&thread_id[thread_index], NULL, 
            jacobian_columns, (void *) args) ) {
            fprintf(stderr, "ERROR -- pthread_create\n");
            return 4;
        }
    }

    /* Wainting for the end of all threads */
    for (thread_index = 0; thread_index < threads_total; thread_index++) {
        if ( pthread_join(thread_id[thread_index], NULL) ) {
            fprintf(stderr, "ERROR -- pthread_join\n");
            return 5;
        }
    }

    /* Showing the results */
    printf("f("); // the function with its arguments and its value
    for (i = 0; i < domain_dim; i++) {
        printf("%e%s", creal(input_values[i]), i == domain_dim-1 ? "" : ", ");
    }
    printf(") = \n");
    printf("[");
    for (i = 0; i < codomain_dim; i++) {
        printf("%e%s", creal(evaluated_f[i]), i == codomain_dim-1 ? "" : ", ");
    }
    printf("]\n\n");

    printf("(Jf)("); // the Jacobian in the same point with its entries
    for (i = 0; i < domain_dim; i++) {
        printf("%e%s", creal(input_values[i]), i == domain_dim-1 ? "" : ", ");
    }
    printf(") = \n");
    print_matrix(codomain_dim, domain_dim, jacobian_matrix);
    
    /* Deallocating the used memory */     
    free_matrix(codomain_dim, jacobian_matrix);
    free(thread_id);
    free(input_values);
    free(evaluated_f);

    return 0;
}

void *jacobian_columns (void *arg) {
    int d_in, d_out; // directions
    t_args *args = (t_args *) arg;

    /* Getting shorter names for everything passed through the void pointer arg */
    int domain_dim = args->problem_info->domain_dim,
        codomain_dim = args->problem_info->codomain_dim,
        thread_index = args->thread_info->thread_index,
        threads_total = args->thread_info->threads_total;
    
    double eps = args->problem_info->eps;
    
    double complex *input_values = args->problem_info->input_values,
                *output_values = args->problem_info->output_values;
    
    double **jacobian_matrix = args->problem_info->jacobian_matrix;
    /* End of shorter names section */

    /* Calculating directional derivatives on the given point and storing 
     the results in the Jacobian matrix */
    for (d_in = thread_index; d_in < domain_dim; d_in += threads_total) {
        input_values[d_in] += eps*I;
        
        output_values = f(domain_dim, codomain_dim, input_values, output_values);
        
        input_values[d_in] = creal(input_values[d_in]);

        for (d_out = 0; d_out < codomain_dim; d_out++) {
            jacobian_matrix[d_out][d_in] = cimag(output_values[d_out]) / eps;
        }
    }

    /* Deallocating memory used by the thread task arguments */
    free(input_values);
    free(output_values);
    free(args->problem_info);
    free(args->thread_info);
    free(args);

    // Every result is already anywhere else because Jacobian matrix is a reference
    pthread_exit(NULL);
}

void print_matrix (int rows, int columns, double **matrix) {
    int i, j;

    printf("[\n");
    for (i = 0; i < rows; i++) {
        printf("[");
        for (j = 0; j < columns; j++) {
            printf("%e%s", matrix[i][j], j == columns-1 ? "" : ", ");
        }
        printf("]%s\n", i == rows-1 ? "" : ", ");
    }
    printf("]\n");
}

double **allocate_matrix (int rows, int columns) {
    int i;

    double **matrix = (double **) malloc( rows * sizeof(double *) );
    if ( matrix == NULL ) {
        fprintf(stderr, "ERROR -- malloc -- some matrix rows\n");
        exit(3);
    }

    for (i = 0; i < rows; i++) {
        matrix[i] = (double *) malloc( columns * sizeof(double) );

        if ( matrix[i] == NULL ) {
            fprintf(stderr, "ERROR -- malloc -- some matrix entries of row %d\n", i);
            exit(3);
        }
    }

    return matrix;
}

void free_matrix (int rows, double **matrix) {
    int i;

    for (i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    
    free(matrix);
}