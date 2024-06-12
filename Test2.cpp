#include <iostream>
#include <vector>
#include <omp.h>

void perform_operations(std::vector<double>& A, std::vector<double>& B) {
    #pragma omp parallel for
    for (size_t i = 0; i < A.size(); ++i) {
        // Example operation on A, storing results in B
        B[i] = A[i] * 2.0;  // Modify as per your operations
    }
}

int main() {
    int n = 100; // Size of the vector
    std::vector<double> A(n, 1.0); // Initialize vector A with 1.0
    std::vector<double> B(n, 0.0); // Initialize vector B with 0.0

    // Perform operations on vector A and store results in B
    perform_operations(A, B);

    // Perform element-wise summation reduction on B
    std::vector<double> sum(n, 0.0);

    #pragma omp parallel for reduction(+:sum[:n])
    for (int i = 0; i < n; ++i) {
        sum[i] += B[i];
    }

    // Print the result
    for (int i = 0; i < n; ++i) {
        std::cout << "sum[" << i << "] = " << sum[i] << std::endl;
    }

    return 0;
}
