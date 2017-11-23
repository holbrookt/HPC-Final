#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N_FETCH 1
#define N_PUSH 1

float learn_rate;

float estimate(float *coef, float *data, int n_terms) {
    float sum = coef[0];
    int i;
    for (i = 0; i < n_terms; ++i) {
        sum += coef[i+1] * data[i];
    }
    return 1 / (1 + exp(est));
}

void SGD(float **training, float *coef, int n_terms, int n_rounds) {
    int i, j, ep;
    float est, err;
    for (i = 0; i < n_rounds; ++i) {
        for (ep = 0; ep < n_train; ++ep) {
            est = estimate(coef, training[0], n_terms);
            err = training[n_terms] - est;
            // sum of error?
            coef[0] += + learn_rate * err * est * (1.0-est);
            for (j = 0; j < n_terms; ++j) {
                coef[i+1] += learn_rate * err * est * (1.0-est) * training[i];
            }
        }
    }
}

int main() {

    int step = 0;
    int accruedGradients = 0;

    while (true) {
        if (step % )
    }

}