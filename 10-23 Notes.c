

#define N 2000000



#pragma omp parallel for private(i) shared(sum) // Creates threads with a private i variable and a shared variable, sum

#pragma omp critical // Creates a critical section

#pragma opm atomic // Creates an atomic section, which doesn't worry about locking and unlocking

int tid = omp_get_thread_num(); // Gives back the id of the thread

/* Reduction clause is useful */

#pragma omp parallel for reduction(+:sum) // sum is the variable, and we want to do reduction on sum, and we're going to add all the partial sums together. Our variable is sum, and the reduction type is "+", which adds them all together
// There are a bunch of different kinds of reductions. They're in the class presentation

#pragma omp paralle sections // Creates sections for your parallel execution

#pragma omp section // This is one of the sections you created


