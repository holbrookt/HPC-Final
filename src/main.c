#include "main.h"

// "Prediction" Function
// Calculates the sigmoid of ( weight_0 (aka the bias) + sum(weight_i*element_i))
double predict( double * row, Weights_t weights) {
    double sol = weights->values[0];
    int i;
    for ( i = 1; i < weights->length; i++) {
       sol += weights->values[i] * row[i-1];
    }
    return sigmoid(sol);
}

// Gradient Decent optimization of Logistic Regrssion.
// The goal is to minimize the error between the predicted value and the real value.
// 1. Where the p(x) = sigmoid(weight_0 (aka the bias) + sum(weight_i*element_i))
// 2. The error is = Real - p(x)
// 3. The gradient_i = learning_rate * error * p(x) * 1-p(x) * element_i
// 4. The gradient_bias = learning_rate * error * p(x) * 1-p(x)
void logisticRegression(double **data, Weights_t weights, double *solutions, int number_of_entries) {
    // Cutoff when the change in the weights is less than epsilon.
    double epsilon = 0.000001;
    // the learning rate
    double gamma = 0.1;
    // Limit the number of iterations.
    int max_iters = 10000;
    int iter = 0;
    
    Weights_t prev_weights = malloc(sizeof(Weights_t));
    prev_weights->length = weights->length;
    prev_weights->values = (double*)malloc(prev_weights->length* sizeof(double));
    int i, j, k;
    while (iter < max_iters) {
        double error;
        for (j=0; j < number_of_entries; j++) {
            // Get p(x) per item 1 ^
            double sol = predict(data[j], prev_weights);
            // Get error per item 2 ^
            error = solutions[j] - sol;
            // Add bias gradient per item 4 ^
            weights->values[0] += gamma * error * sol * (1- sol);
            // Add gradient to eache element per item 3 ^
            for (i=1; i < weights->length; i++) {
                weights->values[i] += (gamma * error * sol * (1 - sol) * data[j][i-1]);
            }
        }
        print_weights(weights);
        // normalize the ditance between the weights to see % change
        double dist = normalize(weights, prev_weights);
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
        print_weights(weights);
    }
    free(prev_weights->values);
    free(prev_weights);
}

// Test the quality of the predictor against training data
// This should really be validated against a *diffrent* set of data
int test(double ** data, Weights_t weights, double *solutions, int number_of_features, int number_of_entries) {
    int i, j;
    int total = 0;
    int correct = 0;
    for (i = 0; i < number_of_entries; i++) {
        // predict and compare rounded prediction against real value
        double solution = predict(data[i], weights);
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
    
    // Setup Weights
    Weights_t weights = malloc(sizeof(Weights_t));
    weights->length = number_of_features+1;
    weights->values = (double*)malloc((number_of_features+1)* sizeof(double));
    
    int i =0;
    for (i = 0; i < number_of_entries; i++) {
        data[i] = malloc(number_of_features * sizeof(double));
    }
    printf("Reading File\n");
    readfile(argv[1], data, solutions, number_of_features); 
    
    printf("Running Logistic Regresion\n");
    logisticRegression(data, weights, solutions, number_of_entries);
    
    printf("Testing quality of predictor\n");
    test(data, weights, solutions, number_of_features, number_of_entries);
    
    for (i = 0; i < number_of_entries; i++) {
        free(data[i]);
    }
    free(data);
    free(solutions);
    free(weights->values);
    free(weights);
    return 0;
}
