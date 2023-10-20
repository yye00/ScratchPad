# Function to load modules from a comma-separated list
load_modules() {
    # Get the argument passed to the function
    local modules="$1"

    # Remove spaces from the input, so 'mod1, mod2' becomes 'mod1,mod2'
    modules=${modules// /}

    # Flag to check if all modules loaded successfully
    local all_success=true

    # Check if the string is non-empty
    if [ -n "$modules" ]; then
        # Convert the comma-separated string into an array of module names
        IFS=',' read -ra module_array <<< "$modules"

        # Loop through the array and attempt to load each module
        for module in "${module_array[@]}"; do
            echo "Loading module: $module"

            # Try to load the module. Error messages will be directed to stderr.
            if ! module load "$module"; then
                # If the module failed to load, print the error and update the flag
                echo "Error loading module: $module" >&2  # Redirect message to stderr
                all_success=false  # Set the flag to false indicating there was a failure
            fi
        done

        # If not all modules loaded successfully, exit with an error status
        if [ "$all_success" = false ]; then
            return 1
        fi
    else
        echo "No modules specified for loading."
    fi
}

# Function to set environment variables based on the build configuration
set_build_env() {
    # Get the configuration name passed to the function
    local config_name="$1"

    # Based on the configuration, set the appropriate environment variables
    case "$config_name" in
        GCC_DEBUG)
            export CC=gcc
            export CXX=g++
            export CFLAGS='-O0 -g'
            export CXXFLAGS='-O0 -g'
            export LDFLAGS=''
            ;;
        GCC_RELEASE)
            export CC=gcc
            export CXX=g++
            export CFLAGS='-g -O3'
            export CXXFLAGS='-g -O3'
            export LDFLAGS=''
            ;;
        INTEL_DEBUG)
            export CC=icx
            export CXX=icp
            export CFLAGS='-O0 -g'
            export CXXFLAGS='-O0 -g'
            export LDFLAGS=''
            ;;
        INTEL_RELEASE)
            export CC=icc
            export CXX=icpc
            export CFLAGS='-O3'
            export CXXFLAGS='-O3'
            export LDFLAGS=''
            ;;
        *)
            echo "Unknown configuration: $config_name" >&2
            return 1
            ;;
    esac

    echo "Environment set for $config_name configuration."
}

# Function to create a new file in the specified location
create_file() {
  # Check for the right number of arguments
  if [ "$#" -ne 2 ]; then
    echo "Usage: create_file <path> <filename>"
    return 1
  fi

  local path="$1"
  local filename="$2"

  # Create the directory if it doesn't exist
  mkdir -p "$path"

  # Navigate to the directory (optional, depending on requirements)
  cd "$path"

  # Create the file
  touch "$filename"

  # Return to the original directory (optional)
  cd - > /dev/null  # Sending output to /dev/null to silence the 'popd' output

  echo "File '$filename' created in '$path'."
}

# Function to append a line to the specified file
append_line() {
  # Check for the right number of arguments
  if [ "$#" -ne 2 ]; then
    echo "Usage: append_line <filepath> <line>"
    return 1
  fi

  local filepath="$1"
  local line="$2"

  # Check if the file exists
  if [ ! -f "$filepath" ]; then
    echo "Error: File '$filepath' does not exist."
    return 1
  fi

  # Append the line to the file
  echo "$line" >> "$filepath"
}

# Function to create a Lua module file based on environment variables from the parent script
create_lua_modulefile() {
    # ... [Previous content setup]

    # Add 'add_property' if provided
    if [ -n "$MODULE_ADD_PROPERTY" ]; then
        IFS=':' read -ra prop_kv <<< "$MODULE_ADD_PROPERTY"
        lua_module_content+="add_property(\"${prop_kv[0]}\", \"${prop_kv[1]}\")\n"
    fi

    # Add 'remove_property' if provided
    if [ -n "$MODULE_REMOVE_PROPERTY" ]; then
        IFS=':' read -ra prop_kv <<< "$MODULE_REMOVE_PROPERTY"
        lua_module_content+="remove_property(\"${prop_kv[0]}\", \"${prop_kv[1]}\")\n"
    fi

    # ... [Handle other directives similarly, e.g., 'whatis', 'help', 'depends_on']

    # Add 'load' modules if provided
    if [ -n "$MODULE_LOAD" ]; then
        IFS=',' read -ra modules_array <<< "$MODULE_LOAD"
        for module in "${modules_array[@]}"; do
            lua_module_content+="load(\"$module\")\n"
        done
    fi

    # ... [Continue with other directives like 'load_any', 'try_load', 'source_sh', etc.]

    # Finalize the Lua module file content with all directives considered

    # Write the content to a Lua file
    echo -e "$lua_module_content" > "${LUA_FILE_LOCATION}/${LUA_MODULE_FILE_NAME}.lua"

    echo "Lua module file created at ${LUA_FILE_LOCATION}/${LUA_MODULE_FILE_NAME}.lua"
}


#######################################################################################
#######################################################################################
#######################################################################################

# Calling the Python script and capturing the ordered list of packages.
ORDERED_PACKAGES=$(python3 get_ordered_dependencies.py)
check_output "Failed to retrieve ordered package list."

# Iterate over the packages and perform the required actions
while IFS= read -r package; do
    echo "Processing package: $package"

    # Navigate to the appropriate directory and initiate the build process.
    # Replace this with the actual command(s) you need to build your package.
    PACKAGE_BUILD_SCRIPT="root_directory/package_families/${package}/build.sh"
    if [ -f "$PACKAGE_BUILD_SCRIPT" ]; then
        echo "Building $package..."
        bash "$PACKAGE_BUILD_SCRIPT"
        
        # Check if the build was successful
        check_output "Error: Failed to build $package"
    else
        echo "Build script for $package not found."
        # Exit because we expect every package to have a build script
        exit 1
    fi

done <<< "$ORDERED_PACKAGES"

# (Rest of your script)
