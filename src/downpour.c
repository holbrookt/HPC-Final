#include <mpi.h>

#include "main.h"

#define N_FETCH 1
#define N_PUSH 1

// "Prediction" Function
// Calculates the sigmoid of ( weight_0 (aka the bias) + sum(weight_i*element_i))
double predict( double * data, int start, Weights_t weights) {
    double sol = weights->values[0];
    int i;
    for ( i = 1; i < weights->length; i++) {
       sol += weights->values[i] * data[start+(i-1)];
    }
    return sigmoid(sol);
}

// Gradient Decent optimization of Logistic Regrssion.
// The goal is to minimize the error between the predicted value and the real value.
// 1. Where the p(x) = sigmoid(weight_0 (aka the bias) + sum(weight_i*element_i))
// 2. The error is = Real - p(x)
// 3. The gradient_i = learning_rate * error * p(x) * 1-p(x) * element_i
// 4. The gradient_bias = learning_rate * error * p(x) * 1-p(x)
void logisticRegression(double *data, Weights_t weights, double *solutions, int number_of_entries, int iter_cap, int task ) {
    // Cutoff when the change in the weights is less than epsilon.
    double epsilon = 0.000001;
    // the learning rate
    double gamma = 0.1;
    // Limit the number of iterations.
    int max_iters = iter_cap;
    int iter = 0;
    
    Weights_t prev_weights = malloc(sizeof(Weights_t));
    prev_weights->length = weights->length;
    prev_weights->values = (double*)malloc(prev_weights->length* sizeof(double));
    int i, j, k;
    while (iter < max_iters) {
        double error;
        printf("[%d] number_of_entires: %d \n", task, number_of_entries);
        for (j=0; j < number_of_entries; j++) {
            if (task != 0) {
                printf("[%d] In loop\n", task);
            }
            //printf("[%d] sol: %f\n", task, solutions[j]);
            // Get p(x) per item 1 ^
            double sol = predict(data, j*prev_weights->length, prev_weights);
            if (task != 0) {
                printf("[%d] predicted: %f \n", task, sol);
                printf("[%d] solution: %f \n", task, solutions[j]);
                printf("[%d] Will try to index into data: %d\n", task, ((j*weights->length)+task-1));
                printf("[%d] data at index: %f\n", task, data[(j*weights->length)+task-1]);
            } 
            // Get error per item 2 ^
            error = solutions[j] - sol;
            if (j == 10) {
                printf("[%d D%d] sol: %f -- index: %d -- %d: %f\n", task, j, solutions[j], task, (j*weights->length)+task-1 , data[(j*weights->length)+task-1]);
            }
           
            //printf("[%d] predicted:  %f actual %f\n", task, sol, solutions[j]);
            // Add bias gradient per item 4 ^
            if (task !=0) {
                int k;
                for (k=0; k < weights->length; k++) {
                    printf("%f \n", weights->values[k]);
                }
                printf("[%d] Updating weights\n", task);
            }
            if (task == 0) {
                weights->values[0] += gamma * error * sol * (1- sol);
            } else {
                weights->values[task] += (gamma * error * sol * (1 - sol) * data[(j*weights->length) + task-1]);
            }
            if (task !=0) {
                printf("[%d] DONE Updating weights\n", task);
            }

        }
        if (task !=0) {
            printf("[%d] DONE running loop weights\n", task);
        }
        
        print_weights_ranked(weights, task);
        // normalize the ditance between the weights to see % change
        double dist = normalize(weights, prev_weights);
        //printf("Dist: %f\n", dist);
        if ( dist < epsilon) {
            break;
        }
        // Store current weights in seperate variable so that we can get % change
        // in the next iteration
        for (i = 0; i < weights->length; i++) {
            prev_weights->values[i] = weights->values[i];
            // printf("%.3f  ", weights[i]);
        }
        printf("Iteration %d: distance = %f\n", iter++, dist);
    }
    free(prev_weights->values);
    free(prev_weights);
}

// Test the quality of the predictor against training data
// This should really be validated against a *diffrent* set of data
int test(double * data, Weights_t weights, double *solutions, int number_of_features, int number_of_entries) {
    int i, j;
    int total = 0;
    int correct = 0;
    for (i = 0; i < number_of_entries; i++) {
        // predict and compare rounded prediction against real value
        double solution = predict(data, i*weights->length, weights);
        if (round(solution) == solutions[i]) {
            correct++;
        }
        total++;
    }
    printf("Percent correct = %d/%d\n", correct, total);
    return 0;
}

