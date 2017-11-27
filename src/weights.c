#include "weights.h"

void print_weights(Weights_t weights) {
    int i;
    printf("Result: ");
    for (i = 0; i < weights->length; i++) {
        printf("%f ", weights->values[i]);
    }
    printf("\n");
}

void print_weights_ranked(Weights_t weights, int rank) {
    int i;
    printf("[%d] Result: ", rank);
    for (i = 0; i < weights->length; i++) {
        printf("%f ", weights->values[i]);
    }
    printf("\n");
}

