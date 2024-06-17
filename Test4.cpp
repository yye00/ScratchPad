#include <iostream>
#include <cstdlib> // for getenv and atoi
#include <string>
#include <stdexcept> // for std::invalid_argument

int get_env_var_as_int(const std::string &var_name, int default_value) {
    const char *val = std::getenv(var_name.c_str());
    if (val == nullptr) {
        // Environment variable is not set, return default value
        return default_value;
    }
    try {
        // Try to convert the value to an integer
        int result = std::stoi(val);
        return result;
    } catch (const std::invalid_argument &) {
        // Conversion failed, return default value
        return default_value;
    } catch (const std::out_of_range &) {
        // Value is out of range for an int, return default value
        return default_value;
    }
}

int main() {
    // Example usage
    int my_var = get_env_var_as_int("MY_ENV_VAR", 42);
    std::cout << "Value of MY_ENV_VAR: " << my_var << std::endl;

    return 0;
}
