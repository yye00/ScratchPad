#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <mkl.h>
#include <complex>
#include <vector>
#include <iostream>
#include <omp.h>

namespace py = pybind11;

// Function to initialize OpenMP threads
void initialize_omp_threads() {
    #pragma omp parallel
    {
        // This parallel region will spawn the worker threads
        #pragma omp single
        {
            std::cout << "Initialized OpenMP with " << omp_get_num_threads() << " threads." << std::endl;
        }
    }
}

void batch_matrix_multiply(const py::list& A_list, py::array_t<std::complex<float>> B, const py::list& B_shapes_list) {
    py::buffer_info bufB = B.request();

    if (bufB.ndim != 1) {
        throw std::runtime_error("Input matrix B must be 1D");
    }

    std::complex<float>* current_B = static_cast<std::complex<float>*>(bufB.ptr);
    int total_elements_B = bufB.size;

    std::complex<float>* next_B = new std::complex<float>[total_elements_B];

    for (size_t idx = 0; idx < A_list.size(); ++idx) {
        py::array_t<std::complex<float>> A = A_list[idx].cast<py::array_t<std::complex<float>>>();
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

        #pragma omp parallel
        {
            std::vector<const std::complex<float>*> a_array(batch_size);
            std::vector<const std::complex<float>*> b_array(batch_size);
            std::vector<std::complex<float>*> c_array(batch_size);

            #pragma omp for
            for (int i = 0; i < batch_size; ++i) {
                a_array[i] = static_cast<std::complex<float>*>(bufA.ptr);
                b_array[i] = current_B + i * rowsB * colsB;
                c_array[i] = next_B + i * rowsA * colsB;

                const std::complex<float> alpha(1.0f, 0.0f);
                const std::complex<float> beta(0.0f, 0.0f);
                CBLAS_TRANSPOSE trans = CblasNoTrans;

                cblas_cgemm(CblasRowMajor, trans, trans, 
                            rowsA, colsB, colsA, 
                            reinterpret_cast<const MKL_Complex8*>(&alpha), reinterpret_cast<const MKL_Complex8*>(a_array[i]), colsA, 
                            reinterpret_cast<const MKL_Complex8*>(b_array[i]), colsB, 
                            reinterpret_cast<const MKL_Complex8*>(&beta), reinterpret_cast<MKL_Complex8*>(c_array[i]), colsB);
            }
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
    m.def("initialize_omp_threads", &initialize_omp_threads, "Initialize OpenMP threads early in the application");
    m.def("batch_matrix_multiply", &batch_matrix_multiply, "Perform batch matrix multiplication using MKL and update B in place");
}
