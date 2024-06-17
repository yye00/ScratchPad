#include <omp.h>
#include <iostream>
#include <vector>
#include <cstring> // for memcpy

// Function to perform work in the inner loop
void do_work(double* B, int total_B_elements, int i, int j) {
    // Example work: increment each element of B by 1
    for (int k = 0; k < total_B_elements; ++k) {
        B[k] += 1.0;
    }
}

int main() {
    const int outer_loop_size = 10;
    const int inner_loop_size = 20;
    const int total_B_elements = 5;

    // Initialize array B
    std::vector<double> B(total_B_elements, 0.0);

    // Allocate an array for storing the reduction results from each thread
    std::vector<std::vector<double>> thread_results(omp_get_max_threads(), std::vector<double>(total_B_elements, 0.0));

    // Parallelize the outer loop
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        std::vector<double>& B_private = thread_results[thread_id];

        #pragma omp for
        for (int i = 0; i < outer_loop_size; ++i) {
            // Manually copy B to B_private using memcpy
            std::memcpy(B_private.data(), B.data(), total_B_elements * sizeof(double));

            // Perform some work with the private copy of B in the outer loop
            // Example: multiply each element by i
            for (int k = 0; k < total_B_elements; ++k) {
                B_private[k] *= i;
            }

            // Inner loop
            for (int j = 0; j < inner_loop_size; ++j) {
                do_work(B_private.data(), total_B_elements, i, j);
            }
        }
    }
#include <omp.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Function to perform work in the inner loop
void do_work(double* B, int total_B_elements, int i, int j) {
    // Example work: increment each element of B by 1
    for (int k = 0; k < total_B_elements; ++k) {
        B[k] += 1.0;
    }
}

int main() {
    const int outer_loop_size = 10;
    const int inner_loop_size = 20;
    const int total_B_elements = 5;

    // Initialize array B
    std::vector<double> B(total_B_elements, 0.0);

    // Allocate an array for storing the reduction results from each thread
    std::vector<std::vector<double>> thread_results(omp_get_max_threads(), std::vector<double>(total_B_elements, 0.0));

    // Parallelize the outer loop
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        std::vector<double>& B_private = thread_results[thread_id];

        #pragma omp for
        for (int i = 0; i < outer_loop_size; ++i) {
            // Manually copy B to B_private
            std::copy(B.begin(), B.end(), B_private.begin());

            // Perform some work with B_private in the outer loop
            // Example: multiply each element by i
            for (int k = 0; k < total_B_elements; ++k) {
                B_private[k] *= i;
            }

            // Inner loop
            for (int j = 0; j < inner_loop_size; ++j) {
                do_work(B_private.data(), total_B_elements, i, j);
            }
        }
    }

    // Perform reduction outside the parallel region
    for (int t = 0; t < omp_get_max_threads(); ++t) {
        for (int k = 0; k < total_B_elements; ++k) {
            B[k] += thread_results[t][k];
        }
    }

    // Print the result
    for (int k = 0; k < total_B_elements; ++k) {
        std::cout << "B[" << k << "] = " << B[k] << std::endl;
    }

    return 0;
}
  
    // Perform reduction outside the parallel region
    for (int t = 0; t < omp_get_max_threads(); ++t) {
        for (int k = 0; k < total_B_elements; ++k) {
            B[k] += thread_results[t][k];
        }
    }

    // Print the result
    for (int k = 0; k < total_B_elements; ++k) {
        std::cout << "B[" << k << "] = " << B[k] << std::endl;
    }

    return 0;
}


