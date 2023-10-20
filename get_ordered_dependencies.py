import os
import sys
from collections import deque

def tokenize_input(input_str):
    """
    Split the input string by comma and strip whitespace to get individual dependencies.
    """
    return [dep.strip() for dep in input_str.split(",")]

def find_script_file(start_path, target_file):
    """
    Recursively search for the script file in the directory tree.
    """
    for root, _, files in os.walk(start_path):
        if target_file in files:
            return os.path.join(root, target_file)
    return None

def parse_dependencies(script_path):
    """
    Extract the MODULE_DEPENDENCIES from a script file.
    """
    dependencies = []
    try:
        with open(script_path, 'r') as file:
            lines = file.readlines()
            for line in lines:
                if "MODULE_DEPENDENCIES" in line:
                    # Assume the format is MODULE_DEPENDENCIES="dep1,dep2,..."
                    dep_str = line.split('=')[1]
                    dep_str = dep_str.strip('"\n')  # Remove quotes and newline
                    dependencies = tokenize_input(dep_str)
                    break
    except IOError as e:
        print(f"Error reading {script_path}: {e}", file=sys.stderr)
    return dependencies

def construct_dependency_graph(start_path, root_dependencies):
    """
    Construct a dependency graph (DAG) and return an ordered list of packages to build.
    """
    build_order = []
    visited = set()
    processing = deque(root_dependencies)

    while processing:
        dep = processing.popleft()
        if dep in visited:
            continue  # Skip dependencies we've already processed
        
        script_file = find_script_file(start_path, f"{dep}.sh")
        if script_file:
            sub_dependencies = parse_dependencies(script_file)
            # If the current node has dependencies that we haven't processed, 
            # we need to process them first before adding the current node to the build order.
            if any(sub_dep not in visited for sub_dep in sub_dependencies):
                processing.appendleft(dep)  # Re-add the current package to the queue
                processing.extendleft(reversed(sub_dependencies))  # Add new dependencies to the front of the queue
            else:
                visited.add(dep)
                build_order.append(dep)
        else:
            print(f"Script file for dependency {dep} not found.", file=sys.stderr)
            sys.exit(1)

    return build_order

def main():
    # Command-line argument handling
    if len(sys.argv) < 2:
        print("Usage: python get_ordered_dependencies.py 'dep1,dep2,...'", file=sys.stderr)
        sys.exit(1)

    input_dependencies = sys.argv[1]
    root_dependencies = tokenize_input(input_dependencies)

    # Assuming the script is run from the root directory containing the package directories
    start_path = '.'

    # Construct the dependency graph and output the result
    ordered_dependencies = construct_dependency_graph(start_path, root_dependencies)
    for dep in ordered_dependencies:
        print(dep)  # This output can be captured by the calling shell script

if __name__ == "__main__":
    main()
