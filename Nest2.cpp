#include <iostream>
#include <vector>
#include <functional>
#include <cmath> // For example computations

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

    // Create a thread pool with a specified number of threads
    const size_t num_inner_threads = 4;
    ThreadPool thread_pool(num_inner_threads);

    // Outer loop
    for (int i = 0; i < outer_loop_size; ++i) {
        // Enqueue tasks for the inner loop
        for (int j = 0; j < inner_loop_size; ++j) {
            thread_pool.enqueue([i, j] { do_inner_work(i, j); });
        }
    }

    // Thread pool destructor will wait for all tasks to complete
    return 0;
}
