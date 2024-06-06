import h5py
import numpy as np
from concurrent.futures import ThreadPoolExecutor
import time

def write_to_hdf5(filename, datasets, dictionary):
    with h5py.File(filename, 'w') as f:
        # Write the list of numpy arrays
        for i, data in enumerate(datasets):
            f.create_dataset(f"dataset_{i}", data=data)
        
        # Write the dictionary
        for key, value in dictionary.items():
            f.attrs[key] = value

def async_write_to_hdf5(filename, datasets, dictionary):
    with ThreadPoolExecutor() as executor:
        future = executor.submit(write_to_hdf5, filename, datasets, dictionary)
        return future

# Example usage
if __name__ == "__main__":
    # Create some example data
    datasets = [np.random.rand(100, 100) for _ in range(5)]
    dictionary = {"key1": "value1", "key2": 42, "key3": 3.14}

    # Start the asynchronous writing
    future = async_write_to_hdf5("example.h5", datasets, dictionary)
    
    # Continue with other tasks while writing is in progress
    print("Writing to HDF5 started...")
    for i in range(5):
        print(f"Main script working... {i}")
        time.sleep(1)
    
    # Ensure the writing is complete before exiting
    future.result()
    print("Writing to HDF5 completed.")
