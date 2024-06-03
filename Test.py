import numpy as np
import cotengra as ctg
import time

# Function to create a matrix train
def create_matrix_train(num_matrices, dim):
    matrices = [np.random.rand(dim, dim) for _ in range(num_matrices)]
    return matrices

# Function to contract matrix train with cotengra
def contract_with_cotengra(matrix_train, num_threads=4):
    # Create the list of tensors and contraction indices
    tensors = {}
    einsum_expr = ''
    for i, matrix in enumerate(matrix_train):
        tensors[f'T{i}'] = matrix
        if i == 0:
            einsum_expr += f'T{i}[i0,j0]'
        else:
            einsum_expr += f',T{i}[i{i-1},j{i}]'
    einsum_expr += '->' + ''.join([f'j{i}' for i in range(len(matrix_train))])

    # Create a cotengra optimizer with parallel execution
    opt = ctg.HyperOptimizer(
        methods=['greedy', 'kahypar'],
        parallel='threads',  # Use threads for parallelization
        max_repeats=128,
        max_time=3600,
        progbar=True,
    )

    # Contract using cotengra
    tn = ctg.ContractionTree.from_equation(einsum_expr, *tensors.values())
    tn.optimize(opt)
    result = tn.contract(output_backend='numpy', executor=ctg.ThreadPoolExecutor(max_workers=num_threads))
    return result

# Parameters
num_matrices = 40
dim = 10

# Create the matrix train
matrix_train = create_matrix_train(num_matrices, dim)

# Benchmark cotengra contraction
start_time = time.time()
result_cotengra = contract_with_cotengra(matrix_train)
end_time = time.time()
print(f"cotengra contraction time: {end_time - start_time} seconds")
