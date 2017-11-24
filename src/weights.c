#include "weights.h"

void print_weights(Weights_t weights) {
    int i;
    printf("Result: ");
    for (i = 0; i < weights->length; i++) {
        printf("%f ", weights->values[i]);
    }
    printf("\n");
}
