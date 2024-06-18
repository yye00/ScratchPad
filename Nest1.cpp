#include <iostream>
#include <complex>
#include <omp.h>
#include <cstring> // For memcpy

// Function to perform work in the inner loop
void do_inner_work(int i, int j, std::complex<double>* complex_vec, int vector_size) {
    // Modify the vector elements
    for (int k = 0; k < vector_size; ++k) {
        complex_vec[k] += std::complex<double>(i, j);
    }
}

// zaxpy-like function to perform element-wise addition (reduction)
void zaxpy(int n, const std::complex<double>* x, std::complex<double>* y) {
    for (int i = 0; i < n; ++i) {
        y[i] += x[i];
    }
}

int main() {
    const int outer_loop_size = 90;
    const int inner_loop_size = 10;
    const int vector_size = 5;

    // Initialize a vector of complex numbers
    std::complex<double>* complex_vec = new std::complex<double>[vector_size];
    for (int i = 0; i < vector_size; ++i) {
        complex_vec[i] = std::complex<double>(1.0, 1.0);
    }

    // Enable nested parallelism by setting the maximum number of active parallel levels
    omp_set_max_active_levels(2);

    // Specify the number of threads for the outer and inner loops
    const int num_outer_threads = 4;
    const int num_inner_threads = 2;

    // Array to hold results for each thread in the outer loop
    std::complex<double>** thread_results = new std::complex<double>*[num_outer_threads];
    for (int t = 0; t < num_outer_threads; ++t) {
        thread_results[t] = new std::complex<double>[vector_size];
        std::fill(thread_results[t], thread_results[t] + vector_size, std::complex<double>(0.0, 0.0));
    }

    #pragma omp parallel num_threads(num_outer_threads)
    {
        int outer_thread_id = omp_get_thread_num();
        // Allocate memory for the private copy and use memcpy
        std::complex<double>* complex_vec_private = new std::complex<double>[vector_size];
        std::memcpy(complex_vec_private, complex_vec, vector_size * sizeof(std::complex<double>));

        #pragma omp parallel for num_threads(num_inner_threads) firstprivate(complex_vec_private)
        for (int j = 0; j < inner_loop_size; ++j) {
            do_inner_work(outer_thread_id, j, complex_vec_private, vector_size);
        }

        // Store the result in the thread-specific results array
        zaxpy(vector_size, complex_vec_private, thread_results[outer_thread_id]);

        delete[] complex_vec_private;
    }

    // Final reduction: sum the results from each thread
    std::complex<double>* reduction_result = new std::complex<double>[vector_size];
    std::fill(reduction_result, reduction_result + vector_size, std::complex<double>(0.0, 0.0));
    for (int t = 0; t < num_outer_threads; ++t) {
        zaxpy(vector_size, thread_results[t], reduction_result);
        delete[] thread_results[t];
    }
    delete[] thread_results;

    // Print the result
    for (int k = 0; k < vector_size; ++k) {
        std::cout << "reduction_result[" << k << "] = " << reduction_result[k] << std::endl;
    }

    delete[] complex_vec;
    delete[] reduction_result;

    return 0;
}
