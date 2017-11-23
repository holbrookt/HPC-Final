#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

//heat_scale dataset is 270x13
//double weights[13] = { 0 };

#define SIZEOF(x) (sizeof(x) / sizeof((x)[0]))

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
        // bias
        // data[example_number][0] = 1;
        
        // Align the data. Start at 1 and continue until null because
        // we don't know the length of the line_tokens array because
        // We could have been missing data in the middle
        i = 1;
        while (line_tokens[i] != NULL) {
            // Grab the reported index
            int index = atoi(strtok(line_tokens[i], ":"));
            // set the target index to the reported value
            data[example_number][index] = atof(strtok(NULL, ":"));
            i++;
        }
        
        printf("Data %d:   ", example_number); 
        for (i = 1; i<=number_of_features; i++) {
            printf("%.3f  ", data[example_number][i]);
        }
        printf("\n");
        example_number++;
    }
}

double normalize(double * left, double * right) {
    double sum = 0;
    int i;
    for (i=0; i<13; i++)
    {
        double minus = left[i] - right[i];
        double r = minus * minus;
        sum += r;
    }

    return sqrt(sum);
}

double predict( double * row, double * weights) {
    double sol = weights[0];
    int i;
    for ( i = 1; i < 13; i++) {
       sol += weights[i] * row[i-1];
    }
    return sigmoid(sol);
}

void logisticRegression(double **data, double *weights, double *solutions) {

    // t
    double epsilon = 0.000001;
    // the learning rate
    double gamma = 0.0001;
    int max_iters = 10000;
    int iter = 0;
    
    printf("Starting logistic Regression\n");

    double prev_weights[13] = { 0 };
    int i, j, k;
    while (iter < max_iters) {
        double gradient = 0;
        double error;
        for (j=0; j < 270; j++) {
            double sol = predict(data[j], prev_weights);
            error = solutions[j] - sol;
            weights[0] = prev_weights[0] + gamma * error * sol * (1- sol);
            //gradient = solutions[j] * data[j][i] * sol;
            if (j == 268) {
                printf("[%d] Error: %f Weight diff: %f\n", j, error, weights[0] - prev_weights[0]);
            }
            for (i=1; i < SIZEOF(weights); i++) {
                weights[i] = prev_weights[i] + gamma * error * sol * (1 - sol) * data[j][i];
            }
        }
        double dist = normalize(weights, prev_weights);
        if ( dist < epsilon) {
            break;
        }
        for (i = 0; i < SIZEOF(weights); i++) {
            prev_weights[i] = weights[i];
        }
        printf("Iteration %d: distance = %f\n", iter++, dist);
    }
    printf("Result: ");
    for (i = 0; i < 13; i++) {
        printf("%f ", weights[i]);
    }
    printf("\n");
}

int test(double ** data, double *weights, double *solutions) {
    int i, j;
    int total = 0;
    int correct = 0;
    for (i = 0; i < 270; i++) {
        double solution = predict(data[i], weights);
        if (i == 268) {
            printf("Real %f -- Predicted %f\n", solutions[i], solution);
        }
        if (round(solution) == solutions[i]) {
            correct++;
        }
        total++;
    }
    printf("Percent Error = %d/%d\n", correct, total);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: ./LogisticRegression DataFile #_of_entries #_of_features\n");
        exit(1);
    }
    double number_of_entries = atoi(argv[2]);
    double number_of_features = atoi(argv[3]);
    double **data = malloc( number_of_entries * sizeof(double *));
    double *solutions = malloc( number_of_entries * sizeof(double));
    double *weights = calloc((number_of_features+1), sizeof(double));
    int i =0;
    for (i = 0; i < number_of_entries; i++) {
        data[i] = malloc(number_of_features * sizeof(double));
    }
    printf("Reading File\n");
    readfile(argv[1], data, solutions, number_of_features); 
    printf("Running Logistic Regresion\n");
    logisticRegression(data, solutions, weights);
    test(data, weights, solutions);
    printf("Done\n");
    return 0;
}
