#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define DEBUG 1
#define EXAMPLE 1

double sigmoid(double x) {
    double e = 2.718281828;
    return 1.0 / (1.0 + pow(e, -x));
}

void readfile(char* filename, double ** data, double *solutions, int number_of_features) {
    FILE* fp = fopen(filename, "r");
    //char*
    char line[150];
    int example_number = 0;
    while(fgets(line, sizeof(line), fp)){
        char ** line_tokens = malloc((number_of_features+1)*sizeof(char*));
        int i = 0;
        int term_length = strcspn(line, "\n\r");
        line[term_length] = '\0';
        line_tokens[i] = strtok(line, " ");
        // The first token is the solution
        solutions[example_number] = (double)atoi(line_tokens[i]); 
        while (line_tokens[i] != NULL) {
            line_tokens[++i] = strtok(NULL, " ");
        }
        
        // Align the data. Start at 0 and continue until null because
        // we don't know the length of the line_tokens array because
        // We could have been missing data in the middle
        i = 1;
        while (line_tokens[i] != NULL) {
            // Grab the reported index
            int index = atoi(strtok(line_tokens[i], ":"));
            // set the target index to the reported value
            data[example_number][index-1] = atof(strtok(NULL, ":"));
            i++;
        }
        
        if (DEBUG == 1 && example_number == EXAMPLE) {
            printf("Data %d:   ", example_number); 
            for (i = 0; i<number_of_features; i++) {
                printf("%.3f  ", data[example_number][i]);
            }
            printf("   Solution: %f\n", solutions[example_number]);
        }
        example_number++;
    }
    printf("number_of_entries: %d\n", example_number);
}

double normalize(double * left, double * right, int number_of_features) {
    double sum = 0;
    int i;
    for (i=0; i<number_of_features; i++)
    {
        double minus = left[i] - right[i];
        double r = minus * minus;
        sum += r;
    }

    return sqrt(sum);
}

double predict( double * row, double * weights, int number_of_features) {
    double sol = weights[0];
    int i;
    for ( i = 1; i < (number_of_features+1); i++) {
       sol += weights[i] * row[i-1];
    }
    return sigmoid(sol);
}

void print_weights(double *weights, int number_of_features) {
    int i;
    printf("Result: ");
    for (i = 0; i < (number_of_features+1); i++) {
        printf("%f ", weights[i]);
    }
    printf("\n");
}

void logisticRegression(double **data, double *weights, double *solutions, int number_of_features, int number_of_entries) {
    // t
    double epsilon = 0.000001;
    // the learning rate
    double gamma = 0.1;
    int max_iters = 1000;
    int iter = 0;
    
    printf("The fuck? %f\n", solutions[EXAMPLE]);
    double *prev_weights = (double*)malloc((number_of_features+1)* sizeof(double));
    int i, j, k;
    double bias = 0;
    while (iter < max_iters) {
        double gradient = 0;
        double error;
        for (j=0; j < number_of_entries; j++) {
            double sol = predict(data[j], prev_weights, number_of_features);
            error = solutions[j] - sol;
            bias += gamma*error*sol*(1-sol);
            weights[0] += gamma * error * sol * (1- sol);
            if ( j == EXAMPLE) {
                printf("[%d] adjustment: %f\n", j, gamma*error*sol*(1-sol));
                printf("[%d] sol: %f predict: %f error: %f\n", j, solutions[j], sol, error);
                printf("[%d] bias prev %f, bias new %f\n", j, weights[0], prev_weights[0]);
            }
            for (i=1; i < (number_of_features+1); i++) {
                weights[i] += (gamma * error * sol * (1 - sol) * data[j][i-1]);
            }
        }
        print_weights(weights, number_of_features);
        double dist = normalize(weights, prev_weights, number_of_features);
        printf("dist: %f\n", dist);
        if ( dist < epsilon) {
            break;
        }
        for (i = 0; i < (number_of_features+1); i++) {
            prev_weights[i] = weights[i];
            // printf("%.3f  ", weights[i]);
        }
        printf("Iteration %d: distance = %f   bias: %f \n", iter++, dist, bias);
        print_weights(weights, number_of_features);
    }
}

int test(double ** data, double *weights, double *solutions, int number_of_features, int number_of_entries) {
    int i, j;
    int total = 0;
    int correct = 0;
    for (i = 0; i < number_of_entries; i++) {
        double solution = predict(data[i], weights, number_of_features);
        if (i == EXAMPLE) {
            printf("Real %f -- Predicted %f (%f)\n", solutions[i], solution, round(solution));
        }
        if (round(solution) == solutions[i]) {
            correct++;
        }
        total++;
    }
    printf("Percent correct = %d/%d\n", correct, total);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: ./LogisticRegression DataFile #_of_entries #_of_features\n");
        exit(1);
    }
    int number_of_entries = atoi(argv[2]);
    int number_of_features = atoi(argv[3]);
    double **data = malloc( number_of_entries * sizeof(double *));
    double *solutions = malloc( number_of_entries * sizeof(double));
    double *weights = (double*)malloc((number_of_features+1)* sizeof(double));
    int i =0;
    for (i = 0; i < number_of_entries; i++) {
        data[i] = malloc(number_of_features * sizeof(double));
    }
    printf("Reading File\n");
    readfile(argv[1], data, solutions, number_of_features); 
    printf("Running Logistic Regresion\n");
    logisticRegression(data, weights, solutions, number_of_features, number_of_entries);
    test(data, weights, solutions, number_of_features, number_of_entries);
    printf("Done\n");
    return 0;
}
