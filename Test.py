import numpy as np
import tensornetwork as tn
import cotengra as ctg
import time

# Function to create a matrix train
def create_matrix_train(num_matrices, dim):
    matrices = [np.random.rand(dim, dim) for _ in range(num_matrices)]
    return matrices

# Function to contract matrix train with tensornetwork
def contract_with_tensornetwork(matrix_train):
    nodes = [tn.Node(matrix) for matrix in matrix_train]
    for i in range(len(nodes) - 1):
        nodes[i][1] ^ nodes[i + 1][0]  # Connect the matrices
    result = tn.contractors.greedy(nodes)  # Use the greedy contractor
    return result.tensor

# Function to contract matrix train with cotengra
def contract_with_cotengra(matrix_train, num_threads=4):
    opt = ctg.ReusableHyperOptimizer(
        methods=['greedy', 'kahypar'],
        parallel='threads',  # Use threads for parallelization
        max_repeats=128,
        max_time=3600,
        progbar=True,
    )
    
    # Create a tensor network from the matrix train
    tn_cores = [tn.Node(matrix) for matrix in matrix_train]
    edges = [tn_cores[i][1] ^ tn_cores[i+1][0] for i in range(len(tn_cores) - 1)]
    
    tn_net = tn.TensorNetwork(tn_cores)
    
    # Perform contraction using cotengra's parallel contractor
    tn_net.contract_with(opt)
    
    return tn_net.get_final_tensor()

# Parameters
num_matrices = 40
dim = 10

# Create the matrix train
matrix_train = create_matrix_train(num_matrices, dim)

# Benchmark tensornetwork contraction
start_time = time.time()
result_tensornetwork = contract_with_tensornetwork(matrix_train)
end_time = time.time()
print(f"tensornetwork contraction time: {end_time - start_time} seconds")

# Benchmark cotengra contraction
start_time = time.time()
result_cotengra = contract_with_cotengra(matrix_train)
end_time = time.time()
print(f"cotengra contraction time: {end_time - start_time} seconds")
