#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

//heat_scale dataset is 270x13
double data[270][13];
double solutions[270];
double weights[13] = { 0 };


double sigmoid(double x) {
    double e = 2.718281828;
    return 1.0 / (1.0 + pow(e, -x));
}

void readfile(char* filename) {
    FILE* fp = fopen(filename, "r");
    //char*
    char line[150];
    int example_number = 0;
    while(fgets(line, sizeof(line), fp)){
        char ** line_tokens = malloc(14*sizeof(char*));
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
        data[example_number][0] = 1;
        
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
        for (i = 0; i<=13; i++) {
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

void logisticRegression() {

    // t
    double epsilon = 0.000001;
    // the learning rate
    double gamma = 0.00005;
    int max_iters = 20000;
    int iter = 0;
    
    printf("Starting logistic Regression\n");

    double prev_weights[13] = { 0 };
    int i, j, k;
    while (iter < max_iters) {
        for (i=0; i < 13; i++) {
            double gradient = 0;
            for (j=0; j < 270; j++) {
                double z_j = prev_weights[0]; //data[j][0];
                for (k=1; k < 13; k++) {
                    z_j += prev_weights[k] * data[j][k];
                }
                gradient = solutions[j] * data[j][i] * sigmoid(-solutions[j]*z_j);
            }
            weights[i] = prev_weights[i] + gamma * gradient;
        }
        double dist = normalize(weights, prev_weights);
        if ( dist < epsilon) {
            break;
        }
        for (i = 0; i < 13; i++) {
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

int test() {
    int i, j;
    for (i = 0; i < 270; i++) {
        double sol = weights[0];
        for (j = 1; j < 13; j++) {
            sol += data[i][j]*weights[j];
        }
        printf("Real %f -- Predicted %f\n", solutions[i], sigmoid(sol));
    }
    return 0;
}

int main() {
    printf("Reading File\n");
    readfile("data/heart_scale"); 
    printf("Running Logistic Regresion\n");
    logisticRegression();
    test();
    printf("Done\n");
    return 0;
}
