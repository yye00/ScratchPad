#include <iostream>
#include <vector>
#include <omp.h>

// Function to perform work in the inner loop
void do_inner_work(int i, int j) {
    // Example work: print the indices and perform some dummy computation
    std::cout << "Processing i=" << i << ", j=" << j << std::endl;
    double result = std::sqrt(i * j);
}

int main() {
    const int outer_loop_size = 90;
    const int inner_loop_size = 10;

    // Initialize data (example)
    std::vector<int> outer_data(outer_loop_size);
    std::iota(outer_data.begin(), outer_data.end(), 0);

    // Enable nested parallelism by setting the maximum number of active parallel levels
    omp_set_max_active_levels(2);

    // Specify the number of threads for the outer and inner loops
    const int num_outer_threads = 4;
    const int num_inner_threads = 2;

    #pragma omp parallel for num_threads(num_outer_threads)
    for (int i = 0; i < outer_loop_size; ++i) {
        #pragma omp parallel for num_threads(num_inner_threads)
        for (int j = 0; j < inner_loop_size; ++j) {
            do_inner_work(i, j);
        }
    }

    return 0;
}
