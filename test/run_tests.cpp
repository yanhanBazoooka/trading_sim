#include "simulator_test.hpp"
#include <filesystem>

int main() {
    // Create test output directory
    std::filesystem::create_directory("test_output");
    
    // Change to test output directory
    std::filesystem::current_path("test_output");
    
    SimulatorTest test;
    test.run_all_tests();
    
    return 0;
} 