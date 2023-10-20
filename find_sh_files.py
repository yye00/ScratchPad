import os
import sys
import fnmatch

def parse_input(input_str):
    """
    Tokenize the input string by comma, ignoring spaces, to produce a list of paths.
    """
    return [path.strip() for path in input_str.split(',')]

def find_sh_files(directories):
    """
    Search for *.sh files in the given list of directories.
    """
    found_files = []
    for directory in directories:
        for root, dirnames, files in os.walk(directory):
            for filename in fnmatch.filter(files, '*.sh'):
                # Construct the relative file path
                rel_file_path = os.path.relpath(os.path.join(root, filename), start='.')
                found_files.append(rel_file_path)
    return found_files

def main():
    # Ensure proper usage
    if len(sys.argv) < 2:
        print("Usage: python find_sh_files.py 'dir1,dir2,...'", file=sys.stderr)
        sys.exit(1)

    # Get the list of directories from the input argument
    input_dirs = sys.argv[1]
    directories = parse_input(input_dirs)

    # Find all *.sh files
    sh_files = find_sh_files(directories)

    # Print the relative paths, one per line, for the calling bash script to capture.
    for file_path in sh_files:
        print(file_path)

if __name__ == "__main__":
    main()
