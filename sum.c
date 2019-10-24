

#define N 2000000



#pragma omp parallel for private(i) shared(sum) // Creates threads with a private i variable and a shared variable, sum

#pragma omp critical // Creates a critical section 

#pragma opm atomic // Creates an atomic section, which doesn't worry about locking and unlocking
