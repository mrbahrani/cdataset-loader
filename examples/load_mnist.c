#include "cdataset_loader.h"
#include <stdlib.h>

int main() {
    const char* directory = "./data";
    float* X_train = malloc(10000*sizeof(float));
    float* X_test = malloc(1000*sizeof(float));

    int* y_train = malloc(10000*sizeof(int));
    int* y_test = malloc(1000*sizeof(int));
    load_mnist(directory, X_train, y_train, 10000, X_test, y_test, 1000);
}