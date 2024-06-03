import numpy as np
import tensornetwork as tn
import cotengra as ctg
import time
from concurrent.futures import ThreadPoolExecutor

# Function to create a tensor train
def create_tensor_train(num_sites, bond_dim, phys_dim):
    tensors = []
    for i in range(num_sites):
        if i == 0 or i == num_sites - 1:
            tensors.append(np.random.rand(phys_dim, bond_dim))
        else:
            tensors.append(np.random.rand(bond_dim, phys_dim, bond_dim))
    return tensors

# Function to contract tensor train with tensornetwork
def contract_with_tensornetwork(tensor_train):
    nodes = [tn.Node(tensor) for tensor in tensor_train]
    for i in range(len(nodes) - 1):
        nodes[i][1] ^ nodes[i + 1][0]  # Connect the bond dimensions
    result = tn.contractors.greedy(nodes)  # Use the greedy contractor
    return result.tensor

# Function to contract tensor train with cotengra
def contract_with_cotengra(tensor_train, num_threads=4):
    def contract_pair(tensor1, tensor2):
        return np.tensordot(tensor1, tensor2, axes=1)

    tensors = tensor_train.copy()
    with ThreadPoolExecutor(max_workers=num_threads) as executor:
        while len(tensors) > 1:
            results = []
            for i in range(0, len(tensors) - 1, 2):
                future = executor.submit(contract_pair, tensors[i], tensors[i + 1])
                results.append(future)
            tensors = [result.result() for result in results]
            if len(tensors) % 2 == 1:
                tensors.append(tensors[-1])
    return tensors[0]

# Parameters
num_sites = 40
bond_dim = 10
phys_dim = 2

# Create the tensor train
tensor_train = create_tensor_train(num_sites, bond_dim, phys_dim)

# Benchmark tensornetwork contraction
start_time = time.time()
result_tensornetwork = contract_with_tensornetwork(tensor_train)
end_time = time.time()
print(f"tensornetwork contraction time: {end_time - start_time} seconds")

# Benchmark cotengra contraction
start_time = time.time()
result_cotengra = contract_with_cotengra(tensor_train)
end_time = time.time()
print(f"cotengra contraction time: {end_time - start_time} seconds")
