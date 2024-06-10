#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <mkl.h>
#include <vector>
#include <iostream>

namespace py = pybind11;

py::array_t<double> batch_matrix_multiply(const py::list& A_list, py::array_t<double> B, const py::list& B_shapes_list) {
    py::buffer_info bufB = B.request();

    if (bufB.ndim != 1) {
        throw std::runtime_error("Input matrix B must be 1D");
    }

    double* ptrB = static_cast<double*>(bufB.ptr);
    int total_elements_B = bufB.size;
    double* result_data = new double[total_elements_B];
    std::copy(ptrB, ptrB + total_elements_B, result_data);

    double* current_B = result_data;
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

        std::vector<size_t> result_shape = {static_cast<size_t>(batch_size), static_cast<size_t>(rowsA), static_cast<size_t>(colsB)};
        py::array_t<double> result(result_shape);
        py::buffer_info buf_result = result.request();

        double* ptrA = static_cast<double*>(bufA.ptr);
        double* ptr_result = static_cast<double*>(buf_result.ptr);

        std::vector<const double*> a_array(batch_size);
        std::vector<const double*> b_array(batch_size);
        std::vector<double*> c_array(batch_size);

        for (int i = 0; i < batch_size; ++i) {
            a_array[i] = ptrA;
            b_array[i] = current_B + i * rowsB * colsB;
            c_array[i] = ptr_result + i * rowsA * colsB;
        }

        const double alpha = 1.0;
        const double beta = 0.0;

        CBLAS_TRANSPOSE trans = CblasNoTrans;

        cblas_dgemm_batch(CblasRowMajor, &trans, &trans, 
                          &rowsA, &colsB, &colsA, 
                          &alpha, a_array.data(), &colsA, 
                          b_array.data(), &colsB, 
                          &beta, c_array.data(), &colsB, 
                          1, &batch_size);

        // Swap pointers
        std::swap(current_B, next_B);
        std::copy(ptr_result, ptr_result + total_elements_B, current_B);
    }

    delete[] next_B;

    // Create a capsule to manage the memory
    py::capsule free_when_done(current_B, [](void* f) {
        double* foo = reinterpret_cast<double*>(f);
        delete[] foo;
    });

    return py::array_t<double>(
        {total_elements_B},  // shape
        {sizeof(double)},    // stride
        current_B,           // the data pointer
        free_when_done       // the capsule
    );
}

PYBIND11_MODULE(example, m) {
    m.def("batch_matrix_multiply", &batch_matrix_multiply, "Perform batch matrix multiplication using MKL and update B");
}
