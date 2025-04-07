# Tests

This directory contains unit and integration tests for the UAV Optical Flow Navigation System.

## Directory Structure

- `/unit` - Unit tests for individual components

    - `/core` - Tests for math core components
    
    - `/nav-dr` - Tests for dead reckoning algorithms
  
    - `/nav-of` - Tests for optical flow processing
  
    - `/nav-sf` - Tests for sensor fusion algorithms
  
    - `/io` - Tests for input/output operations

- `/integration` - Integration tests that validate multiple components working together

## Running Tests

We use Google Test framework for C++ testing. To run the tests:

``` bash
# Build the tests
cd build
cmake ..
make tests

# Run all tests
./tests/run_tests

# Run specific test suite
./tests/run_tests --gtest_filter=DeadReckoningTests.*

# Run with verbose output
./tests/run_tests --gtest_filter=OpticalFlowTests.* --gtest_output=verbose
```

## Adding New Tests

1. Create a new test file in the appropriate subdirectory

    - Name the file as <component_name>_test.cpp
    
    - For a new component, create a corresponding subdirectory if needed


2. Write test cases following the Google Test format:

``` cpp
#include "gtest/gtest.h"
#include "component_to_test.h"

TEST(TestSuiteName, TestName) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(expected, actual);
}
```

3. Add the test file to the appropriate `CMakeLists.txt`


## Continuous Integration

All tests are automatically run on each pull request. PRs cannot be merged if tests are failing.

## Writing Good Tests

- Each test should focus on a single functionality

- Use appropriate test fixtures to reduce code duplication

- Mock external dependencies when necessary

- Test both normal cases and edge cases

- Keep tests independent from each other

- Make test failures descriptive to aid debugging