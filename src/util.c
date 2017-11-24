#include <math.h>
#include "util.h"

double sigmoid(double x) {
    double e = 2.718281828;
    return 1.0 / (1.0 + pow(e, -x));
}

double normalize(Weights_t left, Weights_t right) {
    double sum = 0;
    int i;
    for (i=0; i < left->length; i++)
    {
        double minus = left->values[i] - right->values[i];
        double r = minus * minus;
        sum += r;
    }

    return sqrt(sum);
}