double index_into_1d(double * data_1d, int row, int col, int num_cols) {
    return data_1d[(row*num_cols) + col];
}

void convert_2d_to_1d(double ** data_2d, double * data_1d, int rows, int cols) {
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            //printf("%f ", data_2d[i][j]);
            data_1d[ (i*cols) + j] = data_2d[i][j];
        }
        //printf("\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Usage: ./LogisticRegression DataFile #_of_entries #_of_features batch_size\n");
        exit(1);
    }

    int numtasks, rank, sendcount, recvcount, source;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    int number_of_entries = atoi(argv[2]);
    int number_of_features = atoi(argv[3]);
    int batch_size = atoi(argv[4]);
    double **data = malloc( number_of_entries * sizeof(double *));
    double *data_1d = malloc( number_of_entries * number_of_features * sizeof(double));
    double *solutions = malloc( number_of_entries * sizeof(double));
    double *recvbuf = malloc((number_of_features+1)*sizeof(double));
    // This will dump some numbers
    // This would be ok if we were using randomized entries
    // but since in the data is orginized its biasing away from a know value
    // Oh well....
    sendcount = (number_of_entries/numtasks) * number_of_features; 
    //double *recvbuf = malloc( sendcount * sizeof(double)); 
    printf("Sending %d to %d children\n", sendcount, numtasks);    
    // Setup Weights
    Weights_t weights = malloc(sizeof(Weights_t));
    weights->length = number_of_features+1;
    weights->values = (double*)malloc((number_of_features+1)* sizeof(double));
    double *collectedWeights = NULL;
    
    int i =0;
    if (rank == 0) {
        
        for (i = 0; i < number_of_entries; i++) {
            data[i] = malloc(number_of_features * sizeof(double));
        }
        printf("Reading File\n");
        readfile(argv[1], data, solutions, number_of_features); 
        convert_2d_to_1d(data, data_1d, number_of_entries, number_of_features);
        collectedWeights = malloc(numtasks*weights->length* sizeof(double)); 
        printf("Data 1: %f %f %f\n", data_1d[3], data_1d[4], data_1d[5]);
        
        for (i = 0; i < weights->length; i++) {
            weights->values[i] = 0;
            recvbuf = 0;
        }
    }


    double start = MPI_Wtime();
    int j;
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(data_1d, number_of_entries, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(solutions, number_of_entries * number_of_features, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    for (j = 0; j <50/batch_size; j++) {
        printf("First Batch\n");
        MPI_Bcast(recvbuf, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        int k;
        for (k=0; k < weights->length; k++) {
            weights->values[k] = recvbuf[k];
            printf("[%d] %d: %f \n", rank, k, weights->values[k]);
        }
        
        for (k=0; k < weights->length; k++) {
            printf("[%d] %d: %f \n", rank, k, weights->values[k]);
        }
        
        
        printf("[%d] Running Logistic Regresion\n", rank);
        logisticRegression(data_1d, weights, solutions, number_of_entries, batch_size, rank); 
        MPI_Barrier(MPI_COMM_WORLD);
        printf("MPI Gather \n");
        MPI_Gather(weights->values, weights->length, MPI_DOUBLE, collectedWeights, weights->length, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        printf("[%d] Done Gather \n", rank);
        if (rank == 0) {
            printf("Combining weights\n");
            for (i = 0; i < (weights->length); i++) {
                weights->values[i] = collectedWeights[i+(i*weights->length)];
                recvbuf[i] = weights->values[i];
            }
            print_weights(weights);
        }
    }
    double end = MPI_Wtime();
    double total_time = end-start;

    printf("Took %f\n", total_time);
    if (rank == 0) {
        // Gather and test solution.
        printf("Testing quality of predictor\n");
        test(data_1d, weights, solutions, number_of_features, number_of_entries);

        /*for (i = 0; i < number_of_entries; i++) {
          printf("Freeing %d\n",i);
          free(data[i]);
          }

          free(data);
          free(solutions);
          free(weights->values);
          free(weights); */
    }
    MPI_Finalize();

    return 0;
}

/*
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
*/
