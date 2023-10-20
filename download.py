import os
import sys
import fnmatch
import hashlib
import urllib.request

def parse_input(input_str):
    return [path.strip() for path in input_str.split(',')]

def find_sh_files(directories):
    found_files = []
    for directory in directories:
        for root, dirnames, files in os.walk(directory):
            for filename in fnmatch.filter(files, '*.sh'):
                rel_file_path = os.path.relpath(os.path.join(root, filename), start='.')
                found_files.append(rel_file_path)
    return found_files

def download_and_verify(url, md5sum):
    file_name = url.split('/')[-1]
    urllib.request.urlretrieve(url, file_name)

    # Verify the MD5
    file_md5 = hashlib.md5(open(file_name, 'rb').read()).hexdigest()
    if file_md5 != md5sum:
        os.remove(file_name)
        raise ValueError(f"MD5 checksum failed for {file_name}")

    return file_name

def process_file(file_path, flat_file):
    # Parse the script's relative path to get the package name and version
    path_parts = file_path.split(os.sep)
    package_info = "/".join(path_parts[-2:])  # Assuming structure: .../folder_name/version.sh
    package_name_version = package_info.replace('.sh', '')

    # Read the script file and find DOWNLOAD_URL and PKG_MD5SUM
    with open(file_path, 'r') as script_file:
        script_content = script_file.read()
    
    lines = script_content.split('\n')
    download_url = next((line.split('=')[1] for line in lines if "DOWNLOAD_URL" in line), None)
    pkg_md5sum = next((line.split('=')[1] for line in lines if "PKG_MD5SUM" in line), None)

    if download_url is None or pkg_md5sum is None:
        raise ValueError(f"DOWNLOAD_URL or PKG_MD5SUM not found in {file_path}")

    # Check if package info exists in the flat file
    with open(flat_file, 'r') as f:
        existing_packages = f.read()

    if package_name_version in existing_packages and download_url in existing_packages and pkg_md5sum in existing_packages:
        return  # Package already listed, nothing to do

    # Download and verify the package
    try:
        download_and_verify(download_url, pkg_md5sum)
    except Exception as e:
        sys.exit(str(e))  # Exit on any error

    # Update the flat file with the new package information
    with open(flat_file, 'a') as f:
        new_entry = f"{package_name_version},{download_url},{pkg_md5sum}\n"
        f.write(new_entry)

def main():
    if len(sys.argv) < 3:
        print("Usage: python find_sh_files.py flat_file 'dir1,dir2,...'", file=sys.stderr)
        sys.exit(1)

    flat_file = sys.argv[1]
    input_dirs = sys.argv[2]
    directories = parse_input(input_dirs)

    sh_files = find_sh_files(directories)

    for file_path in sh_files:
        process_file(file_path, flat_file)

if __name__ == "__main__":
    main()
