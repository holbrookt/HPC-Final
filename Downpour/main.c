#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "data.h"

#define N_FETCH 1
#define N_PUSH 1

double learn_rate;

double estimate(double *coef, double *data, int n_terms) {
    double sum = coef[0];
    int i;
    for (i = 0; i < n_terms; ++i) {
        sum += coef[i+1] * data[i];
    }
    return 1 / (1 + exp(est));
}

void SGD(double **training, double *coef, int n_terms, int n_rounds) {
    int i, j, ep;
    double est, err;
    for (i = 0; i < n_rounds; ++i) {
        for (ep = 0; ep < n_train; ++ep) {
            est = estimate(coef, training[0], n_terms);
            err = training[n_terms] - est;
            coef[0] += + learn_rate * err * est * (1.0-est);
            for (j = 0; j < n_terms; ++j) {
                coef[i+1] += learn_rate * err * est * (1.0-est) * training[i];
            }
        }
    }
}


int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Usage: %s DataFile #_of_entries #_of_features\n", argv[0]);
        exit(1);
    }
    int number_of_entries = atoi(argv[2]);
    int number_of_features = atoi(argv[3]);

    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv[4]);
    double start = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);

    int proc_data_len = number_of_entries / (numprocs - 1);
    if (rank == numprocs - 1) {
        int remainder = number_of_entries % (numprocs - 1);
        proc_data_len += remainder;
    }

    int i = 0;

    if (rank == 0) {
        double **data = malloc(number_of_entries * sizeof(double *));
        for (i = 0; i < proc_data_len; ++i) {
            data[i] = (double *) malloc((number_of_features+1) * sizeof(double));
        }
        readfile(argv[1], data, number_of_features);
        send_data_shards(data, proc_data_len, remainder, numprocs);
        recv_gradients();
        // calculate weights
    } else {
        double **data = malloc(proc_data_len * sizeof(double *));
        for (i = 0; i < proc_data_len; ++i) {
            data[i] = (double *) malloc((number_of_features+1) * sizeof(double));
        }
        MPI_Recv(data, proc_data_len, MPI_DOUBLE, 0,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //calculate gradients
        send_gradients();
    }

    
    MPI_Finalize();
    free(data);
}

void send_data_shards(double **data, int data_per_proc, 
                      int remainder, int numprocs) {
    int dest_proc;
    for (dest_proc = 1; dest_proc < numprocs; ++dest_proc) {
        int len = data_per_proc;
        if (dest_proc == numprocs - 1) {
            len += remainder;
        }
        MPI_Send(&data[data_per_proc * (dest_proc - 1)],
                  len, MPI_DOUBLE, dest_proc, 0, MPI_COMM_WORLD);
    }
}