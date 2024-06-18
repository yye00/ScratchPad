#include <iostream>
#include <vector>
#include <complex>
#include <numeric>
#include <omp.h>

// Function to perform work in the inner loop
void do_inner_work(int i, int j, std::vector<std::complex<double>>& complex_vec) {
    // Modify the vector elements
    for (auto& elem : complex_vec) {
        elem += std::complex<double>(i, j);
    }
}

int main() {
    const int outer_loop_size = 90;
    const int inner_loop_size = 10;
    const int vector_size = 5;

    // Initialize a vector of complex numbers
    std::vector<std::complex<double>> complex_vec(vector_size, std::complex<double>(1.0, 1.0));

    // Enable nested parallelism by setting the maximum number of active parallel levels
    omp_set_max_active_levels(2);

    // Specify the number of threads for the outer and inner loops
    const int num_outer_threads = 4;
    const int num_inner_threads = 2;

    // Array to hold results for each thread in the outer loop
    std::vector<std::vector<std::complex<double>>> thread_results(num_outer_threads, std::vector<std::complex<double>>(vector_size, std::complex<double>(0.0, 0.0)));

    #pragma omp parallel num_threads(num_outer_threads)
    {
        int outer_thread_id = omp_get_thread_num();
        std::vector<std::complex<double>> complex_vec_private = complex_vec; // Deep copy for each outer thread

        #pragma omp parallel for num_threads(num_inner_threads) firstprivate(complex_vec_private)
        for (int j = 0; j < inner_loop_size; ++j) {
            int inner_thread_id = omp_get_thread_num();
            do_inner_work(outer_thread_id, j, complex_vec_private);
        }

        // Store the result in the thread-specific results array
        for (int k = 0; k < vector_size; ++k) {
            thread_results[outer_thread_id][k] += complex_vec_private[k];
        }
    }

    // Final reduction: sum the results from each thread
    std::vector<std::complex<double>> reduction_result(vector_size, std::complex<double>(0.0, 0.0));
    for (int t = 0; t < num_outer_threads; ++t) {
        for (int k = 0; k < vector_size; ++k) {
            reduction_result[k] += thread_results[t][k];
        }
    }

    // Print the result
    for (int k = 0; k < vector_size; ++k) {
        std::cout << "reduction_result[" << k << "] = " << reduction_result[k] << std::endl;
    }

    return 0;
}
