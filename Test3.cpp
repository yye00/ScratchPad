#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <mkl.h>
#include <vector>
#include <iostream>

namespace py = pybind11;

void batch_matrix_multiply(const py::list& A_list, py::array_t<double> B, const py::list& B_shapes_list) {
    py::buffer_info bufB = B.request();

    if (bufB.ndim != 1) {
        throw std::runtime_error("Input matrix B must be 1D");
    }

    double* current_B = static_cast<double*>(bufB.ptr);
    int total_elements_B = bufB.size;

    double* next_B = new double[total_elements_B];

    for (size_t idx = 0; idx < A_list.size(); ++idx) {
        py::array_t<double> A = A_list[idx].cast<py::array_t<double>>();
        py::buffer_info bufA = A.request();
        std::vector<int> B_shape = B_shapes_list[idx].cast<std::vector<int>>();

        if (bufA.ndim != 2) {
            throw std::runtime_error("Each matrix in A_list must be 2D");
        }

        int rowsA = bufA.shape[0];
        int colsA = bufA.shape[1];

        int batch_size = B_shape[0];
        int rowsB = B_shape[1];
        int colsB = B_shape[2];

        if (total_elements_B != batch_size * rowsB * colsB) {
            throw std::runtime_error("The shape of B does not match the given batch size and dimensions");
        }

        if (colsA != rowsB) {
            throw std::runtime_error("Matrix dimensions must match for multiplication");
        }

        for (int i = 0; i < batch_size; ++i) {
            const double* a = static_cast<double*>(bufA.ptr);
            const double* b = current_B + i * rowsB * colsB;
            double* c = next_B + i * rowsA * colsB;

            const double alpha = 1.0;
            const double beta = 0.0;
            CBLAS_TRANSPOSE trans = CblasNoTrans;

            cblas_dgemm(CblasRowMajor, trans, trans, 
                        rowsA, colsB, colsA, 
                        alpha, a, colsA, 
                        b, colsB, 
                        beta, c, colsB);
        }

        // Swap pointers
        std::swap(current_B, next_B);
    }

    // Update the original numpy array to point to the new data
    py::detail::array_proxy(B.ptr())->data = reinterpret_cast<char*>(current_B);

    // Free the old B data
    delete[] next_B;
}

PYBIND11_MODULE(example, m) {
    m.def("batch_matrix_multiply", &batch_matrix_multiply, "Perform batch matrix multiplication using MKL and update B in place");
}
