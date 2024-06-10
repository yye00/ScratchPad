#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <mkl.h>
#include <complex>
#include <vector>
#include <iostream>

namespace py = pybind11;

// Wrapper for cblas_gemm_batch for float
void cblas_gemm_batch_wrapper_float(int rowsA, int colsB, int colsA, const std::vector<const float*>& a_array, const std::vector<const float*>& b_array, std::vector<float*>& c_array, int batch_size) {
    const float alpha = 1.0f;
    const float beta = 0.0f;
    CBLAS_TRANSPOSE trans = CblasNoTrans;

    cblas_sgemm_batch(CblasRowMajor, &trans, &trans, 
                      &rowsA, &colsB, &colsA, 
                      &alpha, const_cast<const float**>(a_array.data()), &colsA, 
                      const_cast<const float**>(b_array.data()), &colsB, 
                      &beta, const_cast<float**>(c_array.data()), &colsB, 
                      1, &batch_size);
}

// Wrapper for cblas_gemm_batch for double
void cblas_gemm_batch_wrapper_double(int rowsA, int colsB, int colsA, const std::vector<const double*>& a_array, const std::vector<const double*>& b_array, std::vector<double*>& c_array, int batch_size) {
    const double alpha = 1.0;
    const double beta = 0.0;
    CBLAS_TRANSPOSE trans = CblasNoTrans;

    cblas_dgemm_batch(CblasRowMajor, &trans, &trans, 
                      &rowsA, &colsB, &colsA, 
                      &alpha, const_cast<const double**>(a_array.data()), &colsA, 
                      const_cast<const double**>(b_array.data()), &colsB, 
                      &beta, const_cast<double**>(c_array.data()), &colsB, 
                      1, &batch_size);
}

// Wrapper for cblas_gemm_batch for std::complex<float>
void cblas_gemm_batch_wrapper_complex64(int rowsA, int colsB, int colsA, const std::vector<const std::complex<float>*>& a_array, const std::vector<const std::complex<float>*>& b_array, std::vector<std::complex<float>*>& c_array, int batch_size) {
    const std::complex<float> alpha(1.0f, 0.0f);
    const std::complex<float> beta(0.0f, 0.0f);
    CBLAS_TRANSPOSE trans = CblasNoTrans;

    cblas_cgemm_batch(CblasRowMajor, &trans, &trans, 
                      &rowsA, &colsB, &colsA, 
                      reinterpret_cast<const MKL_Complex8*>(&alpha), reinterpret_cast<const MKL_Complex8**>(const_cast<const MKL_Complex8**>(reinterpret_cast<const MKL_Complex8**>(a_array.data()))), &colsA, 
                      reinterpret_cast<const MKL_Complex8**>(const_cast<const MKL_Complex8**>(reinterpret_cast<const MKL_Complex8**>(b_array.data()))), &colsB, 
                      reinterpret_cast<const MKL_Complex8*>(&beta), reinterpret_cast<MKL_Complex8**>(c_array.data()), &colsB, 
                      1, &batch_size);
}

// Wrapper for cblas_gemm_batch for std::complex<double>
void cblas_gemm_batch_wrapper_complex128(int rowsA, int colsB, int colsA, const std::vector<const std::complex<double>*>& a_array, const std::vector<const std::complex<double>*>& b_array, std::vector<std::complex<double>*>& c_array, int batch_size) {
    const std::complex<double> alpha(1.0, 0.0);
    const std::complex<double> beta(0.0, 0.0);
    CBLAS_TRANSPOSE trans = CblasNoTrans;

    cblas_zgemm_batch(CblasRowMajor, &trans, &trans, 
                      &rowsA, &colsB, &colsA, 
                      reinterpret_cast<const MKL_Complex16*>(&alpha), reinterpret_cast<const MKL_Complex16**>(const_cast<const MKL_Complex16**>(reinterpret_cast<const MKL_Complex16**>(a_array.data()))), &colsA, 
                      reinterpret_cast<const MKL_Complex16**>(const_cast<const MKL_Complex16**>(reinterpret_cast<const MKL_Complex16**>(b_array.data()))), &colsB, 
                      reinterpret_cast<const MKL_Complex16*>(&beta), reinterpret_cast<MKL_Complex16**>(c_array.data()), &colsB, 
                      1, &batch_size);
}

template <typename T>
void cblas_gemm_batch_wrapper(int rowsA, int colsB, int colsA, const std::vector<const T*>& a_array, const std::vector<const T*>& b_array, std::vector<T*>& c_array, int batch_size);

template <>
void cblas_gemm_batch_wrapper<float>(int rowsA, int colsB, int colsA, const std::vector<const float*>& a_array, const std::vector<const float*>& b_array, std::vector<float*>& c_array, int batch_size) {
    cblas_gemm_batch_wrapper_float(rowsA, colsB, colsA, a_array, b_array, c_array, batch_size);
}

template <>
void cblas_gemm_batch_wrapper<double>(int rowsA, int colsB, int colsA, const std::vector<const double*>& a_array, const std::vector<const double*>& b_array, std::vector<double*>& c_array, int batch_size) {
    cblas_gemm_batch_wrapper_double(rowsA, colsB, colsA, a_array, b_array, c_array, batch_size);
}

template <>
void cblas_gemm_batch_wrapper<std::complex<float>>(int rowsA, int colsB, int colsA, const std::vector<const std::complex<float>*>& a_array, const std::vector<const std::complex<float>*>& b_array, std::vector<std::complex<float>*>& c_array, int batch_size) {
    cblas_gemm_batch_wrapper_complex64(rowsA, colsB, colsA, a_array, b_array, c_array, batch_size);
}

template <>
void cblas_gemm_batch_wrapper<std::complex<double>>(int rowsA, int colsB, int colsA, const std::vector<const std::complex<double>*>& a_array, const std::vector<const std::complex<double>*>& b_array, std::vector<std::complex<double>*>& c_array, int batch_size) {
    cblas_gemm_batch_wrapper_complex128(rowsA, colsB, colsA, a_array, b_array, c_array, batch_size);
}

template <typename T>
void batch_matrix_multiply(const py::list& A_list, py::array_t<T> B, const py::list& B_shapes_list) {
    py::buffer_info bufB = B.request();

    if (bufB.ndim != 1) {
        throw std::runtime_error("Input matrix B must be 1D");
    }

    T* current_B = static_cast<T*>(bufB.ptr);
    int total_elements_B = bufB.size;

    T* next_B = new T[total_elements_B];

    for (size_t idx = 0; idx < A_list.size(); ++idx) {
        py::array_t<T> A = A_list[idx].cast<py::array_t<T>>();
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

        std::vector<const T*> a_array(batch_size);
        std::vector<const T*> b_array(batch_size);
        std::vector<T*> c_array(batch_size);

        for (int i = 0; i < batch_size; ++i) {
            a_array[i] = static_cast<T*>(bufA.ptr);
            b_array[i] = current_B + i * rowsB * colsB;
            c_array[i] = next_B + i * rowsA * colsB;
        }

        cblas_gemm_batch_wrapper(rowsA, colsB, colsA, a_array, b_array, c_array, batch_size);

        // Swap pointers
        std::swap(current_B, next_B);
    }

    // Update the original numpy array to point to the new data
    py::detail::array_proxy(B.ptr())->data = reinterpret_cast<char*>(current_B);

    // Free the old B data
